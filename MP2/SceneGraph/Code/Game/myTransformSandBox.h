#ifndef MYTRANSFORMSANDBOX_H
#define MYTRANSFORMSANDBOX_H
#include "SimpleSceneGraph.h"
#include <map>
#include <string>

class myTransformSandBox
{
public:
	myTransformSandBox(void);
	~myTransformSandBox(void);

	//using the Strategy design pattern
	virtual void render();

protected:

	void GetPositionForNode( const simpleSceneGraph::GeometryNode* n, float result[3] );
	void LookFrom( const std::string& fromNodeName );
	void LookAt( const std::string& toNodeName );
	void renderAxis( float lenght );
	void ResetRotationStrategiesTo( simpleSceneGraph::TransformNode::Strategy* strategy ) const;
	simpleSceneGraph::Node* _root;
	simpleSceneGraph::Node::Traversal* _trav;

private:
	std::map<std::string, simpleSceneGraph::GeometryNode* > m_geometryNodes;
	std::map<std::string, simpleSceneGraph::TransformNode* > m_rotationNodes;
	simpleSceneGraph::Camera* m_camera;

	simpleSceneGraph::TransformNode::Strategy* m_worldAlignedBillboardStrategy;
	simpleSceneGraph::TransformNode::Strategy* m_axisAlignedBillboardStrategy;
	simpleSceneGraph::TransformNode::Strategy* m_defaultRotationStrategy;
};

#endif //MYTRANSFORMSANDBOX_H