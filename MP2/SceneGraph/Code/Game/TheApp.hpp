#pragma once


//-----------------------------------------------------------------------------
class TheApp;


//-----------------------------------------------------------------------------
extern TheApp* g_theApp;
static const int NUM_KEYS = 250; //Can't move into class due to use as array sizes.
static const int NUM_CONTROLLERS = 4;


//-----------------------------------------------------------------------------
struct KeyButtonState
{
	bool m_isKeyDown;
	bool m_didKeyJustChange;
};


//-----------------------------------------------------------------------------
class TheApp
{
public:

	double m_screenWidth;
	double m_screenHeight;

	TheApp( const double screenWidth, const double screenHeight );

	void SetKeyDownStatus( unsigned char asKey, bool isNowDown );
	bool isKeyDown( unsigned char keyID );
	bool WasKeyJustPressed( unsigned char keyID );
	bool WasKeyPressedOnce( unsigned char keyID ) const;
	void Update();


private:
	KeyButtonState m_keys[ NUM_KEYS ];
};
