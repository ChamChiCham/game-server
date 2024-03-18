#include "CShapeDataMgr.h"

#include <fstream>
#include <iostream>
#include <string>
#include <list>

#include "Define.h"

CShapeDataMgr* CShapeDataMgr::instance = nullptr;

SShapeData::SShapeData()
{}


CShapeDataMgr::CShapeDataMgr()
{
	init();
}

void CShapeDataMgr::init()
{
	data.resize(SHAPE_MAX);

	data[SHAPE_SQUARE].coords =
	{
		0.5f,	0.f,	0.5f,
		0.5f,	0.f,	-0.5f,
		-0.5f,	0.f,	0.5f,
		-0.5f,	0.f,	0.5f,
		0.5f,	0.f,	-0.5f,
		-0.5f,	0.f,	-0.5f
	};
	data[SHAPE_SQUARE].normals =
	{
		0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
		0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
	};
	data[SHAPE_SQUARE].textures =
	{
		1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f
	};
}	

SShapeData& CShapeDataMgr::getData(const int _idx)
{
	return data[_idx];
}

CShapeDataMgr* CShapeDataMgr::getInst()
{
	if (!instance) {
		instance = new CShapeDataMgr();
	}
	return instance;
}
