#include </usr/local/include/librdkafka/rdkafkacpp.h>
#include "auction_listener.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <sys/time.h>
#include <getopt.h>
#include <unistd.h>
#include <regex>
#include <vector>

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


void AuctionListener::msg_consume(RdKafka::Message* message, std::vector<fish_product> &list_of_products, void* opaque) {
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
        // printf("%.*s\n",
        //        static_cast<int>(message->len()),
        //        static_cast<const char *>(message->payload()));

               auto msg = message->payload();

               std::cout << "string message:" <<std::endl;
               const char *message_payload;
               message_payload = static_cast<char*>(message->payload());
               std::string s(message_payload);


                std::smatch m;
                std::regex str_expr1 ("fish_id");
                std::regex str_expr2 ("fish_type");
                std::regex str_expr3 ("quantity");
                std::regex str_expr4 ("fisherman_id");
                std::regex str_expr5 ("fisherman_name");
                std::regex str_expr6 ("fisherman_surname");
                std::regex str_expr7 ("price");

                std::array <std::regex, 7> my_regex_matches{ {str_expr1, str_expr2, str_expr3, str_expr4, str_expr5, str_expr6, str_expr7} };

                std::array<std::string, 7> fish_product_array {{}};
                for (int k = 0; k<7; k++) {
                  std::regex_search ( s, m, my_regex_matches[k] );
                  for (unsigned i=0; i<m.size(); ++i) {
                  
                        std::string remaining = s.substr(m.position() + 1);
      
                        size_t pos = remaining.find_first_of(":");
                        size_t pos_ = remaining.find_first_of(",");
                        size_t interval = pos_ - pos;
                        fish_product_array[k] = remaining.substr(pos + 1, interval-1);
                  }
                }

                fish_product product;
                std::stringstream strm(fish_product_array[0]);
                strm >> product.fish_id;

                product.fish_type = fish_product_array[1].substr(1, fish_product_array[1].size() - 2);

                std::string word;
                word = fish_product_array[2].substr(1, fish_product_array[2].size() - 2);
                product.quantity = atof(word.c_str());

                std::stringstream strm__(fish_product_array[3]);
                strm__ >> product.fisherman_id;

                product.fisherman_name = fish_product_array[4].substr(1, fish_product_array[4].size() - 2);
                product.fisherman_surname = fish_product_array[5].substr(1, fish_product_array[5].size() - 2);

                std::stringstream strm___(fish_product_array[6]);
                strm___ >> product.price;

                              
                list_of_products.push_back(product);
           
                std::cout << "one item added to the list of products : " + product.fish_type << std::endl;    
                // std::cout << product.fish_id << std::endl;
                // std::cout << product.fish_type << std::endl;
                // std::cout << product.quantity << std::endl;
                // std::cout << product.fisherman_id << std::endl;
                // std::cout << product.fisherman_name << std::endl;
                // std::cout << product.fisherman_surname << std::endl;
                // std::cout << product.price << std::endl;      
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
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      run = 0;
  }
}

void AuctionListener::AuctionServiceListener(std::vector<fish_product> &list_of_products) {

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

    topics.push_back(std::string("TestTopic"));

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
    this->msg_consume(msg, list_of_products, NULL);
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

