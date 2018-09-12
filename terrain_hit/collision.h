#pragma once
#include	<d3dx9.h>

// ���ʂ̕�����
struct	PLANE{
	float	a,b,c,d;			// ���ʕ������̌W���i ax+by+cz+d=0 )
};

// �v���g�^�C�v�錾
void CreatePlaneInfo(const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,const D3DXVECTOR3& p3 ,PLANE& plane);

bool LinetoPlaneCross(	const PLANE& plane,		//���ʂ̕�����
						const D3DXVECTOR3& line_o,	//�������ʂ�_
						const D3DXVECTOR3& line_v,	//�����̕����x�N�g��
						D3DXVECTOR3& ans);		//��_���W

bool CheckInTriangle(	const D3DXVECTOR3& a,
						const D3DXVECTOR3& b,
						const D3DXVECTOR3& c,	
						const D3DXVECTOR3& p);

float LengthPointToPlane(const D3DXVECTOR3& p, const PLANE& plane, D3DXVECTOR3& crosspoint);
