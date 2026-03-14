#include <fstream>
#include <sstream>
#include <vector>

#include "Message.hpp"
#include "MessageInputHandler.hpp"
#include "Generator.hpp"

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
	auto messageCount {InputHandler::readInput<std::size_t>("Enter number of messages to read: ")};
	Generator gen {"default.bin", messageCount};

	return 0;
}
