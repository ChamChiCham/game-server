#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Struct.h"

struct SShapeData
{
	std::vector<GLfloat> coords;
	std::vector<GLfloat> normals;
	std::vector<unsigned int> indices;
	std::vector<GLfloat> textures;

	SShapeData();
};

class CShapeDataMgr
{
private:
	std::vector<SShapeData> data;
	static CShapeDataMgr* instance;

private:
	CShapeDataMgr();

public:
	void init();

	SShapeData& getData(const int _idx);

	static CShapeDataMgr* getInst();

};

