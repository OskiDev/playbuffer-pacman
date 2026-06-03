#include "InputManager.h"
#include <iostream>
#include <conio.h>
#include "Play.h"

void InputManager::BindKey(Play::KeyboardButton key, std::function<void()> callback, bool singlePress)
{
	bindingRequests[key] = { callback, singlePress };
}

void InputManager::UnbindKey(Play::KeyboardButton key)
{
	if (!keyBindings.contains(key))
	{
		return;
	}
	unbindRequestVector.emplace_back(key);
}

void InputManager::Update()
{
	for (std::unordered_map<Play::KeyboardButton, inputBinding>::iterator bindingMapIt = keyBindings.begin(); bindingMapIt != keyBindings.end(); ++bindingMapIt)
	{
		if (bindingMapIt->second.singlePress)
		{
			if (Play::KeyPressed(bindingMapIt->first))
			{
				bindingMapIt->second();
			}
		}
		else
		{
			if (Play::KeyDown(bindingMapIt->first))
			{
				bindingMapIt->second();
			}
		}
	}

	for (Play::KeyboardButton& key : unbindRequestVector)
	{
		keyBindings.erase(key);
	}

	for (std::unordered_map<Play::KeyboardButton, inputBinding>::iterator requestBindingIt = bindingRequests.begin(); requestBindingIt != bindingRequests.end(); ++requestBindingIt)
	{
		keyBindings[requestBindingIt->first] = std::move(requestBindingIt->second);
	}

	unbindRequestVector.clear();
	bindingRequests.clear();
}