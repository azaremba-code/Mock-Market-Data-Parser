# Mock Market Data Parser
A simplified binary market data generator modeling exchange-level order book events [TODO: and an optimized parser that parses this format].

## Generator
This part of the project generates replayable binary files containing a stream of order book messages. Currently, the three supported types are:

* Add Order
* Cancel Order
* Fulfill Order

The goal is to model realistic exchange semantics while keeping the system simple and deterministic.

Each order can be manually entered by the user [TODO: or be generated probabilistically].

## File Format Specification
Each file represents market activity for a single symbol (e.g., AAPL).

A file consists of:

```
[File Header]
[Message 1]
[Message 2]
...
[Message N]
```

All fields use fixed-width integer types for deterministic binary layout. The file will be a continuous stream of binary data. 
All multi-byte integer fields are encoded in little-endian byte order. Fixed-width byte arrays (e.g., char[8]) are written as raw byte sequences.

### File Header Specification
The file header consists of the symbol (e.g., AAPL) and N, the number of messages.

| Field  | Size | Type    | Description |
|--------|------|---------|-------------|
| Symbol | 8 B  | char[8] | ASCII symbol (e.g., "AAPL"). If shorter than 8 bytes, padded with `\0`. No null terminator required. |
| N      | 4 B  | uint32  | Number of messages in the file. |

## Message Specification
Each message will include a message header, which will include the message type (e.g., Add, Cancel, or Fulfill) and a timestamp for that message.

| Field     | Size | Type   | Description |
|-----------|------|--------|-------------|
| Type      | 1 B  | uint8  | 0 = Add, 1 = Cancel, 2 = Fulfill |
| Timestamp | 8 B  | uint64 | Milliseconds since Unix epoch (UTC) |

After each message header, there will be an Add, Cancel, or Fulfill order depending on the message type.

### Add Order Specification
Each Add Order will consist of an order ID, price, quantity, and side (e.g., buy or sell).

| Field    | Size | Type   | Description |
|----------|------|--------|-------------|
| ID       | 8 B  | uint64 | Unique order identifier. No two active orders share the same ID. |
| Price    | 4 B  | uint32 | Limit price in cents (e.g., 1234 = $12.34). Buy orders accept lower prices; sell orders accept higher prices. |
| Quantity | 4 B  | uint32 | Maximum number of shares to be bought or sold. |
| Side     | 1 B  | uint8  | 0 = Buy, 1 = Sell |

### Cancel Order Specification
Each Cancel Order will consist of an order ID to cancel (partial cancels are not supported).

| Field | Size | Type   | Description |
|-------|------|--------|-------------|
| ID    | 8 B  | uint64 | Must match an existing active Add Order ID. |

### Fulfill Order Specification
Each Fulfill Order will consist of an order ID to fulfill, a price, and a quantity.

| Field    | Size | Type   | Description |
|----------|------|--------|-------------|
| ID       | 8 B  | uint64 | Must match an existing active Add Order ID. |
| Price    | 4 B  | uint32 | Execution price in cents (e.g., 1234 = $12.34). Must not exceed limit price for Buy orders and must not be less than limit price for Sell orders. |
| Quantity | 4 B  | uint32 | Executed quantity. Must not exceed remaining quantity of the referenced Add Order. |

