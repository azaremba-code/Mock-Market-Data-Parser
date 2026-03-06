#ifndef MESSAFE_INPUT_HANDLER_HPP
#define MESSAFE_INPUT_HANDLER_HPP

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <type_traits>

#include "Message.hpp"

namespace MessageInputHandler {
	template <typename T>
	T readInput(std::string prompt) {
		std::cout << prompt;
		T result {};
		std::cin >> result;
		return result;
	}

	template <>
	MessageType readInput(std::string prompt) {
		std::cout << prompt;
		std::uint32_t result {};
		std::cin >> result;
		return static_cast<MessageType>(result);
	}

	using namespace std::string_literals;
	
	inline std::unique_ptr<Message> readAddOrder(Timestamp timestamp) {
		const auto id {readInput<ID>("Please enter order ID: "s)};
		const auto price {readInput<Price>("Please enter order price: "s)};
		const auto quantity {readInput<Quantity>("Please enter quantity: "s)};
		static const std::string buyAndSellClarifier {
			"(buy = "s + std::to_string(AddOrder::buyValue) +
			", sell = "s + std::to_string(AddOrder::sellValue) + ")"s
		};
		const auto side {readInput<AddOrder::Side>("Please enter side "s + buyAndSellClarifier + ": "s)};
		return std::make_unique<AddOrder>(timestamp, id, price, quantity, side);
	}

	inline std::unique_ptr<Message> readCancelOrder(Timestamp timestamp) {
		const auto id {readInput<ID>("Please enter the ID of an order to cancel: "s)};
		return std::make_unique<CancelOrder>(timestamp, id);
	}

	inline std::unique_ptr<Message> readFulfillOrder(Timestamp timestamp) {
		const auto id {readInput<ID>("Please enter the ID of an order to cancel: "s)};
		const auto price {readInput<Price>("Please enter order price: "s)};
		const auto quantity {readInput<Quantity>("Please enter quantity: "s)};
		return std::make_unique<FulfillOrder>(timestamp, id, price, quantity);
	}

	inline std::unique_ptr<Message> readMessage() {
		const auto timestamp {readInput<Timestamp>("Please enter message timestamp: "s)};
		static constexpr MessageType addOrderCode {0};
		static constexpr MessageType cancelOrderCode {1};
		static constexpr MessageType fulfillOrderCode {2};
		static_assert(std::is_same_v<decltype(readAddOrder), decltype(readCancelOrder)> && std::is_same_v<decltype(readCancelOrder), decltype(readFulfillOrder)>);
		using CreationFunction = std::function<decltype(readAddOrder)>;
		static const std::unordered_map<MessageType, CreationFunction> creationFunctionMap {
			{addOrderCode, readAddOrder},
			{cancelOrderCode, readCancelOrder},
			{fulfillOrderCode, readFulfillOrder}
		};
		
		static const std::string messageTypeClarifier {
			"(add = "s + std::to_string(addOrderCode) +
			", cancel = "s + std::to_string(cancelOrderCode) +
			", fulfill = "s + std::to_string(fulfillOrderCode) + ")"s
		};
		const auto messageCode {readInput<MessageType>("Please enter message type "s + messageTypeClarifier + ": "s)};
		return creationFunctionMap.at(messageCode)(timestamp);
	}
}

#endif
