#include <fstream>
#include <sstream>

#include "Message.hpp"
#include "MessageInputHandler.hpp"

int main() {
	std::unique_ptr<Message> message {MessageInputHandler::readMessage()};
	std::ofstream out {"test.bin", std::ios::binary};
	out << *message;
	
	return 0;
}
