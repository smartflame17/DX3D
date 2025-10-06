#include "Keyboard.h"

bool Keyboard::IsKeyPressed(unsigned char keycode) const noexcept
{
	return keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keyBuffer.size() > 0) {
		Keyboard::Event e = keyBuffer.front();
		keyBuffer.pop();
		return e;
	}
	else return Keyboard::Event();		// return default event (invalid key)
}

bool Keyboard::IsKeyEmpty() const noexcept
{
	return keyBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	keyBuffer = std::queue<Event>();	// assign new queue
}

char Keyboard::ReadChar() noexcept
{
	if (charBuffer.size() > 0) {
		unsigned char c = charBuffer.front();
		charBuffer.pop();
		return c;
	}
	else return 0;		// return default char
}

bool Keyboard::IsCharEmpty() const noexcept
{
	return charBuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
	charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept
{
	autoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
	autoRepeatEnabled = false;
}

bool Keyboard::IsAutoRepeatEnabled() const noexcept
{
	return autoRepeatEnabled;
}

// Implementation of windows-side handling functions

// Pops buffer until max buffer size
template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
		buffer.pop();
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keyStates[keycode] = true;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Pressed, keycode));	// update keyState and insert to buffer
	TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keyStates[keycode] = false;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Released, keycode));	// update keyState and insert to buffer
	TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char keycode) noexcept
{
	charBuffer.push(keycode);	// insert to buffer
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();	// resets all bits to 0 (released)
}

