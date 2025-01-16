#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "Singleton.hpp"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "KeyCodes.hpp"
#include <array>

class InputManager : public Singleton<InputManager>
{
		bool mAnythingTriggered = false;

		// KEYBOARD
		bool	mKeyTriggered		   = false;
		int32_t mKeyCount			   = 0;
		bool*	mCurrentKeyboardState  = nullptr;
		bool*	mPreviousKeyboardState = nullptr;

		// MOUSE
		bool	  mMouseTriggered	  = false;
		float	  mMouseScroll		  = 0.f;
		uint32_t  mCurrentMouseState  = 0;
		uint32_t  mPreviousMouseState = 0;
		glm::vec2 mCurrentMousePosition {};
		glm::vec2 mPreviousMousePosition {};

		// CONTROLLER
		bool		 mUsingController	  = false;
		bool		 mControllerTriggered = false;
		SDL_Gamepad* mController		  = nullptr;
		float		 mControllerDeadZone  = 0.2f;

		std::array<bool, static_cast<size_t>(Controller::MAX_ENUM)>		 mCurrentControllerState {};
		std::array<bool, static_cast<size_t>(Controller::MAX_ENUM)>		 mPreviousControllerState {};
		std::array<float, static_cast<size_t>(ControllerAxis::MAX_ENUM)> mGamePadAxes {};

		void ConnectController();
		void DisconnectController();

	public:
		void Initialize();
		void Update();
		void ProcessEvents(const SDL_Event& ev);
		void Shutdown();

		glm::vec2 GetLeftJoystick() const;
		glm::vec2 GetRightJoystick() const;
		float	  GetAxis(ControllerAxis axis) const;

		bool Triggered(int key) const;
		bool Triggered(Keyboard key) const;
		bool Triggered(Controller button) const;
		bool Triggered(Mouse button) const;

		bool Pressed(int key) const;
		bool Pressed(Keyboard key) const;
		bool Pressed(Controller button) const;
		bool Pressed(Mouse button) const;

		bool Released(int key) const;
		bool Released(Keyboard key) const;
		bool Released(Controller button) const;
		bool Released(Mouse button) const;

		glm::vec2 GetMousePosition() const;
		glm::vec2 GetPreviousMousePosition() const;
		glm::vec2 GetMouseMovement() const;
		glm::vec2 GetMouseMovementDirection() const;
		float	  GetMouseScroll() const;

		bool AnythingTriggered() const;
		bool KeyboardTriggered() const;
		bool ControllerTriggered() const;
		bool MouseTriggered() const;
		bool UsingController() const;

		bool ControllerConnected() const;
		void VibrateController(float force = 1.f, float duration = 1.f) const;
		void VibrateControllerLowHigh(float low = 1.f, float high = 1.f, float duration = 1.f) const;
};

#endif