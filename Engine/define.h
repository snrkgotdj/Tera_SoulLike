#pragma once

#include "global.h"

#define WINSIZE_X 1600
#define WINSIZE_Y 900

#define SINGLE(Type) public:\
					static Type* GetInst()\
					{\
						static Type mgr;\
						return &mgr;\
					}\
					private:\
						Type();\
						~Type();

#define RET_FAILED -1
#define RET_SUCCESS 1

#define SAFE_RELEASE(p) if(NULL != p) p->Release();
#define SAFE_DELETE(p) if(NULL != p) delete p; p = NULL;
#define SAFE_DELETE_ARR(p) if( NULL != p) delete [] p; p = NULL;

#define DEVICE CDevice::GetInst()->GetDevice()
#define CONTEXT CDevice::GetInst()->GetContext()

#define CLONE(type) virtual type* Clone(){return new type (*this);}

#define MAX_LAYER 32

#define ABGR(R, G, B, A) ((A<<24)|(B<<16)|(G << 8)| R)


#define TARGET_SWAPCHAIN		L"RenderTarget_0"
#define TARGET_FORWARD			L"RenderTarget_1"
#define TARGET_DIFFUSE			L"RenderTarget_2"
#define TARGET_POSITION			L"RenderTarget_3"
#define TARGET_NORMAL			L"RenderTarget_4"
#define TARGET_DEPTH			L"RenderTarget_5"
#define TARGET_LIGHT_DIFFUSE	L"RenderTarget_6"
#define TARGET_LIGHT_SPECULAR	L"RenderTarget_7"
#define TARGET_DIR_SHADOWMAP	L"RenderTarget_8"
#define TARGET_POST				L"RenderTarget_9"
#define TARGET_VELOCITY			L"RenderTarget_10"
#define TARGET_EMISSIVE			L"RenderTarget_11"
#define TARGET_SPECULAR			L"RenderTarget_12"

#define KEY_INPUT(Key, State) CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::Key, State)

typedef XMMATRIX Matrix;
typedef XMFLOAT4 Vec4;
typedef XMFLOAT2 Vec2;


enum class COMPONENT_TYPE
{
	TRANSFORM,
	MESHRENDER,
	COLLIDER,
	ANIMATOR_2D,
	ANIMATOR_3D,
	CAMERA,
	LIGHT,
	TERRAIN,
	TEXT,
	TRAIL,
	AUDIO,
	SCRIPT,
	PARTICLE,
	END,

	COLLIDER_2D,
	COLLIDER_3D
};

enum class SHADER_TYPE
{
	ST_VERTEX = 0x01,
	ST_HULL = 0x02,
	ST_DOMAIN = 0x04,
	ST_GEOMETRY = 0x08,
	ST_COMPUTE = 0x10,
	ST_PIXEL = 0x20,
};

enum class RASTERIZE_TYPE
{
	RT_WIREFRAME,
	RT_FRONT,
	RT_BACK,
	RT_NONE,
	RT_END,
};

enum class SHADER_PARAM
{
	SP_INT_0,
	SP_INT_1,
	SP_INT_2,
	SP_INT_3,
	SP_INT_4,
	SP_INT_5,
	SP_INT_6,
	SP_INT_7,
	SP_INT_END,
	SP_FLOAT_0,
	SP_FLOAT_1,
	SP_FLOAT_2,
	SP_FLOAT_3,
	SP_FLOAT_4,
	SP_FLOAT_5,
	SP_FLOAT_6,
	SP_FLOAT_7,
	SP_FLOAT_END,
	SP_VEC4_0,
	SP_VEC4_1,
	SP_VEC4_2,
	SP_VEC4_3,
	SP_VEC4_4,
	SP_VEC4_5,
	SP_VEC4_6,
	SP_VEC4_7,
	SP_VEC4_END,
	SP_TEX_0,
	SP_TEX_1,
	SP_TEX_2,
	SP_TEX_3,
	SP_TEX_4,
	SP_TEX_5,
	SP_TEX_6,
	SP_TEX_7,
	SP_TEXARR_0,
	SP_TEXARR_1,
	SP_TEXARR_2,
	SP_TEXARR_3,
	SP_TEXARR_4,
	SP_TEXARR_5,
	SP_TEXARR_6,
	SP_TEXARR_7,
	SP_TEX_END,
	SP_END,
};

enum class DIR_TYPE
{
	RIGHT,
	UP,
	FRONT,
	END
};

enum class LIGHT_TYPE
{
	DIR,
	POINT,
	SPOT,
	END,
};

enum class RENDER_MODE
{
	DEFERRED,
	FORWARD,
	END,
};

enum class MRT_TYPE
{
	OUTPUT,
	DEFERRED,
	LIGHT,
	SHADOW,
	POSTEFFECT,
	END,
};

enum class CAMERA_TYPE
{
	MAIN,
	UI,
	TOOL,
	LIGHT,
	END
};

enum class PARTICLE_TYPE
{
	Emiter = 0,
	Test,
	END
};

enum class COLLIDER_TYPE
{
	SPHERE,
	CUBE,
	OBB,
	END
};

enum CELL_VERT
{
	VERT_A = 0,
	VERT_B,
	VERT_C,
};

enum CELL_SIDE
{
	SIDE_AB = 0,
	SIDE_BC,
	SIDE_CA
};

