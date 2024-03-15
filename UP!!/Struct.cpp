#include "Struct.h"

SBuffer::SBuffer() :
	VAO{ 0 },
	VBO{ 0, 0, 0 },
	EBO{ 0 }
{}

SView::SView() :
	eye{ glm::vec3(0.f, 0.f, 0.f) },
	at { glm::vec3(0.f, 0.f, 0.f) },
	up { glm::vec3(0.f, 0.f, 0.f) }
{}
