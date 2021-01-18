#include </usr/local/include/librdkafka/rdkafkacpp.h>
#include "auction_listener.h"

#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <sys/time.h>
#include <getopt.h>
#include <unistd.h>

static volatile sig_atomic_t run = 1;
static bool exit_eof = false;
static int eof_cnt = 0;
static int partition_cnt = 0;
static int verbosity = 1;
static long msg_cnt = 0;
static int64_t msg_bytes = 0;
static void sigterm (int sig) {
  run = 0;
}


/**
 * @brief format a string timestamp from the current time
 */
static void print_time () {

        struct timeval tv;
        char buf[64];
        gettimeofday(&tv, NULL);
        strftime(buf, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
        fprintf(stderr, "%s.%03d: ", buf, (int)(tv.tv_usec / 1000));
}


class ExampleEventCb : public RdKafka::EventCb {
 public:
  void event_cb (RdKafka::Event &event) {

    print_time();

    switch (event.type())
    {
      case RdKafka::Event::EVENT_ERROR:
        if (event.fatal()) {
          std::cerr << "FATAL ";
          run = 0;
        }
        std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_STATS:
        std::cerr << "\"STATS\": " << event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_LOG:
        fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str());
        break;

      case RdKafka::Event::EVENT_THROTTLE:
	std::cerr << "THROTTLED: " << event.throttle_time() << "ms by " <<
	  event.broker_name() << " id " << (int)event.broker_id() << std::endl;
	break;

      default:
        std::cerr << "EVENT " << event.type() <<
            " (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;
    }
  }
};


class ExampleRebalanceCb : public RdKafka::RebalanceCb {
private:
  static void part_list_print (const std::vector<RdKafka::TopicPartition*>&partitions){
    for (unsigned int i = 0 ; i < partitions.size() ; i++)
      std::cerr << partitions[i]->topic() <<
	"[" << partitions[i]->partition() << "], ";
    std::cerr << "\n";
  }

public:
  void rebalance_cb (RdKafka::KafkaConsumer *consumer,
		     RdKafka::ErrorCode err,
                     std::vector<RdKafka::TopicPartition*> &partitions) {
    std::cerr << "RebalanceCb: " << RdKafka::err2str(err) << ": ";

    part_list_print(partitions);

    RdKafka::Error *error = NULL;
    RdKafka::ErrorCode ret_err = RdKafka::ERR_NO_ERROR;

    if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
      if (consumer->rebalance_protocol() == "COOPERATIVE")
        error = consumer->incremental_assign(partitions);
      else
        ret_err = consumer->assign(partitions);
      partition_cnt += (int)partitions.size();
    } else {
      if (consumer->rebalance_protocol() == "COOPERATIVE") {
        error = consumer->incremental_unassign(partitions);
        partition_cnt -= (int)partitions.size();
      } else {
        ret_err = consumer->unassign();
        partition_cnt = 0;
      }
    }
    eof_cnt = 0; /* FIXME: Won't work with COOPERATIVE */

    if (error) {
      std::cerr << "incremental assign failed: " << error->str() << "\n";
      delete error;
    } else if (ret_err)
      std::cerr << "assign failed: " << RdKafka::err2str(ret_err) << "\n";

  }
};


void msg_consume(RdKafka::Message* message, void* opaque) {
  switch (message->err()) {
    case RdKafka::ERR__TIMED_OUT:
      break;

    case RdKafka::ERR_NO_ERROR:
      /* Real message */
      msg_cnt++;
      msg_bytes += message->len();
      if (verbosity >= 3)
        std::cerr << "Read msg at offset " << message->offset() << std::endl;
      RdKafka::MessageTimestamp ts;
      ts = message->timestamp();
      if (verbosity >= 2 &&
	  ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
	std::string tsname = "?";
	if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
	  tsname = "create time";
        else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
          tsname = "log append time";
        std::cout << "Timestamp: " << tsname << " " << ts.timestamp << std::endl;
      }
      if (verbosity >= 2 && message->key()) {
        std::cout << "Key: " << *message->key() << std::endl;
      }
      if (verbosity >= 1) {
        printf("%.*s\n",
               static_cast<int>(message->len()),
               static_cast<const char *>(message->payload()));
      }
      break;

    case RdKafka::ERR__PARTITION_EOF:
      /* Last message */
      if (exit_eof && ++eof_cnt == partition_cnt) {
        std::cerr << "%% EOF reached for all " << partition_cnt <<
            " partition(s)" << std::endl;
        run = 0;
      }
      break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      run = 0;
      break;

    default:
      /* Errors */
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      run = 0;
  }
}

void AuctionListener::AuctionServiceListener() {

    std::string brokers = "localhost";
  std::string errstr;
  std::string topic_str;
  std::string mode;
  std::string debug;
  std::vector<std::string> topics;
  bool do_conf_dump = false;
  int opt;
        std::cout << "auction listener starts here" << std::endl;
  /*
   * Create configuration objects
   */
  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
  RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

  ExampleRebalanceCb ex_rebalance_cb;
  conf->set("rebalance_cb", &ex_rebalance_cb, errstr);

  conf->set("enable.partition.eof", "true", errstr);

  if (conf->set("group.id",  "0", errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << errstr << std::endl;
                 exit(1);
       }

    topics.push_back(std::string("0"));

//   while ((opt = getopt(argc, argv, "g:b:z:qd:eX:AM:qv")) != -1) {
//     switch (opt) {
//     case 'g':
//       if (conf->set("group.id",  optarg, errstr) != RdKafka::Conf::CONF_OK) {
//         std::cerr << errstr << std::endl;
//         exit(1);
//       }
//       break;
//     case 'b':
//       brokers = optarg;
//       break;
//     case 'z':
//       if (conf->set("compression.codec", optarg, errstr) !=
// 	  RdKafka::Conf::CONF_OK) {
// 	std::cerr << errstr << std::endl;
// 	exit(1);
//       }
//       break;
//     case 'e':
//       exit_eof = true;
//       break;
//     case 'd':
//       debug = optarg;
//       break;
//     case 'M':
//       if (conf->set("statistics.interval.ms", optarg, errstr) !=
//           RdKafka::Conf::CONF_OK) {
//         std::cerr << errstr << std::endl;
//         exit(1);
//       }
//       break;
//     case 'X':
//       {
// 	char *name, *val;

// 	if (!strcmp(optarg, "dump")) {
// 	  do_conf_dump = true;
// 	  continue;
// 	}

// 	name = optarg;
// 	if (!(val = strchr(name, '='))) {
//           std::cerr << "%% Expected -X property=value, not " <<
//               name << std::endl;
// 	  exit(1);
// 	}

// 	*val = '\0';
// 	val++;

// 	/* Try "topic." prefixed properties on topic
// 	 * conf first, and then fall through to global if
// 	 * it didnt match a topic configuration property. */
//         RdKafka::Conf::ConfResult res = RdKafka::Conf::CONF_UNKNOWN;
// 	if (!strncmp(name, "topic.", strlen("topic.")))
//           res = tconf->set(name+strlen("topic."), val, errstr);
//         if (res == RdKafka::Conf::CONF_UNKNOWN)
// 	  res = conf->set(name, val, errstr);

// 	if (res != RdKafka::Conf::CONF_OK) {
//           std::cerr << errstr << std::endl;
// 	  exit(1);
// 	}
//       }
//       break;

//       case 'q':
//         verbosity--;
//         break;

//       case 'v':
//         verbosity++;
//         break;

//     default:
//       goto usage;
//     }
//   }

//   for (; optind < argc ; optind++)
    // topics.push_back(std::string(argv[optind]));

//   if (topics.empty() || optind != argc) {
//   usage:
//     fprintf(stderr,
//             "Usage: %s -g <group-id> [options] topic1 topic2..\n"
//             "\n"
//             "librdkafka version %s (0x%08x)\n"
//             "\n"
//             " Options:\n"
//             "  -g <group-id>   Consumer group id\n"
//             "  -b <brokers>    Broker address (localhost:9092)\n"
//             "  -z <codec>      Enable compression:\n"
//             "                  none|gzip|snappy\n"
//             "  -e              Exit consumer when last message\n"
//             "                  in partition has been received.\n"
//             "  -d [facs..]     Enable debugging contexts:\n"
//             "                  %s\n"
//             "  -M <intervalms> Enable statistics\n"
//             "  -X <prop=name>  Set arbitrary librdkafka "
//             "configuration property\n"
//             "                  Properties prefixed with \"topic.\" "
//             "will be set on topic object.\n"
//             "                  Use '-X list' to see the full list\n"
//             "                  of supported properties.\n"
//             "  -q              Quiet / Decrease verbosity\n"
//             "  -v              Increase verbosity\n"
//             "\n"
//             "\n",
// 	    argv[0],
// 	    RdKafka::version_str().c_str(), RdKafka::version(),
// 	    RdKafka::get_debug_contexts().c_str());
// 	exit(1);
//   }

  if (exit_eof) {
    std::string strategy;
    if (conf->get("partition.assignment.strategy", strategy) ==
        RdKafka::Conf::CONF_OK && strategy == "cooperative-sticky") {
      std::cerr << "Error: this example has not been modified to " <<
        "support -e (exit on EOF) when the partition.assignment.strategy " <<
        "is set to " << strategy << ": remove -e from the command line\n";
      exit(1);
    }
  }

  /*
   * Set configuration properties
   */
  conf->set("metadata.broker.list", brokers, errstr);
        std::cout << "auction listener configuration set" << std::endl;
  if (!debug.empty()) {
    if (conf->set("debug", debug, errstr) != RdKafka::Conf::CONF_OK) {
      std::cerr << errstr << std::endl;
      exit(1);
    }
  }

  ExampleEventCb ex_event_cb;
  conf->set("event_cb", &ex_event_cb, errstr);

  if (do_conf_dump) {
    int pass;

    for (pass = 0 ; pass < 2 ; pass++) {
      std::list<std::string> *dump;
      if (pass == 0) {
        dump = conf->dump();
        std::cout << "# Global config" << std::endl;
      } else {
        dump = tconf->dump();
        std::cout << "# Topic config" << std::endl;
      }

      for (std::list<std::string>::iterator it = dump->begin();
           it != dump->end(); ) {
        std::cout << *it << " = ";
        it++;
        std::cout << *it << std::endl;
        it++;
      }
      std::cout << std::endl;
    }
    exit(0);
  }

  conf->set("default_topic_conf", tconf, errstr);
  delete tconf;

  signal(SIGINT, sigterm);
  signal(SIGTERM, sigterm);


  /*
   * Consumer mode
   */

  /*
   * Create consumer using accumulated global configuration.
   */
  RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
  if (!consumer) {
    std::cerr << "Failed to create consumer: " << errstr << std::endl;
    exit(1);
  }

  delete conf;

  std::cout << "% Created consumer " << consumer->name() << std::endl;


  /*
   * Subscribe to topics
   */
  RdKafka::ErrorCode err = consumer->subscribe(topics);
  if (err) {
    std::cerr << "Failed to subscribe to " << topics.size() << " topics: "
              << RdKafka::err2str(err) << std::endl;
    exit(1);
  }

  /*
   * Consume messages
   */
  while (run) {
    RdKafka::Message *msg = consumer->consume(1000);
    msg_consume(msg, NULL);
    delete msg;
  }

#ifndef _WIN32
  alarm(10);
#endif

  /*
   * Stop consumer
   */
  consumer->close();
  delete consumer;

  std::cerr << "% Consumed " << msg_cnt << " messages ("
            << msg_bytes << " bytes)" << std::endl;

  /*
   * Wait for RdKafka to decommission.
   * This is not strictly needed (with check outq_len() above), but
   * allows RdKafka to clean up all its resources before the application
   * exits so that memory profilers such as valgrind wont complain about
   * memory leaks.
   */
  RdKafka::wait_destroyed(5000);
        
}

void Auction::place_auction() {

        std::map<std::string, uint32_t>::iterator itr;
        for( itr = Auction::auction_table.begin() ; itr != Auction::auction_table.end() ; ++itr) {
            AuctionProduct product(itr->second);
            //timer starts here
            ++product.represented_time;
            std::cout << "auction placement start here" << std::endl;
            std::cout << "represented time of this product:" <<std::endl;
            std::cout << product.represented_time << std::endl;

        }


}