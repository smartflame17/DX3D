#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window;
public:
	class Event		// internal class that saves current state of the key, and the keycode itself (The engine should support press/release of key)
	{
	public:
		enum class Type
		{
			Pressed,
			Released,
			Invalid
		};
	private :
		Type type;
		unsigned char code;		// ASCII code corresponding to key
	public:
		// constructors
		Event() noexcept: 
			type(Type::Invalid), 
			code(0)
		{}
		Event(Type type, unsigned char code) noexcept :
			type(type),
			code(code)
		{}

		bool IsPressed() const noexcept {
			return type == Type::Pressed;
		}
		bool IsReleased() const noexcept {
			return type == Type::Released;
		}
		bool IsValid() const noexcept {
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept {
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard) = delete;	// singleton stuff

	// key events
	bool IsKeyPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;									// invoke event from event queue
	bool IsKeyEmpty() const noexcept;
	void FlushKey() noexcept;									// clears out event queue

	// char events (text input)
	char ReadChar() noexcept;
	bool IsCharEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;			// calls flush on both key and char event

	// auto-repeat handling (windows behaviour)
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeatEnabled() const noexcept;

private:	// Windows-side handling (invisible to user)
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;		// removes from queue if exceeding bufferSize

private:
	static constexpr unsigned int nKeys = 256u;					// Windows handles all keys within a byte
	static constexpr unsigned int bufferSize = 16u;				// Saves up to 16 events per queue
	bool autoRepeatEnabled = false;
	std::bitset<nKeys> keyStates;							// bitset over multiple keys
	std::queue<Event> keyBuffer;							// buffer for key input
	std::queue<char> charBuffer;							// buffer for char (text) input
};