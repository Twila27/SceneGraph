#pragma once


#define BIT(x) (1 << (x)) //2^x, or the xth bit.


//-----------------------------------------------------------------------------
class Vector2;
class PolarCoords;


//-----------------------------------------------------------------------------
const float PI = 3.14159265359f;
const float HALF_PI = PI / 2.f;


//-----------------------------------------------------------------------------
template <class T> T GetMax( T a, T b ) { return ( a < b ? b : a ); }
float Interpolate( float start, float end, float fractionTowardEnd );
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float CalcFractionInRange(float inValue, float rangeStart, float rangeEnd);
float ClampFloat( float inValue, float min, float max );
float ClampFloatZeroToOne( float inValue );
float WrapNumberWithinCircularRange( float inValue, float minValue, float maxValue );
int WrapNumberWithinCircularRange( int inValue, int minValue, int maxValue );


//-----------------------------------------------------------------------------
float DegreesToRadians( float degrees );
float RadiansToDegrees( float radians );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float Atan2Degrees( const Vector2& direction );
float DotProduct( const Vector2& lhs, const Vector2& rhs );


//-----------------------------------------------------------------------------
float CalcDistBetweenPoints( const Vector2& pos1, const Vector2& pos2 );
float CalcDistSquaredBetweenPoints( const Vector2& pos1, const Vector2& pos2 );
float CalcShortestAngularDisplacement( float fromDegrees, float toDegrees );
bool DoDiscsOverlap( const Vector2& center1, float radius1, const Vector2& center2, float radius2 );
bool IsPointInDisc( const Vector2& point, const Vector2& discCenter, float discRadius );


//-----------------------------------------------------------------------------
float GetRandomFloatZeroToOne();
float GetRandomFloatInRange( float min, float max );
int GetRandomIntInRange( float min, float max );
bool CheckRandomChance( float chanceForSuccess );