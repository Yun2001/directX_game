#pragma once
#include "collision.h"

// 地形データの面の方程式情報
struct PLANEINFO{
	PLANE	plane;				//平面の方程式
	D3DXVECTOR3	p0, p1, p2;		//頂点座標
};
void StageHitInit(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice);
void StageHitExit();
void CreateStagePlaneInfoAll(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice);
bool SearchAllSurface(float x, float z, D3DXVECTOR3& ans);
