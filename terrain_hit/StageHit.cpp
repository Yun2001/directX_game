//*****************************************************************************
//!	@file	StageHit.cpp
//!	@brief	
//!	@note	�X�e�[�W�Ƃ̓����蔻��
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<d3dx9.h>
#include	"StageHit.h"
#include	"collision.h"

//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
PLANEINFO*		g_PlaneInfo=nullptr;
int				g_NumFaces;

//==============================================================================
//!	@fn		StageHitInit
//!	@brief�@�n�`�Ƃ̓����蔻�菉��������
//!	@param	���b�V��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//==============================================================================
void StageHitInit(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice){
	CreateStagePlaneInfoAll(lpMesh,lpdevice);
}

//==============================================================================
//!	@fn		StageHitExit
//!	@brief�@�n�`�Ƃ̓����蔻��I������
//!	@retval	�Ȃ�
//==============================================================================
void StageHitExit(){
	if(g_PlaneInfo != nullptr){
		delete[] g_PlaneInfo;
	}
}

//==============================================================================
//!	@fn		CreateStagePlaneInfoAll
//!	@brief�@�w�肳�ꂽ���b�V���̃|���S���ɂ��ē����蔻����s�����߂̕��ʕ��������𐶐�����
//!	@param	���b�V��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//==============================================================================
void CreateStagePlaneInfoAll(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice)
{
	// ���_�f�[�^�t�H�[�}�b�g
	struct VERTEX{
		float	x, y, z;
	};
	VERTEX*					pVertices;						// ���_���i�[�p

	LPDIRECT3DVERTEXBUFFER9	lpVertexBuffer;					// ���_�o�b�t�@
	LPDIRECT3DINDEXBUFFER9	lpIndexBuffer;					// �C���f�b�N�X�o�b�t�@
	WORD*					pIndices;						// �C���f�b�N�X�ԍ�
	int						idx0, idx1, idx2;				// �R�p�`�|���S���̒��_�C���f�b�N�X�i�[�p

	HRESULT					hr;								// �߂�l
	LPD3DXMESH				clonemesh;						// ���b�V���̃N���[��

	// �w�x�y�̍��W�������܂܂�郁�b�V���N���[�����쐬
	hr = lpMesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ), lpdevice, &clonemesh);		
	if (FAILED(hr)){
		MessageBox(0, "error", "error", MB_OK);
	}
	else{
		clonemesh->GetVertexBuffer(&lpVertexBuffer);		// ���_�o�b�t�@�擾
		clonemesh->GetIndexBuffer(&lpIndexBuffer);		// �C���f�b�N�X�o�b�t�@�擾
		g_NumFaces = clonemesh->GetNumFaces();			// �R�p�`���擾

		lpVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);	// ���_�o�b�t�@���b�N�i���_�f�[�^�擪�A�h���X�擾�j
		lpIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);		// �C���f�b�N�X�o�b�t�@���b�N�i�ʍ\���f�[�^�擪�A�h���X�擾�j

		g_PlaneInfo = new PLANEINFO[g_NumFaces];		// �ʐ����m��

		// �|���S���������[�v
		for (int i = 0; i<g_NumFaces; i++){
			idx0 = *pIndices++;							// �R�p�`�|���S���̒��_�C���f�b�N�X�擾�i�O�Ԗځj
			idx1 = *pIndices++;							// �R�p�`�|���S���̒��_�C���f�b�N�X�擾�i�P�Ԗځj
			idx2 = *pIndices++;							// �R�p�`�|���S���̒��_�C���f�b�N�X�擾�i�Q�Ԗځj

			g_PlaneInfo[i].p0.x = (pVertices + idx0)->x;
			g_PlaneInfo[i].p0.y = (pVertices + idx0)->y;
			g_PlaneInfo[i].p0.z = (pVertices + idx0)->z;

			g_PlaneInfo[i].p1.x = (pVertices + idx1)->x;
			g_PlaneInfo[i].p1.y = (pVertices + idx1)->y;
			g_PlaneInfo[i].p1.z = (pVertices + idx1)->z;
			
			g_PlaneInfo[i].p2.x = (pVertices + idx2)->x;
			g_PlaneInfo[i].p2.y = (pVertices + idx2)->y;
			g_PlaneInfo[i].p2.z = (pVertices + idx2)->z;

			//���ʂ̕��������쐬����
			CreatePlaneInfo(g_PlaneInfo[i].p0, g_PlaneInfo[i].p1, g_PlaneInfo[i].p2, g_PlaneInfo[i].plane);
		}
		lpVertexBuffer->Unlock();		//���_�o�b�t�@���A�����b�N
		lpIndexBuffer->Unlock();		//�C���f�b�N�X�o�b�t�@���A�����b�N

		lpVertexBuffer->Release();		// ���_�o�b�t�@���
		lpIndexBuffer->Release();		// �C���f�b�N�X�o�b�t�@���
	}
	clonemesh->Release();				// �N���[�����b�V�����
}

//==============================================================================
//!	@fn		SearchAllSurface
//!	@brief�@�w�肳�ꂽ���b�V�����ׂĂ̂R�p�`�|���S���ɂ��Ă̌�_�����߂�
//!	@param	�w���W
//!	@param	�y���W
//!	@param	��_
//!	@retval	true ��_���� false ��_�Ȃ�
//==============================================================================
bool SearchAllSurface(float x, float z, D3DXVECTOR3& ans){

	D3DXVECTOR3		DownVector(0, 1, 0);				// �����̕����x�N�g��
	D3DXVECTOR3		CrossPoint;							// ��_
	D3DXVECTOR3		CurrentPosition(x, 0, z);			// ���݈ʒu�i�������Ȃ��j

	bool FindFlag = false;								// �|���S���������������ǂ���
	bool status;										

	// �S�|���S���������[�v
	for (int i = 0; i<g_NumFaces; i++){
		// ���ʂƒ����̌�_�����߂�
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//���ʂ̕�����
			CurrentPosition,							//�����̋N�_�i���̌��݈ʒu�j
			DownVector,									//�����̕����x�N�g��
			CrossPoint);								//��_���W
		if (status){
			// ��_���R�p�`�̓����ɂ��邩�H
			if (CheckInTriangle(g_PlaneInfo[i].p0, g_PlaneInfo[i].p1, g_PlaneInfo[i].p2, CrossPoint)){
				CurrentPosition.y = CrossPoint.y;
				FindFlag = true;
				break;
			}
		}
	}
	if (FindFlag){
		ans = CurrentPosition;
		return true;
	}
	else{
		return false;
	}
}


//******************************************************************************
//	End of file.
//******************************************************************************
