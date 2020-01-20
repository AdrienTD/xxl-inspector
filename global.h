// Global header file

#define WIN32_LEAN_AND_MEAN
#define CINTERFACE
#include <windows.h>
#include <cstdio>
#include <io.h>
#include <d3d9.h>
#define _USE_MATH_DEFINES
#include <map>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <cfloat>
#include <vector>

#define tstringify(x) #x
#define sstringify(x) tstringify(x)
#ifdef PATCHVERSTR
#define INSPECTOR_VERSION "v" sstringify(PATCHVERSTR)
#else
#define INSPECTOR_VERSION "DEV"
#endif

#define naked __declspec(naked)
/*
#define callvf(p, f) \
__asm mov ecx, p \
__asm mov eax, [ecx] \
__asm call [eax+f*4]
*/
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

template<class T> struct KVector {
	T *_data;
	size_t _capacity;
	size_t _size;

	T *begin() {return _data;}
	T *end() {return _data + _size;}

	size_t size() const {return _size;}
	size_t capacity() const {return _capacity;}
};

struct SGameStartInfo {
	HWND hwnd;
	uint width, height;
	uint unk1, unk2;
	char fullscreen;
};

struct KFile;

struct KClass {
	virtual void destructor_placeholder() = 0;
	virtual char isSubclass(int cls) = 0;
	virtual void reset() = 0;
#if XXLVER >= 2
	virtual void unknown2() = 0;
#endif
	virtual int getClassGroup() = 0;
	virtual int getClassID() = 0;
	virtual int sendEvent(int event, void *param) = 0;
	virtual char unknown3(void *arg0, void *arg4, void *arg8, void *argC) = 0;
	virtual int unknown4(void *arg0) = 0;
	virtual int unknown5(void *arg0, void *arg4) = 0;
	virtual int deserialize(KFile *file, void *arg4) = 0;

	char isSubclass(int clgrp, int clid) {
		return isSubclass(clgrp | (clid << 6));
	}
};

struct KclInfos {
	struct Tab {
		void *pnt;
		uint allocatedSize;
		uint numElements;
	};

	void *vtbl;
	Tab cls[15];
};

struct KFile {
	virtual void unknown0(void* a, void* b, void* c) = 0; 	// Returns 0
	virtual void unknown1(void* a, void* b, void* c) = 0;
	virtual void unknown2(void* a) = 0;
#if XXLVER == 1
	virtual void unknownX(void* a) = 0;
#endif
	virtual void unknown3() = 0;
	virtual void read(void *dst, size_t size) = 0;
	virtual void write(void *src, size_t size) = 0;
};

struct CKLoadingManager {
	//0x2AC
#if XXLVER == 1
	char unknown[0x480];
#elif XXLVER == 2
	uint uu0;
	uint uu1;
	KClass **gameObjects;
	uint numGameObjects;
	char unknown[0x284];
	// char unknown[0x294];
#elif XXLVER == 4
	uint uu0;
	uint uu1;
	KClass **gameObjects;
	uint numGameObjects;
	char unknown[0x288];
	//char unknown[0x298];
#endif
	uint currentSector;
	uint ss0;
	uint ss1;
	uint ss2;
	uint sectorToLoad;
	virtual void unknown00() = 0;
	virtual void unknown04() = 0;
	virtual void unknown08() = 0;
	virtual void unknown0C() = 0;
	virtual void unknown10() = 0;
	virtual void unknown14() = 0;
	virtual void unknown18() = 0;
	virtual void unknown1C() = 0;
	virtual void readRef(KFile *file, KClass **refPointer) = 0;
};

struct CKYellowPages {
#if XXLVER == 1
	void *p_00,*p_04,*p_08,*p_0C;
	void *p_10,*p_14,*p_18; KclInfos* kclInfos;
	void *p_20,*p_24,*p_28,*p_2C;
	void *gameLoop,*p_34,*p_38,*p_3C;
	void *p_40; CKLoadingManager* loadingManager; void*p_48,*p_4C;
	void *srvCollision,*p_54,*p_58,*p_5C;
	void *p_60,*p_64,*p_68,*p_6C;
	void *p_70,*p_74,*srvBeacon,*p_7C;
	void *p_80,*p_84,*p_88,*gameManager;
#elif XXLVER == 2
	void *p_00,*p_04,*p_08,*p_0C;
	void *p_10,*p_14,*p_18; KclInfos* kclInfos;
	void *p_20,*p_24,*p_28,*p_2C;
	void *gameLoop,*p_34,*p_38,*p_3C;
	void *p_40,*p_44; CKLoadingManager* loadingManager; void*p_4C;
	void *p_50,*p_54,*p_58,*p_5C;
	void *p_60,*p_64,*p_68,*p_6C;
	void *p_70,*p_74,*p_78,*p_7C;
	void *gameManager;
#elif XXLVER == 4
	void *a;
	void *p_00,*p_04,*p_08,*p_0C;
	void *p_10,*p_14,*p_18; KclInfos* kclInfos;
	void *p_20,*p_24,*p_28,*p_2C;
	void *gameLoop,*p_34,*p_38,*p_3C;
	void *b;
	void *p_40; CKLoadingManager* loadingManager; void*p_48,*p_4C;
	void *p_50,*p_54,*p_58,*p_5C;
	void *p_60,*p_64,*p_68,*p_6C;
	void *p_70,*p_74;
	void *p_80,*p_84,*p_88,*gameManager;
#endif
};

struct CMatrix  {
	void *u[4];
	float c[16];
	float d[16];
};

struct CNode : KClass {
	//void *vtbl;
	CMatrix *matrix;
	CNode *nextNode;
	CNode *parentNode;
	void *p_10, *p_14;
	CNode *childNode;
	void *geometry;
};

struct KHook : KClass {
	KHook *nextHook;
	int unk_08;
#if XXLVER == 1
	KClass *life;
	CNode *node;
#endif
};

struct KGroup : KClass {
#if XXLVER < 2
	KGroup *next;
	int unk_08;	// Could be a boolean value?
	KGroup *child;
	int unk_10;
	KGroup *parent;
	KClass *life;
	KClass *bundle;
	KHook *hook;
#else
	int unk_04;
	int unk_08;
	int unk_0C;
	int unk_10;
	KGroup *next;
	KGroup *child;
	KGroup *parent;
	int unk_20;
	KClass *bundle;
	KHook *hook;
#endif
};

struct CKService : KClass {
	CKService *next;
	int cunknown;
};

#if XXLVER >= 2

struct KAction {
	void *param;
	KClass *object;
	uint16_t type, aaa;
};

struct CKTriggerDomain;
struct CKTrigger;
struct CKCondition;
struct CKComparedData;

struct CKSrvTrigger : CKService {
	CKTriggerDomain *rootDomain;
};

struct CKTriggerDomain : KClass {
	KVector<CKTrigger*> triggers;
	KVector<CKTriggerDomain*> subdomains;
};

struct CKTrigger : KClass {
#if XXLVER >= 4 // TODO: unsure if 4 or 3, but not 2
	uint32_t unk1, unk2;
#endif
	CKCondition *condition;
	KAction *actions;
	size_t numActions;
};

struct CKCondition : KClass {
	CKCondition *next;
};

struct CKCombiner : CKCondition {
	int flags;	// 0=and,  4=ior
	CKCondition *firstCondition;
};

struct CKComparator : CKCondition {
	int flags;
	// 00: <=
	// 01: negation
	// 04: >=
	// 08: ==
	CKComparedData *left, *right;
};

struct CKComparedData : KClass {
	void *data;
	int unk1, unk2;
	int type : 4;	// 0: obj bool/byte, 1: obj int, 4: const bool/byte, 5: const int, 8: ?
	int : 0;
};

#endif

typedef size_t (__cdecl *pfm_msize)(void *memblock);
#define ax_msize ((pfm_msize)0x60FB19)

typedef void (__thiscall *pfKfRefToPnt)(void* _this, uint *ref);

#if XXLVER == 1
#define kfRefToPnt ((pfKfRefToPnt)0x40D2C0)
#define yellowPages (*(CKYellowPages**)0x6621F4)
#define axWndProc 0x479C10
#define m_CKGameManager_currentLevel 4
#define m_CKGameManager_nextLevel 8
#define m_CKGameManager_levelObject 0x20
#define m_CKLevel_currentSector 0x24
#define m_CKLevel_nextSector 0x34

#pragma pack(push, 1)
struct ClassContainer {
	void *objects;
	uint flags;
	void *u1;
	ushort u2, count;
	ushort clSize; char pl, u3;
};
#pragma pack(pop)

#elif XXLVER == 2

#define kfRefToPnt ((pfKfRefToPnt)0x40ED30)
#define yellowPages (*(CKYellowPages**)0x663D04)
#define axWndProc 0x49B190
#define m_CKGameManager_currentLevel 0x6C
#define m_CKGameManager_nextLevel 0x70
#define m_CKGameManager_levelObject 0x18
#define m_CKLevel_currentSector 0x1C
#define m_CKLevel_nextSector 0x2C

#pragma pack(push, 1)
struct ClassContainer {
//0x00:
	void *objects;
	void *u0;
	uint flags;
	void *u1;
//0x10:
	ushort u2, count;
	ushort u3, clSize;
	char pl, u4; ushort u5;
};
#pragma pack(pop)

#elif XXLVER == 4
#define kfRefToPnt ((pfKfRefToPnt)0x40CE40)
#define yellowPages (*(CKYellowPages**)0x765BF8)
#define axWndProc 0x4DB0C0
#define m_CKGameManager_currentLevel 0x1A0
#define m_CKGameManager_nextLevel 0x1A4
#define m_CKGameManager_levelObject 0x18
#define m_CKLevel_currentSector 0x1C
#define m_CKLevel_nextSector 0x2C

#pragma pack(push, 1)
struct ClassContainer {
//0x00:
	void *objects;
	void *u0;
	uint flags;
	void *u1;
//0x10:
	ushort u2, count;
	ushort u3, clSize;
	char pl, u4; ushort u5;
};
#pragma pack(pop)

#else
#error Incorrect XXL version. Be sure that the XXLVER macro is defined and set to 1, 2 or 4.
#endif

typedef struct
{
	void **lpVtbl;
	IDirect3D9 *original;
} HDirect3D9;

typedef struct
{
	void **lpVtbl;
	IDirect3DDevice9 *original;
	HDirect3D9 *d3d;
} HDirect3DDevice9;

extern char title[];

//extern char setting_higher_time_precision;
extern IDirect3DDevice9 *gamed3ddev;
extern FILE *logfile;

extern void *odmvtbld[], *devvtbld[];

extern SGameStartInfo* gameStartInfo;
//extern CKYellowPages* yellowPages;
extern std::map<KClass*, char*> lvlObjectNames[32], gameObjectNames;
extern uint lvlNumSectors;

extern bool enableCrateRandomizer;

void SetImmediateJump(void *p, uint j);
void SetMemProtection(void *mem, int flags);

void IGInit();
void IGNewFrame();
void IGRender();
void IGDX9Reset();
LRESULT __stdcall IGWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

char *getClassName(int grp, int id);
