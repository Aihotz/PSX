#include "InputManager.hpp"
#include <cctype>

void InputManager::Initialize()
{
	// KEYBOARD
	// get the key count and allocate memory for it
	const bool* keyboard_status = SDL_GetKeyboardState(&mKeyCount);
	mCurrentKeyboardState		= new bool[mKeyCount];
	mPreviousKeyboardState		= new bool[mKeyCount];

	// copy the state of the keyboard to the states
	memcpy(mCurrentKeyboardState, keyboard_status, mKeyCount);
	memcpy(mPreviousKeyboardState, keyboard_status, mKeyCount);

	// MOUSE
	// initialize data
	mCurrentMouseState	= 0;
	mPreviousMouseState = 0;

	mCurrentMousePosition  = glm::vec2 {};
	mPreviousMousePosition = glm::vec2 {};

	// CONTROLLER
	mUsingController = true;
	mController		 = nullptr;

	// initialize state arrays
	std::fill(mCurrentControllerState.begin(), mCurrentControllerState.end(), false);
	std::fill(mPreviousControllerState.begin(), mPreviousControllerState.end(), false);
	std::fill(mGamePadAxes.begin(), mGamePadAxes.end(), 0.0f);
}

void InputManager::ConnectController()
{
	// disconnect the controller first if necessary
	if (mController)
		DisconnectController();

	// get the first connected controller
	mController = SDL_OpenGamepad(0);

	mUsingController = true;
}

void InputManager::DisconnectController()
{
	if (mController)
	{
		SDL_CloseGamepad(mController);
		mController = nullptr;
	}

	mUsingController = false;
}

void InputManager::Update()
{
	SDL_PumpEvents();

	// KEYBOARD
	// get the current state of the keyboard
	const bool* keyboard_state = SDL_GetKeyboardState(nullptr);

	// update the previous and current states of the keyboard
	memcpy(mPreviousKeyboardState, mCurrentKeyboardState, mKeyCount);
	memcpy(mCurrentKeyboardState, keyboard_state, mKeyCount);

	// determine if ANY key was triggered this frame
	mKeyTriggered = false;
	for (int i = 0; i < mKeyCount; i++)
	{
		if (mCurrentKeyboardState[i] == true && mPreviousKeyboardState[i] == false)
		{
			mKeyTriggered = true;
			break;
		}
	}

	// MOUSE
	// update the previous state of the mouse
	mPreviousMouseState	   = mCurrentMouseState;
	mPreviousMousePosition = mCurrentMousePosition;

	// update the current state of the mouse
	mCurrentMouseState = SDL_GetMouseState(&mCurrentMousePosition.x, &mCurrentMousePosition.y);

	// determine if ANY mouse button was triggered this frame
	mMouseTriggered = false;
	for (size_t i = 1; i < static_cast<size_t>(Mouse::MAX_ENUM); i++)
	{
		if ((mCurrentMouseState & SDL_BUTTON_MASK(i)) && (mPreviousMouseState & SDL_BUTTON_MASK(i)) == false)
		{
			mMouseTriggered = true;
			break;
		}
	}

	// CONTROLLER
	mControllerTriggered = false;

	// only update controller if one is present
	if (mController)
	{
		// update the previous state of the controller
		mPreviousControllerState = mCurrentControllerState;

		// get the current state of controller's axes
		for (size_t i = 0; i < static_cast<size_t>(ControllerAxis::MAX_ENUM); i++)
		{
			// SDL_GetGamepadAxis returns a number in the range
			// [-32768,32767], or [-int16_t::max, int16_t::max]
			// (except for triggers, which are in range [0, 32767]
			// divide by int16_t::max to keep the numbers in range
			// [-1.0f, 1.0f] (or [0.0f, 1.0f] in the case of triggers).

			constexpr int16_t max_value = std::numeric_limits<int16_t>().max();

			mGamePadAxes[i]
				= SDL_GetGamepadAxis(mController, static_cast<SDL_GamepadAxis>(i)) / static_cast<float>(max_value);
			mGamePadAxes[i] = glm::clamp(mGamePadAxes[i], -1.0f, 1.0f);
		}

		// get the current state of the controller
		// triggers information is computed separately
		for (size_t i = 0; i < mCurrentControllerState.size(); i++)
			mCurrentControllerState[i] = SDL_GetGamepadButton(mController, static_cast<SDL_GamepadButton>(i));

		// check if ANY button was triggered this frame
		for (size_t i = 0; i < mCurrentControllerState.size(); i++)
		{
			if (mCurrentControllerState[i] == true && mPreviousControllerState[i] == false)
			{
				mControllerTriggered = true;
				break;
			}
		}
	}

	// check if ANYTHING was triggered this frame
	mAnythingTriggered = mKeyTriggered || mMouseTriggered || mControllerTriggered;

	// check if the controller was the last thing in use

	// prevent a direct assignment to avoid chanching the state
	// if mUsingController if nothing is triggered
	if (mKeyTriggered || mMouseTriggered)
		mUsingController = false;

	if (mController)
	{
		if (mControllerTriggered)
			mUsingController = true;

		// do not check if we are already using the controller
		if (mUsingController == false)
		{
			// check if any axis is beyond the death zone
			// if an axis is moved, nothing is triggered,
			// but the controller should be considered in use

			for (size_t i = 0; i < mGamePadAxes.size(); i++)
			{
				if (glm::abs(mGamePadAxes[i]) > mControllerDeadZone)
				{
					mUsingController = true;
					break;
				}
			}
		}
	}
}

void InputManager::ProcessEvents(const SDL_Event& ev)
{
	switch (ev.type)
	{
		case SDL_EVENT_GAMEPAD_ADDED:	ConnectController(); break;
		case SDL_EVENT_GAMEPAD_REMOVED: DisconnectController(); break;
		case SDL_EVENT_MOUSE_WHEEL:		mMouseScroll = ev.wheel.y; break;
	}
}

void InputManager::Shutdown()
{
	delete[] mPreviousKeyboardState;
	mPreviousKeyboardState = nullptr;

	delete[] mCurrentKeyboardState;
	mCurrentKeyboardState = nullptr;

	DisconnectController();
}

glm::vec2 InputManager::GetLeftJoystick() const
{
	return glm::vec2(GetAxis(ControllerAxis::LeftX), -GetAxis(ControllerAxis::LeftY));
}

glm::vec2 InputManager::GetRightJoystick() const
{
	return glm::vec2(GetAxis(ControllerAxis::RightX), -GetAxis(ControllerAxis::RightY));
}

float InputManager::GetAxis(ControllerAxis axis) const
{
	if (mController == nullptr)
		return 0.0f;

	// only return values of the axis past the dead zone
	float value = mGamePadAxes[static_cast<size_t>(axis)];
	if (glm::abs(value) > mControllerDeadZone)
		return value;

	return 0.0f;
}

bool InputManager::Triggered(int key) const
{
	Keyboard key_code
		= static_cast<Keyboard>(SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(std::tolower(key)), nullptr));
	return Triggered(key_code);
}

bool InputManager::Triggered(Keyboard key) const
{
	return mCurrentKeyboardState[static_cast<size_t>(key)] == true
		&& mPreviousKeyboardState[static_cast<size_t>(key)] == false;
}

bool InputManager::Triggered(Controller button) const
{
	if (mController == nullptr)
		return false;

	return mCurrentControllerState[static_cast<size_t>(button)] == true
		&& mPreviousControllerState[static_cast<size_t>(button)] == false;
}

bool InputManager::Triggered(Mouse button) const
{
	int mask = SDL_BUTTON_MASK(static_cast<size_t>(button));

	return (mCurrentMouseState & mask) && (mPreviousMouseState & mask) == false;
}

bool InputManager::Pressed(int key) const
{
	Keyboard key_code
		= static_cast<Keyboard>(SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(std::tolower(key)), nullptr));
	return Pressed(key_code);
}

bool InputManager::Pressed(Keyboard key) const
{
	return mCurrentKeyboardState[static_cast<size_t>(key)];
}

bool InputManager::Pressed(Controller button) const
{
	if (mController == nullptr)
		return false;

	return mCurrentControllerState[static_cast<size_t>(button)] == true;
}

bool InputManager::Pressed(Mouse button) const
{
	int mask = SDL_BUTTON_MASK(static_cast<size_t>(button));

	return (mCurrentMouseState & mask);
}

bool InputManager::Released(int key) const
{
	Keyboard key_code
		= static_cast<Keyboard>(SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(std::tolower(key)), nullptr));
	return Released(key_code);
}

bool InputManager::Released(Keyboard key) const
{
	return mCurrentKeyboardState[static_cast<size_t>(key)] == false
		&& mPreviousKeyboardState[static_cast<size_t>(key)] == true;
}

bool InputManager::Released(Controller button) const
{
	if (mController == nullptr)
		return false;

	return mCurrentControllerState[static_cast<size_t>(button)] == false
		&& mPreviousControllerState[static_cast<size_t>(button)] == true;
}

bool InputManager::Released(Mouse button) const
{
	int mask = SDL_BUTTON_MASK(static_cast<size_t>(button));

	return (mCurrentMouseState & mask) == false && (mPreviousMouseState & mask);
}

glm::vec2 InputManager::GetMousePosition() const
{
	return mCurrentMousePosition;
}

glm::vec2 InputManager::GetPreviousMousePosition() const
{
	return mPreviousMousePosition;
}

glm::vec2 InputManager::GetMouseMovement() const
{
	return mCurrentMousePosition - mPreviousMousePosition;
}

glm::vec2 InputManager::GetMouseMovementDirection() const
{
	return glm::normalize(GetMouseMovement());
}

float InputManager::GetMouseScroll() const
{
	return mMouseScroll;
}

bool InputManager::AnythingTriggered() const
{
	return mAnythingTriggered;
}

bool InputManager::KeyboardTriggered() const
{
	return mKeyTriggered;
}

bool InputManager::ControllerTriggered() const
{
	return mControllerTriggered;
}

bool InputManager::MouseTriggered() const
{
	return mMouseTriggered;
}

bool InputManager::UsingController() const
{
	return mUsingController;
}

bool InputManager::ControllerConnected() const
{
	return mController != nullptr;
}

void InputManager::VibrateController(float force, float duration) const
{
	VibrateControllerLowHigh(force, force, duration);
}

void InputManager::VibrateControllerLowHigh(float low, float high, float duration) const
{
	if (mController == nullptr)
		return;

	// clamp if necessary
	low	 = glm::clamp(low, 0.0f, 1.0f);
	high = glm::clamp(high, 0.0f, 1.0f);

	// get the values in range 0x0000 to 0xFFFF
	uint16_t low_force	= static_cast<uint16_t>(low * 0xFFFF);
	uint16_t high_force = static_cast<uint16_t>(high * 0xFFFF);

	SDL_RumbleGamepad(mController, low_force, high_force, static_cast<uint32_t>(duration * 1000));
}