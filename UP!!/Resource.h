#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <list>
#include <random>

#include "CShaderMgr.h"
#include "CShapeDataMgr.h"
#include "Define.h"
#include "Struct.h"

class CShape
{
private:
	SBuffer		buffer;
	SShapeData*	data = nullptr;
	glm::vec3	color = glm::vec3(1.f);
	std::vector<glm::mat4>	mats;
	unsigned int texture = -1;

public:
	virtual ~CShape();


protected:
	void setUniform(const SView& _view, const glm::mat4& _proj, const GLuint _program);
	void drawBuffer(const int _mode);

public:

	void updateBuffer();
	
	void setTexture(std::string_view _filename);
	void setData(const int _shape);

	void draw(const SView& _view, const glm::mat4& _proj, const int _mode);

	void scale(const int _idx, const float _fir, const float _sec, const float _thi);
	void scale(const int _idx, const glm::vec3 _vec);

	void rotate(const int _idx, const float _deg, const float _fir, const float _sec, const float _thi);
	void rotate(const int _idx, const float _deg, const glm::vec3 _vec);
	void rotate(const int _idx, const float _x, const float _y, const float _z, const float _deg,
		const float _fir, const float _sec, const float _thi);
	void rotate(const int _idx, const glm::vec3 _axis, const float _deg, const glm::vec3 _vec);

	void translate(const int _idx, const float _fir, const float _sec, const float _thi);
	void translate(const int _idx, const glm::vec3 _vec);

	void setColor(const glm::vec3& _color);
	void setColor(const float _r, const float _g, const float _b);

	void clearMatrix(const int _idx);
	void setMatrix(CShape& _other);
};