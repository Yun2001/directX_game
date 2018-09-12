#pragma once
#define		XFILEMAX		PARTSMAX
#include	<d3d9.h>
#include	<d3dx9.h>
/*--------------------------------------------------------

	�R�c�I�u�W�F�N�g�z�u�\���̒�`

---------------------------------------------------------*/
// �I�u�W�F�N�g��
enum	{
	HIP,
	BODY, 
	HEAD, 
	ARMR0, 
	ARML0, 
	LEGR0, 
	LEGL0, 
	ARMR1, 
	ARML1, 
	LEGR1, 
	LEGL1, 
	WINGR0,
	WINGL0,
	WINGR1,
	WINGL1,
	TAIL0,
	PARTSMAX
};

// �\���̌^�^�O
struct	_3DObjectInitData{
	int			ParentObjectNo;				// �e�I�u�W�F�N�g�m��
	int			ModelNo;					// ���f���ԍ�
	D3DVECTOR	FirstPosition;				// �ŏ��̈ʒu
	D3DVECTOR	FirstAngle;					// �ŏ��̊p�x
};

struct	_XFileData{
	int			ModelNo;					// ���f���ԍ�
	char*		XfileName;					// �w�t�@�C����
};
