#include "Resource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// -----
// CShape member function
// -----

CShape::~CShape()
{}

void CShape::setUniform(const SView& _view, const glm::mat4& _proj, const GLuint _program)
{

	unsigned int objColorLocation = glGetUniformLocation(_program, "objectColor");
	glUniform3f(objColorLocation, color.r, color.g, color.b);

	unsigned int viewPosLocation = glGetUniformLocation(_program, "viewPos");
	glUniform3f(viewPosLocation, _view.eye.x, _view.eye.y, _view.eye.z);

	glm::mat4 MM = glm::mat4(1.f);
	for (auto& mat : mats) {
		MM = mat * MM;
	}

	glm::mat4 MV = glm::lookAt(_view.eye, _view.at, _view.up);
	glm::mat4 MP = _proj;

	// uniform matrix 
	unsigned int transformLocation = glGetUniformLocation(_program, "model");
	unsigned int viewLocation = glGetUniformLocation(_program, "view");
	unsigned int projectionLocation = glGetUniformLocation(_program, "projection");

	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &MM[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &MP[0][0]);


}

void CShape::drawBuffer(const int _mode)
{
	glBindVertexArray(buffer.VAO);

	if (data->indices.empty()) {
		glDrawArrays(_mode, 0, static_cast<GLsizei>(data->coords.size() / 3));
	}
	else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
		glDrawElements(_mode, static_cast<GLsizei>(data->indices.size()), GL_UNSIGNED_INT, 0);
	}
}

void CShape::updateBuffer()
{
	if (data == nullptr) {
		std::cerr << "data isn't set" << std::endl;
		return;
	}

	if (texture == -1) {
		std::cerr << "texture isn't set" << std::endl;
		return;
	}

	if (buffer.VAO != 0) {
		glDeleteVertexArrays(1, &buffer.VAO);
		glDeleteBuffers(3, buffer.VBO);
	}

	glGenBuffers(3, buffer.VBO);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, data->coords.size() * sizeof(GLfloat), data->coords.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, data->normals.size() * sizeof(GLfloat), data->normals.data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, data->textures.size() * sizeof(GLfloat), data->textures.data(), GL_STATIC_DRAW);


	glGenVertexArrays(1, &buffer.VAO);
	glBindVertexArray(buffer.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(2);
}


void CShape::setTexture(std::string_view _fileName)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int widthImage, heightImage, numberOfChannel;
	unsigned char* data = stbi_load(_fileName.data(), &widthImage, &heightImage, &numberOfChannel, 0);

	GLenum format;
	if (numberOfChannel == 3)
		format = GL_RGB;
	else if (numberOfChannel == 4)
		format = GL_RGBA;
	else {
		stbi_image_free(data);
		return;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, widthImage, heightImage, 0, format, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	this->texture = texture;
}

void CShape::setData(const int _shape)
{
	data = &CShapeDataMgr::getInst()->getData(_shape);
}

void CShape::draw(const SView& _view, const glm::mat4& _proj, const int _mode)
{
	GLuint _program = CShaderMgr::getInst()->getProgram(SHADER_TEXTURE_PROGRAM);
	glUseProgram(_program);

	setUniform(_view, _proj, _program);

	glBindTexture(GL_TEXTURE_2D, texture);

	drawBuffer(_mode);
}

void CShape::scale(const int _idx, const float _fir, const float _sec, const float _thi)
{
	scale(_idx, glm::vec3(_fir, _sec, _thi));
}

void CShape::scale(const int _idx, const glm::vec3 _vec)
{
	while (mats.size() <= _idx) {
		mats.push_back(glm::mat4(1.f));
	}

	glm::mat4 m = glm::scale(glm::mat4(1.f), _vec);
	mats[_idx] = m * mats[_idx];
}

void CShape::rotate(const int _idx, const float _deg, const float _fir, const float _sec, const float _thi)
{
	rotate(_idx, _deg, glm::vec3(_fir, _sec, _thi));
}

void CShape::rotate(const int _idx, const float _deg, const glm::vec3 _vec)
{
	while (mats.size() <= _idx) {
		mats.push_back(glm::mat4(1.f));
	}

	glm::mat4 m = glm::rotate(glm::mat4(1.f), glm::radians(_deg), _vec);
	mats[_idx] = m * mats[_idx];
}

void CShape::rotate(const int _idx, const float _x, const float _y, const float _z, const float _deg, const float _fir, const float _sec, const float _thi)
{
	rotate(_idx, glm::vec3(_x, _y, _z), _deg, glm::vec3(_fir, _sec, _thi));
}

void CShape::rotate(const int _idx, const glm::vec3 _axis, const float _deg, const glm::vec3 _vec)
{
	while (mats.size() <= _idx) {
		mats.push_back(glm::mat4(1.f));
	}

	glm::mat4 m = glm::mat4(1.f);
	m = glm::translate(glm::mat4(1.f), -_axis) * m;
	m = glm::rotate(glm::mat4(1.f), glm::radians(_deg), _vec) * m;
	m = glm::translate(glm::mat4(1.f), _axis) * m;
	mats[_idx] = m * mats[_idx];
}

void CShape::translate(const int _idx, const float _fir, const float _sec, const float _thi)
{
	translate(_idx, glm::vec3(_fir, _sec, _thi));
}

void CShape::translate(const int _idx, const glm::vec3 _vec)
{
	while (mats.size() <= _idx) {
		mats.push_back(glm::mat4(1.f));
	}
	glm::mat4 m = glm::translate(glm::mat4(1.f), _vec);
	mats[_idx] = m * mats[_idx];
}

void CShape::setColor(const glm::vec3& _color)
{
	color = _color;
}

void CShape::setColor(const float _r, const float _g, const float _b)
{
	setColor(glm::vec3(_r, _g, _b));
}

void CShape::clearMatrix(const int _idx)
{
	if (mats.size() <= _idx)
		return;
	mats[_idx] = glm::mat4(1.f);
}

void CShape::setMatrix(CShape& _other)
{
	mats = _other.mats;
}

Queen::Queen()
{
	shape.setData(SHAPE_SQUARE);
	shape.setColor(1.f, 1.f, 1.f);
	shape.setTexture(IMAGE_QUEEN);
	shape.scale(0, 0.25f, 0.25f, 0.25f);
	shape.translate(1, -0.125f - 0.75f, 1.f, -0.125f - 0.75f);
	shape.updateBuffer();
}

void Queen::draw(const SView& _view, const glm::mat4& _proj, const int _mode)
{
	shape.draw(_view,_proj,_mode);
}

void Queen::move(const unsigned char x, const unsigned char y)
{
	shape.clearMatrix(2);
	shape.translate(2, 0.25f * static_cast<float>(x), 0.f, 0.25 * static_cast<float>(y));
}
