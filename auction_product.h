#pragma once

#include <iostream>


class AuctionProduct {

public:
        std::string type;
        int final_price;
        static int initial;
        int base_price;
        int represented_time = 0;
        std::string fisherman;

        AuctionProduct(int base_price, std::string &fish_type, double quantity, std::string fisherman_name, std::string fisherman_surname);
        void IncreasePrice(int added);
        void DeliverFinalPrice();
        
        
};