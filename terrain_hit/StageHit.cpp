//*****************************************************************************
//!	@file	StageHit.cpp
//!	@brief	
//!	@note	ステージとの当たり判定
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<d3dx9.h>
#include	"StageHit.h"
#include	"collision.h"

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
PLANEINFO*		g_PlaneInfo=nullptr;
int				g_NumFaces;

//==============================================================================
//!	@fn		StageHitInit
//!	@brief　地形との当たり判定初期化処理
//!	@param	メッシュ
//!	@param	デバイスオブジェクト
//!	@retval	なし
//==============================================================================
void StageHitInit(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice){
	CreateStagePlaneInfoAll(lpMesh,lpdevice);
}

//==============================================================================
//!	@fn		StageHitExit
//!	@brief　地形との当たり判定終了処理
//!	@retval	なし
//==============================================================================
void StageHitExit(){
	if(g_PlaneInfo != nullptr){
		delete[] g_PlaneInfo;
	}
}

//==============================================================================
//!	@fn		CreateStagePlaneInfoAll
//!	@brief　指定されたメッシュのポリゴンについて当たり判定を行うための平面方程式情報を生成する
//!	@param	メッシュ
//!	@param	デバイスオブジェクト
//!	@retval	なし
//==============================================================================
void CreateStagePlaneInfoAll(const LPD3DXMESH lpMesh, const LPDIRECT3DDEVICE9	lpdevice)
{
	// 頂点データフォーマット
	struct VERTEX{
		float	x, y, z;
	};
	VERTEX*					pVertices;						// 頂点情報格納用

	LPDIRECT3DVERTEXBUFFER9	lpVertexBuffer;					// 頂点バッファ
	LPDIRECT3DINDEXBUFFER9	lpIndexBuffer;					// インデックスバッファ
	WORD*					pIndices;						// インデックス番号
	int						idx0, idx1, idx2;				// ３角形ポリゴンの頂点インデックス格納用

	HRESULT					hr;								// 戻り値
	LPD3DXMESH				clonemesh;						// メッシュのクローン

	// ＸＹＺの座標だけが含まれるメッシュクローンを作成
	hr = lpMesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ), lpdevice, &clonemesh);		
	if (FAILED(hr)){
		MessageBox(0, "error", "error", MB_OK);
	}
	else{
		clonemesh->GetVertexBuffer(&lpVertexBuffer);		// 頂点バッファ取得
		clonemesh->GetIndexBuffer(&lpIndexBuffer);		// インデックスバッファ取得
		g_NumFaces = clonemesh->GetNumFaces();			// ３角形数取得

		lpVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);	// 頂点バッファロック（頂点データ先頭アドレス取得）
		lpIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);		// インデックスバッファロック（面構成データ先頭アドレス取得）

		g_PlaneInfo = new PLANEINFO[g_NumFaces];		// 面数分確保

		// ポリゴン数分ループ
		for (int i = 0; i<g_NumFaces; i++){
			idx0 = *pIndices++;							// ３角形ポリゴンの頂点インデックス取得（０番目）
			idx1 = *pIndices++;							// ３角形ポリゴンの頂点インデックス取得（１番目）
			idx2 = *pIndices++;							// ３角形ポリゴンの頂点インデックス取得（２番目）

			g_PlaneInfo[i].p0.x = (pVertices + idx0)->x;
			g_PlaneInfo[i].p0.y = (pVertices + idx0)->y;
			g_PlaneInfo[i].p0.z = (pVertices + idx0)->z;

			g_PlaneInfo[i].p1.x = (pVertices + idx1)->x;
			g_PlaneInfo[i].p1.y = (pVertices + idx1)->y;
			g_PlaneInfo[i].p1.z = (pVertices + idx1)->z;
			
			g_PlaneInfo[i].p2.x = (pVertices + idx2)->x;
			g_PlaneInfo[i].p2.y = (pVertices + idx2)->y;
			g_PlaneInfo[i].p2.z = (pVertices + idx2)->z;

			//平面の方程式を作成する
			CreatePlaneInfo(g_PlaneInfo[i].p0, g_PlaneInfo[i].p1, g_PlaneInfo[i].p2, g_PlaneInfo[i].plane);
		}
		lpVertexBuffer->Unlock();		//頂点バッファをアンロック
		lpIndexBuffer->Unlock();		//インデックスバッファをアンロック

		lpVertexBuffer->Release();		// 頂点バッファ解放
		lpIndexBuffer->Release();		// インデックスバッファ解放
	}
	clonemesh->Release();				// クローンメッシュ解放
}

//==============================================================================
//!	@fn		SearchAllSurface
//!	@brief　指定されたメッシュすべての３角形ポリゴンについての交点を求める
//!	@param	Ｘ座標
//!	@param	Ｚ座標
//!	@param	交点
//!	@retval	true 交点あり false 交点なし
//==============================================================================
bool SearchAllSurface(float x, float z, D3DXVECTOR3& ans){

	D3DXVECTOR3		DownVector(0, 1, 0);				// 直線の方向ベクトル
	D3DXVECTOR3		CrossPoint;							// 交点
	D3DXVECTOR3		CurrentPosition(x, 0, z);			// 現在位置（高さ情報なし）

	bool FindFlag = false;								// ポリゴンが見つかったかどうか
	bool status;										

	// 全ポリゴン数分ループ
	for (int i = 0; i<g_NumFaces; i++){
		// 平面と直線の交点を求める
		status = LinetoPlaneCross(g_PlaneInfo[i].plane,	//平面の方程式
			CurrentPosition,							//直線の起点（球の現在位置）
			DownVector,									//直線の方向ベクトル
			CrossPoint);								//交点座標
		if (status){
			// 交点が３角形の内部にあるか？
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
