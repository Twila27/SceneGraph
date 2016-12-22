#include "Engine/Renderer/Vertexes.hpp"


Vertex3D_PCT::Vertex3D_PCT( const Vector3& position /*= Vector3::ZERO*/, const Vector2& texCoords /*= Vector2::ZERO*/, const Rgba& color /*= Rgba::WHITE */ )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
{
}
