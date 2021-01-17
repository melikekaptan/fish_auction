project(fish_auction)
find_package(rdkafka)
find_package(rdkafka++)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
#set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

#include_directories(~/kafka/librdkafka/src-cpp/)
#include_directories(~/kafka/librdkafka/src/)
add_library(auction
	auction_product.cpp
	auction_product.h
	auction.cpp
	auction.h)


target_include_directories(auction PUBLIC /usr/include/librdkafka)
target_link_libraries(auction ssl crypto pthread rdkafka++)

add_executable(auction-test auction_test.cpp)
target_link_libraries(auction-test auction ssl crypto pthread rdkafka++ dl)
#target_link_libraries(auction-test auction ssl crypto pthread rdkafka++ rdkafka)
