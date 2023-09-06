#pragma once


#include "Mesh.hpp"

class RectPrism {
public:
	RectPrism();
	~RectPrism();

	void setMesh(Mesh *_mesh);

private:
	Mesh *m_mesh;
};