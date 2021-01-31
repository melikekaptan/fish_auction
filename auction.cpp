#include "auction.h"
#include "auction_product.h"
#include "auction_listener.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>
#include <map>
#include <thread>


void SendBack(AuctionProduct product) {

    std::cout << "product sent back" << std::endl;


}

void SellProduct() {

    std::cout << "Product sold" << std::endl;
}


void BidListener(AuctionProduct product, std::array<bool, 5> increase) {

        int added = 10;
        std::cout << "bid listener listens" << std::endl;

        for (int i=0; i < 5; i++) {
            if (increase[i] == 1)
                product.IncreasePrice(added);
                std::cout << "increased price of the product:" << std::endl;
                std::cout << product.initial << std::endl;
            if (increase[i] == 0) {
                product.DeliverFinalPrice();
                std::cout << "final price of the product:" <<std::endl;
                std::cout << product.final_price << std::endl;
                    if (product.final_price == product.base_price)
                        SendBack(product);
                    else 
                        SellProduct();
                break;
            }
        }
}

std::vector <fish_product> Auction::list_of_products {};

Auction::Auction () {

    try { 
        AuctionListener listener;
        listener.AuctionServiceListener(list_of_products);
    }
    catch (const std::exception& e) {
        std::cout << "auction listener cannot start" << std::endl;
		throw std::runtime_error (e.what());
    }

}


void Auction::place_auction() {

        for(auto p : Auction::list_of_products) {
            AuctionProduct product(p.price, p.fish_type, p.quantity);

            std::array<bool, 5> increase = {1,1,1,0,0};

            //timer starts here
            std::thread t(BidListener, product, increase);

            t.join();

            std::cout << p.fish_type << std::endl;
            std::cout << "represented " <<std::endl;

        }
}

