#pragma once

class IntVector3
{
public:

	inline IntVector3();
	inline IntVector3( int initialX, int initialY, int initialZ );
	inline IntVector3( const IntVector3& vectorToCopy );

	inline const IntVector3 operator*( float scalarToScaleBy ) const;
	inline bool operator==( const IntVector3& compareTo ) const;


public:
	int x;
	int y;
	int z;
};

//--------------------------------------------------------------------------------------------------------------
// Do-nothing default ctor: because it saves time to leave trash values rather than allocate and initialize.
inline IntVector3::IntVector3()
	: x()
	, y()
{
}


//--------------------------------------------------------------------------------------------------------------
inline IntVector3::IntVector3( int initialX, int initialY, int initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}


//--------------------------------------------------------------------------------------------------------------
inline IntVector3::IntVector3( const IntVector3& vectorToCopy )
	: x( vectorToCopy.x )
	, y( vectorToCopy.y )
	, z( vectorToCopy.z )
{
}


//--------------------------------------------------------------------------------------------------------------
inline const IntVector3 IntVector3::operator*( float scalarToScaleBy ) const
{
	return IntVector3( x * (int)scalarToScaleBy, y * (int)scalarToScaleBy, z * (int)scalarToScaleBy );
}


//--------------------------------------------------------------------------------------------------------------
inline bool IntVector3::operator==( const IntVector3& compareTo ) const
{
	return ( x == compareTo.x ) && ( y == compareTo.y ) && ( z == compareTo.z );
}
