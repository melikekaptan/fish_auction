#include <iostream>


class AuctionProduct {

public:
  
        uint32_t final_price;
        static uint32_t initial;
        uint32_t represented_time = 0;

        AuctionProduct(uint32_t base_price);
        uint32_t IncreasePrice(uint32_t added);
        void SendBack();
        
};