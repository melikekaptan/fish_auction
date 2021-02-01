#include "auction_product.h"

#include <iostream>

int AuctionProduct::initial = 0;

AuctionProduct::AuctionProduct(int price, std::string &fish_type, double quantity,  std::string fisherman_name, std::string fisherman_surname) {
       
        AuctionProduct::initial = price;
        AuctionProduct::base_price = price;
        AuctionProduct::type = fish_type;
        AuctionProduct::fisherman = fisherman_name + fisherman_surname;
        std::cout << fish_type + " from " + fisherman + ", product base price is: " << std::endl;
        std::cout << base_price << std::endl;

}

void AuctionProduct::IncreasePrice(int added) {

     AuctionProduct::initial += added;

}

void AuctionProduct::DeliverFinalPrice() {

    AuctionProduct::final_price = AuctionProduct::initial;

}

