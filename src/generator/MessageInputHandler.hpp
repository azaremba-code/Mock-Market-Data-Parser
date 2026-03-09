#ifndef MESSAGE_INPUT_HANDLER_HPP
#define MESSAGE_INPUT_HANDLER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>
#include <type_traits>
#include <cassert>
#include <limits>
#include <unordered_set>

#include "Message.hpp"
#include "InputHandler.hpp"

namespace MessageInputHandler {
	using namespace std::string_literals;
	
	inline std::unique_ptr<Message> readAddOrder(Timestamp timestamp) {
		const auto id {InputHandler::readInput<ID>("Please enter order ID: "s)};
		const auto price {InputHandler::readInput<Price>("Please enter order price: "s)};
		const auto quantity {InputHandler::readInput<Quantity>("Please enter quantity: "s)};
		static const std::string buyAndSellClarifier {
			"(buy = "s + std::to_string(AddOrder::buyValue) +
			", sell = "s + std::to_string(AddOrder::sellValue) + ")"s
		};
		static const std::unordered_set validSideInputs {AddOrder::buyValue, AddOrder::sellValue};
		static const auto validSideInput = [&] (const auto& input) { return validSideInputs.contains(static_cast<AddOrder::SideType>(input)); };
		const auto side {InputHandler::readInput<AddOrder::Side>("Please enter side "s + buyAndSellClarifier + ": "s, validSideInput)};
		return std::make_unique<AddOrder>(timestamp, id, price, quantity, side);
	}

	inline std::unique_ptr<Message> readCancelOrder(Timestamp timestamp) {
		const auto id {InputHandler::readInput<ID>("Please enter the ID of an order to cancel: "s)};
		return std::make_unique<CancelOrder>(timestamp, id);
	}

	inline std::unique_ptr<Message> readFulfillOrder(Timestamp timestamp) {
		const auto id {InputHandler::readInput<ID>("Please enter the ID of an order to fulfill: "s)};
		const auto price {InputHandler::readInput<Price>("Please enter order price: "s)};
		const auto quantity {InputHandler::readInput<Quantity>("Please enter quantity: "s)};
		return std::make_unique<FulfillOrder>(timestamp, id, price, quantity);
	}

	inline std::unique_ptr<Message> readMessage() {
		const auto timestamp {InputHandler::readInput<Timestamp>("Please enter message timestamp: "s)};

		static_assert(std::is_same_v<decltype(readAddOrder), decltype(readCancelOrder)> && std::is_same_v<decltype(readCancelOrder), decltype(readFulfillOrder)>);
		using CreationFunction = decltype(&readAddOrder);
		static const std::unordered_map<MessageIdentifier, CreationFunction> creationFunctionMap {
			{AddOrder::identifierCode, readAddOrder},
			{CancelOrder::identifierCode, readCancelOrder},
			{FulfillOrder::identifierCode, readFulfillOrder}
		};
		
		static const std::string messageTypeClarifier {
			"(add = "s + std::to_string(AddOrder::identifierCode) +
			", cancel = "s + std::to_string(CancelOrder::identifierCode) +
			", fulfill = "s + std::to_string(FulfillOrder::identifierCode) + ")"s
		};
		const auto& validIdentifierCode = [&] (const auto& input) { return creationFunctionMap.contains(input); };
		const auto messageCode {InputHandler::readInput<MessageIdentifier>("Please enter message type "s + messageTypeClarifier + ": "s, validIdentifierCode)};
		return creationFunctionMap.at(messageCode)(timestamp);
	}
}

#endif
