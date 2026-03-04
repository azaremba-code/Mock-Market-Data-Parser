# Mock Market Data Parser
A simplified binary market data generator modeling exchange-level order book events [TODO: and an optimized parser that parses this format].

## Generator
This part of the project generates replayable binary files containing a stream of order book messages. Currently, the three supported type are:

* Add Order
* Cancel Order
* Fulfill Order

The goal is to model realistic exchange semantics while keeping the system simple and deterministic.

Each order can be manually entered by the user [TODO: or be generated probabilistically].

## File Format Specification
Each file represents market activity for a single symbol (e.g. AAPL).

A file consists of:

```
[File Header]
[Message 1]
[Message 2]
...
[Message N]
```

All fields use fixed-width integer types for deterministic binary layout. The file will be a continuous stream of binary data in little-endian (host order).

### File Header Specification
The file header consists of the symbol (e.g. AAPL) and N, the number of messages.

```
Symbol:		8 Bytes (char[8] e.g. "AAPL\0\0\0\0" or "NAMEHERE" [note: possibly no null-terminator])
N:			4 Bytes (uint32  e.g. 5000 messages)
```

## Message Specification
Each message will include a message header, which will include the message type (e.g. Add, Cancel, or Fulfill) and a timestamp for that message.

```
Type:		1 Byte  (uint8  e.g. 0, 1, or 2 for Add, Cancel, and Fulfill respectively)
Timestamp:	8 Bytes (uint64 e.g. 1772636759000 ms since epoch)
```

After each message header, there will be an Add, Cancel, of Fulfill order depending on the message header type.

### Add Order Specification
Each Add Order will consist of an order ID, price, quantity, and side (e.g. buy or sell).

```
ID:			8 Bytes (uint64 e.g. 1000. ID is unique - no two orders have the same ID)
Price:		4 Bytes (uint32 e.g. 1234 = $12.34 limit price. Buy side will accept lower prices and sell side will accept higher prices)
Quantity:	4 Bytes (uint32 e.g. 50 shares maximum to be bought/sold)
Side:		1 Byte  (uint8  e.g. 0 or 1 for buy and sell respectively)
```

### Cancel Order Specification
Each Cancel Order will consist of an order ID to cancel (partial cancels are not supported).

```
ID:			8 Bytes (uint64 e.g. 1000. ID is guaranteed to match an existing Add Order ID)
```

### Fulfill Order Specification
Each Fulfill Order will consist of an order ID to fulfill, a price, and a quantity.

```
ID:			8 Bytes (uint64 e.g. 1000. ID is guaranteed to match an existing Add Order ID)
Price:		4 Bytes (uint32 e.g. 1234 = $12.34 execution price. Must be lower than limit price for buy orders and higher than limit price for sell orders)
Quantity:	4 Bytes (uint32 e.g. 30 shares to be bought/sold at execution price. Must be less than or equal to corresponding Add Order remaining quantity)
```

