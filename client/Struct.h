#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>

struct SBuffer
{
	GLuint	VAO;
	GLuint  VBO[3];
	GLuint	EBO;

	SBuffer();

};

struct SView
{
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;

	SView();
};

struct SLight
{
	glm::vec3 pos = glm::vec3(0.f);
	glm::vec3 color = glm::vec3(1.f);

	float& x = pos.x;
	float& y = pos.y;
	float& z = pos.z;
	float& r = color.r;
	float& g = color.g;
	float& b = color.b;
};