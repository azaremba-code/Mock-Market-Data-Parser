#ifndef MESSAGE_READER_HPP
#define MESSAGE_READER_HPP

#include <istream>
#include <memory>
#include <unordered_map>
#include <cassert>

#include "Message.hpp"

namespace MessageReader {
	inline std::unique_ptr<Message> readAddOrder(Timestamp timestamp, std::istream& inputStream) {
		ID orderID {};
		inputStream.read(reinterpret_cast<char*>(&orderID), sizeof(orderID));
		Price limitPrice {};
		inputStream.read(reinterpret_cast<char*>(&limitPrice), sizeof(limitPrice));
		Quantity quantity {};
		inputStream.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));
		AddOrder::Side side {};
		inputStream.read(reinterpret_cast<char*>(&side), sizeof(side));
		
		return std::make_unique<AddOrder>(timestamp, orderID, limitPrice, quantity, side);
	}

	inline std::unique_ptr<Message> readCancelOrder(Timestamp timestamp, std::istream& inputStream) {
		ID orderID {};
		inputStream.read(reinterpret_cast<char*>(&orderID), sizeof(orderID));
		
		return std::make_unique<CancelOrder>(timestamp, orderID);
	}

	inline std::unique_ptr<Message> readFulfillOrder(Timestamp timestamp, std::istream& inputStream) {
		ID orderID {};
		inputStream.read(reinterpret_cast<char*>(&orderID), sizeof(orderID));
		Price executionPrice {};
		inputStream.read(reinterpret_cast<char*>(&executionPrice), sizeof(executionPrice));
		Quantity quantity {};
		inputStream.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));

		return std::make_unique<FulfillOrder>(timestamp, orderID, executionPrice, quantity);
	}

	inline std::unique_ptr<Message> readMessage(MessageIdentifier messageIdentifier, std::istream& inputStream) {
		Timestamp timestamp {};
		inputStream.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));

		static_assert(std::is_same_v<decltype(readAddOrder), decltype(readCancelOrder)> && std::is_same_v<decltype(readCancelOrder), decltype(readFulfillOrder)>);
		using GetDefaultMessage = decltype(&readAddOrder);
		static const std::unordered_map<MessageIdentifier, GetDefaultMessage> messageMap {
			{AddOrder::identifierCode, readAddOrder},
			{CancelOrder::identifierCode, readCancelOrder},
			{FulfillOrder::identifierCode, readFulfillOrder}
		};
	
		assert(messageMap.contains(messageIdentifier));
		return messageMap.at(messageIdentifier)(timestamp, inputStream);
	}
}

#endif
