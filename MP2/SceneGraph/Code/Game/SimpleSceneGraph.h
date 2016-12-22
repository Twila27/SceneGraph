#pragma once
#include <vector>
#include "Vector3.hpp"
#include <memory>
#include <cassert>
#include <iostream>
#include <string>
#include <cmath>
#include "geometry.h"

#define EMPTY_SPHERE -1.f

extern int g_numberSceneGraphNodesVisited;
extern int g_numberSphereFrustumIntersectionTestsPerformed;
extern int g_numberGeometryNodesRendered;

extern bool g_renderNodeAndGeometryNodeBoundingVolumes;
extern bool g_renderTransformNodeBoundingVolumes;
extern bool g_renderGeometry;

extern double g_perspective_fieldOfView_yDegrees;
extern double g_perspective_aspect;
extern double g_perspective_zFar; 
extern double g_perspective_zNear;


//A simple reference implementation of a scene graph.
//The point was to keep it as simple as possible while 
//demonstrating core elements of the idea behind a scene graph and traversals.
namespace simpleSceneGraph {

	static const float WORLD_UP_VECTOR[ 3 ] = { 0.f, 1.f, 0.f };

	void CrossProduct( const float* l, const float* r, float* result );

	void Normalize( const float* input, float* output );

	//replace with your own matrix class
	struct Matrix {

		Matrix() //Default ctor: Identity matrix.
		{
			_m[0] = 1.0; _m[1] = 0.0; _m[2] = 0.0; _m[3] = 0.0;
			_m[4] = 0.0; _m[5] = 1.0; _m[6] = 0.0; _m[7] = 0.0;
			_m[8] = 0.0; _m[9] = 0.0; _m[10] =1.0; _m[11] =0.0;
			_m[12] = 0.0; _m[13] = 0.0; _m[14] = 0.0; _m[15] = 1.0;

		}

		Matrix(const float* src)
		{
			for (size_t i = 0; i < 16; ++i)
				_m[i] = src[i];
		}

		Matrix&  operator=(const Matrix& rhs)
		{
			_m[0] = rhs._m[0];
			_m[1] = rhs._m[1];
			_m[2] = rhs._m[2];
			_m[3] = rhs._m[3];
			_m[4] = rhs._m[4];
			_m[5] = rhs._m[5];
			_m[6] = rhs._m[6];
			_m[7] = rhs._m[7];
			_m[8] = rhs._m[8];
			_m[9] = rhs._m[9];
			_m[10] = rhs._m[10];
			_m[11] = rhs._m[11];
			_m[12] = rhs._m[12];
			_m[13] = rhs._m[13];
			_m[14] = rhs._m[14];
			_m[15] = rhs._m[15];
			return *this;
		}

		inline friend void mult(const Matrix& l, const Matrix& r, Matrix& res)
		{
			assert(&l != &res && &r != &res);//prevent in-place overwrites
			 //assume that the arguments are valid float[16] buffers
			res._m[0] = l._m[0] * r._m[0] + l._m[1] * r._m[4] + l._m[2] * r._m[8] + l._m[3] * r._m[12];
			res._m[1] = l._m[0] * r._m[1] + l._m[1] * r._m[5] + l._m[2] * r._m[9] + l._m[3] * r._m[13];
			res._m[2] = l._m[0] * r._m[2] + l._m[1] * r._m[6] + l._m[2] * r._m[10] + l._m[3] * r._m[14];
			res._m[3] = l._m[0] * r._m[3] + l._m[1] * r._m[7] + l._m[2] * r._m[11] + l._m[3] * r._m[15];

			res._m[4] = l._m[4] * r._m[0] + l._m[5] * r._m[4] + l._m[6] * r._m[8] + l._m[7] * r._m[12];
			res._m[5] = l._m[4] * r._m[1] + l._m[5] * r._m[5] + l._m[6] * r._m[9] + l._m[7] * r._m[13];
			res._m[6] = l._m[4] * r._m[2] + l._m[5] * r._m[6] + l._m[6] * r._m[10] + l._m[7] * r._m[14];
			res._m[7] = l._m[4] * r._m[3] + l._m[5] * r._m[7] + l._m[6] * r._m[11] + l._m[7] * r._m[15];

			res._m[8] = l._m[8] * r._m[0] + l._m[9] * r._m[4] + l._m[10] * r._m[8] + l._m[11] * r._m[12];
			res._m[9] = l._m[8] * r._m[1] + l._m[9] * r._m[5] + l._m[10] * r._m[9] + l._m[11] * r._m[13];
			res._m[10] = l._m[8] * r._m[2] + l._m[9] * r._m[6] + l._m[10] * r._m[10] + l._m[11] * r._m[14];
			res._m[11] = l._m[8] * r._m[3] + l._m[9] * r._m[7] + l._m[10] * r._m[11] + l._m[11] * r._m[15];

			res._m[12] = l._m[12] * r._m[0] + l._m[13] * r._m[4] + l._m[14] * r._m[8] + l._m[15] * r._m[12];
			res._m[13] = l._m[12] * r._m[1] + l._m[13] * r._m[5] + l._m[14] * r._m[9] + l._m[15] * r._m[13];
			res._m[14] = l._m[12] * r._m[2] + l._m[13] * r._m[6] + l._m[14] * r._m[10] + l._m[15] * r._m[14];
			res._m[15] = l._m[12] * r._m[3] + l._m[13] * r._m[7] + l._m[14] * r._m[11] + l._m[15] * r._m[15];

		}
		const Matrix& GetTranspose()
		{
			Matrix m;
			m = *this;

			//in-place transpose
			std::swap( m._m[ 1 ], m._m[ 4 ] );
			std::swap( m._m[ 2 ], m._m[ 8 ] );
			std::swap( m._m[ 3 ], m._m[ 12 ] );
			std::swap( m._m[ 6 ], m._m[ 9 ] );
			std::swap( m._m[ 7 ], m._m[ 13 ] );
			std::swap( m._m[ 11 ], m._m[ 14 ] );

			return m;
		}
		float  _m[16];
	};

	struct BoundingSphere //Empty for non-geometry nodes.
	{
		BoundingSphere( float radius = EMPTY_SPHERE, Vector3 center = Vector3::ZERO )
			: m_radius( radius ), m_center( center ) {}

		bool IsSphereEmpty() const { return m_radius == EMPTY_SPHERE; }
		BoundingSphere extendToIncludeVertex( const Vector3& vertex );
		BoundingSphere extendToIncludeVertexList( const std::vector<Vector3>& list );
		BoundingSphere extendToIncludeSphere( const BoundingSphere& other);
		BoundingSphere transformByMatrix( const Matrix& m ); 

		float m_radius;
		Vector3 m_center;
	};

	struct Plane { //Dot product of two data members, i.e. dot(N, P0) yields plane equation == 0.
		Vector3 m_unitLengthNormalToThePlane; //"N" in plane equation.
		Vector3 m_pointOnPlane; //"P0" in plane equation.
	};

	enum FrustumSide {
		Near, Far, Top, Bottom, Left, Right, NUM_FRUSTUM_SIDES
	};
	
	class Camera
	{
	public:
		Camera( float position[3], float camRot_y )
		{
			float horizontalFieldOfViewDegrees = m_verticalFieldOfViewDegrees * m_aspectRatio;
			float halfFovX_degrees = horizontalFieldOfViewDegrees / 2.f;
			float halfFovY_degrees = m_verticalFieldOfViewDegrees / 2.f;

			float halfFovX_radians = halfFovX_degrees * ( 3.1415f / 180.f );
			float halfFovY_radians = halfFovY_degrees * ( 3.1415f / 180.f );

			//Picking normals that all face OUTWARDS, NOT INWARDS from frustum. To be dotted against bounding sphere centers in Camera::IsNodeOutsideCameraFrustum() per-plane.
			m_ViewFrustum[ FrustumSide::Near ].m_unitLengthNormalToThePlane = Vector3( 0.f, 0.f, 1.f ); //-z axis is into the screen in this class.
			m_ViewFrustum[ FrustumSide::Near ].m_pointOnPlane = Vector3( 0.f, 0.f, -m_nearClippingPlaneDistance ); //Distance between 0 (when in cam space) and the near plane.

			m_ViewFrustum[ FrustumSide::Far ].m_unitLengthNormalToThePlane = Vector3( 0.f, 0.f, -1.f ); //-z axis is into the screen in this class.
			m_ViewFrustum[ FrustumSide::Far ].m_pointOnPlane = Vector3( 0.f, 0.f, -m_farClippingPlaneDistance ); //Distance between 0 (when in cam space) and the far plane.

			m_ViewFrustum[ FrustumSide::Left ].m_unitLengthNormalToThePlane = Vector3( -cos( halfFovX_radians ), 0.f, sin( halfFovX_radians ) );
			m_ViewFrustum[ FrustumSide::Left ].m_pointOnPlane = Vector3( 0.f, 0.f, 0.f ); //Non-near/far planes will pass through the cam space origin.

			m_ViewFrustum[ FrustumSide::Right ].m_unitLengthNormalToThePlane = Vector3( cos( halfFovX_radians ), 0.f, sin( halfFovX_radians ) );
			m_ViewFrustum[ FrustumSide::Right ].m_pointOnPlane = Vector3( 0.f, 0.f, 0.f ); //Non-near/far planes will pass through the cam space origin.

			m_ViewFrustum[ FrustumSide::Top ].m_unitLengthNormalToThePlane = Vector3( 0.f, cos(halfFovY_radians), sin( halfFovY_radians ) );
			m_ViewFrustum[ FrustumSide::Top ].m_pointOnPlane = Vector3( 0.f, 0.f, 0.f ); //Non-near/far planes will pass through the cam space origin.

			m_ViewFrustum[ FrustumSide::Bottom ].m_unitLengthNormalToThePlane = Vector3( 0.f, -cos( halfFovY_radians ), sin( halfFovY_radians ) );
			m_ViewFrustum[ FrustumSide::Bottom ].m_pointOnPlane = Vector3( 0.f, 0.f, 0.f ); //Non-near/far planes will pass through the cam space origin.


			//Tc's orientation 3x3 is just identity matrix.
			m_Tc._m[ 0 ] = 1.f;		m_Tc._m[ 1 ] = 0.f;		m_Tc._m[ 2 ] = 0.f;		m_Tc._m[ 3 ] = 0.f;
			m_Tc._m[ 4 ] = 0.f;		m_Tc._m[ 5 ] = 1.f;		m_Tc._m[ 6 ] = 0.f;		m_Tc._m[ 7 ] = 0.f;
			m_Tc._m[ 8 ] = 0.f;		m_Tc._m[ 9 ] = 0.f;		m_Tc._m[ 10 ] = 1.f;	m_Tc._m[ 11 ] = 0.f;
			m_Tc._m[ 12 ] = position[ 0 ]; //x.
			m_Tc._m[ 13 ] = position[ 1 ]; //y.
			m_Tc._m[ 14 ] = position[ 2 ]; //z.
			m_Tc._m[ 15 ] = 1.f; //w.

			//Rc's position 1x3 is just zero vector, but orientation is canonical rotation-about-y/j here.
			m_Rc._m[ 0 ] = cos( camRot_y );		m_Rc._m[ 1 ] = 0.f;		m_Rc._m[ 2 ] = -sin( camRot_y );		m_Rc._m[ 3 ] = 0.f;
			m_Rc._m[ 4 ] = 0.f;					m_Rc._m[ 5 ] = 1.f;		m_Rc._m[ 6 ] = 0.f;						m_Rc._m[ 7 ] = 0.f;
			m_Rc._m[ 8 ] = sin( camRot_y );		m_Rc._m[ 9 ] = 0.f;		m_Rc._m[ 10 ] = cos( camRot_y );		m_Rc._m[ 11 ] = 0.f;
			m_Rc._m[ 12 ] = 0.f; 				m_Rc._m[ 13 ] = 0.f; 	m_Rc._m[ 14 ] = 0.f; 					m_Rc._m[ 15 ] = 1.f; //w.

			float Rc[] = {
				cos( camRot_y ), 0.0f, -sin( camRot_y ), 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				sin( camRot_y ), 0.0f, cos( camRot_y ), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
		}

		const float* GetPosition() const { //Returns 1x3 matrix.
			float pos[ 3 ];

			pos[ 0 ] = m_Tc._m[ 12 ];
			pos[ 1 ] = m_Tc._m[ 13 ];
			pos[ 2 ] = m_Tc._m[ 14 ];

			return pos;
		}
		void SetPosition( const float position[ 3 ] )
		{
			m_Tc._m[ 12 ] = position[ 0 ]; //x.
			m_Tc._m[ 13 ] = position[ 1 ]; //y.
			m_Tc._m[ 14 ] = position[ 2 ]; //z.
		}
		void Translate(float x, float y, float z) //Adds to m_Tc.
		{
			m_Tc._m[ 12 ] += x;
			m_Tc._m[ 13 ] += y;
			m_Tc._m[ 14 ] += z;
		}
		
		const float* GetOrientation() const //Returns 3x3 matrix.
		{
			float ori[ 9 ];

			ori[ 0 ] = m_Rc._m[ 0 ];
			ori[ 1 ] = m_Rc._m[ 1 ];
			ori[ 2 ] = m_Rc._m[ 2 ]; //Ic: camera basis i-vector relative to world.
			
			ori[ 3 ] = m_Rc._m[ 4 ];
			ori[ 4 ] = m_Rc._m[ 5 ];
			ori[ 5 ] = m_Rc._m[ 6 ]; //Jc.
			
			ori[ 6 ] = m_Rc._m[ 8 ];
			ori[ 7 ] = m_Rc._m[ 9 ];
			ori[ 8 ] = m_Rc._m[ 10 ]; //Kc.

			return ori;
		}
		void SetOrientation( const float iDir[ 3 ], const float jDir[ 3 ], const float kDir[ 3 ] )
		{
			m_Rc._m[ 0 ] = iDir[ 0 ];	m_Rc._m[ 1 ] = iDir[ 1 ];	m_Rc._m[ 2 ] = iDir[ 2 ];
			m_Rc._m[ 4 ] = jDir[ 0 ];	m_Rc._m[ 5 ] = jDir[ 1 ];	m_Rc._m[ 6 ] = jDir[ 2 ];
			m_Rc._m[ 8 ] = kDir[ 0 ];	m_Rc._m[ 9 ] = kDir[ 1 ];	m_Rc._m[ 10 ] = kDir[ 2 ];
		}
		void RotateAboutY( float angle ) //Subtracts from m_Rc's y-axis.
		{
			//Only need to recalculate our new heading, rest of _m unchanged.
			m_Rc._m[ 0 ] = cos( angle );
			m_Rc._m[ 2 ] = -sin( angle );
			m_Rc._m[ 8 ] = sin( angle );
			m_Rc._m[ 10 ] = cos( angle );
		}

		const Matrix GetViewMatrix() const { return m_View;  }
		const Matrix ComputeViewMatrix()
		{
			//World-to-camera: Rc*Tc, but we want camera-to-world, so take its inverse.
			mult( m_Rc, m_Tc, m_ViewInverse ); //world-to-camera.

			//Inverse for rotation matrix Rc is transpose, because Rc is an orthogonal matrix.
			Matrix inverseRc = m_Rc.GetTranspose();

			//Inverse for translation matrix Tc is negation.
			Matrix inverseTc = m_Tc;
			inverseTc._m[ 12 ] = -m_Tc._m[ 12 ];
			inverseTc._m[ 13 ] = -m_Tc._m[ 13 ];
			inverseTc._m[ 14 ] = -m_Tc._m[ 14 ];

			//Inverse(Rc*Tc) == Inverse(Tc)*Inverse(Rc).
			mult( inverseTc, inverseRc, m_View ); //camera-to-world.

			return GetViewMatrix();
		}

		bool IsNodeOutsideCameraFrustum( const BoundingSphere& nodeBoundingSphere ) const;

	public:
		const float m_nearClippingPlaneDistance = g_perspective_zNear;
		const float m_farClippingPlaneDistance = g_perspective_zFar;
		const float m_verticalFieldOfViewDegrees = g_perspective_fieldOfView_yDegrees;
		const float m_aspectRatio = g_perspective_aspect; //screen width divide by height
		Plane m_ViewFrustum[ NUM_FRUSTUM_SIDES ]; //Expressed relative to camera space/origin.

	private:
		Matrix m_Tc; //Camera position in world.
		Matrix m_Rc; //Camera orientation in world.
		Matrix m_View; //Inverse(Tc)*Inverse(Rc) == -Tc * transpose(Rc).
		Matrix m_ViewInverse; //Rc*Tc.
	};


	class Node {
	public:

		typedef std::vector<Node*> Children;
		typedef Children::const_iterator const_iterator_child;

		BoundingSphere m_boundingSphere;

		struct Traversal {
			Matrix              _view;
			//matrix              _invView; //could also be handy for other algorithms
			std::vector<Matrix> _stack;

			void    initToViewMatrix(const Matrix& view)
			{
				_stack.erase(_stack.begin(), _stack.end()); //Wipe matrix stack.
				_stack.push_back(view); //Start with just the View transform.
			}

			void    pushMatrixStack(const Matrix& modelMatrix) //KEY: HOW THE MODEL-VIEW STACK REALLY TICKS!
			{
				//Mv because this is the current Model(s)*View matrix.
				Matrix mv = _stack.back(); //Initially just View transform! Back() treated as "top" of stack.
				_stack.push_back(mv); //Looks like cloning, but is overwritten by next line.
				mult(modelMatrix, mv, _stack.back()); //KEY MATH OPERATION: LEFT/PRE-MULTIPLIES param*mv!
			}

			void popMatrixStack()
			{
				_stack.pop_back();
			}

			const Matrix& topMatrixStack() const { return _stack.back(); }
		};

		Node(const char* name) : _name(name), _parent(nullptr)
		{ 
			s_nodeCount++; //bookkeeping
		}

		//NOTE: Virtual Destructor for the base class.
		//See Scott Meyers, Effective C++, Item 7
		virtual ~Node()
		{
			destroy();
			assert(s_nodeCount >= 1);  s_nodeCount--;//bookkeeping
		}

		virtual BoundingSphere ComputeBoundingSphere();

		void addChild(Node* child) 
		{
			child->setParent(this);
			_children.push_back(child); 
		}

		virtual void update() //not const-correct, changes scene graph.
		{
			Children::const_iterator it = _children.begin(), ite = _children.end();
			for (; it != ite; ++it)
				(*it)->update();

		}

		virtual void cull( const Camera* camera, Traversal* trav );

		const std::string& name() const { return _name; }

		const_iterator_child   begin_child() const { return _children.begin();  }

		const_iterator_child   end_child()  const { return _children.end();  }
		
		//For bookkeeping purposes, to show that shared pointers cleanup automatically!
		static size_t getNodeCount()
		{
			return s_nodeCount;
		}

		void destroy()
		{
			Children::iterator itc = _children.begin(), itce = _children.end();
			for (; itc != itce; ++itc) {
				(*itc)->destroy();
				delete *itc;
			}
			_children.clear();
		}

		static void checkNodeCount() { assert(s_nodeCount == 0); }

		const Node* GetParent() const { return _parent; }
		const std::string& GetName() const { return _name; }

	protected:
		bool m_isNotSeenByViewFrustum;
		void    setParent(Node* parent) { _parent = parent; }
		void RenderBoundingSphere( Traversal* trav );
		Node*           _parent;
		Children        _children;
		std::string     _name;
		//static variables are BAD for mulithreading, btw.
		static size_t s_nodeCount;
	};

	class TransformNode : public Node {
	public:

		//The Strategy Design pattern
		class Strategy {
		public:
			Strategy(){}
			virtual ~Strategy(){}

			virtual void update(Matrix& m) = 0;
		};

		TransformNode(const char* name) : Node(name) , _strategy(nullptr) {}

		~TransformNode(){}

		void setStrategy(Strategy* s) { _strategy = s; }
		void DeleteStrategy();

		virtual void update()
		{
			if(_strategy)
				_strategy->update(_transform);
			Node::update();
		}

		virtual void cull( const Camera* camera, Traversal* trav );

		const Matrix& GetTransform() const { return _transform; }

		virtual BoundingSphere ComputeBoundingSphere();

	protected:
		Matrix  _transform;
		Strategy*  _strategy;
	};

	class GeometryNode : public Node {
	public:

		enum Type { TEAPOT, CUBE, SPHERE, CONE, TORUS, DODECAHEDRON, OCTAHEDRON, TETRAHEDRON, ICOSAHEDRON, SIERPINKSKI_SPONGE, CYLINDER };

		GeometryNode(const char* name, Type type = TEAPOT) : Node(name), _type(type) {}

		virtual BoundingSphere ComputeBoundingSphere() override;

		virtual void cull( const Camera* camera, Traversal* trav )
		{
			//std::cout << "GeometryNode::cull " << _name << std::endl;
			Node::cull(camera, trav); //Will handle bounding volume test, bounding volume rendering, and ++#nodesVisited's.
			if ( m_isNotSeenByViewFrustum ) return;

			if ( g_renderGeometry ) RenderGeometry( trav );
			g_numberGeometryNodesRendered++;
		}

		void RenderGeometry( Traversal* trav ) const
		{
			glLoadMatrixf( trav->topMatrixStack()._m );
			GLdouble offsetXYZ[ 3 ] = { 0.0, 0.0, 0.0 };
			double earthSize = 2.0;
			switch ( _type )
			{
			case TEAPOT:
				glColor3f( 1.0f, 1.0f, 0.0 ); //Yellow sun.
				renderWireTeapot( .75 * earthSize );
				break;

			case CUBE:
				glColor3f( 0.62f, 0.26f, 0.13f ); //Dark orange mercury.
				renderWireCube( .38 * earthSize );
				break;

			case SPHERE:
				glColor3f( 0.75f, 0.19f, 0.10f ); //Red-orange venus.
				renderWireSphere( .5 * .95 * earthSize, 10.0, 10.0 );
				break;

			case CONE:
				glColor3f( 0.0f, 0.0f, 1.0f ); //Blue earth.
				renderWireCone( earthSize, earthSize, 10.0, 10.0 );
				break;

			case TORUS:
				glColor3f( 0.83f, 0.64f, 0.59f ); //Pale, barely orange mars.
				renderWireTorus( .27 * earthSize, .53 * earthSize, 10.0, 10.0 );
				break;

			case DODECAHEDRON:
				glColor3f( 0.74f, 0.68f, 0.53f ); //Light, sandy jupiter.
				renderWireDodecahedron();
				break;

			case OCTAHEDRON:
				glColor3f( 0.90f, 0.67f, 0.35f ); //Golden saturn.
				renderWireOctahedron();
				break;

			case TETRAHEDRON:
				glColor3f( 0.78f, 0.90f, 1.0f ); //Light pale blue uranus.
				renderWireTetrahedron();
				break;

			case ICOSAHEDRON:
				glColor3f( 0.39f, 0.56f, 0.87f ); //Dark pale blue neptune.
				renderWireIcosahedron();
				break;

			case CYLINDER:
				glColor3f( 0.52f, 0.51f, 0.52f ); //Dark, sandy pluto.
				renderWireCylinder( .5 * earthSize, 10.0, 10.0, 10.0 );
				break;

			case SIERPINKSKI_SPONGE:
				glColor3f( 0.72f, 0.72f, 0.72f ); //Gray moon.
				renderWireSierpinskiSponge( 5.0, offsetXYZ, 2.0 );
				break;

			default:
				;
			}
		}

		BoundingSphere GetBoundingSphereForGeometry( GeometryNode::Type type );

	protected:
		Type    _type;
	};

	//Some sample transformation strategies, to play with
	class RotateStrategy : public TransformNode::Strategy {
	public:
		const double degreesToRad = (3.14159 / 180.0);
		RotateStrategy( double pitch = 0.0, double roll = 0.0, double initialHeading = 0.0 ) 
			: h( initialHeading * degreesToRad ), p( pitch * degreesToRad ), r( roll * degreesToRad ) {}

		~RotateStrategy()
		{
			std::cout << "RotateStrategy::~RotateStrategy" << std::endl;
		}

		virtual void update(Matrix& m)
		{
			//Impose a spin?
			h += (rand() / (float)RAND_MAX) * .01; //About the y-axis.
// 			p += .01;
// 			r += .01;

			Matrix z;
			//Rotation about Z axis
			z._m[0] = cos(r);		z._m[1] = -sin(r);		z._m[2] = 0.0;		z._m[3] = 0.0;
			z._m[4] = sin(r);		z._m[5] = cos(r);		z._m[6] = 0.0;		z._m[7] = 0.0;
			z._m[8] = 0.0;			z._m[9] = 0.0;			z._m[10] = 1.0;		z._m[11] = 0.0;
			z._m[12] = 0.0;			z._m[13] = 0.0;			z._m[14] = 0.0;		z._m[15] = 1.0;

			Matrix y;
			//Rotation about y axis
			y._m[0] = cos(h);		y._m[1] = 0.0;			y._m[2] = sin(h);	y._m[3] = 0.0;
			y._m[4] = 0.0;			y._m[5] = 1.0;			y._m[6] = 0.0;		y._m[7] = 0.0;
			y._m[8] = -sin(h);		y._m[9] = 0.0;			y._m[10] = cos(h);	y._m[11] = 0.0;
			y._m[12] = 0.0;			y._m[13] = 0.0;			y._m[14] = 0.0;		y._m[15] = 1.0;

			Matrix x;
			//Rotation about x axis
			x._m[0] = 1.0;			x._m[1] = 0.0;			x._m[2] = 0.0;		x._m[3] = 0.0;
			x._m[4] = 0.0;			x._m[5] = cos(p);		x._m[6] = -sin(p);	x._m[7] = 0.0;
			x._m[8] = 0.0;			x._m[9] = sin(p);		x._m[10] = cos(p);	x._m[11] = 0.0;
			x._m[12] = 0.0;			x._m[13] = 0.0;			x._m[14] = 0.0;		x._m[15] = 1.0;

			Matrix yx;
			mult(y, x, yx);
			mult(z, yx, m);
		}
	private:
		double h;
		double p;
		double r;
	};

	class TranslateStrategy : public TransformNode::Strategy {
	public:
		TranslateStrategy(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

		~TranslateStrategy()
		{
			std::cout << "TranslateStrategy::~TranslateStrategy" << std::endl;
		}

		virtual void update(Matrix& m)
		{
			m._m[0] = 1.0;  m._m[1] =      m._m[2] =       m._m[3] = 0.0;
			m._m[4] = 0.0;  m._m[5] = 1.0; m._m[6] = 0.0;  m._m[7] = 0.0;
			m._m[8] = 0.0;  m._m[9] = 0.0; m._m[10] = 1.0; m._m[11] = 0.0;
			m._m[12] = m_x;  m._m[13] = m_y;  m._m[14] = m_z; m._m[15] = 1.0;
		}
	private:
		double  m_x;
		double  m_y;
		double  m_z;
	};

	class CircularOrbitStrategy : public TransformNode::Strategy {
	public:
		CircularOrbitStrategy( double radius = 1.0, double initialAngle = 0.0 ) : _angle( initialAngle ), _radius( radius ) {}

		~CircularOrbitStrategy( )
		{
			std::cout << "CircularOrbitStrategy::~CircularOrbitStrategy" << std::endl;
		}

		virtual void update( Matrix& m )
		{
			POINT pos;
			GetCursorPos( &pos );
			_angle += pos.x * 0.00001;
			//std::cout << "CircularOrbitStrategy::update with angle=" << _angle << std::endl;
			//Say, orbit around y (in XZ)
			m._m[ 0 ] = 1.0;  m._m[ 1 ] = m._m[ 2 ] = m._m[ 3 ] = 0.0;
			m._m[ 4 ] = 0.0;  m._m[ 5 ] = 1.0; m._m[ 6 ] = 0.0;  m._m[ 7 ] = 0.0;
			m._m[ 8 ] = 0.0;  m._m[ 9 ] = 0.0; m._m[ 10 ] = 1.0; m._m[ 11 ] = 0.0;
			m._m[ 12 ] = _radius*cos( _angle );  m._m[ 13 ] = 0.0;  m._m[ 14 ] = _radius*sin( _angle ); m._m[ 15 ] = 1.0;
		}
	private:
		double  _angle;
		double  _radius;
	};

	class EllipticalOrbitStrategy : public TransformNode::Strategy {
	public:
		EllipticalOrbitStrategy( double radiusInX = 1.0, double radiusInZ = 2.0, double initialAngle = 0.0 )
			: _angle( initialAngle ), _radiusInX( radiusInX ), _radiusInZ( radiusInZ ) {}

		~EllipticalOrbitStrategy( )
		{
			std::cout << "EllipticalOrbitStrategy::~EllipticalOrbitStrategy" << std::endl;
		}

		virtual void update( Matrix& m )
		{
			POINT pos;
			GetCursorPos( &pos );
			_angle += pos.y * 0.00001;
			//std::cout << "EllipticalOrbitStrategy::update with angle=" << _angle << std::endl;
			//Say, orbit around y (in XZ)
			m._m[ 0 ] = 1.0;  m._m[ 1 ] = m._m[ 2 ] = m._m[ 3 ] = 0.0;
			m._m[ 4 ] = 0.0;  m._m[ 5 ] = 1.0; m._m[ 6 ] = 0.0;  m._m[ 7 ] = 0.0;
			m._m[ 8 ] = 0.0;  m._m[ 9 ] = 0.0; m._m[ 10 ] = 1.0; m._m[ 11 ] = 0.0;
			m._m[ 12 ] = _radiusInX*cos( _angle );  m._m[ 13 ] = 0.0;  m._m[ 14 ] = _radiusInZ*sin( _angle ); m._m[ 15 ] = 1.0;
		}
	private:
		double  _angle;
		double  _radiusInX;
		double  _radiusInZ;
	};

	class BillboardStrategy : public TransformNode::Strategy
	{
	public:
		BillboardStrategy( Camera* sceneCamera, bool isConstrainedToAxisY ) : m_sceneCamera(sceneCamera), m_isAxisAligned(isConstrainedToAxisY) {}
		~BillboardStrategy()
		{
			std::cout << "EllipticalOrbitStrategy::~EllipticalOrbitStrategy" << std::endl;
		}

		virtual void update( Matrix& m ) //Run billboard algorithm code.
		{
			float I_bb[ 3 ], J_bb[ 3 ], K_bb[ 3 ];

			if ( m_isAxisAligned == false ) //World-aligned: tilt all on axes to face camera.
			{
				const float* camPos = m_sceneCamera->GetPosition();

				K_bb[ 0 ] = camPos[ 0 ] - m._m[ 12 ];
				K_bb[ 1 ] = camPos[ 1 ] - m._m[ 13 ];
				K_bb[ 2 ] = camPos[ 2 ] - m._m[ 14 ];
				Normalize( K_bb, K_bb );

				CrossProduct( WORLD_UP_VECTOR, K_bb, I_bb );
				Normalize( I_bb, I_bb );

				CrossProduct( K_bb, I_bb, J_bb );

			}
			if ( m_isAxisAligned == true ) //View/cam direction is no longer necessarily our z/k-axis.
			{
				
				const float* camPos = m_sceneCamera->GetPosition();

				float cameraViewingDirection[ 3 ];
				cameraViewingDirection[ 0 ] = camPos[ 0 ] - m._m[ 12 ];
				cameraViewingDirection[ 1 ] = camPos[ 1 ] - m._m[ 13 ];
				cameraViewingDirection[ 2 ] = camPos[ 2 ] - m._m[ 14 ];
				Normalize( cameraViewingDirection, cameraViewingDirection );

				J_bb[ 0 ] = WORLD_UP_VECTOR[ 0 ];
				J_bb[ 1 ] = WORLD_UP_VECTOR[ 1 ];
				J_bb[ 2 ] = WORLD_UP_VECTOR[ 2 ]; //given to be y-axis in this class.

				CrossProduct( J_bb, cameraViewingDirection, I_bb );
				Normalize( I_bb, I_bb );

				CrossProduct( I_bb, J_bb, K_bb );
			}

			//Assign ijk_bb to method param.
			m._m[ 0 ] = I_bb[ 0 ];		m._m[ 1 ] = I_bb[ 1 ];		m._m[ 2 ] = I_bb[ 2 ];
			m._m[ 4 ] = J_bb[ 0 ];		m._m[ 5 ] = J_bb[ 1 ];		m._m[ 6 ] = J_bb[ 2 ];
			m._m[ 8 ] = K_bb[ 0 ];		m._m[ 9 ] = K_bb[ 1 ];		m._m[ 10 ] = K_bb[ 2 ];
		}

	private:
		Camera* m_sceneCamera;
		bool m_isAxisAligned;
	};

}//namespace 
