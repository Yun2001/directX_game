#pragma once
#define		XFILEMAX		PARTSMAX
#include	<d3d9.h>
#include	<d3dx9.h>
/*--------------------------------------------------------

	３Ｄオブジェクト配置構造体定義

---------------------------------------------------------*/
// オブジェクト名
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

// 構造体型タグ
struct	_3DObjectInitData{
	int			ParentObjectNo;				// 親オブジェクトＮｏ
	int			ModelNo;					// モデル番号
	D3DVECTOR	FirstPosition;				// 最初の位置
	D3DVECTOR	FirstAngle;					// 最初の角度
};

struct	_XFileData{
	int			ModelNo;					// モデル番号
	char*		XfileName;					// Ｘファイル名
};
