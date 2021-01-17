#include "auction_product.h"

#include <iostream>

uint32_t AuctionProduct::initial = 0;

AuctionProduct::AuctionProduct(uint32_t base_price) {
        std::cout << "auction product formed" << std::endl;
        AuctionProduct::initial = base_price;
        std::cout << "product base price is: " << std::endl;
        std::cout << base_price << std::endl;
}

uint32_t AuctionProduct::IncreasePrice(uint32_t added) {

    return AuctionProduct::initial + added;

}

void AuctionProduct::SendBack() {


}