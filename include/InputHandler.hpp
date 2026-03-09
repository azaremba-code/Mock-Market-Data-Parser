#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

namespace InputHandler {	
	using namespace std::string_literals;

	void ignoreRemainingLine() {
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	bool lineHasUnextractedInput() {
		return !std::cin.eof() && std::cin.peek() != '\n';
	}

	//TODO: Possibly replace with std::predicate<T>
	template <typename F, typename T>
	concept Validator = requires(const F& func, const T& input) {
		{func(input)} -> std::convertible_to<bool>;
	};

	template <typename T>
	concept Inputtable = requires(T& t) {
		std::cin >> t;
	};

	inline constexpr auto alwaysTrue = [] (const auto&) { return true; };

	template <Inputtable T, Validator<T> F = decltype(alwaysTrue)>
	T readInput(const std::string& prompt, const F& inputValidator = alwaysTrue) {
		std::cout << prompt;
		T result {};
		std::cin >> result;
		static const std::string retryMessage {"Please try again: "s};
		if (!std::cin) {
			if (std::cin.eof()) {
				throw std::runtime_error("Standard Input EOF detected");
			}
			std::cin.clear();
			ignoreRemainingLine();
			return readInput<T>("Failed extraction. "s + retryMessage, inputValidator);
		}
		if (lineHasUnextractedInput()) {
			ignoreRemainingLine();
			return readInput<T>("Superfluous input. "s + retryMessage, inputValidator);
		}
		if (!inputValidator(result)) {
			return readInput<T>("Invalid input. "s + retryMessage, inputValidator);
		}
		return result;
	}

	template <typename T>
	concept CharType = 
		std::same_as<T, char> ||
		std::same_as<T, unsigned char> ||
		std::same_as<T, signed char>;

	template <typename T>
	concept InputtableCharType = Inputtable<T> && CharType<T>; // required for proper overload resolution
	
	template <InputtableCharType T, Validator<T> F = decltype(alwaysTrue)>
	T readInput(const std::string& prompt, const F& inputValidator = alwaysTrue) {
		return static_cast<T>(readInput<std::uint32_t>(prompt, inputValidator));
	}
}

#endif
