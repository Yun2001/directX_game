//*****************************************************************************
//!	@file	game.cpp
//!	@brief	
//!	@note	�Q�[������
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
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
CDirectXGraphics	*g_DXGrobj = nullptr;			// DirectX Graphics�I�u�W�F�N�g
CDirect3DXFile		*g_DXXFileBall = nullptr;		// �w�t�@�C���I�u�W�F�N�g(�{�[��)
CDirect3DXFile		*g_DXXFileStage = nullptr;		// �w�t�@�C���I�u�W�F�N�g(�X�e�[�W)
CDirect3DXFile		*g_DXXFileObj_sky = nullptr;	// sky_�w�t�@�C���I�u�W�F�N�g

D3DXMATRIX			g_MatView;						// �J�����s��
D3DXMATRIX			g_MatProjection;				// �v���W�F�N�V�����ϊ��s��
D3DXMATRIX			g_MatBall;						// ���[���h�ϊ��s��
D3DXMATRIX			g_MatBallTotal = {				// �ώZ�s��i�P�ʍs��ŏ������j
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
D3DXMATRIX			g_MatTotal = {				// �ώZ�s��i�P�ʍs��ŏ������j
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

D3DXMATRIX			g_MatStage;						// �X�e�[�W

HANDLE				g_hEventHandle;					// �C�x���g�n���h��
bool				g_EndFlag = false;				// �I���t���O
std::thread			g_gamemainthread;				// �Q�[�����C���X���b�h

D3DXVECTOR3			g_ballangle = { 0.0f, 0.0f, 0.0f };			// �{�[���̉�]�p�x
D3DXVECTOR3			g_balltrans = { 0.0f, 0.0f, 0.0f };			// ���s�ړ���

D3DXVECTOR3			g_eye[100];;			// �J�����ʒu
D3DXVECTOR3			g_lookat[100];			// �J���������_
D3DXVECTOR3			g_up[100];				// �J����������x�N�g��

CDirect3DXFile		*g_XFile[16];	// �w�t�@�C���I�u�W�F�N�g
D3DXMATRIX			g_World[16];
D3DXMATRIX			g_pearentChild[16];
extern _XFileData  XFileData[16];
extern _3DObjectInitData	ObjectInitData[16];

static int Num = 0;
CDebugFont g_dbgfont;				//CDebugFont
static int gameoverflg = 0;

struct VERTEX3D{
	float x, y, z;
	// rhw�@�͂���Ȃ��i3D����2D�ɕϊ��ς݂�m�点����́j
	D3DCOLOR diffuse;
	float tu, tv;
};
LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;			//Direct3D�̊e�@�\�ւ̃|�C���^
LPDIRECT3DTEXTURE9 g_ui;

//==============================================================================
//!	@fn		GameInit
//!	@brief	�Q�[����������
//!	@param	�C���X�^���X�l
//!	@param	�E�C���h�E�n���h���l
//!	@param	�E�C���h�E�T�C�Y���i�␳�ς݂̒l�j
//!	@param	�E�C���h�E�T�C�Y�����i�␳�ς݂̒l�j
//!	@param�@	�t���X�N���[���t���O�@true ; �t���X�N���[���@false : �E�C���h�E
//!	@retval	true �����@false ���s
//==============================================================================
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height,bool fullscreen)
{
	bool sts;
	g_DXGrobj = new CDirectXGraphics();	// DirectX Graphics�I�u�W�F�N�g����
	sts = g_DXGrobj->Init(hwnd, fullscreen, width, height);	// DirectX Graphics�I�u�W�F�N�g������

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "DirectX �������G���[", MB_OK);
		return false;
	}

	// �w�t�@�C���I�u�W�F�N�g�i�{�[���j����
	g_DXXFileBall = new CDirect3DXFile();
	sts = g_DXXFileBall->LoadXFile("assets/ball.x", g_DXGrobj->GetDXDevice());

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "ball.x �ǂݍ��݃G���[", MB_OK);
		return false;
	}

	// �w�t�@�C���I�u�W�F�N�g�i�X�e�[�W�j����
	g_DXXFileStage = new CDirect3DXFile();
	sts = g_DXXFileStage->LoadXFile("assets/stage1.x", g_DXGrobj->GetDXDevice());

	if (!sts){
		MessageBox(hwnd, "ERROR!!", "stage1.x �ǂݍ��݃G���[", MB_OK);
		return false;
	}

	//�X�J�C�h�[������
	g_DXXFileObj_sky = new CDirect3DXFile();
	sts = g_DXXFileObj_sky->LoadXFile("assets/skydome.x", g_DXGrobj->GetDXDevice());
	if (!sts){
		MessageBox(hwnd, "ERROR!!", "skydome.x �ǂݍ��݃G���[", MB_OK);
		return false;
	}

	for (int i = 0; i<16; i++){
		g_XFile[i] = new CDirect3DXFile();
		g_XFile[i]->LoadXFile(XFileData[i].XfileName, g_DXGrobj->GetDXDevice());
	}
	//�����̂��Ƃ������l�����s�������
	for (int i = 0; i<16; i++){
		MakeWorldMatrix(g_World[i], ObjectInitData[i].FirstAngle, ObjectInitData[i].FirstPosition);
	}


	// �J�����ϊ��s��쐬
	D3DXMatrixLookAtLH(&g_MatView,
		&g_eye[0],				// ���_
		&g_lookat[0],			// �����_
		&g_up[0]);				// �����

	// �J�����s����Œ�p�C�v���C���փZ�b�g
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &g_MatView);

	// �v���W�F�N�V�����ϊ��s��쐬
	D3DXMatrixPerspectiveFovLH(&g_MatProjection,
		D3DX_PI / 2,					// ����p
		(float)width / (float)height,	// �A�X�y�N�g��
		1.0f,						// �j�A�v���[��
		10000.0f);					// �t�@�[�v���[��

	// �X�J�C�h�[���p�s���P�ʍs��
	D3DXMatrixIdentity(&g_MatStage);

	// �ˉe�ϊ��s����Œ�p�C�v���C���փZ�b�g
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_PROJECTION, &g_MatProjection);
	// �y�o�b�t�@�L��
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ���C�g�L��
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_LIGHTING, true);
	// �J�����O������
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// �����Z�b�g
	g_DXGrobj->GetDXDevice()->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	// DirectT Input������
	InitInput(hinst, hwnd);

	//CDebugFont�Q�[����������
	g_dbgfont.CreateFont(g_DXGrobj->GetDXDevice());

	// �n�`�Ƃ̓����蔻�菉��������
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
	////�\���̂̃|�C���^�ł��邩��A���[���Z�q���g��
	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						//
	//g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//g_pd3dDevice->LightEnable(0, FALSE);
	////�e�N�X�`���ǂݍ���
	//D3DXCreateTextureFromFile(g_pd3dDevice, "assets\\result_logo.png", &g_ui);

	// �C�x���g�n���h������
	g_hEventHandle = CreateEvent(NULL, false, false, NULL);
	if (g_hEventHandle == NULL){
		MessageBox(hwnd, "ERROR!!", "CreateEvent �G���[", MB_OK);
		return false;
	}
	// �X���b�h����(�Q�[�����C��)
	g_gamemainthread = std::thread(GameMain);

	return	true;
}

//==============================================================================
//!	@fn		GameInput
//!	@brief	�Q�[�����͌��m����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameInput(){
	UpdateInput();				// ���͂����m����
}

//==============================================================================
//!	@fn		GameUpdate
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameUpdate(){

	static int idx=0;
	//static int jumpflg = 0;

	// ��]�p�x�̏�����
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
		g_MatBallTotal = {				// �ώZ�s��i�P�ʍs��ŏ������j
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		gameoverflg = 0;
	}


	// �{�[���p�s����쐬
	MakeWorldMatrix(g_MatBall, g_MatBallTotal, g_ballangle, g_balltrans);

	// ������ƒx��Ă���TPS�J�����̌v�Z
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

	// �S�ʂ��������ă|���S���Ƃ̌�_��������
	static D3DXVECTOR3 ans(0,0,0);
	bool sts = SearchAllSurface(g_MatBallTotal._41, g_MatBallTotal._43, ans);
	if (sts){
	// ��_�����������Ƃ�
		g_MatBallTotal._42 = ans.y+25;
	}
	else{
	// ��_��������Ȃ������Ƃ�
		//g_MatBallTotal._42 = ans.y;
		g_MatBallTotal._42 -= 5.0f;
		g_balltrans.z = 0;
	}
	if (g_MatBallTotal._42<-400){
		gameoverflg = 1;
		g_MatBallTotal._42 += 5.0f;

	}





	// �J�����ϊ��s��쐬
	D3DXMatrixLookAtLH(&g_MatView,
		&g_eye[useidx],				// ���_
		&g_lookat[useidx],			// �����_
		&g_up[useidx]);				// �����

	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_VIEW, &g_MatView);	// �J�����ϊ��s����Z�b�g


	MakeWorldMatrix(g_World[Num], g_MatTotal, ObjectInitData[Num].FirstAngle, ObjectInitData[Num].FirstPosition);
	//�e�q�֌W������
	for (int i = 0; i<16; i++){
		g_pearentChild[i] = g_World[i];
		for (int j = ObjectInitData[i].ParentObjectNo; j != -1; j = ObjectInitData[j].ParentObjectNo){

			g_pearentChild[i] *= g_World[j];

		}
	}



}

//==============================================================================
//!	@fn		GameRender
//!	@brief	�Q�[���X�V����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameRender(){
	HRESULT  hr;

	// �^�[�Q�b�g�o�b�t�@�̃N���A�A�y�o�b�t�@�̃N���A
	g_DXGrobj->GetDXDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// �`��̊J�n���c�h�q�d�b�s�w�ɒʒm
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
	//	D3DXMatrixScaling(&mxScale, 2.0f, 2.0f, 2.0f);//�g�k��
	//	D3DXMatrixTranslation(&mxTrans, 0, 0, 0);
	//	//���s�ړ��s������֐�     �i,x,y,x�j
	//	mxWorld = mxTrans;//��]������Ƃ��Ɏg���p
	//	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &mxWorld);

	//	D3DXMATRIX mxRot;	//�r���{�[�h�̉�]
	//	D3DXMATRIX mxWorkView;//�r���[�s��R�s�[�p

	//	mxWorkView = g_MatView; //�r���[�s����R�s�[(���̎��_�ō쐬�ς݂ɂ��Ă���)//�iLookAt�Ȃ񂽂�
	//	mxWorkView._41 = 0;   //�s��̒��̕��s�ړ��������O�ɂ���i��]���c�����s�ړ��s�������
	//	mxWorkView._42 = 0;
	//	mxWorkView._43 = 0;
	//	D3DXMatrixInverse(&mxRot, NULL, &mxWorkView);//"�t�s��"�����֐��i��]�̋t�s�����肽��
	//	//~~�t�s�񂵂����ʂ̕ϐ�, , �t�s�񂳂���ϐ�

	//	mxWorld = mxRot * mxTrans;
	//	//~~~~~~�r���{�[�h�̕��s�ړ��s������炩���ߍ���Ă���
	//	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &mxWorld);//�r���{�[�h�̃��[���h�ϊ��ݒ�

	//	g_DXGrobj->GetDXDevice()->SetTexture(0, g_ui); //�e�N�X�`���ϐ�
	//	g_DXGrobj->GetDXDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vx_UI, sizeof(VERTEX3D));

	//	//---------------------------------------------------------------------
	//}

	//�X�J�C�h�[���`��
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatTotal);	// ���[���h�ϊ��s����Z�b�g
	g_DXXFileObj_sky->Draw(g_DXGrobj->GetDXDevice());				// sky_�w�t�@�C���`��
	
	// �X�e�[�W�`��
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatStage);	// ���[���h�ϊ��s����Z�b�g
	g_DXXFileStage->Draw(g_DXGrobj->GetDXDevice());						// �w�t�@�C���`��

	// �{�[����`��
	g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_MatBallTotal);	// ���[���h�ϊ��s����Z�b�g
	g_DXXFileBall->Draw(g_DXGrobj->GetDXDevice());							// �w�t�@�C���`��

	//	//�����X�^�[�`�揈��
	for (int i = 0; i<16; i++){
		g_DXGrobj->GetDXDevice()->SetTransform(D3DTS_WORLD, &g_pearentChild[i]);	// ���[���h�ϊ��s����Z�b�g
		g_XFile[i]->Draw(g_DXGrobj->GetDXDevice());
	}

	DebugPrint();
	g_DXGrobj->GetDXDevice()->EndScene();	// �`��̏I����҂�

	hr = g_DXGrobj->GetDXDevice()->Present(NULL, NULL, NULL, NULL);	// �o�b�N�o�b�t�@����v���C�}���o�b�t�@�֓]��
	if (hr != D3D_OK){
		g_DXGrobj->GetDXDevice()->Reset(&g_DXGrobj->GetDXD3dpp());
	}
}

//==============================================================================
//!	@fn		GameMain
//!	@brief	�Q�[�����[�v����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameMain()
{
	while (1){
		DWORD sts = WaitForSingleObject(g_hEventHandle, 1000);	// �C�x���g�t���O���Z�b�g�����̂�҂i1000ms�Ń^�C���A�E�g�j
		if (sts == WAIT_FAILED){
			break;
		}
		else if (sts == WAIT_TIMEOUT){
			if (g_EndFlag){
				break;
			}
			continue;
		}

		GameInput();					// ����
		GameUpdate();					// �X�V
		GameRender();					// �`��
	}
}

//==============================================================================
//!	@fn		GameExit
//!	@brief	�Q�[���I������
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameExit()
{

	g_gamemainthread.join();					// �Q�[�����C���X���b�h�̏I����҂�

	CloseHandle(g_hEventHandle);				// �C�x���g�n���h���N���[�Y

	StageHitExit();								// �X�e�[�W�Ƃ̓����蔻��̏I������

	if (g_DXXFileBall != nullptr){
		delete g_DXXFileBall;					//	X�t�@�C���I�u�W�F�N�g(�{�[��)�̉��
	}

	if (g_DXXFileStage != nullptr){
		delete g_DXXFileStage;					//	X�t�@�C���I�u�W�F�N�g(�X�e�[�W)�̉��
	}

	if (g_DXGrobj != NULL){
		g_DXGrobj->Exit();						// DirectX�̃O���t�B�b�N�I�u�W�F�N�g�̏I������
	}

	if (g_DXGrobj != NULL){						// DirectX�̃O���t�B�b�N�I�u�W�F�N�g�̉��
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


	// DirectT Input�I������
	UninitInput();

}

//==============================================================================
//!	@fn		GameSetEvent
//!	@brief	�Q�[�����[�v���̃C�x���g�I�u�W�F�N�g���Z�b�g����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameSetEvent(){
	if (!g_EndFlag){
		SetEvent(g_hEventHandle);	// �I���t���O���Z�b�g����Ă��Ȃ���΃C�x���g���Z�b�g����
	}
}

//==============================================================================
//!	@fn		GameSetEndFlag
//!	@brief	�Q�[�����[�v�̏I���t���O���Z�b�g����
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void GameSetEndFlag(){
	g_EndFlag = true;				// �I���t���O���I���ɂ���
}


//==============================================================================
//!	@fn		DebugPrint
//!	@brief	�f�o�b�O����\������
//!	@param�@	
//!	@retval	�Ȃ�
//==============================================================================
void DebugPrint(){

	D3DXVECTOR3 bspos1, bspos2, bspos3;

	char buf[128];
	
	if (gameoverflg==0){
		sprintf_s(buf, "�X�e�[�W��ł� ");
	}
	else{
		sprintf_s(buf, "�X�e�[�W���痎���܂��� SPACE�Ŗ߂�");
	}
	g_dbgfont.DrawTextA(10, 10, buf);

	bspos1 = D3DXVECTOR3(g_MatBallTotal._41, g_MatBallTotal._42, g_MatBallTotal._43);

	sprintf_s(buf, "(X:%4.0f  Y:%4.0f  Z:%4.0f)",  bspos1.x, bspos1.y, bspos1.z);
	g_dbgfont.DrawTextA(10, 40, buf);
	

}

//******************************************************************************
//	End of file.
//******************************************************************************
