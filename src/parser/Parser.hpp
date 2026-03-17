#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>

#include "Message.hpp"
#include "MessageReader.hpp"

class Parser {
public:
	Parser(const std::filesystem::path& inputFilePath = "default.bin") :
		m_messages {},
		m_symbolName {},
		m_inputFile {inputFilePath, std::ios::binary}
	{
		m_inputFile.exceptions(std::ifstream::failbit);
		parseSymbolName();
		parseAllMessages();
	}

	Parser(const Parser&) = delete;
	Parser& operator=(const Parser&) = delete;
	Parser(Parser&&) = delete;
	Parser& operator=(Parser&&) = delete;
	
	~Parser() = default;

	const std::string& getSymbolName() {
		return m_symbolName;
	}

	const std::vector<std::unique_ptr<Message>>& getMessages() const {
		return m_messages;
	}

	void printMessages() const {
		std::cout << "Symbol name: " << m_symbolName << "\n";
		for (const auto& message : m_messages) {
			std::cout << "\n" << message;
		}
	}
	
private:
	std::vector<std::unique_ptr<Message>> m_messages {};
	std::string m_symbolName {};
	std::ifstream m_inputFile {};

	void parseSymbolName() {
		m_symbolName.resize(WriteUtils::maxSymbolLength);
		m_inputFile.read(m_symbolName.data(), WriteUtils::maxSymbolLength);
	}
	
	bool parseMessage() {
		constexpr auto eofChar {std::char_traits<decltype(m_inputFile)::char_type>::eof()};
		if (m_inputFile.peek() == eofChar) {
			return false;
		}

		MessageIdentifier messageIdentifier {};
		m_inputFile.read(reinterpret_cast<char*>(&messageIdentifier), sizeof(messageIdentifier));

		m_messages.push_back(MessageReader::readMessage(messageIdentifier, m_inputFile));
		return true;
	}

	void parseAllMessages() {
		while (parseMessage());
	}
};

#endif
