#ifndef WRITE_UTILS_HPP
#define WRITE_UTILS_HPP

#include <vector>
#include <cstddef>
#include <string_view>

namespace WriteUtils {
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

	inline constexpr std::size_t maxSymbolLength {8};

	void writeBytes(std::string_view str, std::vector<std::byte>& out, std::size_t maxWriteLength = maxSymbolLength, std::byte pad = static_cast<std::byte>('\0')) {
		const std::size_t strBytesCount {std::min(str.size(), maxWriteLength)};
		const auto strBytes {std::as_bytes(std::span {str.data(), strBytesCount})};
		for (std::byte byte : strBytes) {
			out.push_back(byte);
		}
		const std::size_t padBytesCount {maxWriteLength - strBytesCount};
		for (std::size_t i {0uz}; i < padBytesCount; i++) {
			out.push_back(pad);
		}
	}
}
#endif
