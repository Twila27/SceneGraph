#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <string>
#include <cmath>
#include "SimpleSceneGraph.h"

int g_numberSceneGraphNodesVisited = 0;
int g_numberSphereFrustumIntersectionTestsPerformed = 0;
int g_numberGeometryNodesRendered = 0;

bool g_renderNodeAndGeometryNodeBoundingVolumes = true;
bool g_renderTransformNodeBoundingVolumes = true;
bool g_renderGeometry = true;

double g_perspective_fieldOfView_yDegrees = 45.0;
double g_perspective_aspect = 640.f / 480.f; //Screen width / height, here 4:3.
double g_perspective_zNear = 0.1;
double g_perspective_zFar = 1000.0;

namespace simpleSceneGraph {

	simpleSceneGraph::BoundingSphere Node::ComputeBoundingSphere() //Only extends its bounding volume over its children bounding volumes, nothing extra.
	{
		BoundingSphere currentSphere;

		//std::cout << "Node::computeBoundingSphere() " << _name << std::endl;
		Children::const_iterator it = _children.begin(), ite = _children.end();
		for ( ; it != ite; ++it )
		{
			Node* child = *it;
			m_boundingSphere = currentSphere.extendToIncludeSphere( child->ComputeBoundingSphere() );
		}
		m_boundingSphere = currentSphere;

		return currentSphere;
	}

	void Node::cull( const Camera* camera, Traversal* trav )
	{
		//std::cout << "Node::cull " << _name << std::endl;
		g_numberSceneGraphNodesVisited++;

		//Transform to cam space and then intersection test. Only need to do a translation transform/mult to reach cam space.
		BoundingSphere copy = m_boundingSphere;
		copy.transformByMatrix( trav->topMatrixStack()._m ); //All nodes use this line to go world -> flycam space, but TransformNode does it prior to pushMatrixStack call.

		m_isNotSeenByViewFrustum = camera->IsNodeOutsideCameraFrustum( copy );
		//if (m_isNotSeenByViewFrustum) return; //Would stop rendering children of this node. Commented out to still draw children, but we will color culled ones in red.

		if ( g_renderNodeAndGeometryNodeBoundingVolumes )
		{
			glColor3f( 1.f, 1.f, 1.f );
			RenderBoundingSphere( trav );
		}

		Children::const_iterator it = _children.begin(), ite = _children.end();
		for ( ; it != ite; ++it )
			( *it )->cull( camera, trav );
	}

	void Node::RenderBoundingSphere( Traversal* trav )
	{
		Matrix boundingSphereAsModelMatrix; //Apply as anti-translation like in Assault?
		boundingSphereAsModelMatrix._m[ 12 ] = /*-*/m_boundingSphere.m_center.x;
		boundingSphereAsModelMatrix._m[ 13 ] = /*-*/m_boundingSphere.m_center.y;
		boundingSphereAsModelMatrix._m[ 14 ] = /*-*/m_boundingSphere.m_center.z;

		trav->pushMatrixStack( boundingSphereAsModelMatrix );
		glLoadMatrixf( trav->topMatrixStack()._m );	//Now we're both in camera space and translated to sphere center.
		renderWireSphere( m_boundingSphere.m_radius, 8.f, 8.f );
		trav->popMatrixStack();
	}

	size_t Node::s_nodeCount = 0;

	void CrossProduct( const float* l, const float* r, float* result )
	{
		result[ 0 ] = l[ 1 ] * r[ 2 ] - r[ 1 ] * l[ 2 ];
		result[ 1 ] = l[ 2 ] * r[ 0 ] - r[ 2 ] * l[ 0 ];
		result[ 2 ] = l[ 0 ] * r[ 1 ] - r[ 0 ] * l[ 1 ];
	}

	void Normalize( const float* input, float* output )
	{
		float length = sqrt( input[ 0 ] * input[ 0 ] + input[ 1 ] * input[ 1 ] + input[ 2 ] * input[ 2 ] );
		assert( length != 0.f );
		output[ 0 ] = input[ 0 ] / length;
		output[ 1 ] = input[ 1 ] / length;
		output[ 2 ] = input[ 2 ] / length;
	}

	void TransformNode::DeleteStrategy()
	{
		if ( _strategy != nullptr )
		{
			delete _strategy;
			_strategy = nullptr;
		}
	}

	void TransformNode::cull( const Camera* camera, Traversal* trav )
	{
		//std::cout << "TransformNode::cull " << _name << std::endl;
		g_numberSceneGraphNodesVisited++;

		//Transform to cam space and then intersection test. Only need to do a translation transform/mult to reach cam space.
		BoundingSphere copy = m_boundingSphere;
		copy.transformByMatrix( trav->topMatrixStack()._m ); //All nodes use this line to go world -> flycam space, but TransformNode does it prior to pushMatrixStack call.

		m_isNotSeenByViewFrustum = camera->IsNodeOutsideCameraFrustum( copy );


		//Transform to cam space and then intersection test. Only need to do a translation transform/mult to reach cam space.
//		m_boundingSphere.transformByMatrix( trav->topMatrixStack()._m ); //All nodes use this line, but TransformNode does it prior to pushMatrixStack call.
		//Else it'd multiply in its _transform member once in ComputeBoundingSpheres and once again here.

//		m_isNotSeenByViewFrustum = camera->IsNodeOutsideCameraFrustum( m_boundingSphere );

		if ( g_renderTransformNodeBoundingVolumes )
		{
			if ( m_isNotSeenByViewFrustum ) glColor3f( 1.f, 0.f, 0.f ); //Red for bad. False by default.
			else glColor3f( 0.f, 1.f, 0.f ); //Green for good.
			RenderBoundingSphere( trav );
		}

		//SHOULD IT STILL TRANSFORM BOUNDING VOLUME / RENDER IT? -- [YES AND NO! CHILDREN MAY STILL BE IN/OVERLAP FRUSTUM, SO IT NEEDS TO PUSH _TRANSFORM--DON'T TRANSFORM THE BOUNDING VOLUME!]
		trav->pushMatrixStack( _transform ); //KEY: WHERE THE LEFT/PRE-MULT HAPPENS!

		//Render bounding sphere here, after applying _transform?
		Children::const_iterator it = _children.begin(), ite = _children.end();
		for ( ; it != ite; ++it )
			( *it )->cull( camera, trav );
		trav->popMatrixStack();
	}

	simpleSceneGraph::BoundingSphere TransformNode::ComputeBoundingSphere() //Extends to include children bounding volumes AFTER transforming them relative to itself, and then transforms its own.
	{
		BoundingSphere currentSphere;

		//std::cout << "TransformNode::computeBoundingSphere() " << _name << std::endl;
		Children::const_iterator it = _children.begin(), ite = _children.end();
		for ( ; it != ite; ++it )
		{
			Node* child = *it;
			BoundingSphere childBoundingSphere = child->ComputeBoundingSphere();
			childBoundingSphere.transformByMatrix( _transform ); //Transform children volumes--copies of them, or actual refs to them?? COPIES.
			currentSphere.extendToIncludeSphere( childBoundingSphere );
		}
		m_boundingSphere = currentSphere;// .transformByMatrix( _transform ); //Transform its own volume ONLY IN CULL TRAVERSAL, NOT HERE.

		return currentSphere;
	}

	simpleSceneGraph::BoundingSphere BoundingSphere::extendToIncludeVertex( const Vector3& vertex )
	{
		if ( IsSphereEmpty() )
		{
			m_radius = 0.f;
			m_center = vertex;
			return *this;
		}

		Vector3 vertexToCenterDisplacement = m_center - vertex;
		float vertexToCenterDistance = vertexToCenterDisplacement.CalcLength();

		if ( vertexToCenterDistance > m_radius )
		{
			vertexToCenterDisplacement.Normalize();
			Vector3 pointOnSphereInDirectionOfVertex = vertexToCenterDisplacement * m_radius;
			Vector3 pointInModelSpaceUnits = pointOnSphereInDirectionOfVertex + m_center; //Assuming m_center is in node/model space units.
			m_center = ( pointInModelSpaceUnits + vertex ) * 0.5f; //Sphere translated to halfway between this "point" and param vertex.
			m_radius = ( vertexToCenterDistance + m_radius ) * 0.5f; //Dilate sphere from this midpoint to enclose both points.
		}
		//Else we're inside the sphere, no extension occurs.

		return *this;
	}

	simpleSceneGraph::BoundingSphere BoundingSphere::extendToIncludeVertexList( const std::vector<Vector3>& list )
	{
		if ( IsSphereEmpty() ) return *this;

		//Directly calling extendToIncludeVertex(vert[i]) yields order-dependent inaccurately sized results, so instead two-pass method:
		unsigned int minIndexX, minIndexY, minIndexZ; //Indices of verts with min outlier along the single dimension.
		unsigned int maxIndexX, maxIndexY, maxIndexZ; //Indices of verts with max outlier along the single dimension.


		//1st pass for initial fit: find 2 most extreme vertices in list, call extendToIncludeVert on them first.
		for ( unsigned int vertexIndex = 0; vertexIndex < list.size(); vertexIndex++ )
		{
			if ( vertexIndex == 0 ) //Initial config.
			{
				minIndexX = vertexIndex;
				minIndexY = vertexIndex;
				minIndexZ = vertexIndex;
				maxIndexX = vertexIndex;
				maxIndexY = vertexIndex;
				maxIndexZ = vertexIndex;
				continue;
			}
			
			Vector3 currentVertex = list[ vertexIndex ];
			if ( currentVertex.x < list[ minIndexX ].x ) minIndexX = vertexIndex;
			if ( currentVertex.y < list[ minIndexY ].y ) minIndexY = vertexIndex;
			if ( currentVertex.z < list[ minIndexZ ].z ) minIndexZ = vertexIndex;
			if ( currentVertex.x > list[ maxIndexX ].x ) maxIndexX = vertexIndex;
			if ( currentVertex.y > list[ maxIndexY ].y ) maxIndexY = vertexIndex;
			if ( currentVertex.z > list[ maxIndexZ ].z ) maxIndexZ = vertexIndex;
		}

		float distX = (list[ maxIndexX ] - list[ minIndexX ]).CalcLength();
		float distY = (list[ maxIndexY ] - list[ minIndexY ]).CalcLength();
		float distZ = (list[ maxIndexZ ] - list[ minIndexZ ]).CalcLength();

		float minInMinAxis, maxInMaxAxis; //Of all 3 dimensions, which has the most extreme value.
		if ( distX > distY && distX > distZ ) maxInMaxAxis = maxIndexX;
		if ( distY > distX && distY > distZ ) maxInMaxAxis = maxIndexY;
		if ( distZ > distY && distZ > distX ) maxInMaxAxis = maxIndexZ;

		if ( distX < distY && distX < distZ ) minInMinAxis = minIndexX;
		if ( distY < distX && distY < distZ ) minInMinAxis = minIndexY;
		if ( distZ < distY && distZ < distX ) minInMinAxis = minIndexZ;

		Vector3 extremesAdded = list[ minInMinAxis ] + list[ maxInMaxAxis ];
		m_center = ( extremesAdded ) * 0.5f;
		m_radius = ( extremesAdded.CalcLength() ) * 0.5f;

		//2nd pass for refined fit: verify all verts inside sphere, using mins/maxs from 1st pass.
		for ( unsigned int vertexIndex = 0; vertexIndex < list.size(); vertexIndex++ )
			extendToIncludeVertex( list[ vertexIndex ] ); //Checks inside quit if circumscribed.

		return *this;
	}

	simpleSceneGraph::BoundingSphere BoundingSphere::extendToIncludeSphere( const BoundingSphere& other )
	{
		if ( IsSphereEmpty() )
		{
			if ( other.IsSphereEmpty() ) return BoundingSphere();
			else
			{
				*this = other;
				return other;
			}
		}
		
		//Reduces to the single-vertex case, but needs to include point furthest from param sphere.
		Vector3 centerTocenterDisplacement = other.m_center - m_center;
		float centerToCenterDistance = centerTocenterDisplacement.CalcLength();

		float biggerRadius = other.m_radius;
		float smallerRadius = m_radius;
		if ( m_radius > biggerRadius ) //Does this sphere circumscribe the param sphere?
		{
			smallerRadius = biggerRadius;
			biggerRadius = m_radius;

			//Since this sphere contains param sphere, no extension need occur.
			if ( biggerRadius > ( smallerRadius + centerToCenterDistance ) ) return *this;
		}

		//Since param sphere contains this sphere, no extension need occur.
		if ( biggerRadius > ( smallerRadius + centerToCenterDistance ) ) return other;
		
		//If neither sphere contains the other...
		centerTocenterDisplacement.Normalize();
		Vector3 pointOnParamSphereInDirectionOfThisSphereCenter = centerTocenterDisplacement * other.m_radius;
		Vector3 pointOnParamSphereInDirectionOppositeThisSphereCenter = other.m_center + pointOnParamSphereInDirectionOfThisSphereCenter;
		
		return extendToIncludeVertex( pointOnParamSphereInDirectionOppositeThisSphereCenter );
	}

	simpleSceneGraph::BoundingSphere BoundingSphere::transformByMatrix( const Matrix& m )
	{
		if ( IsSphereEmpty() ) return *this;

		//Transform *this, the sphere, by m--we will ignore rotations, being a sphere, so it's just the translation bottom row (12-14) that matters.
		m_center.x += m._m[ 12 ];
		m_center.y += m._m[ 13 ];
		m_center.z += m._m[ 14 ];
		//We also assume no scaling, so no radius alterations needed.

		return *this;
	}

	simpleSceneGraph::BoundingSphere GeometryNode::ComputeBoundingSphere() //Same as Node class but starting non-empty.
	{
		BoundingSphere currentSphere = GetBoundingSphereForGeometry( _type ); //Unlike rest, not starting empty.

		//std::cout << "GeometryNode::computeBoundingSphere() " << _name << std::endl;
		Children::const_iterator it = _children.begin(), ite = _children.end();
		for ( ; it != ite; ++it )
		{
			Node* child = *it;
			m_boundingSphere = currentSphere.extendToIncludeSphere( child->ComputeBoundingSphere() );
		}
		m_boundingSphere = currentSphere;

		return currentSphere;
	}

	simpleSceneGraph::BoundingSphere GeometryNode::GetBoundingSphereForGeometry( GeometryNode::Type type )
	{
		return m_boundingSphere = BoundingSphere( 1.f, Vector3::ZERO ); //CURRENTLY MAKES ALL SPHERES UP TO ROOT R=1, C=ZERO.
		//The position will be recalculated as result of scene graph traversals, so centering it at zero is desired here.
		//Would ideally compute the radius using vertex data, but it's inaccessible to us in this sandbox.
	}

	bool Camera::IsNodeOutsideCameraFrustum( const BoundingSphere& nodeBoundingSphereInCameraSpace ) const
	{
		g_numberSphereFrustumIntersectionTestsPerformed++;

		//For each plane, if the signed distance to the plane ("D" in plane equation) > radius of the sphere, we're outside the frustum, where:
		//N.x * center.x + N.y * center.y + N.z * center.z + D == the signed distance from the plane (a la Mod 1).

		//if sign pos and distance > radius, you're outside

		//THESE CALCULATIONS TAKE PLACE IN CAM SPACE: CAMERA TRANSLATION == ORIGIN.

		//Does the distance need to be signed? YES.
		float signedDistanceToNearPlane = ( nodeBoundingSphereInCameraSpace.m_center.z - (-m_nearClippingPlaneDistance) ); // sphere.z - (-near). -z is forward/into screen.
		if ( signedDistanceToNearPlane > nodeBoundingSphereInCameraSpace.m_radius ) return true; //If signed distance < 0, then the near plane is between cam origin and sphere.z (i.e. IN frustum unless it's beyond far-plane).

		float signedDistanceToFarPlane = ( -nodeBoundingSphereInCameraSpace.m_center.z - m_farClippingPlaneDistance ); // -sphere.z - far. -sphere.z because we dot with <0,0,-1> i.e. far-plane's normal.
		if ( signedDistanceToFarPlane > nodeBoundingSphereInCameraSpace.m_radius ) return true; //If signed dist < 0, then sphere.z b/t far plane and cam origin (i.e. inside so long as it wasn't behind near plane).

		float signedDistanceToLeftPlane = ( ( m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.x * nodeBoundingSphereInCameraSpace.m_center.x ) + 
			( m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.z * nodeBoundingSphereInCameraSpace.m_center.z ) ); //entire expression == left-plane's normal dotted with sphere center (plus a plane equation "D" == 0 as left plane contains origin).
		if ( signedDistanceToLeftPlane > nodeBoundingSphereInCameraSpace.m_radius ) return true;

		float signedDistanceToRightPlane = ( ( m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.x * nodeBoundingSphereInCameraSpace.m_center.x ) +
			( m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.z * nodeBoundingSphereInCameraSpace.m_center.z ) );
		if ( signedDistanceToRightPlane > nodeBoundingSphereInCameraSpace.m_radius ) return true;

		float signedDistanceToTopPlane = ( ( m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.y * nodeBoundingSphereInCameraSpace.m_center.y ) +
			( m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.z * nodeBoundingSphereInCameraSpace.m_center.z ) );
		if ( signedDistanceToTopPlane > nodeBoundingSphereInCameraSpace.m_radius ) return true;

		float signedDistanceToBottomPlane = ( ( m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.y * nodeBoundingSphereInCameraSpace.m_center.y ) +
			( m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.z * nodeBoundingSphereInCameraSpace.m_center.z ) );
		if ( signedDistanceToBottomPlane > nodeBoundingSphereInCameraSpace.m_radius ) return true;

		return false; //None of the planes found it to be outside.

		//TODO: deeper implementation:
			//If I'm completely inside the frustum, we can ignore the rest of the view-frustum culling checks for all the bounding volume's children.
			//i.e. "disable view-frustum culling for the children", but don't do this on the first implementation, just return a bool.
			//Will need to use the inverse of this process: if the distance to the plane is more than a radius away in the opposite direction, I'm inside.
	}

}

//call from somewhere...
//The function should produce the following output.
/*
Node count is : 0
Building graph
Done building graph.Node count is : 5
Update traversal
RotateStrategy::update with angle = 0.01
TranslateStrategy::update with angle = 0.02
Cull traversal
Node::cull root
TransformNode::cull transform1
GeometryNode::cull
Node::cull geom1
TransformNode::cull transform2
GeometryNode::cull
Node::cull geom2
Cleaning up the graph, node count = 5
RotateStrategy::~RotateStrategy
TranslateStrategy::~TranslateStrategy
Final node count is : 0
*/
void trySimpleSceneGraph()
{
//     using namespace simpleSceneGraph;
//     std::cout << "Node count is: " << Node::getNodeCount() << std::endl;
//     std::cout << "Building graph" << std::endl;
// 
//     Node* root = new Node("root");
//     TransformNode* transform1(new TransformNode("transform1"));
//     root->addChild(transform1);
//     RotateStrategy* rotateStrategy = new RotateStrategy();
//     transform1->setStrategy(rotateStrategy);
//     GeometryNode* geom1= new GeometryNode("geom1");
//     transform1->addChild(geom1);
// 
//     //second branch
//     TransformNode* transform2 = new TransformNode("transform2");
//     root->addChild(transform2);
//     TranslateStrategy* translateStrategy = new TranslateStrategy();
//     transform2->setStrategy(translateStrategy);
//     GeometryNode* geom2 = new GeometryNode("geom2");
//     transform2->addChild(geom2);
//     std::cout << "Done building graph. Node count is: " << Node::getNodeCount() << std::endl;
// 
//     //let's iterate over children of the root node:
//     Node::const_iterator_child it = root->begin_child(), ite = root->end_child();
//     for (; it != ite; ++it)
//         const std::string& name = (*it)->name();
// 
//     //create the traversal 
//     Node::Traversal* trav = new Node::Traversal();
//     matrix view;//compute it
//     trav->initToViewMatrix(view);
//     std::cout << "Update traversal" << std::endl;
//     root->update();
//     std::cout << "Cull traversal" << std::endl;
//     root->cull(trav);
// 
//     std::cout << "Cleaning up the graph, node count = " << Node::getNodeCount() << std::endl;
//     //In reality, you want to go with shared_ptr approach that is based on reference counting
//     //in order to automate memory cleanup according to the RAII paradigm.
//     delete trav;
//     delete root;
//     delete transform1;
//     delete geom1;
//     delete transform2;
//     delete geom2;
//     delete rotateStrategy;
//     delete translateStrategy;
// 
//     std::cout << "Final node count is: " << Node::getNodeCount() << std::endl;
//     assert(Node::getNodeCount() == 0);
}
