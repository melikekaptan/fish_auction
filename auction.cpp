#include "auction.h"
#include "auction_product.h"


#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <map>

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
            ++product.represented_time;
            std::cout << "auction placement start here" << std::endl;
            std::cout << "represented time of this product:" <<std::endl;
            std::cout << product.represented_time << std::endl;

        }

}


// void Auction::BidListener(AuctionProduct product) {

//         if (added)
//             product.IncreasePrice(uint32_t added);
// }