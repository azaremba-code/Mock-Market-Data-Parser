#include <fstream>
#include <sstream>
#include <vector>

#include "Message.hpp"
#include "MessageInputHandler.hpp"

std::ostream& operator<<(std::ostream& o, const std::byte& b) {
    return o << static_cast<int>(b);
}

void printVec(const auto& vec) {
    std::cout << "{ ";
    for (const auto& e : vec) {
        std::cout << e << " ";
    }
    std::cout << "}\n";
}

int main() {
	// std::unique_ptr<Message> message {MessageInputHandler::readMessage()};
	std::vector<std::byte> bytes {};
	std::uint16_t num {13u + 256u * 27u};
	writeBytes(num, bytes);
	printVec(bytes);
	
	std::ofstream out {"test.bin", std::ios::binary};
	out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
	
	return 0;
}
