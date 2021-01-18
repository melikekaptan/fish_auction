CC = g++
CFLAGS = -I/usr/local/include/librdkafka -L/usr/local/lib -lrdkafka++
 
# ****************************************************
# Targets needed to bring the executable up to date
 
auction_test: auction_test.o auction_product.o auction.o auction_listener.o timer.o
	$(CC) $(CFLAGS) -o auction_test auction_test.o auction_product.o auction.o auction_listener.o timer.o
 
# The main.o target can be written more simply
 
auction_test.o: auction_test.cpp auction_product.h auction.h
	$(CC) $(CFLAGS) -c auction_test.cpp
 
auction_product.o: auction_product.h
 
auction.o: auction.h auction_product.h

auction_listener.o: auction_listener.h auction_listener.cpp

timer.o: timer.h timer.cpp

binaries = auction_test auction_product auction auction_listener timer

.PHONY: clean

clean:
	rm -f $(binaries) *.o
