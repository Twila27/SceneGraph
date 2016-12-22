#pragma once


#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------
class TheInput;
class XboxController;


//-----------------------------------------------------------------------------
extern TheInput* g_theInput;
enum Controllers { CONTROLLER_ONE, CONTROLLER_TWO, CONTROLLER_THREE, CONTROLLER_FOUR, NUM_CONTROLLERS };
enum ControllerButtons { A_BUTTON, B_BUTTON, X_BUTTON, Y_BUTTON, LEFT_SHOULDER_BUTTON, RIGHT_SHOULDER_BUTTON, 
	BACK_BUTTON, START_BUTTON, LEFT_STICK_BUTTON, RIGHT_STICK_BUTTON, NUM_BUTTONS };


//-----------------------------------------------------------------------------
struct KeyButtonState
{
	bool m_isKeyDown;
	bool m_didKeyJustChange;
};


//-----------------------------------------------------------------------------
struct MouseButtonState
{
	bool m_isMouseButtonDown;
	bool m_didMouseButtonJustChange;
};


//-----------------------------------------------------------------------------
class TheInput
{
public:
	TheInput();
	void Update();


	//Keyboard.
	void SetKeyDownStatus( unsigned char asKey, bool isNowDown ); //Used by WinMain key callback.
	void SetMouseDownStatus( int unityStyleID, bool isNowDown );
	bool IsKeyDown( unsigned char keyID ) const;
	bool WasKeyJustPressed( unsigned char keyID ) const;
	bool WasKeyPressedOnce( unsigned char keyID ) const;


	//Controller.
	bool IsButtonDown( ControllerButtons buttonID, Controllers controllerID ) const;
	bool WasButtonJustPressed( ControllerButtons buttonID, Controllers controllerID ) const;
	bool WasButtonPressedOnce( ControllerButtons buttonID, Controllers controllerID ) const;

	Vector2 GetLeftStickPosition( Controllers controllerID ) const;
	float GetLeftStickPositionAsRadius( Controllers controllerID ) const;
	float GetLeftStickPositionAsAngleInRadians( Controllers controllerID ) const;

	Vector2 GetRightStickPosition( Controllers controllerID ) const;
	float GetRightStickPositionAsRadius( Controllers controllerID ) const;
	float GetRightStickPositionAsAngleInRadians( Controllers controllerID ) const;

	float GetLeftTrigger( Controllers controllerID ) const;
	float GetRightTrigger( Controllers controllerID ) const;


	//Mouse.
	bool IsMouseButtonDown( int unityStyleID ) const;
	bool WasMouseButtonJustPressed( int unityStyleID ) const;
	bool WasMouseButtonPressedOnce( int unityStyleID );

	IntVector2 GetCursorPosition() const { return m_cursorPos; }
	void SetCursorPosition( const IntVector2& cursorPos );
	void SetCursorSnapToPos( const IntVector2& snapPos ) { m_snapToPos = snapPos; }

	float GetCursorDeltaX() { return (float)m_cursorDelta.x; }
	float GetCursorDeltaY() { return (float)m_cursorDelta.y; }

	void DisplayCursor() { m_isCursorVisible = true;  }
	void HideCursor() { m_isCursorVisible = false; }

	void OnGainedFocus() { m_hasFocus = true; }
	void OnLostFocus() { m_hasFocus = false; }

	int GetMouseWheelDelta() const { return m_mouseWheelDelta; }
	void SetMouseWheelDelta( short wheelDelta );

private:
	static const int NUM_KEYS = 250;
	KeyButtonState m_keys[ NUM_KEYS ];

	static const int NUM_MOUSE_BUTTONS = 3; //ELEMENT 0 EXISTS FOR COMPATIBILITY WITH VK_LBUTTON (0x01) / VK_RBUTTON (0x02).
	MouseButtonState m_mouseButtons[ NUM_MOUSE_BUTTONS ]; //DO NOT ACCESS FIRST ELEMENT.

	XboxController* m_controllers[ Controllers::NUM_CONTROLLERS ];
	
	IntVector2 m_cursorPos;
	IntVector2 m_snapToPos;
	IntVector2 m_cursorDelta;
	bool m_isCursorVisible;

	bool m_hasFocus;

	static const int MOUSEWHEEL_DELTA_STANDARD_AMOUNT = 120;
	int m_mouseWheelDelta;
};
