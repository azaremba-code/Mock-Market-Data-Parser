#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

#include "Message.hpp"
#include "MessageInputHandler.hpp"
#include "WriteUtils.hpp"

class Generator {
public:
	Generator(const std::filesystem::path& outputFilePath = "default.bin", std::size_t initialMessageReadCount = 0) :
		m_messages {},
		m_symbolName {getName()},
		m_outputFile {outputFilePath, std::ios::binary}
	{
		m_outputFile.exceptions(std::ofstream::failbit);
		readMessages(initialMessageReadCount);
	}

	Generator(const Generator&) = delete;
	Generator& operator=(const Generator&) = delete;
	Generator(Generator&&) = delete;
	Generator& operator=(Generator&&) = delete;

	~Generator() {
		writeToFileNoClear();
	}

	void setName() {
		m_symbolName = getName();
	}
	
	void readMessage() {
		m_messages.push_back(MessageInputHandler::readMessage());
	}

	void readMessages(std::size_t n = 1) {
		for (std::size_t i {0uz}; i < n; i++) {
			readMessage();
		}
	}

	void clearMessages() {
		m_messages.clear();
	}

	void writeToFile() {
		writeToFileNoClear();
		clearMessages();
	}

private:
	std::vector<std::unique_ptr<Message>> m_messages {};
	std::string m_symbolName {};
	std::ofstream m_outputFile {};

	void writeToFileNoClear() {
		std::vector<std::byte> bytes {};
		WriteUtils::writeBytes(m_symbolName, bytes);
		for (const auto& messagePtr : m_messages) {
			WriteUtils::writeBytes(messagePtr->getIdentifierCode(), bytes);
			messagePtr->writeBytes(bytes);
		}
		m_outputFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
	}
	
	static std::string getName() {
		static constexpr auto doesNotExceedMax = [] (const std::string& input) {
                        return input.size() <= WriteUtils::maxSymbolLength;
                };
                return InputHandler::readInput<std::string>("Enter symbol name: ", doesNotExceedMax);
	}
};

#endif

