#pragma once
#include "collision.h"

// �n�`�f�[�^�̖ʂ̕��������
struct PLANEINFO{
	PLANE	plane;				//���ʂ̕�����
	D3DXVECTOR3	p0, p1, p2;		//���_���W
};
void StageHitInit(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice);
void StageHitExit();
void CreateStagePlaneInfoAll(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice);
bool SearchAllSurface(float x, float z, D3DXVECTOR3& ans);
