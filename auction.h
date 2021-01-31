#pragma once

#include "auction_product.h"
#include "auction_listener.h"

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


        static std::vector <fish_product> list_of_products;
        Auction (); 
        //void ReadAuctionTable();
        void place_auction();
        void SendBack();

};


