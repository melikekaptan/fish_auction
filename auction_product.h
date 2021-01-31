#pragma once

#include <iostream>


class AuctionProduct {

public:
  
        int final_price;
        static int initial;
        int base_price;
        uint32_t represented_time = 0;

        AuctionProduct(int base_price, std::string &fish_type, double quantity);
        void IncreasePrice(int added);
        void DeliverFinalPrice();
        
        
};