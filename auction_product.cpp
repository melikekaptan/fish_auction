#include "auction_product.h"

#include <iostream>

int AuctionProduct::initial = 0;

AuctionProduct::AuctionProduct(int price, std::string &fish_type, double quantity) {
       
        AuctionProduct::initial = price;
        AuctionProduct::base_price = price;
        AuctionProduct::type = fish_type;
        std::cout << "product base price is: " << std::endl;
        std::cout << base_price << std::endl;

}

void AuctionProduct::IncreasePrice(int added) {

     AuctionProduct::initial += added;

}

void AuctionProduct::DeliverFinalPrice() {

    AuctionProduct::final_price = AuctionProduct::initial;

}

