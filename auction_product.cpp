#include "auction_product.h"

#include <iostream>

uint32_t AuctionProduct::initial = 0;

AuctionProduct::AuctionProduct(uint32_t price) {
        std::cout << "auction product formed" << std::endl;
        AuctionProduct::initial = price;
        AuctionProduct::base_price = price;
        std::cout << "product base price is: " << std::endl;
        std::cout << base_price << std::endl;
}

void AuctionProduct::IncreasePrice(uint32_t added) {

     AuctionProduct::initial += added;

}

void AuctionProduct::DeliverFinalPrice() {

    AuctionProduct::final_price = AuctionProduct::initial;

}

