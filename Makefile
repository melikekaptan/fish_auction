CC = g++
CFLAGS = -I/usr/local/include/librdkafka -L/usr/local/lib -lrdkafka++
 
# ****************************************************
# Targets needed to bring the executable up to date
 
auction_test: auction_test.o auction_product.o auction.o
	$(CC) $(CFLAGS) -o auction_test auction_test.o auction_product.o auction.o
 
# The main.o target can be written more simply
 
auction_test.o: auction_test.cpp auction_product.h auction.h
	$(CC) $(CFLAGS) -c auction_test.cpp
 
auction_product.o: auction_product.h
 
auction.o: auction.h auction_product.h
