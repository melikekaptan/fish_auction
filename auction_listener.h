#pragma once

#include </usr/local/include/librdkafka/rdkafkacpp.h>

#include <vector>

struct fish_product{
            int fish_id;
            std::string fish_type;
            double quantity;
            int fisherman_id;
            std::string fisherman_name;
            std::string fisherman_surname;
            int price;
        };

class AuctionListener {

    public:
        
        void msg_consume(RdKafka::Message* message, std::vector<fish_product>&, void* opaque);
        void AuctionServiceListener(std::vector<fish_product>&);
};