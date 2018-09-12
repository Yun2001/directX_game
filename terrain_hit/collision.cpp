//*****************************************************************************
//!	@file	collision.cpp
//!	@brief	
//!	@note	当たり判定に使用するための関数群
//!	@author	
//*****************************************************************************

//-----------------------------------------------------------------------------
//	Include header files.
//-----------------------------------------------------------------------------
#include	<math.h>
#include	"collision.h"

//==============================================================================
//!	@fn		CreatePlaneInfo
//!	@brief　平面の方程式を求める（頂点は時計回りで渡す）
//!	@param	頂点１座標
//!	@param	頂点２座標
//!	@param	頂点３座標
//!	@param	平面の方程式
//!	@retval	なし
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

	D3DXVec3Cross(&normal, &p1p2, &p2p3);		// 外積を求める
	D3DXVec3Normalize(&normal, &normal);		// 長さを1にしておく
	plane.a = normal.x;
	plane.b = normal.y;
	plane.c = normal.z;

	plane.d = -(plane.a * p1.x + plane.b * p1.y + plane.c * p1.z);
}

//==============================================================================
//!	@fn		LinetoPlaneCross
//!	@brief　直線と平面の交点を求める
//!	@param	平面の方程式
//!	@param	直線の起点
//!	@param	直線の方向ベクトル
//!	@param	交点座標
//!	@retval	true 交点あり　false 交点なし
//==============================================================================
bool LinetoPlaneCross(const PLANE&		plane,		//平面の方程式
	const D3DXVECTOR3&	line_o,						//直線の起点
	const D3DXVECTOR3&	line_v,						//直線の方向ベクトル
	D3DXVECTOR3&			ans)					//交点座標
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
//!	@brief　３角形の内部にあるかどうかを判定
//!	@param	３角形の第１頂点座標
//!	@param	３角形の第２頂点座標
//!	@param	３角形の第３頂点座標
//!	@param	判定したい頂点座標
//!	@retval	true 内部　false 外部
//==============================================================================
bool CheckInTriangle(const D3DXVECTOR3&	a,
						const D3DXVECTOR3&	b,
						const D3DXVECTOR3&	c,	
						const D3DXVECTOR3&	p ){
	D3DXVECTOR3  ab,bc,ca;			// ３辺のベクトル
	D3DXVECTOR3  ap,bp,cp;			// 内部の点とのベクトル
	D3DXVECTOR3	 normal;			// ３角形の法線ベクトル
	D3DXVECTOR3	 n1,n2,n3;			// ３辺と内部の点との法線ベクトル

	ab = b - a;
	bc = c - b;
	ca = a - c;

	ap = p - a;
	bp = p - b;
	cp = p - c;

	D3DXVec3Cross(&normal, &ab, &bc);		// 外積を計算

	D3DXVec3Cross(&n1, &ab, &ap);			// 外積を計算
	D3DXVec3Cross(&n2, &bc, &bp);			// 外積を計算
	D3DXVec3Cross(&n3, &ca, &cp);			// 外積を計算

	if(D3DXVec3Dot(&n1,&normal) < 0) return false;			// 為す角度が鈍角
	if(D3DXVec3Dot(&n2,&normal) < 0) return false;			// 為す角度が鈍角
	if(D3DXVec3Dot(&n3,&normal) < 0) return false;			// 為す角度が鈍角
	return true;
}

//==============================================================================
//!	@fn		LengthPointToPlane
//!	@brief　平面と点の距離を求める	また同時に交点の座標を求める
//!	@param	点の座標
//!	@param	平面の方程式
//!	@param	交点の座標
//!	@retval	点と平面の距離
//==============================================================================
float LengthPointToPlane(const D3DXVECTOR3& p,
							const PLANE& plane,
							D3DXVECTOR3& crosspoint)
{
	float dot_work = plane.a*p.x+plane.b*p.y+plane.c*p.z + plane.d;
	float normal_len = plane.a*plane.a+plane.b*plane.b+plane.c*plane.c;
	
	// 交点を求める
	float t = -1.0f * dot_work / normal_len;

	crosspoint.x = p.x + plane.a * t;
	crosspoint.y = p.y + plane.b * t;
	crosspoint.z = p.z + plane.c * t;

	float length = fabsf(dot_work); // ÷ sqrtf(normal_len);	// 平面の法線を正規化しているのでsqrtは省きました

	return length;
}
//******************************************************************************
//	End of file.
//******************************************************************************
