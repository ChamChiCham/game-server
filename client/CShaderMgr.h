#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Define.h"

class CShaderMgr
{
private:
	static CShaderMgr* instance;
	CShaderMgr();
	~CShaderMgr();
public:
	static CShaderMgr* getInst();


private:
	std::vector<GLuint> programs;


private:
	const bool makeProgram(const char* _vert, const char* _frag);


public:
	void init();
	const GLuint getProgram(const size_t _idx);

};

