#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <ostream>
#include <concepts>
#include <bit>
#include <cstdint>
#include <span>
#include <iterator>

#include <vector>

using ID = std::uint64_t;
using Timestamp = std::uint64_t;
using Price = std::uint32_t;
using Quantity = std::uint32_t;

using MessageIdentifier = std::uint8_t;

class Message {
public:
	virtual void writeBytes(std::vector<std::byte>& out) const = 0;

	virtual MessageIdentifier getIdentifierCode() const = 0;

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

template <std::integral IntegralType>
void writeBytes(IntegralType num, std::vector<std::byte>& out) {
	static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
		"Mixed-endian architectures are not supported.");
	if constexpr (std::endian::native == std::endian::big) {
		num = std::byteswap(num);
	}
	const auto bytes {std::as_bytes(std::span {&num, 1uz})};
	for (std::byte byte : bytes) {
		out.push_back(byte);
	}
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
		::writeBytes(Message::m_timestamp, out);
		::writeBytes(m_orderID, out);
		::writeBytes(m_limitPrice, out);
		::writeBytes(m_quantity, out);
		::writeBytes(static_cast<SideType>(m_side), out);
	}
	
	MessageIdentifier getIdentifierCode() const override {
		return identifierCode;
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
		::writeBytes(Message::m_timestamp, out);
		::writeBytes(m_orderID, out);
        }

	MessageIdentifier getIdentifierCode() const override {
		return identifierCode;
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
		::writeBytes(Message::m_timestamp, out);
		::writeBytes(m_orderID, out);
		::writeBytes(m_executionPrice, out);
		::writeBytes(m_quantity, out);
        }
	
	MessageIdentifier getIdentifierCode() const override {
		return identifierCode;
	}

	~FulfillOrder() override = default;

private:
	ID m_orderID {};
	Price m_executionPrice {};
	Quantity m_quantity {};
};

#endif
