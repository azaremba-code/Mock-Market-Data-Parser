#include <fstream>

#include "Message.hpp"

int main() {
	AddOrder addOrder {1, 1, 1, 1, AddOrder::Side::Sell};
	std::ofstream add {"add.bin", std::ios::binary};
	add << addOrder;

	CancelOrder cancelOrder {1, 1};
	std::ofstream cancel {"cancel.bin", std::ios::binary};
	cancel << cancelOrder;

	FulfillOrder fulfillOrder {1, 1, 1, 1};
	std::ofstream fulfill {"fulfill.bin", std::ios::binary};
	fulfill << fulfillOrder;

}
