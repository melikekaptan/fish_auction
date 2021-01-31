CC = g++
CFLAGS = -I/usr/local/include/librdkafka -L/usr/local/lib -lpthread -lrdkafka++
 
# ****************************************************
# Targets needed to bring the executable up to date
 
auction_test: auction_test.o auction_product.o auction.o auction_listener.o
	$(CC) $(CFLAGS) -o auction_test auction_test.o auction_product.o auction.o auction_listener.o
 
# The main.o target can be written more simply
 
auction_test.o: auction_test.cpp auction_product.h auction.h
	$(CC) -g $(CFLAGS) -c auction_test.cpp
 
auction_product.o: auction_product.h
 
auction.o: auction.h auction_product.h

auction_listener.o: auction_listener.h auction_listener.cpp

binaries = auction_test auction_product auction auction_listener

.PHONY: clean

clean:
	rm -f $(binaries) *.o
