#include "auction.h"
#include "auction_product.h"

#include <iostream>
#include <string>


int main () {

    Auction auction;
    auction.AuctionServiceListener();
    auction.place_auction();
}