#include	"data.h"

/*-------------------------------------
	3D�I�u�W�F�N�g�z�u�f�[�^

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

--------------------------------------*/

extern _3DObjectInitData	ObjectInitData[] =
{
	// �e�@    ���f���ԍ�		�����ʒu              	�����p�x    
	 {   -1,	HIP,  		{0.0f,   0.0f,	  0.0f},	{0,0,0},	},			
	 { HIP,		BODY, 	 	{0.0f,   1.3f,   -0.4f},	{0,0,0},	},			
	 { BODY,	HEAD, 	 	{0.0f,   1.85f,  -0.55f},	{0,0,0},	},			
	 { BODY,	ARMR0,		{-1.8f,  1.35f,  -0.3f},	{0,0,0},	},			
	 { BODY,	ARML0,	 	{1.8f,   1.35f,  -0.3f},	{0,0,0},	},			
	 { HIP, 	LEGR0,	 	{-1.0f, -0.6f,   0.45f},	{0,0,0},	},			
	 { HIP, 	LEGL0,	 	{ 1.0f, -0.6f,   0.45f},	{0,0,0},	},			 

	 { ARMR0, 	ARMR1,	 	{-1.18f, -1.27f,   0.7f},	{0,0,0},	},			
	 { ARML0, 	ARML1,	 	{ 1.18f, -1.27f,   0.7f},	{0,0,0},	},			

	 { LEGR0, 	LEGR1,	 	{-1.17f, -2.3f,   -1.26f},	{0,0,0},	},			
	 { LEGL0, 	LEGL1,	 	{ 1.17f, -2.3f,   -1.26f},	{0,0,0},	},			

	 { BODY,	WINGR0,     { 0.1f,   1.17f,   0.0f},   {0,0,0},	},
	 { BODY,	WINGL0,		{-0.1f,   1.17f,   0.0f},   {0,0,0},	},
	 { WINGR0,	WINGR1,     {-2.48f,  4.36f,  1.58f},   {0,0,0},	},
	 { WINGL0,	WINGL1,		{ 2.48f,  4.36f,  1.58f},   {0,0,0},	},
	 { HIP,		TAIL0 ,     { 0.0f,   0.0f,   0.0f },   {0,0,0},	},
	 {   -1,	 -1,	 	{0.0f,   0.0f,   0.0f}, 	{0,0,0},	}			
};			


extern _XFileData  XFileData[] = {
	{HIP,  "assets\\model\\Char\\E2\\e2_hip.x"}, 
	{BODY, "assets\\model\\Char\\E2\\e2_body.x"}, 
	{HEAD, "assets\\model\\Char\\E2\\e2_head.x"}, 
	{ARMR0,"assets\\model\\Char\\E2\\e2_armr0.x"}, 
	{ARML0,"assets\\model\\Char\\E2\\e2_arml0.x"}, 
	{LEGR0,"assets\\model\\Char\\E2\\e2_legr0.x"}, 
	{LEGL0,"assets\\model\\Char\\E2\\e2_legl0.x"}, 
	{ARMR1,"assets\\model\\Char\\E2\\e2_armr1.x"}, 
	{ARML1,"assets\\model\\Char\\E2\\e2_arml1.x"}, 
	{LEGR1,"assets\\model\\Char\\E2\\e2_legr1.x"}, 
	{LEGL1,"assets\\model\\Char\\E2\\e2_legl1.x"}, 
	{WINGR0,"assets\\model\\Char\\E2\\e2_wingr0.x"},
  	{WINGL0,"assets\\model\\Char\\E2\\e2_wingl0.x"},
	{WINGR1,"assets\\model\\Char\\E2\\e2_wingr1.x"},
	{WINGL1,"assets\\model\\Char\\E2\\e2_wingl1.x"},
	{TAIL0 ,"assets\\model\\Char\\E2\\e2_tail0.x"}, 
	{-1,NULL}
};
