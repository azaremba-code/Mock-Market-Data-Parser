#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <ostream>
#include <concepts>
#include <bit>
#include <cstdint>

using ID = std::uint64_t;
using Timestamp = std::uint64_t;
using Price = std::uint32_t;
using Quantity = std::uint32_t;

class Message {
public:
	virtual void writeToStream(std::ostream& stream) const = 0;
	
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
	message.writeToStream(os);
	return os;
}

template <std::integral IntegralType>
void write(std::ostream& os, IntegralType num) {
	IntegralType endianCorrectedNum {(std::endian::native == std::endian::little) ? num : std::byteswap(num)};
	os.write(reinterpret_cast<const char*>(&endianCorrectedNum), sizeof(endianCorrectedNum));	
}

class AddOrder final : public Message {
public:
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

	void writeToStream(std::ostream& os) const override {
		write(os, Message::m_timestamp);
		write(os, m_orderID);
		write(os, m_limitPrice);
		write(os, m_quantity);
		write(os, static_cast<SideType>(m_side));	
	}
		
	~AddOrder() override = default;

private:
	ID m_orderID {};
	Price m_limitPrice {};
	Quantity m_quantity {};
	Side m_side {};
};

class CancelOrder final : public Message {
public:
	CancelOrder() = default;
	
	CancelOrder(Timestamp timestamp, ID orderID) :
		Message {timestamp},
		m_orderID {orderID}
	{}
	
	void writeToStream(std::ostream& os) const override {
		write(os, Message::m_timestamp);
                write(os, m_orderID);
        }
	
	~CancelOrder() override = default;

private:
	ID m_orderID {};
};

class FulfillOrder final : public Message {
public:
	FulfillOrder() = default;

	FulfillOrder(Timestamp timestamp, ID orderID, Price executionPrice, Quantity quantity) :
		Message {timestamp},
		m_orderID {orderID},
		m_executionPrice {executionPrice},
		m_quantity {quantity}
	{}

	void writeToStream(std::ostream& os) const override {
		write(os, Message::m_timestamp);
                write(os, m_orderID);
                write(os, m_executionPrice);
                write(os, m_quantity);
        }
	
	~FulfillOrder() override = default;

private:
	ID m_orderID {};
	Price m_executionPrice {};
	Quantity m_quantity {};
};

#endif
