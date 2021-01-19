#pragma once

#include "auction_product.h"

#include <iostream>
#include <map>
#include <exception>

// class MyException : public std::exception
// {
// public:
//     virtual const char* what() const throw()
//   {
//     return "My exception happened";
//   }
// };


class Auction {

public:

        std::map<std::string, uint32_t> auction_table;

        Auction (); 
        //void ReadAuctionTable();
        void place_auction();

};


