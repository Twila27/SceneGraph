T: Show TransformNode Bounding Volumes (when in TransformNode::cull() )
N: Show Node Bounding Volumes (when in Node::cull() as also called by GeometryNode::cull() )
P: Print statistics (currently all nodes get visited to show unseen/culled bounding spheres as red and seen bounding spheres as green).
R/B/Y: switch strategies of the nodes between normal rotations, world-aligned Billboarding, and Y-axis aligned billboarding.
Mouse: top-left cursor position will freeze all node translation/orbits.
	Moving along the +y axis downward will increase the speed of the sun's translation/orbit.
	Moving along the +x axis rightward will increase the speed of the planets' translation/orbit around the sun.

//Phase 1: Parent-Child Scene Graph Rendering via Model-View Matrix Stack
The scene graph is structured, up through from Mercury to Pluto:
	Root
		Sun's Orbit
			Sun's Rotation
				Sun's Geometry
			Mercury's Orbit
				Mercury's Rotation
					Mercury's Geometry
			Venus' Orbit
				...
			...
			
//Phase 2: View-Frustum Culling the Scene Graph
To test view-frustum culling: expanding the window from its initial size will let the left/right edges be slightly outside the culling frustum.
	It can be verified that the bounding volumes return "outside frustum" during intersection check in camera space by the 'T' key.
	This happens because the aspect ratio of 4:3 works for the initial 640x480 size, but not the 16:9 of 1920x1080 as Alienware m17 r2's have.
	The unimplemented optimization: if the node lies completely inside all frustum planes, auto-return true for the tests of all its children nodes w/o full testing.
		Not really so handy for a scene graph like this one that emphasizes breadth over depth.