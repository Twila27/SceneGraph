#pragma once


#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Rgba.hpp"


struct Vertex3D_PCT
{
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoords;
	Vertex3D_PCT( const Vector3& position = Vector3::ZERO, const Vector2& texCoords = Vector2::ZERO, const Rgba& color = Rgba::WHITE );
};