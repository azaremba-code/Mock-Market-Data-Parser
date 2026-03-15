#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <ostream>
#include <concepts>
#include <bit>
#include <cstdint>
#include <span>
#include <iterator>
#include <vector>
#include <cassert>
#include <memory>

#include "WriteUtils.hpp"

using ID = std::uint64_t;
using Timestamp = std::uint64_t;
using Price = std::uint32_t;
using Quantity = std::uint32_t;

using MessageIdentifier = std::uint8_t;

class Message {
public:
	virtual void writeBytes(std::vector<std::byte>& out) const = 0;

	virtual MessageIdentifier getIdentifierCode() const = 0;

	virtual std::size_t getMessageSize() const = 0;

	virtual void print(std::ostream& o = std::cout) const = 0;

	virtual ~Message() = default;
	
	Message(const Message&) = delete;
	Message& operator=(const Message&) = delete;
	Message(Message&&) = delete;
	Message& operator=(Message&&) = delete;

protected:
	Message(Timestamp timestamp = 0) : 
		m_timestamp {timestamp}
	{}

	Timestamp m_timestamp {};
};

std::ostream& operator<<(std::ostream& os, const Message& message) {
	message.print(os);
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<Message>& messagePtr) {
	assert(messagePtr);
	return os << *messagePtr;
}


class AddOrder final : public Message {
public:
	static inline constexpr MessageIdentifier identifierCode {0};

	using SideType = std::uint8_t;

	static inline constexpr SideType buyValue {0};
	static inline constexpr SideType sellValue {1}; 

	enum class Side : SideType {
		Buy = buyValue,
		Sell = sellValue
	};

	AddOrder() = default; 

	AddOrder(Timestamp timestamp, ID orderID, Price limitPrice, Quantity quantity, Side side) :
		Message {timestamp},
		m_orderID {orderID},
		m_limitPrice {limitPrice},
		m_quantity {quantity},
		m_side {side}
	{}

	void writeBytes(std::vector<std::byte>& out) const override {
		WriteUtils::writeBytes(Message::m_timestamp, out);
		WriteUtils::writeBytes(m_orderID, out);
		WriteUtils::writeBytes(m_limitPrice, out);
		WriteUtils::writeBytes(m_quantity, out);
		WriteUtils::writeBytes(static_cast<SideType>(m_side), out);
	}
	
	MessageIdentifier getIdentifierCode() const override {
		return identifierCode;
	}

	std::size_t getMessageSize() const override {
		return sizeof(AddOrder);
	}
	
	inline friend std::ostream& operator<<(std::ostream& os, const AddOrder::Side& side) {
		return os << ((side == AddOrder::Side::Buy) ? "Buy" : "Sell");
	}
	
	void print(std::ostream& o = std::cout) const override {
		o << "Timestamp: " << Message::m_timestamp << "\n" <<
			"Order ID: " << m_orderID << "\n" <<
			"Quantity: " << m_quantity << "\n" <<
			"Side: " << m_side << "\n";
	}

	~AddOrder() override = default;

private:
	ID m_orderID {};
	Price m_limitPrice {};
	Quantity m_quantity {};
	Side m_side {};
};

inline std::istream& operator>>(std::istream& is, AddOrder::Side& side) {
	std::uint32_t input {};
	is >> input;
	side = static_cast<AddOrder::Side>(input);
	return is;
}


class CancelOrder final : public Message {
public:
	static inline constexpr MessageIdentifier identifierCode {1};

	CancelOrder() = default;
	
	CancelOrder(Timestamp timestamp, ID orderID) :
		Message {timestamp},
		m_orderID {orderID}
	{}

	void writeBytes(std::vector<std::byte>& out) const override {
		WriteUtils::writeBytes(Message::m_timestamp, out);
		WriteUtils::writeBytes(m_orderID, out);
        }

	MessageIdentifier getIdentifierCode() const override {
		return identifierCode;
	}

	std::size_t getMessageSize() const override {
		return sizeof(CancelOrder);
	}

	void print(std::ostream& o = std::cout) const override {
		o << "Timestamp: " << Message::m_timestamp << "\n" <<
			"Order ID: " << m_orderID << "\n";
	}

	~CancelOrder() override = default;

private:
	ID m_orderID {};
};

class FulfillOrder final : public Message {
public:
	static inline constexpr MessageIdentifier identifierCode {2};

	FulfillOrder() = default;

	FulfillOrder(Timestamp timestamp, ID orderID, Price executionPrice, Quantity quantity) :
		Message {timestamp},
		m_orderID {orderID},
		m_executionPrice {executionPrice},
		m_quantity {quantity}
	{}
	
	void writeBytes(std::vector<std::byte>& out) const override {
		WriteUtils::writeBytes(Message::m_timestamp, out);
		WriteUtils::writeBytes(m_orderID, out);
		WriteUtils::writeBytes(m_executionPrice, out);
		WriteUtils::writeBytes(m_quantity, out);
        }
	
	MessageIdentifier getIdentifierCode() const override {
		return identifierCode;
	}

	std::size_t getMessageSize() const override {
		return sizeof(FulfillOrder);
	}

	void print(std::ostream& o = std::cout) const override {
		o << "Timestamp: " << Message::m_timestamp << "\n" <<
			"Order ID: " << m_orderID << "\n" <<
			"Price: " << m_executionPrice << "\n"
			"Quantity: " << m_quantity << "\n";
}

	~FulfillOrder() override = default;

private:
	ID m_orderID {};
	Price m_executionPrice {};
	Quantity m_quantity {};
};

#endif
