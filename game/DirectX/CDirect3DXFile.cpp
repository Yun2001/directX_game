//*****************************************************************************
//!	@file	CDirect3DXFile.cpp
//!	@brief	Xファイル読み込み、描画クラス
//!	@note	
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	"../DirectX/CDirect3DXFile.h"

//==============================================================================
//!	@fn		LoadXFile
//!	@brief	Xファイル読み込み
//!	@param	Xファイル名
//!	@param	デバイスオブジェクト
//!	@retval	TRUE　成功終了/FALSE　失敗終了
//!	@note	
//==============================================================================
bool CDirect3DXFile::LoadXFile( char *xfilename, LPDIRECT3DDEVICE9 lpd3ddevice )
{
	LPD3DXBUFFER	pd3dxmtrlbuffer;
	LPD3DXBUFFER	pd3dxmtrlbuffer2;
	HRESULT			hr;
	unsigned int	i;

	// Ｘファイルを読み込む
	hr = D3DXLoadMeshFromX(	xfilename,				// Ｘファイル名
							D3DXMESH_SYSTEMMEM,		// 読み込みメモリ
							lpd3ddevice,			// デバイスオブジェクト
							NULL,
							&pd3dxmtrlbuffer,		// マテリアルバッファ
							&pd3dxmtrlbuffer2,		// マテリアルバッファ2
							&m_nummaterial,			// マテリアル数
							&m_lpmesh);				// メッシュ

	if( SUCCEEDED(hr) ){
		D3DXMATERIAL	*d3dxmaterials = (D3DXMATERIAL*)pd3dxmtrlbuffer->GetBufferPointer();	// マテリアルのアドレスを取得
		m_lpmeshmaterials = new D3DMATERIAL9[m_nummaterial];		// マテリアルコンテナの作成
		m_lpmeshtextures = new LPDIRECT3DTEXTURE9[m_nummaterial];	// テクスチャコンテナの生成
		
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
		pd3dxmtrlbuffer->Release();		// マテリアルバッファのリリース
		pd3dxmtrlbuffer2->Release();	// マテリアルバッファのリリース
	}else{
		return false;
	}
	return true;
}

//==============================================================================
//!	@fn		UnLoadXFile
//!	@brief	Xファイルオブジェクトアンロード
//!	@param	
//!	@retval	なし
//!	@note	
//==============================================================================
void CDirect3DXFile::UnLoadXFile()
{
	unsigned int i;

	if(m_lpmeshmaterials!=NULL){	// マテリアルオブジェクトの解放
		delete [] m_lpmeshmaterials;
		m_lpmeshmaterials=NULL;
	}

	if(m_lpmeshtextures!=NULL){		// テクスチャオブジェクトの解放
		for( i=0 ; i<m_nummaterial ; i++ ){
			if(m_lpmeshtextures[i]!=NULL)	m_lpmeshtextures[i]->Release();
		}
		delete [] m_lpmeshtextures;
		m_lpmeshtextures=NULL;
	}

	if(m_lpmesh!=NULL){		// メッシュ解放
		m_lpmesh->Release();
		m_lpmesh=NULL;
	}
}

//==============================================================================
//!	@fn		Draw
//!	@brief	Xファイル描画
//!	@param	デバイスオブジェクト
//!	@retval	なし
//!	@note	
//==============================================================================
void CDirect3DXFile::Draw(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;
	for( i=0 ; i<m_nummaterial ; i++ ){
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// マテリアルのセット
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// テクスチャのセット
		m_lpmesh->DrawSubset(i);							// サブセットの描画
	}
}

//==============================================================================
//!	@fn		DrawWithAxis
//!	@brief	Xファイル描画(3軸を描画する)
//!	@param	デバイスオブジェクト
//!	@retval	なし
//!	@note	
//==============================================================================
void CDirect3DXFile::DrawWithAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	unsigned int i;

	DrawAxis(lpd3ddevice);	// ３軸の描画

	for(i=0;i<m_nummaterial;i++){	// サブセットの描画
		lpd3ddevice->SetMaterial(&m_lpmeshmaterials[i]);	// マテリアルのセット
		lpd3ddevice->SetTexture(0,m_lpmeshtextures[i]);		// テクスチャのセット
		m_lpmesh->DrawSubset(i);							// サブセットの描画
	}
}

//==============================================================================
//!	@fn		DrawAxis
//!	@brief	3軸を描画する
//!	@param	デバイスオブジェクト
//!	@retval	なし
//!	@note	
//==============================================================================
void CDirect3DXFile::DrawAxis(LPDIRECT3DDEVICE9 lpd3ddevice)
{
	struct VERTEXDATA{
		float		x,y,z;
		D3DCOLOR	color;
	};

	// ３軸の頂点データ
	struct	VERTEXDATA	linevertices[6]={
		-400,   0,   0,  D3DCOLOR_XRGB(255,0,0),	// Ｘ軸始点 赤
		 400,   0,   0,  D3DCOLOR_XRGB(255,0,0),// Ｘ軸終点 赤
		   0,-400,   0,  D3DCOLOR_XRGB(0,255,0),	// Ｙ軸始点 緑
		   0, 400,   0,  D3DCOLOR_XRGB(0,255,0),// Ｙ軸終点 緑
		   0,   0,-400,  D3DCOLOR_XRGB(0,0,255),	// Ｚ軸始点 青
		   0,   0, 400,  D3DCOLOR_XRGB(0,0,255)	// Ｚ軸終点 青
	};

	lpd3ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);	// 頂点フォーマットをセット
	lpd3ddevice->SetRenderState(D3DRS_LIGHTING, false);	//   光源計算オフ（光源計算を切るとDIFFUSE色がそのままでる）

	lpd3ddevice->DrawPrimitiveUP(D3DPT_LINELIST, 3, &linevertices[0], sizeof(VERTEXDATA));	// ３軸を描画
	lpd3ddevice->SetRenderState(D3DRS_LIGHTING,true);	//   光源計算ON
}

//==============================================================================
//!	@fn		GetMesh
//!	@brief	メッシュオブジェクトポインタを戻す
//!	@param	デバイスオブジェクト
//!	@retval	なし
//!	@note	
//==============================================================================
LPD3DXMESH CDirect3DXFile::GetMesh() const{
	return m_lpmesh;
}

//******************************************************************************
//	End of file.
//******************************************************************************