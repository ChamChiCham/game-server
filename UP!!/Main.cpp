﻿#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#include "CShaderMgr.h"
#include "CShapeDataMgr.h"
#include "Define.h"
#include "Struct.h"
#include "Resource.h"


// --
// declare callback function 
// --

namespace cb
{
	GLvoid Display();
	GLvoid Reshape(int w, int h);
	GLvoid GameLoop(int value);
	GLvoid Mouse(int button, int state, int x, int y);
	GLvoid Keyboard(unsigned char key, int x, int y);
	GLvoid SpecialKeys(int key, int x, int y);
	GLvoid SpecialKeysUp(int key, int x, int y);
	GLvoid Motion(int x, int y);
	GLvoid KeyboardUp(unsigned char key, int x, int y);
}




class CWindowMgr
{
private:

	static CWindowMgr* instance;

	CWindowMgr()
	{}
	~CWindowMgr()
	{
		if (!instance) delete instance;
	}

public:

	CWindowMgr(const CWindowMgr& other) = delete;
	CWindowMgr& operator=(const CWindowMgr& other) = delete;

	static CWindowMgr* getInst()
	{
		if (!instance) {
			instance = new CWindowMgr();
		}
		return instance;
	}


private:

	// default member variable
	SView		view;
	glm::mat4	proj = glm::mat4(1.f);
	glm::vec3	background_color = glm::vec3(0.f, 0.f, 0.f);


	// ---
	// process member variable
	// ---

	std::vector<CShape> shapes;
	
	std::pair<int, int> queen_pos{0, 0};
	char queen_status{ 0 };

public:

	// --
	// Basic func
	// --

	void init(int& argc, char** argv)
	{
		// init GLUT
		glutInit(&argc, argv);
		glutInitDisplayMode(WINDOW_DISPLAYMODE);
		glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
		glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
		glutCreateWindow(WINDOW_TITLE);

		// init GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Unable to initialize GLEW" << std::endl;
			exit(EXIT_FAILURE);
		}
		else
			std::cout << "GLEW Initialized\n";

		CShaderMgr::getInst()->init();


		// set cb func
		glutDisplayFunc(cb::Display);
		glutReshapeFunc(cb::Reshape);
		glutMouseFunc(cb::Mouse);
		glutKeyboardFunc(cb::Keyboard);
		glutKeyboardUpFunc(cb::KeyboardUp);
		glutTimerFunc(10, cb::GameLoop, 1);
		glutMotionFunc(cb::Motion);
		glutSpecialFunc(cb::SpecialKeys);
		glutSpecialUpFunc(cb::SpecialKeysUp);

		// --
		// create shape data
		// --
		CShape board;
		board.setData(SHAPE_SQUARE);
		board.setTexture(IMAGE_BOARD);
		board.setColor(1.f, 1.f, 1.f);
		board.scale(0, 2.f, 2.f, 2.f);
		shapes.push_back(board);

		CShape queen;
		queen.setData(SHAPE_SQUARE);
		queen.setColor(1.f, 1.f, 1.f);
		queen.setTexture(IMAGE_QUEEN);
		queen.scale(0, 0.25f, 0.25f, 0.25f);
		queen.translate(1, -0.125f - 0.75f, 1.f, -0.125f - 0.75f);
		shapes.push_back(queen);
		

		for (auto& shape : shapes)
			shape.updateBuffer();

		
		// --
		// set basic variable
		// --


		proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);

		// 초기 카메라 위치 조정
		view.eye = glm::vec3(0.0f, 5.0f, 0.0f);
		view.at = glm::vec3(0.f, 0.f, 0.0f);
		view.up = glm::vec3(0.f, 0.f, -1.f);

		// --
		// explain
		// --

		std::cout << "---------------------" << std::endl;
		std::cout << "커서 키: 말 이동" << std::endl;
		std::cout << "---------------------" << std::endl;

	}

	// --
	// process function
	// --

	// --
	// define cb func
	// --

	void Display()
	{
		glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int mode = GL_TRIANGLES;

		glEnable(GL_DEPTH_TEST);

		for (auto& shape : shapes)
			shape.draw(view, proj, mode);

		glutSwapBuffers();
	}

	void Mouse(const int _button, const int _state, const int _x, const int _y)
	{

	}

	void Motion(const int _x, const int _y)
	{

	}

	void Keyboard(const unsigned char _key, const int _x, const int _y)
	{
	
	}

	void KeyboardUp(const unsigned char _key, const int _x, const int _y)
	{
		
	}

	void SpecialKeys(const int _key, const int _x, const int _y)
	{
		switch (_key)
		{
		case GLUT_KEY_UP:
			if (queen_status & 0x01)
				break;
			queen_status |= 0x01;

			if (queen_pos.second <= 0)
				break;

			queen_pos.second--;
			shapes[1].translate(1, 0.f, 0.f, -0.25f);
			break;

		case GLUT_KEY_DOWN:
			if (queen_status & 0x02)
				break;
			queen_status |= 0x02;

			if (queen_pos.second >= 7)
				break;

			queen_pos.second++;
			shapes[1].translate(1, 0.f, 0.f, 0.25f);
			break;

		case GLUT_KEY_LEFT:
			if (queen_status & 0x04)
				break;
			queen_status |= 0x04;

			if (queen_pos.first <= 0)
				break;

			queen_pos.first--;
			shapes[1].translate(1, -0.25f, 0.f, 0.f);
			break;

		case GLUT_KEY_RIGHT:
			if (queen_status & 0x08)
				break;
			queen_status |= 0x08;

			if (queen_pos.first >= 7)
				break;

			queen_pos.first++;
			shapes[1].translate(1, 0.25f, 0.f, 0.f);
			break;
		}
	}

	void SpecialKeysUp(const int _key, const int _x, const int _y)
	{
		switch (_key)
		{
		case GLUT_KEY_UP:
			queen_status &= ~0x01;
			break;
		case GLUT_KEY_DOWN:
			queen_status &= ~0x02;
			break;
		case GLUT_KEY_LEFT:
			queen_status &= ~0x04;
			break;
		case GLUT_KEY_RIGHT:
			queen_status &= ~0x08;
			break;
		}
	}

	// --
	// process func
	// --


	void updateState()
	{
	}


	void run()
	{
		glutMainLoop();
	}

};

CWindowMgr* CWindowMgr::instance = nullptr;


// (CALLBACK) Display screen
GLvoid cb::Display()
{
	CWindowMgr::getInst()->Display();
}

// (CALLBACK) Reset Viewport
GLvoid cb::Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

// (CALLBACK) Mouse click event
GLvoid cb::Mouse(int button, int state, int x, int y)
{
	CWindowMgr::getInst()->Mouse(button, state, x, y);
}

GLvoid cb::Keyboard(unsigned char key, int x, int y)
{
	CWindowMgr::getInst()->Keyboard(key, x, y);
}

GLvoid cb::Motion(int x, int y)
{
	CWindowMgr::getInst()->Motion(x, y);
}

GLvoid cb::SpecialKeys(int key, int x, int y)
{
	CWindowMgr::getInst()->SpecialKeys(key, x, y);
}

GLvoid cb::SpecialKeysUp(int key, int x, int y)
{
	CWindowMgr::getInst()->SpecialKeysUp(key, x, y);
}

GLvoid cb::KeyboardUp(unsigned char key, int x, int y)
{
	CWindowMgr::getInst()->KeyboardUp(key, x, y);
}

// (CALLBACK) Main Loop
GLvoid cb::GameLoop(int value)
{

	// Game State Update
	CWindowMgr::getInst()->updateState();

	// render (Display 함수 호출)
	glutPostRedisplay();

	glutTimerFunc(10, GameLoop, 1);
}

int main(int argc, char** argv)
{
	CWindowMgr::getInst()->init(argc, argv);
	CWindowMgr::getInst()->run();
}