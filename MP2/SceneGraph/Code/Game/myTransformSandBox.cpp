
#include ".\mytransformsandbox.h"
#include "Game/TheApp.hpp"

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "geometry.h"
#include <cmath>
#include <utility> //swap

using namespace simpleSceneGraph;

myTransformSandBox::myTransformSandBox( void ) : _root( nullptr ), _trav( nullptr )
{
#define ALL_PLANETS
	float camRot_y = 0.f;
	float camPos_x = 0.f;
	float camPos_y = 0.5f;
	float camPos_z = 35.f;
	float camPos[ 3 ] = { camPos_x, camPos_y, camPos_z };
	m_camera = new Camera( camPos, camRot_y );

	m_worldAlignedBillboardStrategy = new BillboardStrategy( m_camera, false );
	m_axisAlignedBillboardStrategy = new BillboardStrategy( m_camera, true );
	m_defaultRotationStrategy = new RotateStrategy();

	_root = new Node("root");

	GeometryNode* theSun = new GeometryNode( "The Sun", GeometryNode::TEAPOT ); m_geometryNodes[ "The Sun" ] = theSun; //Directly under root.

	//Planets are children of theSun in scene graph.
	GeometryNode* planetMercury = new GeometryNode( "Mercury", GeometryNode::CUBE ); m_geometryNodes[ "Mercury" ] = planetMercury;
#ifdef ALL_PLANETS
	GeometryNode* planetVenus = new GeometryNode( "Venus", GeometryNode::SPHERE ); m_geometryNodes[ "Venus" ] = planetVenus;
	GeometryNode* planetEarth = new GeometryNode( "Earth", GeometryNode::CONE ); m_geometryNodes[ "Earth" ] = planetEarth;
	GeometryNode* planetMars = new GeometryNode( "Mars", GeometryNode::TORUS ); m_geometryNodes[ "Mars" ] = planetMars;
	GeometryNode* planetJupiter = new GeometryNode( "Jupiter", GeometryNode::DODECAHEDRON ); m_geometryNodes[ "Jupiter" ] = planetJupiter;
	GeometryNode* planetSaturn = new GeometryNode( "Saturn", GeometryNode::OCTAHEDRON ); m_geometryNodes[ "Saturn" ] = planetSaturn;
	GeometryNode* planetUranus = new GeometryNode( "Uranus", GeometryNode::TETRAHEDRON ); m_geometryNodes[ "Uranus" ] = planetUranus;
	GeometryNode* planetNeptune = new GeometryNode( "Neptune", GeometryNode::ICOSAHEDRON ); m_geometryNodes[ "Neptune" ] = planetNeptune;
	GeometryNode* planetPlutoed = new GeometryNode( "Pluto", GeometryNode::SIERPINKSKI_SPONGE ); m_geometryNodes[ "Pluto" ] = planetPlutoed;
#endif

// 	//Only depth-2 node so far.
// 	GeometryNode* moonEarth = new GeometryNode("The Moon", GeometryNode::CYLINDER );
// 

	//----------------------------------------------------------------------------------------------------------
	//TransformNodes per GeometryNode: each planet needs an elliptical orbit, while "rotating about its axis".
	TransformNode* theSunOrbitTransform = new TransformNode( "The Sun's Orbit" );
	EllipticalOrbitStrategy* theSunOrbitStrategy = new EllipticalOrbitStrategy(10.0, 20.0); //Expect just a translation, but below may make it a translation we then rotate around.
	theSunOrbitTransform->setStrategy( theSunOrbitStrategy );
	TransformNode* theSunRotationTransform = new TransformNode( "The Sun's Rotation" );
	theSunRotationTransform->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "The Sun's Rotation" ] = theSunRotationTransform;

	TransformNode* mercuryOrbT = new TransformNode( "Mercury's Orbit" );
	CircularOrbitStrategy* mercuryOrbS = new CircularOrbitStrategy( 3.8, 0.0 );
	mercuryOrbT->setStrategy( mercuryOrbS );
	TransformNode* mercuryRotT = new TransformNode( "Mercury's Rotation" );
	mercuryRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Mercury's Rotation" ] = mercuryRotT;

#ifdef ALL_PLANETS

	TransformNode* venusOrbT = new TransformNode( "Venus' Orbit" );
	CircularOrbitStrategy* venusOrbS = new CircularOrbitStrategy( 7.2, 40.0 );
	venusOrbT->setStrategy( venusOrbS );
	TransformNode* venusRotT = new TransformNode( "Venus' Rotation" );
	venusRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Venus' Rotation" ] = venusRotT;

	TransformNode* earthOrbT = new TransformNode( "Earth's Orbit" );
	CircularOrbitStrategy* earthOrbS = new CircularOrbitStrategy( 10.0, 80.0 );
	earthOrbT->setStrategy( earthOrbS );
	TransformNode* earthRotT = new TransformNode( "Earth's Rotation" );
	earthRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Earth's Rotation" ] = earthRotT;

	TransformNode* marsOrbT = new TransformNode( "Mars' Orbit" );
	CircularOrbitStrategy* marsOrbS = new CircularOrbitStrategy( 15.2, 120.0 );
	marsOrbT->setStrategy( marsOrbS );
	TransformNode* marsRotT = new TransformNode( "Mars' Rotation" );
	marsRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Mars's Rotation" ] = marsRotT;


	TransformNode* jupiterOrbT = new TransformNode( "Jupiter's Orbit" );
	CircularOrbitStrategy* jupiterOrbS = new CircularOrbitStrategy( 52.0, 160.0 );
	jupiterOrbT->setStrategy( jupiterOrbS );
	TransformNode* jupiterRotT = new TransformNode( "Jupiter's Rotation" );
	jupiterRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Jupiter's Rotation" ] = jupiterRotT;


	TransformNode* saturnOrbT = new TransformNode( "Saturn's Orbit" );
	CircularOrbitStrategy* saturnOrbS = new CircularOrbitStrategy( 95.0, 200.0 );
	saturnOrbT->setStrategy( saturnOrbS );
	TransformNode* saturnRotT = new TransformNode( "Saturn's Rotation" );
	saturnRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Saturn's Rotation" ] = saturnRotT;


	TransformNode* uranusOrbT = new TransformNode( "Uranus' Orbit" );
	CircularOrbitStrategy* uranusOrbS = new CircularOrbitStrategy( 192.0, 240.0 );
	uranusOrbT->setStrategy( uranusOrbS );
	TransformNode* uranusRotT = new TransformNode( "Uranus' Rotation" );
	uranusRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Uranus' Rotation" ] = uranusRotT;


	TransformNode* neptuneOrbT = new TransformNode( "Neptune's Orbit" );
	CircularOrbitStrategy* neptuneOrbS = new CircularOrbitStrategy( 300.0, 280.0 );
	neptuneOrbT->setStrategy( neptuneOrbS );
	TransformNode* neptuneRotT = new TransformNode( "Neptune's Rotation" );
	neptuneRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Neptune's Rotation" ] = neptuneRotT;


	TransformNode* plutoOrbT = new TransformNode( "Pluto's Orbit" );
	CircularOrbitStrategy* plutoOrbS = new CircularOrbitStrategy( 395.0, 320.0 );
	marsOrbT->setStrategy( plutoOrbS );
	TransformNode* plutoRotT = new TransformNode( "Pluto's Rotation" );
	plutoRotT->setStrategy( m_defaultRotationStrategy );
	m_rotationNodes[ "Pluto's Rotation" ] = plutoRotT;
#endif

	//----------------------------------------------------------------------------------------------------------
	//Actual additions to scene graph here.
	_root->addChild( theSunOrbitTransform );
	theSunOrbitTransform->addChild( theSunRotationTransform );
	theSunRotationTransform->addChild( theSun );

	theSunOrbitTransform->addChild( mercuryOrbT );
	mercuryOrbT->addChild( mercuryRotT );
	mercuryRotT->addChild( planetMercury );

#ifdef ALL_PLANETS

	theSunOrbitTransform->addChild( venusOrbT );
	venusOrbT->addChild( venusRotT );
	venusRotT->addChild( planetVenus );

	theSunOrbitTransform->addChild( earthOrbT );
	earthOrbT->addChild( earthRotT );
	earthRotT->addChild( planetEarth );


	theSunOrbitTransform->addChild( marsOrbT );
	marsOrbT->addChild( marsRotT );
	marsRotT->addChild( planetMars );

	theSunOrbitTransform->addChild( jupiterOrbT );
	jupiterOrbT->addChild( jupiterRotT );
	jupiterRotT->addChild( planetJupiter );

	theSunOrbitTransform->addChild( saturnOrbT );
	saturnOrbT->addChild( saturnRotT );
	saturnRotT->addChild( planetSaturn );

	theSunOrbitTransform->addChild( uranusOrbT );
	uranusOrbT->addChild( uranusRotT );
	uranusRotT->addChild( planetUranus );

	theSunOrbitTransform->addChild( neptuneOrbT );
	neptuneOrbT->addChild( neptuneRotT );
	neptuneRotT->addChild( planetNeptune );

	theSunOrbitTransform->addChild( plutoOrbT );
	plutoRotT->addChild( planetPlutoed );
	plutoOrbT->addChild( plutoRotT );
#endif

#ifdef INCLUDE_ROBOT_ARM
	//first branch
	TransformNode* rotate1 = new TransformNode("rotate1");
	_root->addChild(rotate1);
	RotateStrategy* rotateStrategy = new RotateStrategy(0.0, 90.0, 0.0);
	rotate1->setStrategy(rotateStrategy);
	GeometryNode* geom1 = new GeometryNode("geom1",GeometryNode::Cylinder);
	rotate1->addChild(geom1);

	//second branch
	TransformNode* transform2 = new TransformNode("transform2");
	rotate1->addChild(transform2);
	TranslateStrategy* translateStrategy2 = new TranslateStrategy(0.0, 0.0, 10.0);
	transform2->setStrategy(translateStrategy2);
	TransformNode* rotate2 = new TransformNode("rotate2");
	transform2->addChild(rotate2);
	RotateStrategy* rotateStrategy2 = new RotateStrategy(45.0, 0.0, 0.0);
	rotate2->setStrategy(rotateStrategy2);
	GeometryNode* geom2 = new GeometryNode("geom2", GeometryNode::Cylinder);
	rotate2->addChild(geom2);


	//The Hand Planet
	TransformNode* transform3 = new TransformNode("transform3");
	rotate2->addChild(transform3);
	TranslateStrategy* translateStrategy3 = new TranslateStrategy(0.0, 0.0, 10.0);
	transform3->setStrategy(translateStrategy3);

	TransformNode* orbit3 = new TransformNode("orbit3");
	transform3->addChild(orbit3);
	OrbitStrategy* orbitStrategy3 = new OrbitStrategy();
	orbit3->setStrategy(orbitStrategy3);
	GeometryNode* geom3 = new GeometryNode("geom3", GeometryNode::Sphere);
	orbit3->addChild(geom3);
#endif

	_trav = new Node::Traversal();
}

myTransformSandBox::~myTransformSandBox(void)
{
	delete m_defaultRotationStrategy;
	delete m_axisAlignedBillboardStrategy;
	delete m_worldAlignedBillboardStrategy;
	delete _trav;
	delete _root;
	Node::checkNodeCount();
}

void myTransformSandBox::GetPositionForNode( const GeometryNode* n, float result[3] ) //ASSUMES INTERNAL NODES ARE TransformNodes!
{
	const Node* currentNode = n;
	Matrix previousCumulativeTransform;
	Matrix newestCumulativeTransform;

	while (true)
	{
		if ( currentNode->GetParent()->GetName() == "root" ) break; //True only at root.
		const TransformNode* parentOfCurrentNode = reinterpret_cast<const TransformNode*>( currentNode->GetParent() );
		previousCumulativeTransform = newestCumulativeTransform;
		mult( previousCumulativeTransform, parentOfCurrentNode->GetTransform( ), newestCumulativeTransform ); //RIGHT/POST-MULTIPLYING
		currentNode = parentOfCurrentNode; //Upward traversal.
	} 

	result[ 0 ] = newestCumulativeTransform._m[ 12 ]; //x.
	result[ 1 ] = newestCumulativeTransform._m[ 13 ]; //y.
	result[ 2 ] = newestCumulativeTransform._m[ 14 ]; //z.
}

void myTransformSandBox::LookFrom( const std::string& fromNodeName )
{
	assert( m_geometryNodes.count( fromNodeName ) > 0 );
	float fromNodePosition[ 3 ];
	GetPositionForNode( m_geometryNodes[ fromNodeName ], fromNodePosition );
	m_camera->SetPosition( fromNodePosition );
}

void myTransformSandBox::LookAt( const std::string& toNodeName )
{
	assert( m_geometryNodes.count( toNodeName ) > 0 );
	float toNodePosition[ 3 ];
	GetPositionForNode( m_geometryNodes[ toNodeName ], toNodePosition );

	//Look At Step 1: get direction to target from camera.
	const float* camPosition = m_camera->GetPosition();
	float camToTargetDirK[ 3 ];
	camToTargetDirK[ 0 ] = camPosition[ 0 ] - toNodePosition[ 0 ]; //x.
	camToTargetDirK[ 1 ] = camPosition[ 1 ] - toNodePosition[ 1 ]; //y.
	camToTargetDirK[ 2 ] = camPosition[ 2 ] - toNodePosition[ 2 ]; //z.
	Normalize( camToTargetDirK, camToTargetDirK );

	//Look At Step 2: get the other orientation basis vectors via cross product operations with the world's up vector (assumed 0,1,0).
	float camDirI[ 3 ];
	CrossProduct( WORLD_UP_VECTOR, camToTargetDirK, camDirI );
	Normalize( camDirI, camDirI );

	float camDirJ[ 3 ];
	CrossProduct( camToTargetDirK, camDirI, camDirJ ); //Operands are orthonormalized, so no norm() needed.

	//Look At Step 3: turn the camera to its new ijk orientation Rc matrix.
	m_camera->SetOrientation( camDirI, camDirJ, camToTargetDirK );
}

void myTransformSandBox::renderAxis( float lenght )
{
	//Use Red, Green, Blue for X Y Z respectively
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	//X axis
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(lenght,0.0f,0.0f);
	//Y axis
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,lenght,0.0f);
	//Z axis
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,lenght);
	glEnd();
}

void myTransformSandBox::ResetRotationStrategiesTo( TransformNode::Strategy* strategy ) const //being hit twice because of problems with WasKeyJustPressed
{
	auto m_rotationNodesEnd = m_rotationNodes.end();
	for ( auto rotationNodeIter = m_rotationNodes.begin(); rotationNodeIter != m_rotationNodesEnd; ++rotationNodeIter )
	{
		TransformNode* currentRotationNode = rotationNodeIter->second;
		currentRotationNode->setStrategy( strategy );
	}
}

/*
ROW-major memory layout: from float[16] to matrix
0  1  2  3   |  0  1  2  3
4  5  6  7   |  4  5  6  7
8  9  10 11  |  8  9  10 11
12 13 14 15  |  12 13 14 15

Column-major:
0 4 8  12
1 5 9  13
2 6 10 14
3 7 11 15
*/

int g_frameCount;

void myTransformSandBox::render()
{
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity(); //Wipe whatever was on matrix stack from last iteration off the GL state.

	//All matrices are ROW (C++) major

	g_frameCount++;

	g_renderNodeAndGeometryNodeBoundingVolumes = ( g_theApp->isKeyDown( 'N' ) );

	g_renderTransformNodeBoundingVolumes = ( g_theApp->isKeyDown( 'T' ) );

	g_renderGeometry = true; // ( g_theApp->isKeyDown( 'G' ) );

	//////////////////////////////////////
	// The View Matrix
	//////////////////////////////////////

	float xDir, yDir, zDir;
	xDir = yDir = zDir = 0.f;
	if ( g_theApp->isKeyDown( 'W' ) ) zDir = -1.0f;
	else if ( g_theApp->isKeyDown( 'S' ) ) zDir = 1.0f;
	if ( g_theApp->isKeyDown( 'A' ) ) xDir = -1.0f;
	else if ( g_theApp->isKeyDown( 'D' ) ) xDir = 1.0f;
	if ( g_theApp->isKeyDown( 'Q' ) ) yDir = -1.0f;
	else if ( g_theApp->isKeyDown( 'E' ) ) yDir = 1.0f;

	float speed = .001f;
	if ( g_theApp->isKeyDown( VK_SPACE ) ) speed = .01f;

	static const float MOVE_BOOST = 5.0f;

	//NOT FRAMERATE-INDEPENDENT!
	m_camera->Translate( xDir * speed * MOVE_BOOST, yDir * speed * MOVE_BOOST, zDir * speed * MOVE_BOOST );

	bool isInFlycam = true;
	float dirOfRotAboutY = 0.f;
	if ( isInFlycam )
	{
		if ( g_theApp->isKeyDown( VK_LEFT ) ) 
			dirOfRotAboutY = -1.0f;
		else if ( g_theApp->isKeyDown( VK_RIGHT ) ) 
			dirOfRotAboutY = 1.0f;
	}
	static float angle = 0.f;
	angle -= speed * dirOfRotAboutY;
	if (angle != 0.f) m_camera->RotateAboutY( angle );

	if ( g_theApp->isKeyDown( '1' ) ) LookFrom( "The Sun" );
	if ( g_theApp->isKeyDown( '2' ) ) LookFrom( "Mercury" );
	if ( g_theApp->isKeyDown( '3' ) ) LookFrom( "Venus" );
	if ( g_theApp->isKeyDown( '4' ) ) LookFrom( "Earth" );
	if ( g_theApp->isKeyDown( '5' ) ) LookFrom( "Mars" );
	if ( g_theApp->isKeyDown( '6' ) ) LookFrom( "Jupiter" );
	if ( g_theApp->isKeyDown( '7' ) ) LookFrom( "Saturn" );
	if ( g_theApp->isKeyDown( '8' ) ) LookFrom( "Uranus" );
	if ( g_theApp->isKeyDown( '9' ) ) LookFrom( "Neptune" );
	if ( g_theApp->isKeyDown( '0' ) ) LookFrom( "Pluto" );

	if ( g_theApp->isKeyDown( VK_F1 ) ) LookAt( "The Sun" );
	if ( g_theApp->isKeyDown( VK_F2 ) ) LookAt( "Mercury" );
	if ( g_theApp->isKeyDown( VK_F3 ) ) LookAt( "Venus" );
	if ( g_theApp->isKeyDown( VK_F4 ) ) LookAt( "Earth" );
	if ( g_theApp->isKeyDown( VK_F5 ) ) LookAt( "Mars" );
	if ( g_theApp->isKeyDown( VK_F6 ) ) LookAt( "Jupiter" );
	if ( g_theApp->isKeyDown( VK_F7 ) ) LookAt( "Saturn" );
	if ( g_theApp->isKeyDown( VK_F8 ) ) LookAt( "Uranus" );
	if ( g_theApp->isKeyDown( VK_F9 ) ) LookAt( "Neptune" );
	if ( g_theApp->isKeyDown( VK_F11 ) ) LookAt( "Pluto" );

	glLoadMatrixf(m_camera->ComputeViewMatrix()._m);
	renderAxis(1000.0f);

	//WasKeyJustPressed requires call prior to render() in ogl_transforms.cpp!
	if ( g_theApp->isKeyDown( 'B' ) /*&& g_theApp->WasKeyJustPressed( 'B' )*/ ) //Replace RotateStrategies with world-aligned BillboardStrategies.
	{
		ResetRotationStrategiesTo( m_worldAlignedBillboardStrategy );
	}
	else if ( g_theApp->isKeyDown( 'Y' ) /*&& g_theApp->WasKeyJustPressed( 'Y' )*/ ) //Replace RotateStrategies with y-axis-aligned BillboardStrategies.
	{
		ResetRotationStrategiesTo( m_axisAlignedBillboardStrategy );
	}
	else if ( g_theApp->isKeyDown( 'R' ) /*&& g_theApp->WasKeyJustPressed( 'R' )*/ ) //Reinstate with RotateStrategies.
	{
		ResetRotationStrategiesTo( m_defaultRotationStrategy );
	}

	g_numberSceneGraphNodesVisited = 0;
	g_numberSphereFrustumIntersectionTestsPerformed = 0;
	g_numberGeometryNodesRendered = 0;

	//Scene Graph Depth-first Traversal #1: advances the _transform of TransformNodes through a single application of their respective strategies.
	_root->update();

	//Scene Graph Depth-first Traversal #2: extend and unify bounding volumes using the parent-child relationships of the scene graph (zero MV-matrix stack use).
	_root->ComputeBoundingSphere();

	//Scene Graph Depth-first Traversal #3: actual view frustum vs bounding volume intersection tests.
	_trav->initToViewMatrix( m_camera->GetViewMatrix() ); //Need view for below model-view matrix stack, since View must start at the bottom for _trav->pushMatrixStack to work.
	_root->cull(m_camera, _trav); //To render, at each TransformNode, we call trav->pushMatrixStack(TransformNode::m_transform), which left-mults and pushes back MV, MMV, etc...


	static float amountOfClippingPlaneToRenderWorldUnits = 1000.f;
	const float zFar = m_camera->m_farClippingPlaneDistance;
#ifdef RENDER_UNTRANSFORMED_FRUSTUM_AS_QUADS
	static const float frustumAlpha = .05f;
	glLoadMatrixf( m_camera->GetViewMatrix()._m );
	glBegin( GL_QUADS );
	{
		//Near.
		glColor4f( 0.f, 1.f, 1.f, frustumAlpha );
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, -amountOfClippingPlaneToRenderWorldUnits, m_camera->m_nearClippingPlaneDistance );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, -amountOfClippingPlaneToRenderWorldUnits, m_camera->m_nearClippingPlaneDistance );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, amountOfClippingPlaneToRenderWorldUnits, m_camera->m_nearClippingPlaneDistance );
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, amountOfClippingPlaneToRenderWorldUnits, m_camera->m_nearClippingPlaneDistance );

		//Far.
		glColor4f( 0.f, 0.f, 1.f, frustumAlpha );
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, -amountOfClippingPlaneToRenderWorldUnits, zFar );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, -amountOfClippingPlaneToRenderWorldUnits, zFar );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, amountOfClippingPlaneToRenderWorldUnits, zFar );
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, amountOfClippingPlaneToRenderWorldUnits, zFar );

		//Left.
		glColor4f( 1.f, 0.f, 1.f, frustumAlpha );
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( zFar*m_camera->m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.x, -amountOfClippingPlaneToRenderWorldUnits, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.z );
		glVertex3f( zFar*m_camera->m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.x, amountOfClippingPlaneToRenderWorldUnits, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.z );
		glVertex3f( 0.f, 0.f, 0.f );

		//Right.
		glColor4f( 1.f, 0.f, 0.f, frustumAlpha );
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( zFar*m_camera->m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.x, -amountOfClippingPlaneToRenderWorldUnits, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.z );
		glVertex3f( zFar*m_camera->m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.x, amountOfClippingPlaneToRenderWorldUnits, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.z );
		glVertex3f( 0.f, 0.f, 0.f );

		//Top.
		glColor4f( 0.f, 1.f, 0.f, frustumAlpha );
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, zFar*m_camera->m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.y, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.z );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, zFar*m_camera->m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.y, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.z );
		glVertex3f( 0.f, 0.f, 0.f );

		//Bottom.
		glColor4f( 1.f, 1.f, 0.f, frustumAlpha );
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, zFar*m_camera->m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.y, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.z );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, zFar*m_camera->m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.y, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.z );
		glVertex3f( 0.f, 0.f, 0.f );
	}
	glEnd();
#endif


#define RENDER_UNTRANSFORMED_FRUSTUM_AS_LINES
#ifdef RENDER_UNTRANSFORMED_FRUSTUM_AS_LINES
	glLoadMatrixf( m_camera->GetViewMatrix()._m );
	const float* camPos = m_camera->GetPosition();
	glTranslatef( camPos[ 0 ], camPos[ 1 ], camPos[ 2 ] );
	glLineWidth( 10.f );
	glColor4f( 1.f, 1.f, 1.f, 1.f );
	glBegin( GL_LINES );
	{
		//Near.
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, 0.f, m_camera->m_nearClippingPlaneDistance );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, 0.f, m_camera->m_nearClippingPlaneDistance );

		//Far.
		glVertex3f( -amountOfClippingPlaneToRenderWorldUnits, 0.f, zFar );
		glVertex3f( amountOfClippingPlaneToRenderWorldUnits, 0.f, zFar );

		//Left.
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( zFar*m_camera->m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.x, 0.f, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane.z );

		//Right.
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( zFar*m_camera->m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.x, 0.f, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane.z );

		//Top.
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( 0.f, zFar*m_camera->m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.y, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane.z );

		//Bottom.
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3f( 0.f, zFar*m_camera->m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.y, -zFar*m_camera->m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane.z );
	}
	glEnd();
	glLineWidth( 1.f );
#endif

#define PRINT_STATS
#ifdef PRINT_STATS
	if ( g_theApp->isKeyDown( 'P' ) )
	{
		std::cout << "g_numberSceneGraphNodesVisited: " << g_numberSceneGraphNodesVisited << std::endl; //31 with all planets.
		std::cout << "g_numberSphereFrustumIntersectionTestsPerformed: " << g_numberSphereFrustumIntersectionTestsPerformed << std::endl;
		std::cout << "g_numberGeometryNodesRendered: " << g_numberGeometryNodesRendered << std::endl; //10 with all planets.
	}
#endif

// 	int input;
// 	std::cin >> input;
}
