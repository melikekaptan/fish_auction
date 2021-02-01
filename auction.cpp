#include "auction.h"
#include "auction_product.h"
#include "auction_listener.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <csignal>

static volatile sig_atomic_t run = 1;
static void sigterm (int sig) {
  run = 0;
}

void SendBack(AuctionProduct &product) {

    std::cout << "product sent back " << product.type << std::endl;
    Auction::second_round_list_of_products.push_back(product);

}

void SellProduct(AuctionProduct &product) {

    std::cout << "Product sold: " << product.type << " with final price " << product.final_price << std::endl;
}


void BidListener(AuctionProduct product, std::array<bool, 5> arr) {

        int added = 10;
        std::cout << "bid listener listens" << std::endl;

        for (int i=0; i < 5; i++) {
            if (arr[i] == 1)
                product.IncreasePrice(added);
                std::this_thread::sleep_for (std::chrono::seconds(1));
                std::cout << "increased price of the product:" << std::endl;
                std::cout << product.initial << std::endl;
            if (arr[i] == 0) {
                product.DeliverFinalPrice();
      
                std::cout << "final price of the product:" <<std::endl;
                std::cout << product.final_price << std::endl;
                    if (product.final_price == product.base_price)
                        SendBack(product);
                    else 
                        SellProduct(product);
                break;
            }
        }
}

std::vector <fish_product> Auction::list_of_products {};
std::vector <AuctionProduct> Auction::second_round_list_of_products {};


Auction::Auction () {

     signal(SIGINT, sigterm);
     signal(SIGTERM, sigterm);

    try { 
        std::cout << "Press Ctrl-C when you finish adding the products" << std::endl;
        AuctionListener *listener;
        listener->AuctionServiceListener(list_of_products);

        if (!run) {
            delete listener;
        } 
    }
    catch (const std::exception& e) {
        std::cout << "auction listener cannot start" << std::endl;
		throw std::runtime_error (e.what());
    }

}


void Auction::place_auction() {

     std::vector<std::array<bool, 5>> increase = {{1,1,1,0,0}, {0,0,0,0,0}, {1,0,0,0,0}, {0,0,0,0,0}, {1,1,1,1,0}};

        for(int i = 0; i< list_of_products.size(); i++) {
            AuctionProduct product(list_of_products.at(i).price, list_of_products.at(i).fish_type, list_of_products.at(i).quantity);
            //timer starts here
                std::thread t(BidListener, product, increase.at(i));

                t.join();

                std::cout << list_of_products.at(i).fish_type << std::endl;
                std::cout << "represented " <<std::endl;

        }

        std::cout << second_round_list_of_products.size() << std::endl;

        for (int l=0; l<second_round_list_of_products.size(); l++) {
            std::thread t(BidListener, second_round_list_of_products.at(l), increase.at(l));

                t.join();

                std::cout << second_round_list_of_products.at(l).type << std::endl;
                std::cout << "represented second time" <<std::endl;

        }




}

