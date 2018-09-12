//*****************************************************************************
//!	@file	collision.cpp
//!	@brief	
//!	@note	�����蔻��Ɏg�p���邽�߂̊֐��Q
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<math.h>
#include	"collision.h"

//==============================================================================
//!	@fn		CreatePlaneInfo
//!	@brief�@���ʂ̕����������߂�i���_�͎��v���œn���j
//!	@param	���_�P���W
//!	@param	���_�Q���W
//!	@param	���_�R���W
//!	@param	���ʂ̕�����
//!	@retval	�Ȃ�
//==============================================================================
void CreatePlaneInfo(const D3DXVECTOR3& p1,
	const D3DXVECTOR3& p2,
	const D3DXVECTOR3& p3,
	PLANE& plane)
{
	D3DXVECTOR3		p1p2;
	D3DXVECTOR3		p2p3;
	D3DXVECTOR3		normal;

	p1p2 = p2 - p1;
	p2p3 = p3 - p2;

	D3DXVec3Cross(&normal, &p1p2, &p2p3);		// �O�ς����߂�
	D3DXVec3Normalize(&normal, &normal);		// ������1�ɂ��Ă���
	plane.a = normal.x;
	plane.b = normal.y;
	plane.c = normal.z;

	plane.d = -(plane.a * p1.x + plane.b * p1.y + plane.c * p1.z);
}

//==============================================================================
//!	@fn		LinetoPlaneCross
//!	@brief�@�����ƕ��ʂ̌�_�����߂�
//!	@param	���ʂ̕�����
//!	@param	�����̋N�_
//!	@param	�����̕����x�N�g��
//!	@param	��_���W
//!	@retval	true ��_����@false ��_�Ȃ�
//==============================================================================
bool LinetoPlaneCross(const PLANE&		plane,		//���ʂ̕�����
	const D3DXVECTOR3&	line_o,						//�����̋N�_
	const D3DXVECTOR3&	line_v,						//�����̕����x�N�g��
	D3DXVECTOR3&			ans)					//��_���W
{
	
	float dot = plane.a*line_v.x + plane.b*line_v.y + plane.c*line_v.z;
	
	if (fabs(dot)<FLT_EPSILON)
	{
		return false;
	}
	float t = -(plane.a*line_o.x + plane.b*line_o.y + plane.c*line_o.z + plane.d)/dot;
	ans.x = line_o.x + line_v.x*t;
	ans.y = line_o.y + line_v.y*t;
	ans.z = line_o.z + line_v.z*t;
	return true;


}

//==============================================================================
//!	@fn		CheckInTriangle
//!	@brief�@�R�p�`�̓����ɂ��邩�ǂ����𔻒�
//!	@param	�R�p�`�̑�P���_���W
//!	@param	�R�p�`�̑�Q���_���W
//!	@param	�R�p�`�̑�R���_���W
//!	@param	���肵�������_���W
//!	@retval	true �����@false �O��
//==============================================================================
bool CheckInTriangle(const D3DXVECTOR3&	a,
						const D3DXVECTOR3&	b,
						const D3DXVECTOR3&	c,	
						const D3DXVECTOR3&	p ){
	D3DXVECTOR3  ab,bc,ca;			// �R�ӂ̃x�N�g��
	D3DXVECTOR3  ap,bp,cp;			// �����̓_�Ƃ̃x�N�g��
	D3DXVECTOR3	 normal;			// �R�p�`�̖@���x�N�g��
	D3DXVECTOR3	 n1,n2,n3;			// �R�ӂƓ����̓_�Ƃ̖@���x�N�g��

	ab = b - a;
	bc = c - b;
	ca = a - c;

	ap = p - a;
	bp = p - b;
	cp = p - c;

	D3DXVec3Cross(&normal, &ab, &bc);		// �O�ς��v�Z

	D3DXVec3Cross(&n1, &ab, &ap);			// �O�ς��v�Z
	D3DXVec3Cross(&n2, &bc, &bp);			// �O�ς��v�Z
	D3DXVec3Cross(&n3, &ca, &cp);			// �O�ς��v�Z

	if(D3DXVec3Dot(&n1,&normal) < 0) return false;			// �ׂ��p�x���݊p
	if(D3DXVec3Dot(&n2,&normal) < 0) return false;			// �ׂ��p�x���݊p
	if(D3DXVec3Dot(&n3,&normal) < 0) return false;			// �ׂ��p�x���݊p
	return true;
}

//==============================================================================
//!	@fn		LengthPointToPlane
//!	@brief�@���ʂƓ_�̋��������߂�	�܂������Ɍ�_�̍��W�����߂�
//!	@param	�_�̍��W
//!	@param	���ʂ̕�����
//!	@param	��_�̍��W
//!	@retval	�_�ƕ��ʂ̋���
//==============================================================================
float LengthPointToPlane(const D3DXVECTOR3& p,
							const PLANE& plane,
							D3DXVECTOR3& crosspoint)
{
	float dot_work = plane.a*p.x+plane.b*p.y+plane.c*p.z + plane.d;
	float normal_len = plane.a*plane.a+plane.b*plane.b+plane.c*plane.c;
	
	// ��_�����߂�
	float t = -1.0f * dot_work / normal_len;

	crosspoint.x = p.x + plane.a * t;
	crosspoint.y = p.y + plane.b * t;
	crosspoint.z = p.z + plane.c * t;

	float length = fabsf(dot_work); // �� sqrtf(normal_len);	// ���ʂ̖@���𐳋K�����Ă���̂�sqrt�͏Ȃ��܂���

	return length;
}
//******************************************************************************
//	End of file.
//******************************************************************************
