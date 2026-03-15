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
		std::cout << "test\n";
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
		for (const auto& message : m_messages) {
			std::cout << message << "\n";
		}
	}
	
private:
	std::vector<std::unique_ptr<Message>> m_messages {};
	std::string m_symbolName {};
	std::ifstream m_inputFile {};

	void parseSymbolName() {
		m_inputFile.read(m_symbolName.data(), WriteUtils::maxSymbolLength);
	}
	
	bool parseMessage() {
		if (!m_inputFile) {
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
