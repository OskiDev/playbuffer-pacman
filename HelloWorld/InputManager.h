#pragma once
#include <unordered_map>
#include <functional>

namespace Play
{
	enum KeyboardButton;
}

struct inputBinding
{
	std::function<void()> callbackFunc;
	bool singlePress;
	//overloading invocation operator to invoke callbackFunc when struct is called as a function
	void operator()() { callbackFunc(); }
};

class InputManager
{
public:

	void BindKey(Play::KeyboardButton key, std::function<void()> callback, bool singlePress);

	void UnbindKey(Play::KeyboardButton key);

	void Update();

private:
	std::unordered_map<Play::KeyboardButton, inputBinding> keyBindings;
	std::unordered_map<Play::KeyboardButton, inputBinding> bindingRequests;
	std::vector<Play::KeyboardButton> unbindRequestVector;
};