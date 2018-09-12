//*****************************************************************************
//!	@file	CDirect3DXFile.cpp
//!	@brief	X�t�@�C���ǂݍ��݁A�`��N���X
//!	@note	
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	"../DirectX/CDirect3DXFile.h"

//==============================================================================
//!	@fn		LoadXFile
//!	@brief	X�t�@�C���ǂݍ���
//!	@param	X�t�@�C����
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	TRUE�@�����I��/FALSE�@���s�I��
//!	@note	
//==============================================================================
bool CDirect3DXFile::LoadXFile( char *xfilename, LPDIRECT3DDEVICE9 lpd3ddevice )
{
	LPD3DXBUFFER	pd3dxmtrlbuffer;
	LPD3DXBUFFER	pd3dxmtrlbuffer2;
	HRESULT			hr;
	unsigned int	i;

	// �w�t�@�C����ǂݍ���
	hr = D3DXLoadMeshFromX(	xfilename,				// �w�t�@�C����
							D3DXMESH_SYSTEMMEM,		// �ǂݍ��݃�����
							lpd3ddevice,			// �f�o�C�X�I�u�W�F�N�g
							NULL,
							&pd3dxmtrlbuffer,		// �}�e���A���o�b�t�@
							&pd3dxmtrlbuffer2,		// �}�e���A���o�b�t�@2
							&m_nummaterial,			// �}�e���A����
							&m_lpmesh);				// ���b�V��

	if( SUCCEEDED(hr) ){
		D3DXMATERIAL	*d3dxmaterials = (D3DXMATERIAL*)pd3dxmtrlbuffer->GetBufferPointer();	// �}�e���A���̃A�h���X���擾
		m_lpmeshmaterials = new D3DMATERIAL9[m_nummaterial];		// �}�e���A���R���e�i�̍쐬
		m_lpmeshtextures = new LPDIRECT3DTEXTURE9[m_nummaterial];	// �e�N�X�`���R���e�i�̐���
		
		for( i=0 ; i<m_nummaterial ; i++ ){
			m_lpmeshmaterials[i] = d3dxmaterials[i].MatD3D;
			m_lpmeshmaterials[i].Emissive = m_lpmeshmaterials[i].Specular = m_lpmeshmaterials[i].Ambient = m_lpmeshmaterials[i].Diffuse;

			hr = D3DXCreateTextureFromFile(	lpd3ddevice,
											d3dxmaterials[i].pTextureFilename,
											&m_lpmeshtextures[i]);
			if( FAILED(hr) ){
				m_lpmeshtextures[i]=NULL;
			}
		}
		pd3dxmtrlbuffer->Release();		// �}�e���A���o�b�t�@�̃����[�X
		pd3dxmtrlbuffer2->Release();	// �}�e���A���o�b�t�@�̃����[�X
	}else{
		return false;
	}
	return true;
}

//==============================================================================
//!	@fn		UnLoadXFile
//!	@brief	X�t�@�C���I�u�W�F�N�g�A�����[�h
//!	@param	
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::UnLoadXFile()
{
	unsigned int i;

	if(m_lpmeshmaterials!=NULL){	// �}�e���A���I�u�W�F�N�g�̉��
		delete [] m_lpmeshmaterials;
		m_lpmeshmaterials=NULL;
	}

	if(m_lpmeshtextures!=NULL){		// �e�N�X�`���I�u�W�F�N�g�̉��
		for( i=0 ; i<m_nummaterial ; i++ ){
			if(m_lpmeshtextures[i]!=NULL)	m_lpmeshtextures[i]->Release();
		}
		delete [] m_lpmeshtextures;
		m_lpmeshtextures=NULL;
	}

	if(m_lpmesh!=NULL){		// ���b�V�����
		m_lpmesh->Release();
		m_lpmesh=NULL;
	}
}

//==============================================================================
//!	@fn		Draw
//!	@brief	X�t�@�C���`��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::Draw(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;
	for( i=0 ; i<m_nummaterial ; i++ ){
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// �}�e���A���̃Z�b�g
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// �e�N�X�`���̃Z�b�g
		m_lpmesh->DrawSubset(i);							// �T�u�Z�b�g�̕`��
	}
}

//==============================================================================
//!	@fn		DrawWithAxis
//!	@brief	X�t�@�C���`��(3����`�悷��)
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;

	DrawAxis(lpd3ddevice);	// �R���̕`��

	for(i=0;i<m_nummaterial;i++){	// �T�u�Z�b�g�̕`��
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// �}�e���A���̃Z�b�g
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// �e�N�X�`���̃Z�b�g
		m_lpmesh->DrawSubset(i);							// �T�u�Z�b�g�̕`��
	}
}

//==============================================================================
//!	@fn		DrawAxis
//!	@brief	3����`�悷��
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
void CDirect3DXFile::DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	struct VERTEXDATA{
		float		x,y,z;
		D3DCOLOR	color;
	};

	// �R���̒��_�f�[�^
	struct	VERTEXDATA	linevertices[6]={
		-400,   0,   0,  D3DCOLOR_XRGB(255,0,0),	// �w���n�_ ��
		 400,   0,   0,  D3DCOLOR_XRGB(255,0,0),// �w���I�_ ��
		   0,-400,   0,  D3DCOLOR_XRGB(0,255,0),	// �x���n�_ ��
		   0, 400,   0,  D3DCOLOR_XRGB(0,255,0),// �x���I�_ ��
		   0,   0,-400,  D3DCOLOR_XRGB(0,0,255),	// �y���n�_ ��
		   0,   0, 400,  D3DCOLOR_XRGB(0,0,255)	// �y���I�_ ��
	};

	lpd3ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);	// ���_�t�H�[�}�b�g���Z�b�g
	lpd3ddevice->SetRenderState(D3DRS_LIGHTING, false);	//   �����v�Z�I�t�i�����v�Z��؂��DIFFUSE�F�����̂܂܂ł�j

	lpd3ddevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, &linevertices[0], sizeof(VERTEXDATA));	// �R����`��
	lpd3ddevice->SetRenderState(D3DRS_LIGHTING,true);	//   �����v�ZON
}

//==============================================================================
//!	@fn		GetMesh
//!	@brief	���b�V���I�u�W�F�N�g�|�C���^��߂�
//!	@param	�f�o�C�X�I�u�W�F�N�g
//!	@retval	�Ȃ�
//!	@note	
//==============================================================================
LPD3DXMESH CDirect3DXFile::GetMesh() const{
	return m_lpmesh;
}

//******************************************************************************
//	End of file.
//******************************************************************************