//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	ゲーム処理
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <thread>
#include "CDirectXGraphics.h"
#include "CDirect3DXFile.h"
#include "game.h"
#include "input.h"
#include "mathutil.h"
#include "StageHit.h"
#include "Data.h"
#include "CDebugFont.h"

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
CDirectXGraphics	*g_DXGrobj = nullptr;			// DirectX Graphicsオブジェクト
CDirect3DXFile		*g_DXXFileBall = nullptr;		// Ｘファイルオブジェクト(ボール)
CDirect3DXFile		*g_DXXFileStage = nullptr;		// Ｘファイルオブジェクト(ステージ)
CDirect3DXFile		*g_DXXFileObj_sky = nullptr;	// sky_Ｘファイルオブジェクト

D3DXMATRIX			g_MatView;						// カメラ行列
D3DXMATRIX			g_MatProjection;				// プロジェクション変換行列
D3DXMATRIX			g_MatBall;						// ワールド変換行列
D3DXMATRIX			g_MatBallTotal = {				// 積算行列（単位行列で初期化）
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
D3DXMATRIX			g_MatTotal = {				// 積算行列（単位行列で初期化）
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

D3DXMATRIX			g_MatStage;						// ステージ

HANDLE				g_hEventHandle;					// イベントハンドル
bool				g_EndFlag = false;				// 終了フラグ
std::thread			g_gamemainthread;				// ゲームメインスレッド

D3DXVECTOR3			g_ballangle = { 0.0f, 0.0f, 0.0f };			// ボールの回転角度
D3DXVECTOR3			g_balltrans = { 0.0f, 0.0f, 0.0f };			// 平行移動量

D3DXVECTOR3			g_eye[100];;			// カメラ位置
D3DXVECTOR3			g_lookat[100];			// カメラ注視点
D3DXVECTOR3			g_up[100];				// カメラ上向きベクトル

CDirect3DXFile		*g_XFile[16];	// Ｘファイルオブジェクト
D3DXMATRIX			g_World[16];
D3DXMATRIX			g_pearentChild[16];
extern _XFileData  XFileData[16];
extern _3DObjectInitData	ObjectInitData[16];

static int Num = 0;
CDebugFont g_dbgfont;				//CDebugFont
static int gameoverflg = 0;

struct VERTEX3D{
	float x, y, z;
	// rhw　はいらない（3Dから2Dに変換済みを知らせるもの）
	D3DCOLOR diffuse;
	float tu, tv;
};
LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;			//Direct3Dの各機能へのポインタ
LPDIRECT3DTEXTURE9 g_ui;

//==============================================================================
//!	@fn		GameInit
//!	@brief	ゲーム初期処理
//!	@param	インスタンス値
//!	@param	ウインドウハンドル値
//!	@param	ウインドウサイズ幅（補正済みの値）
//!	@param	ウインドウサイズ高さ（補正済みの値）
//!	@param　	フルスクリーンフラグ　true ; フルスクリーン　false : ウインドウ
//!	@retval	true 成功　false 失敗
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height,bool fullscreen)
{
	bool sts;
	g_DXGrobj = new CDirectXGraphics();	// DirectX Graphicsオブジェクト生成
	sts = g_DXGrobj->Init(hwnd, fullscreen, width, height);	// DirectX Graphicsオブジェクト初期化

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "DirectX 初期化エラー", MB_OK);
		return false;
	}

	// Ｘファイルオブジェクト（ボール）生成
	g_DXXFileBall = new CDirect3DXFile();
	sts = g_DXXFileBall->LoadXFile("assets/ball.x", g_DXGrobj->GetDXDevice());

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "ball.x 読み込みエラー", MB_OK);
		return false;
	}

	// Ｘファイルオブジェクト（ステージ）生成
	g_DXXFileStage = new CDirect3DXFile();
	sts = g_DXXFileStage->LoadXFile("assets/stage1.x", g_DXGrobj->GetDXDevice());

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "stage1.x 読み込みエラー", MB_OK);
		return false;
	}

	//スカイドーム生成
	g_DXXFileObj_sky = new CDirect3DXFile();
	sts = g_DXXFileObj_sky->LoadXFile("assets/skydome.x", g_DXGrobj->GetDXDevice());
	if (!sts){
		MessageBox(hwnd, "ERROR!!", "skydome.x 読み込みエラー", MB_OK);
		return false;
	}

	for (int i = 0; i<16; i++){
		g_XFile[i] = new CDirect3DXFile();
		g_XFile[i]->LoadXFile(XFileData[i].XfileName, g_DXGrobj->GetDXDevice());
	}
	//自分のことだけを考えた行列をつくる
	for (int i = 0; i<16; i++){
		MakeWorldMatrix(g_World[i], ObjectInitData[i].FirstAngle, ObjectInitData[i].FirstPosition);
	}


	// カメラ変換行列作成
	D3DXMatrixLookAtLH(&g_MatView,
		&g_eye[0],				// 視点
		&g_lookat[0],			// 注視点
		&g_up[0]);				// 上向き

	// カメラ行列を固定パイプラインへセット
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &g_MatView);

	// プロジェクション変換行列作成
	D3DXMatrixPerspectiveFovLH(&g_MatProjection,
		D3DX_PI / 2,					// 視野角
		(float)width / (float)height,	// アスペクト比
		1.0f,						// ニアプレーン
		10000.0f);					// ファープレーン

	// スカイドーム用行列を単位行列化
	D3DXMatrixIdentity(&g_MatStage);

	// 射影変換行列を固定パイプラインへセット
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_PROJECTION, &g_MatProjection);
	// Ｚバッファ有効
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ライト有効
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_LIGHTING, true);
	// カリング無効化
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// 環境光セット
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	// DirectT Input初期化
	InitInput(hinst, hwnd);

	//CDebugFontゲーム初期処理
	g_dbgfont.CreateFont(g_DXGrobj->GetDXDevice());

	// 地形との当たり判定初期化処理
	StageHitInit(g_DXXFileStage->GetMesh(), g_DXGrobj->GetDXDevice());

	//D3DPRESENT_PARAMETERS    d3dpp;
	//if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	//{
	//	return E_FAIL;
	//}
	//ZeroMemory(&d3dpp, sizeof(d3dpp));
	//d3dpp.Windowed = TRUE;
	//d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//d3dpp.EnableAutoDepthStencil = TRUE;
	//d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	//if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
	//	D3DDEVTYPE_HAL,
	//	hwnd,
	//	D3DCREATE_MIXED_VERTEXPROCESSING,
	//	&d3dpp,
	//	&g_pd3dDevice)))
	//{
	//	return E_FAIL;
	//}
	////構造体のポインタであるからアロー演算子を使う
	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						//
	//g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//g_pd3dDevice->LightEnable(0, FALSE);
	////テクスチャ読み込み
	//D3DXCreateTextureFromFile(g_pd3dDevice, "assets\\result_logo.png", &g_ui);

	// イベントハンドル生成
	g_hEventHandle = CreateEvent(NULL, false, false, NULL);
	if (g_hEventHandle == NULL){
		MessageBox(hwnd, "ERROR!!", "CreateEvent エラー", MB_OK);
		return false;
	}
	// スレッド生成(ゲームメイン)
	g_gamemainthread = std::thread(GameMain);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	ゲーム入力検知処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameInput(){
	UpdateInput();				// 入力を検知する
}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameUpdate(){

	static int idx=0;
	//static int jumpflg = 0;

	// 回転角度の初期化
	g_ballangle.x = g_ballangle.y = g_ballangle.z = 0.0f;
	//g_balltrans.z = 2.0f;
	//g_ballangle.x += 0.05f;

	if (GetKeyboardPress(DIK_D) && gameoverflg == 0){
		g_ballangle.y += 1.0f;
	}
	if (GetKeyboardPress(DIK_A) && gameoverflg == 0){
		g_ballangle.y -= 1.0f;
	}

	if (GetKeyboardPress(DIK_W) && gameoverflg == 0){
		g_balltrans.z += 0.05f;
		//jumpflg = 10;
	}
	if (GetKeyboardPress(DIK_S) && gameoverflg == 0){
		g_balltrans.z -= 0.05f;
	}
	//if (jumpflg > 0){
	//	g_ballangle.x -= 10.0f;

	//	jumpflg -=1 ;
	//}
	if (GetKeyboardTrigger(DIK_SPACE) && gameoverflg == 1){
		g_balltrans.x = g_balltrans.x = g_balltrans.x = 0.0f;
		g_ballangle.x = g_ballangle.y = g_ballangle.y = 0.0f;
		g_MatBallTotal = {				// 積算行列（単位行列で初期化）
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		gameoverflg = 0;
	}


	// ボール用行列を作成
	MakeWorldMatrix(g_MatBall, g_MatBallTotal, g_ballangle, g_balltrans);

	// ちょっと遅れてくるTPSカメラの計算
	g_up[idx].x = g_MatBallTotal._21;
	g_up[idx].y = g_MatBallTotal._22;
	g_up[idx].z = g_MatBallTotal._23;

	g_lookat[idx].x = g_MatBallTotal._41;
	g_lookat[idx].y = g_MatBallTotal._42;
	g_lookat[idx].z = g_MatBallTotal._43;

	g_eye[idx].x = g_lookat[idx].x - g_MatBallTotal._31 * 80;
	g_eye[idx].y = g_lookat[idx].y - g_MatBallTotal._32 * 80+50;
	g_eye[idx].z = g_lookat[idx].z - g_MatBallTotal._33 * 80;

	int useidx = idx - 20;
	if (useidx < 0)
	{
		useidx = 100 + idx - 20;
	}

	idx++;
	if (idx > 99){
		idx = 0;
	}

	// 全面を検索してポリゴンとの交点を見つける
	static D3DXVECTOR3 ans(0,0,0);
	bool sts = SearchAllSurface(g_MatBallTotal._41, g_MatBallTotal._43, ans);
	if (sts){
	// 交点が見つかったとき
		g_MatBallTotal._42 = ans.y+25;
	}
	else{
	// 交点が見つからなかったとき
		//g_MatBallTotal._42 = ans.y;
		g_MatBallTotal._42 -= 5.0f;
		g_balltrans.z = 0;
	}
	if (g_MatBallTotal._42<-400){
		gameoverflg = 1;
		g_MatBallTotal._42 += 5.0f;

	}





	// カメラ変換行列作成
	D3DXMatrixLookAtLH(&g_MatView,
		&g_eye[useidx],				// 視点
		&g_lookat[useidx],			// 注視点
		&g_up[useidx]);				// 上向き

	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &g_MatView);	// カメラ変換行列をセット


	MakeWorldMatrix(g_World[Num], g_MatTotal, ObjectInitData[Num].FirstAngle, ObjectInitData[Num].FirstPosition);
	//親子関係をつける
	for (int i = 0; i<16; i++){
		g_pearentChild[i] = g_World[i];
		for (int j = ObjectInitData[i].ParentObjectNo; j != -1; j = ObjectInitData[j].ParentObjectNo){

			g_pearentChild[i] *= g_World[j];

		}
	}



}

//==============================================================================
//!	@fn		GameRender
//!	@brief	ゲーム更新処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameRender(){
	HRESULT  hr;

	// ターゲットバッファのクリア、Ｚバッファのクリア
	g_DXGrobj->GetDXDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// 描画の開始をＤＩＲＥＣＴＸに通知
	g_DXGrobj->GetDXDevice()->BeginScene();

	//if (gameoverflg==1){
	//	//--------------------------------------------------------------//

	//	static VERTEX3D vx_UI[4] = {

	//		{ -50, 50, 0, D3DCOLOR_XRGB(255, 255, 255), 0, 0 },//B
	//		{ 50, 50, 0, D3DCOLOR_XRGB(255, 255, 255), 1, 0 },//E
	//		{ -50, -50, 0, D3DCOLOR_XRGB(255, 255, 255), 0, 1 },//A
	//		{ 50, -50, 0, D3DCOLOR_XRGB(255, 255, 255), 1, 1 },//F
	//	};

	//	D3DXMATRIX mxTrans, mxWorld, mxScale;
	//	D3DXMatrixScaling(&mxScale, 2.0f, 2.0f, 2.0f);//拡縮率
	//	D3DXMatrixTranslation(&mxTrans, 0, 0, 0);
	//	//平行移動行列を作る関数     （,x,y,x）
	//	mxWorld = mxTrans;//回転させるときに使う用
	//	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &mxWorld);

	//	D3DXMATRIX mxRot;	//ビルボードの回転
	//	D3DXMATRIX mxWorkView;//ビュー行列コピー用

	//	mxWorkView = g_MatView; //ビュー行列をコピー(この時点で作成済みにしておく)//（LookAtなんたら
	//	mxWorkView._41 = 0;   //行列の中の平行移動部分を０にする（回転を残しつつ平行移動行列を消す
	//	mxWorkView._42 = 0;
	//	mxWorkView._43 = 0;
	//	D3DXMatrixInverse(&mxRot, NULL, &mxWorkView);//"逆行列"を作る関数（回転の逆行列を作りたい
	//	//~~逆行列した結果の変数, , 逆行列させる変数

	//	mxWorld = mxRot * mxTrans;
	//	//~~~~~~ビルボードの平行移動行列をあらかじめ作っておく
	//	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &mxWorld);//ビルボードのワールド変換設定

	//	g_DXGrobj->GetDXDevice()->SetTexture(0, g_ui); //テクスチャ変数
	//	g_DXGrobj->GetDXDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vx_UI, sizeof(VERTEX3D));

	//	//---------------------------------------------------------------------
	//}

	//スカイドーム描画
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatTotal);	// ワールド変換行列をセット
	g_DXXFileObj_sky->Draw(g_DXGrobj->GetDXDevice());				// sky_Ｘファイル描画
	
	// ステージ描画
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatStage);	// ワールド変換行列をセット
	g_DXXFileStage->Draw(g_DXGrobj->GetDXDevice());						// Ｘファイル描画

	// ボールを描画
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatBallTotal);	// ワールド変換行列をセット
	g_DXXFileBall->Draw(g_DXGrobj->GetDXDevice());							// Ｘファイル描画

	//	//モンスター描画処理
	for (int i = 0; i<16; i++){
		g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_pearentChild[i]);	// ワールド変換行列をセット
		g_XFile[i]->Draw(g_DXGrobj->GetDXDevice());
	}

	DebugPrint();
	g_DXGrobj->GetDXDevice()->EndScene();	// 描画の終了を待つ

	hr = g_DXGrobj->GetDXDevice()->Present(NULL, NULL, NULL, NULL);	// バックバッファからプライマリバッファへ転送
	if (hr != D3D_OK){
		g_DXGrobj->GetDXDevice()->Reset(&g_DXGrobj->GetDXD3dpp());
	}
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	ゲームループ処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameMain()
{
	while (1){
		DWORD sts = WaitForSingleObject(g_hEventHandle, 1000);	// イベントフラグがセットされるのを待つ（1000msでタイムアウト）
		if (sts == WAIT_FAILED){
			break;
		}
		else if (sts == WAIT_TIMEOUT){
			if (g_EndFlag){
				break;
			}
			continue;
		}

		GameInput();					// 入力
		GameUpdate();					// 更新
		GameRender();					// 描画
	}
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	ゲーム終了処理
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameExit()
{

	g_gamemainthread.join();					// ゲームメインスレッドの終了を待つ

	CloseHandle(g_hEventHandle);				// イベントハンドルクローズ

	StageHitExit();								// ステージとの当たり判定の終了処理

	if (g_DXXFileBall != nullptr){
		delete g_DXXFileBall;					//	Xファイルオブジェクト(ボール)の解放
	}

	if (g_DXXFileStage != nullptr){
		delete g_DXXFileStage;					//	Xファイルオブジェクト(ステージ)の解放
	}

	if (g_DXGrobj != NULL){
		g_DXGrobj->Exit();						// DirectXのグラフィックオブジェクトの終了処理
	}

	if (g_DXGrobj != NULL){						// DirectXのグラフィックオブジェクトの解放
		delete g_DXGrobj;
		g_DXGrobj = NULL;
	}

	if (g_pd3dDevice != NULL)
	{
		g_pd3dDevice->Release();
	}
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
	}
	//if (g_ui != NULL){
	//	g_ui->Release();
	//}


	// DirectT Input終了処理
	UninitInput();

}

//==============================================================================
//!	@fn		GameSetEvent
//!	@brief	ゲームループ内のイベントオブジェクトをセットする
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameSetEvent(){
	if (!g_EndFlag){
		SetEvent(g_hEventHandle);	// 終了フラグがセットされていなければイベントをセットする
	}
}

//==============================================================================
//!	@fn		GameSetEndFlag
//!	@brief	ゲームループの終了フラグをセットする
//!	@param　	
//!	@retval	なし
//==============================================================================
void GameSetEndFlag(){
	g_EndFlag = true;				// 終了フラグをオンにする
}


//==============================================================================
//!	@fn		DebugPrint
//!	@brief	デバッグ情報を表示する
//!	@param　	
//!	@retval	なし
//==============================================================================
void DebugPrint(){

	D3DXVECTOR3 bspos1, bspos2, bspos3;

	char buf[128];
	
	if (gameoverflg==0){
		sprintf_s(buf, "ステージ上です ");
	}
	else{
		sprintf_s(buf, "ステージから落ちました SPACEで戻る");
	}
	g_dbgfont.DrawTextA(10, 10, buf);

	bspos1 = D3DXVECTOR3(g_MatBallTotal._41, g_MatBallTotal._42, g_MatBallTotal._43);

	sprintf_s(buf, "(X:%4.0f  Y:%4.0f  Z:%4.0f)",  bspos1.x, bspos1.y, bspos1.z);
	g_dbgfont.DrawTextA(10, 40, buf);
	

}

//******************************************************************************
//	End of file.
//******************************************************************************
