#pragma once
#include	<d3dx9.h>

// 平面の方程式
struct	PLANE{
	float	a,b,c,d;			// 平面方程式の係数（ ax+by+cz+d=0 )
};

// プロトタイプ宣言
void CreatePlaneInfo(const D3DXVECTOR3& p1,const D3DXVECTOR3& p2,const D3DXVECTOR3& p3 ,PLANE& plane);

bool LinetoPlaneCross(	const PLANE& plane,		//平面の方程式
						const D3DXVECTOR3& line_o,	//直線が通る点
						const D3DXVECTOR3& line_v,	//直線の方向ベクトル
						D3DXVECTOR3& ans);		//交点座標

bool CheckInTriangle(	const D3DXVECTOR3& a,
						const D3DXVECTOR3& b,
						const D3DXVECTOR3& c,	
						const D3DXVECTOR3& p);

float LengthPointToPlane(const D3DXVECTOR3& p, const PLANE& plane, D3DXVECTOR3& crosspoint);
