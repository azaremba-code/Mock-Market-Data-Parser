#ifndef MESSAGE_HPP
#define MESSAGE_HPP

using ID = std::uint64_t;
using Timestamp = std::uint32_t;
using Price = std::uint32_t;
using Quantity = std::uint32_t;

class Message {
public:
	virtual std::string getString() = 0;
	
	virtual ~Message() = default;

protected:
	Message(Timestamp timestamp = 0) : 
		m_timestamp {timestamp}
	{}

	Timestamp m_timestamp {};
};

class AddOrder final : public Message {
public:
	enum class Side : std::uint8_t {
		Buy,
		Sell
	};

	AddOrder() :
		Message {}	
	{}
	
	~AddOrder() override = default;

private:
	ID orderID {};
	Price price {};
	Quantity quantity {};
	Side side {};
};

class CancelOrder final : public Message {
public:
	CancelOrder() :
		Message {}
	{}

	~CancelOrder() override = default;

private:
	ID orderID {};
};

class FulfillOrder final : public Message {
public:
	FulfillOrder() :
		Message {}
	{}
	
	~FulfillOrder() override = default;

private:
	ID orderID {};
	Price price {};
	Quantity quantity {};
};

#endif
