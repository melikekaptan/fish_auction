#include "auction.h"
#include "auction_product.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <map>
#include <thread>



void BidListener(AuctionProduct product) {

        uint32_t added = 10;
        std::cout << "bid listener listens" << std::endl;
        char user_reaction; 

        for (int i=0; i < 15; i++) {
            std::cout << "is there a bid ? (Y) yes, (N) no";
            std::cin >>user_reaction;
            if (user_reaction == 'Y')
              product.IncreasePrice(added);
            if (user_reaction == 'N') {
              product.DeliverFinalPrice();
              break;
            }
        }
}

Auction::Auction () {

    std::ifstream auction_file;
    std::string line;
    std::string product_name;
    uint32_t price;

    try {
        
        auction_file.open("auction_table.txt");
        
        if (!auction_file.is_open())
            throw ;
         
            while (std::getline(auction_file, line)) {
                std::stringstream s(line);
                std::cout <<  line << std::endl;
                s >> product_name >> price;
               auction_table.emplace(product_name, price);
            }
        
        }
    catch (std::exception& e) {
        std::cout<< e.what();
    }

}

void Auction::place_auction() {

        std::map<std::string, uint32_t>::iterator itr;
        for( itr = Auction::auction_table.begin() ; itr != Auction::auction_table.end() ; ++itr) {
            AuctionProduct product(itr->second);

            //timer starts here
            std::thread t(BidListener, product);

            t.join();

            ++product.represented_time;
            std::cout << itr->second << std::endl;
            std::cout << "represented time of this product:" <<std::endl;
            std::cout << product.represented_time << std::endl;

        }
}

