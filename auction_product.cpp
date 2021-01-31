#include "auction_product.h"

#include <iostream>

int AuctionProduct::initial = 0;

AuctionProduct::AuctionProduct(int price, std::string &fish_type, double quantity) {
        std::cout << "auction product formed" << std::endl;
        AuctionProduct::initial = price;
        AuctionProduct::base_price = price;
        std::cout << "product base price is: " << std::endl;
        std::cout << base_price << std::endl;
}

void AuctionProduct::IncreasePrice(int added) {

     AuctionProduct::initial += added;

}

void AuctionProduct::DeliverFinalPrice() {

    AuctionProduct::final_price = AuctionProduct::initial;

}

