#include "Game/TheApp.hpp"


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//--------------------------------------------------------------------------------------------------------------
TheApp* g_theApp = nullptr;


//--------------------------------------------------------------------------------------------------------------
TheApp::TheApp( const double screenWidth, const double screenHeight )
: m_screenWidth( screenWidth )
, m_screenHeight( screenHeight )
{
	for ( int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex )
	{
		m_keys[ keyIndex ].m_isKeyDown = false;
		m_keys[ keyIndex ].m_didKeyJustChange = false;
	}
}


//--------------------------------------------------------------------------------------------------------------
void TheApp::SetKeyDownStatus( unsigned char asKey, bool isNowDown )
{
	m_keys[ asKey ].m_didKeyJustChange = ( m_keys[ asKey ].m_isKeyDown != isNowDown ? true : false );
	m_keys[ asKey ].m_isKeyDown = isNowDown;
}


//--------------------------------------------------------------------------------------------------------------
bool TheApp::isKeyDown( unsigned char keyID )
{
	return m_keys[ keyID ].m_isKeyDown;
}


//--------------------------------------------------------------------------------------------------------------
bool TheApp::WasKeyJustPressed( unsigned char keyID )
{
	return m_keys[ keyID ].m_didKeyJustChange;
}


//--------------------------------------------------------------------------------------------------------------
bool TheApp::WasKeyPressedOnce( unsigned char keyID ) const
{
	return m_keys[ keyID ].m_didKeyJustChange && m_keys[ keyID ].m_isKeyDown;
}


//--------------------------------------------------------------------------------------------------------------
void TheApp::Update()
{
	for ( int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex )
		m_keys[ keyIndex ].m_didKeyJustChange = false;
}
