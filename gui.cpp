
#include "global.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "imgui_memory_editor.h"

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-----------------------------------

void IGError(int exp, char *msg)
{
	if(exp) return;
	MessageBox(0, msg, "ImGui assertion", 16);
	exit('IG');
}

void IGInit()
{
	//MessageBox(0, "IGInit", 0, 0);
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(gameStartInfo->hwnd);
	ImGui_ImplDX9_Init(gamed3ddev);
	ImGui::StyleColorsClassic();
}

bool showSceneTree = false, showSelectedObject = false, showObjectTree = false,
	showCheats = false, showGameObjectsList = false, showCrateDetector = false,
	showHookTree = false, showTriggerTree = false;
KClass *selectedObject = 0;
int selNodeSize = 32;
static void* g_clfndres = 0;

char* clgrpNames[15] = {"Managers", "Services", "Hooks", "Hook Lives", "Groups", "Group Lives",
          "Components", "Cameras", "Cinematic blocs", "Dictionaries",
          "Geometry", "Nodes", "Game things", "Graphics things", "Errors"};

KClass* GetLvlObject(uint clgrp, uint clid, uint instid)
{
	void *kcls = *(void**)((char*)yellowPages->loadingManager + 4);
	void *grp = *(void**)((char*)kcls + 12*clgrp);
	ClassContainer *cl = (ClassContainer*)grp + clid;
	char pl = cl->pl;
	uint flg = cl->flags;
	ushort cnt = cl->count;
	ushort csz = cl->clSize;
	char *fstobj = (char*)cl->objects;
	if(!cnt) return 0;
	assert(instid < cnt);
	if(pl)
		return (KClass*)(fstobj + csz*instid);
	else if(flg >> 17 == 1)
		return (KClass*)fstobj;
	else
		return (KClass*)(((void**)fstobj)[instid]);
}

uint GetNumLvlClassInstances(uint clgrp, uint clid)
{
	void *kcls = *(void**)((char*)yellowPages->loadingManager + 4);
	void *grp = *(void**)((char*)kcls + 12*clgrp);
	ClassContainer *cl = (ClassContainer*)grp + clid;
	return cl->count;
}

const char * getObjectName(KClass *obj)
{
	const char *name = lvlObjectNames[yellowPages->loadingManager->currentSector][obj];
	if(!name) name = lvlObjectNames[0][obj];
	if(!name) name = gameObjectNames[obj];
	if(!name) name = "?";
	return name;
}

void IGGameObjectsList()
{
	if(!showGameObjectsList) return;
#if XXLVER >= 2
	ImGui::Begin("Game objects", &showGameObjectsList);
	CKLoadingManager* lm = yellowPages->loadingManager;
	for(uint i = 0; i < lm->numGameObjects; i++) {
		KClass *obj = lm->gameObjects[i];
		int grp = obj->getClassGroup(), id = obj->getClassID();
		char *name = gameObjectNames[obj];
		if(!name) name = "?";

		int flags = ImGuiTreeNodeFlags_Leaf;
		if(selectedObject == obj) flags |= ImGuiTreeNodeFlags_Selected;
		if(ImGui::TreeNodeEx(obj, flags, "%s (%i,%i): %s", getClassName(grp, id), grp, id, name))
			ImGui::TreePop();
		if(ImGui::IsItemClicked()) {
			selectedObject = obj;
		}
	}
	ImGui::End();
#endif
}

#if XXLVER == 1
typedef KHook* (__thiscall *ft_CKGrpBonus_spawnBonus)(KGroup *grp, int type, Vector3 *position);
typedef void (__thiscall *ft_CKHkBasicBonus_bonusFollowHero)(KHook* bonus, Vector3 *vec, KHook *hero);
typedef void (__thiscall *ft_CKHkBasicBonus_bonusFall)(KHook* bonus, Vector3 *vec);
typedef void (__thiscall *ft_CKProjectileTypeAsterixBomb_bombSpawn)(KClass* projBomb, Vector3 *pos, Vector3 *dest, float hypo_speed, KHook *boss, Vector3 *unk4, float unk5);
const ft_CKGrpBonus_spawnBonus CKGrpBonus_spawnBonus = (ft_CKGrpBonus_spawnBonus)0x54BE20;
const ft_CKHkBasicBonus_bonusFollowHero CKHkBasicBonus_bonusFollowHero = (ft_CKHkBasicBonus_bonusFollowHero)0x54E2C0;
const ft_CKHkBasicBonus_bonusFall CKHkBasicBonus_bonusFall = (ft_CKHkBasicBonus_bonusFall)0x54DF10;
const ft_CKProjectileTypeAsterixBomb_bombSpawn CKProjectileTypeAsterixBomb_bombSpawn = (ft_CKProjectileTypeAsterixBomb_bombSpawn)0x5354B0; // virtual function: I should have called it from the vtbl

static bool g_cheat_rainingBonuses = false, g_cheat_infinitePotion = false, g_cheat_rainingBombs = false;

void SpawnBombFromHero(KClass *projBomb, KHook *hkBoss, KHook *hkHero)
{
	Vector3 &astpos = *(Vector3*)((char*)hkHero + 0x6c);
	Vector3 &astdir = *(Vector3*)((char*)hkHero + 0xb4);
	Vector3 spawnpos = astpos + astdir * 0.5f + Vector3(0,1,0);
	Vector3 dest = astpos + astdir * 5; // + Vector3(0,5,0);
	Vector3 nullvec(0,0,0);
	CKProjectileTypeAsterixBomb_bombSpawn(projBomb, &spawnpos, &dest, 10.0f, hkBoss, &nullvec, 1.4f);
}

void SpawnBombFromAllHeroes(KClass *projBomb, KHook *hkBoss, KHook *hkAsterix, KHook *hkObelix)
{
	SpawnBombFromHero(projBomb, hkBoss, hkAsterix);
	SpawnBombFromHero(projBomb, hkBoss, hkObelix);
}
#endif

void IGCheats()
{
	if(!showCheats) return;
	if(*(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel) == -1) return;
	ImGui::Begin("Cheats", &showCheats);

	void *tim = *(void**)((char*)yellowPages->gameLoop + 8);
	ImGui::DragFloat("Game speed", (float*)((char*)tim + 0x10), 0.1f);

#if XXLVER == 1
	KHook *asterixHook = (KHook*)GetLvlObject(2, 28, 0);
	if(asterixHook) {
		ImGui::DragFloat3("Asterix Pos", (float*)((char*)asterixHook + 0x6c), 0.3f);
	}
	KHook *obelixHook = (KHook*)GetLvlObject(2, 29, 0);
	if(obelixHook) {
		ImGui::DragFloat3("Obelix Pos", (float*)((char*)obelixHook + 0x6c), 0.3f);
	}
	KHook *idefixHook = (KHook*)GetLvlObject(2, 30, 0);
	if(idefixHook) {
		ImGui::DragFloat3("Idefix Pos", (float*)((char*)idefixHook + 0x6c), 0.3f);
	}
	KClass *savegame = GetLvlObject(12, 100, 0);
	if(savegame) {
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Potion:");
		ImGui::SameLine();
		if(ImGui::Button("Enable"))
			*(float*)((char*)savegame + 0x9C) = HUGE_VAL;
		ImGui::SameLine();
		if(ImGui::Button("Disable"))
			*(float*)((char*)savegame + 0x9C) = 0.0f;
		ImGui::SameLine();
		ImGui::Checkbox("Lock", &g_cheat_infinitePotion);
	}
			
	/*if(asterixHook)
		if(ImGui::Button("Place Asterix at Egypt skip")) {
			*(float*)((char*)asterixHook + 0x6c) = 278;
			*(float*)((char*)asterixHook + 0x70) = 12;
			*(float*)((char*)asterixHook + 0x74) = 247;
		}*/
	/*KClass *srvCamera = GetLvlObject(1, 3, 0);
	if(srvCamera && asterixHook && obelixHook) {
		if(ImGui::Button("Switch")) {
			CNode **camtargets = (CNode**)((char*)srvCamera + 0xE8);
			uint32_t *axflags, *oxflags, *ixflags;
			axflags = (uint32_t*)((char*)asterixHook + 0x18);
			oxflags = (uint32_t*)((char*)obelixHook + 0x18);
			ixflags = (uint32_t*)((char*)idefixHook + 0x18);
			if((*axflags & 3) == 0) // Is player controlling Asterix?
			{
				*axflags = 3;
				*ixflags = 0;
				camtargets[0] = idefixHook->node;
			}
			else // or Obelix?
			{
				*axflags = 0;
				*ixflags = 3;
				camtargets[0] = asterixHook->node;
			}
			
			//std::swap(camtargets[0], camtargets[1]);
			//camtargets[0] = idefixHook->node;
			//camtargets[1] = idefixHook->node;
		}
	}*/

	KHook *hkInGameInterface = (KHook*)GetLvlObject(2, 132, 0);
	if(hkInGameInterface) {
		if(ImGui::Button("Hide HUD")) {
			typedef void (__thiscall *ft_goVanish)(KHook *hook);
			const ft_goVanish goVanish = (ft_goVanish)0x515CB0;
			goVanish(hkInGameInterface);
		}
	}

	// ImGui::Checkbox("Randomize crates", &enableCrateRandomizer);
	// if(ImGui::IsItemHovered())
	// 	ImGui::SetTooltip("Takes effect after loading a level");

	ImGui::Checkbox("Randomize bonuses", &g_cheat_bonusRandomizer);

	KGroup *grpBonus = (KGroup*)GetLvlObject(4, 48, 0);
	KClass *srvCamera = (KClass*)GetLvlObject(1, 3, 0);
	if(grpBonus && asterixHook) {
		if(ImGui::Button("Spawn bonus over my head")) {
			Vector3 pos, vec(0,0,0);
			pos = *(Vector3*)((char*)asterixHook + 0x6c) + Vector3(0,1,0);
			KHook *bonusHook = CKGrpBonus_spawnBonus(grpBonus, 2, &pos);
			if(bonusHook) {
				//CKHkBasicBonus_bonusFollowHero(bonusHook, &vec, asterixHook);
				CKHkBasicBonus_bonusFall(bonusHook, &vec);
			}
		}
	}

	ImGui::Checkbox("It's raining bonuses!", &g_cheat_rainingBonuses);
	ImGui::Checkbox("Or raining bombs!", &g_cheat_rainingBombs);

	KClass *projBomb = GetLvlObject(12, 79, 0);
	KHook *hkBoss = (KHook*)GetLvlObject(2, 177, 0);

	KGroup *trio = (KGroup*)GetLvlObject(4, 12, 0);
	if(trio) {
		if(ImGui::Button("Switch (F2)"))
			trio->sendEvent(0x4800 | (*((char*)trio + 0x2D) ? 1 : 0), (void*)0);
	}

	// KClass *projBomb, Vector3 *pos, Vector3 *dest, float unk2 = 25.0f, CHkBoss *boss, Vector3 *unk4 = (0,0,0), float unk5 = 1.4f
	if(projBomb && (asterixHook || obelixHook)) {
		ImGui::SameLine();
		if(ImGui::Button("Drop bomb (F3)"))
			SpawnBombFromAllHeroes(projBomb, nullptr, asterixHook, obelixHook);
	}

#elif XXLVER == 2
	KClass *asterixHook = GetLvlObject(2, 218, 2);
	if(asterixHook) {
		ImGui::DragFloat3("Asterix Pos", (float*)((char*)asterixHook + 0x3D4), 0.3f);
	}
	KClass *obelixHook = GetLvlObject(2, 218, 1);
	if(obelixHook) {
		ImGui::DragFloat3("Obelix Pos", (float*)((char*)obelixHook + 0x3D4), 0.3f);
	}
#elif XXLVER == 4
	KClass *asterixHook = GetLvlObject(2, 117, 2);
	if(asterixHook) {
		ImGui::DragFloat3("Asterix Pos", (float*)((char*)asterixHook + 0x540), 0.3f);
	}
	KClass *obelixHook = GetLvlObject(2, 117, 1);
	if(obelixHook) {
		ImGui::DragFloat3("Obelix Pos", (float*)((char*)obelixHook + 0x540), 0.3f);
	}
	KClass *potionGroup = GetLvlObject(4, 35, 0);
	if(potionGroup) {
		if(ImGui::Button("Potion")) {
			__asm {
				mov ecx, potionGroup
				mov eax, 0x00576510
				call eax
			}
		}
		static bool constantPotion = false;
		ImGui::Checkbox("Const Potion", &constantPotion);
		if(constantPotion)
		{
			__asm {
				mov ecx, potionGroup
				mov [ecx+0x44], ecx
				mov eax, 0x00576510
				call eax
			}
		}
	}

#endif
	ImGui::End();
}

void ApplyCheats()
{
#if XXLVER == 1
	//KHook *asterixHook = (KHook*)GetLvlObject(2, 28, 0);
	KGroup *grpBonus = (KGroup*)GetLvlObject(4, 48, 0);
	KClass *srvCamera = (KClass*)GetLvlObject(1, 3, 0);
	KClass *projBomb = GetLvlObject(12, 79, 0);
	KHook *hkBoss = (KHook*)GetLvlObject(2, 177, 0);
	//KGroup *trio = (KGroup*)GetLvlObject(4, 12, 0);
	KClass *savegame = GetLvlObject(12, 100, 0);

	static DWORD rbLastSecond = 0;
	DWORD curSecond = GetTickCount() / 100;
	if(curSecond != rbLastSecond) {
		rbLastSecond = curSecond;
		if(g_cheat_rainingBonuses && grpBonus && srvCamera) {
			Vector3 pos, vec(0,0,0);
			const int BONUS_DIST = 16;
			//const Vector3 &astpos = *(Vector3*)((char*)asterixHook + 0x6c);
			const Vector3 &campos = *(Vector3*)((char*)srvCamera + 0x88);
			Vector3 camdir = *(Vector3*)((char*)srvCamera + 0xAC);
			camdir.y = 0;
			Vector3 camside = camdir.cross(Vector3(0,1,0));
			pos = campos + Vector3(0,5,0);
			pos += camdir * (rand() % BONUS_DIST);
			pos += camside * ((rand() % BONUS_DIST) - BONUS_DIST/2);
			KHook *bonusHook = nullptr;
			for(int i = 0; i < 10 && !bonusHook; i++)
				bonusHook = CKGrpBonus_spawnBonus(grpBonus, (rand()&7)+1, &pos);
			if(bonusHook)
				CKHkBasicBonus_bonusFall(bonusHook, &vec);
		}
		if(g_cheat_rainingBombs && srvCamera && projBomb) {
			Vector3 pos, nv(0,0,0);
			const int BONUS_DIST = 16;
			const Vector3 &campos = *(Vector3*)((char*)srvCamera + 0x88);
			Vector3 camdir = *(Vector3*)((char*)srvCamera + 0xAC);
			camdir.y = 0;
			Vector3 camside = camdir.cross(Vector3(0,1,0));
			pos = campos + Vector3(0,5,0);
			pos += camdir * ((rand() % BONUS_DIST) - BONUS_DIST/2);
			pos += camside * ((rand() % BONUS_DIST) - BONUS_DIST/2);
			Vector3 dest = pos - Vector3(0,5,0) + camdir * 3;
			CKProjectileTypeAsterixBomb_bombSpawn(projBomb, &pos, &dest, 10.0f, hkBoss, &nv, 0.1f);
		}
	}

	if(g_cheat_infinitePotion && savegame) {
		*(float*)((char*)savegame + 0x9C) = HUGE_VAL;
	}

#endif
}

void IGObjElement(KClass *obj, int a, int csz = 0)
{
	const char *name = getObjectName(obj);
	int flags = ImGuiTreeNodeFlags_Leaf;
	if(selectedObject == obj) flags |= ImGuiTreeNodeFlags_Selected;
	if(ImGui::TreeNodeEx(obj, flags, "%i at %p: %s", a, obj, name))
		ImGui::TreePop();
	if(ImGui::IsItemClicked()) {
		selectedObject = obj;
		if(csz) selNodeSize = csz;
	}
}

void IGObjectTree()
{
	if(!showObjectTree) return;
	if(*(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel) == -1) return;
	ImGui::Begin("Object Tree", &showObjectTree);
	void *kcls = *(void**)((char*)yellowPages->loadingManager + 4);
	for(int i = 0; i < 15; i++)
	{
		void *grp = *(void**)((char*)kcls + 12*i);
		if(ImGui::TreeNode(clgrpNames[i]))
		{
			for(int j = 0; j < yellowPages->kclInfos->cls[i].numElements; j++)
			{
				ClassContainer *cl = (ClassContainer*)grp + j;
				char pl = cl->pl;
				uint flg = cl->flags;
				ushort cnt = cl->count;
				ushort csz = cl->clSize;
				char *fstobj = (char*)cl->objects;
				if(!cnt) continue;
				if(ImGui::TreeNode((void*)(i|(j<<6)), "%s (%i)", getClassName(i,j), cnt))
				{
					if(pl) {
						for(int a = 0; a < cnt; a++)
							IGObjElement((KClass*)(fstobj + csz*a), a, csz);
					}
					else if(flg >> 17 == 1)
						IGObjElement((KClass*)fstobj, 0, csz);
					else {
						for(int a = 0; a < cnt; a++)
							IGObjElement((KClass*)(((void**)fstobj)[a]), a, csz);
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

static inline bool inrange(float t, float mini, float maxi)
{
	return (t >= mini) && (t <= maxi);
}

// Beginning of Greece
// Red:    114 26, 153 60 (before the first door)
// Yellow: 185 -1, 235 59 (after the first door)
// Grren:  237 12, 280 50 (After the lion fight)
static ImVec4 GetNodeColor(CNode *node)
{
	float x = node->matrix->c[12], z = node->matrix->c[14];
	if(inrange(x, 114, 153) && inrange(z, 26, 60))
		return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	if(inrange(x, 185, 235) && inrange(z, -1, 59))
		return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	if(inrange(x, 237, 280) && inrange(z, 12, 50))
		return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void IGSTNode(CNode *node)
{
	if(!node) return;
	int grp = node->getClassGroup(), id = node->getClassID();
	bool hasChildren = false;
	if(node->isSubclass(0x24b)) // CSGBranch
		hasChildren = (node->childNode) ? true : false;
	int flags = 0;
	if(!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf;
	if(node == selectedObject) flags |= ImGuiTreeNodeFlags_Selected;
	//ImGui::PushStyleColor(ImGuiCol_Text, GetNodeColor(node));
	const char *name = getObjectName(node);
	bool b = ImGui::TreeNodeEx(node, flags, "%s (%i,%i) at 0x%p: %s", getClassName(grp, id), grp, id, node, name);
	//ImGui::PopStyleColor();
	if(ImGui::IsItemClicked())
		selectedObject = node;
	if(b) {
		if(hasChildren) {
			IGSTNode(node->childNode);
		}
		ImGui::TreePop();
	}
	IGSTNode(node->nextNode);
}

void IGSceneTree()
{
	if(!showSceneTree) return;
	if(*(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel) == -1) return;
	ImGui::Begin("Scene Tree", &showSceneTree);
	uint ref = 11 | (1 << 6);
	kfRefToPnt(yellowPages->loadingManager, &ref);
	IGSTNode((CNode*)ref);
	ImGui::End();
}

struct IOFile : KFile {
	FILE *file;
	void read(void *dst, size_t size) override {fread(dst, size, 1, file);}
	void write(void *src, size_t size) override {fwrite(src, size, 1, file);}
	IOFile(const char *filename, const char *mode) {file = fopen(filename, mode);}
	~IOFile() {if(file) fclose(file);}
};

void IGSelectedObject()
{
	if(!showSelectedObject) return;
	ImGui::Begin("Selected Object", &showSelectedObject);
	if(!selectedObject)
		ImGui::Text("No object selected!\nPlease select one in the Scene Tree, Level Tree or Hook Tree.");
	else {
		KClass *obj = selectedObject;
		int grp = obj->getClassGroup(), id = obj->getClassID();
		ImGui::Text("%s (%i,%i) at 0x%p", getClassName(grp, id), grp, id, obj);
		ImGui::Text("%s", getObjectName(obj));
		static char eventinput[10], eventparam[12]; static int eventresult = -1;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 2);
		ImGui::InputTextWithHint("##EventInput", "Event ID", eventinput, 9, ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::InputTextWithHint("##EventParam", "Parameter", eventparam, 11, 0);
		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
		if(ImGui::Button("Send##event"))
			eventresult = obj->sendEvent(strtoul(eventinput, nullptr, 16), (void*)atoi(eventparam));
		ImGui::SameLine();
		ImGui::Text("Last event result: %i", eventresult);
#if XXLVER == 1
		if(ImGui::Button("Serialize")) {
			IOFile file("test.bin", "wb");
			file.savingManager = yellowPages->savingManager;
			file.loadingManager = yellowPages->loadingManager;
			// file.savingManager = nullptr;
			// file.loadingManager = nullptr;
			obj->serialize(&file, (void*)4);
		}
#endif
		if(obj->isSubclass(0x14b)) // CSGMovable
			if(ImGui::CollapsingHeader("Movable")) {
				CNode *node = (CNode*)obj;
				ImGui::DragFloat3("Local  Position", (float*)&(node->matrix->c[12]));
				ImGui::DragFloat3("Global Position", (float*)&(node->matrix->d[12]));

				KClass *srvCamera = GetLvlObject(1, 3, 0);
				if(srvCamera) {
					CNode **camtargets = (CNode**)((char*)srvCamera + 0xE8);
					if(ImGui::Button("Attach camera"))
						camtargets[0] = node;
				}
			}
#if XXLVER == 1
		if(obj->isSubclass(2, 44)) // CKHkCorkscrew
			if(ImGui::CollapsingHeader("Corkscrew")) {
				ImGui::DragFloat("Speedy thing?", (float*)((char*)obj + 0x14));
				ImGui::DragFloat("Next height", (float*)((char*)obj + 0x28));
				if(ImGui::Button("L0")) *(float*)((char*)obj + 0x28) = 0.0f;
				if(ImGui::Button("L6")) *(float*)((char*)obj + 0x28) = 6.0f;
			}
#endif
		ImGui::Separator();
		static MemoryEditor memedit;
		ImGui::InputInt("Size", &selNodeSize);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Selected address: ");
		ImGui::SameLine();
		if(ImGui::Button("To Obj Select"))
			selectedObject = *(KClass**)((char*)obj + memedit.DataPreviewAddr);
		ImGui::SameLine();
		if(ImGui::Button("To Hex View"))
			g_clfndres = *(KClass**)((char*)obj + memedit.DataPreviewAddr);
		ImGui::Text("%p/%p/%p", memedit.DataEditingAddr, memedit.DataPreviewAddr, memedit.GotoAddr);
		memedit.DrawContents(obj, selNodeSize);
	}
	ImGui::End();
}

char* strguard(char *str, char *failsafe)
{
	return str ? str : failsafe;
}

void IGCrateDetector()
{
	if(!showCrateDetector) return;
	if(*(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel) == -1) return;

	KClass *asterix = 0, *axGround = 0;
	KGroup *crateGroup = 0; KHook *firstCrateHook = 0;
#if XXLVER == 1
	asterix = GetLvlObject(2, 28, 0);
	if(asterix)
		axGround = *(KClass**)((char*)asterix + 0x280);
	crateGroup = (KGroup*)GetLvlObject(4, 60, 1);
	if(crateGroup)
		firstCrateHook = *(KHook**)((char*)crateGroup + 0x20);
#endif

	static float ts = 5, tp[2] = {10,10};
	ImGui::Begin("Crate detector", &showCrateDetector);
	ImGui::DragFloat("Scale", &ts);
	ImGui::DragFloat2("Translate", tp);
#if XXLVER == 1
	if(ImGui::Button("Center on Asterix")) {
		KHook *asterixHook = (KHook*)GetLvlObject(2, 28, 0);
		if(asterixHook) {
			float *pos = (float*)((char*)asterixHook + 0x6c);
			tp[0] = (float)gameStartInfo->width  * 0.5f - pos[0] * ts;
			tp[1] = (float)gameStartInfo->height * 0.5f - pos[2] * ts;
		}
	}
#endif
	ImDrawList *drawlist = ImGui::GetWindowDrawList();
	ImGui::End();
	drawlist->PushClipRectFullScreen();
	//drawlist->AddLine(ImVec2(0,0), ImVec2(64,64), -1);
#if XXLVER <= 2
	//for(uint i = 0; i < GetNumLvlClassInstances(2, 112); i++) {
	int i = 0;
	for(KHook *hook = firstCrateHook; hook; hook = hook->nextHook) {
		//KClass *hook = GetLvlObject(2, 112, i);
		CNode *branch = *(CNode**)((char*)hook + ((XXLVER==2)?0x1C:0x10));
		KClass *crateGround = *(KClass**)((char*)hook + 0x50);
		//if(logfile) fprintf(logfile, "branch: %p\n", branch);
		float x = branch->matrix->c[12], z = branch->matrix->c[14];
		char s[2] = {*(char*)((char*)hook + 8) + '0', 0};
		//char s[2] = {(i < 10) ? (i+'0') : 'X', 0};
		ImVec2 drawpos(floor(x*ts+tp[0]), floor(z*ts+tp[1]));
		drawlist->AddText(drawpos, ImGui::GetColorU32(GetNodeColor(branch)), s);
		if(hook == firstCrateHook)
			drawlist->AddRect(drawpos, ImVec2(drawpos.x+10,drawpos.y+14), 0xFF00FF00);
		if(crateGround == axGround)
			drawlist->AddRect(drawpos, ImVec2(drawpos.x+8,drawpos.y+12), 0xFFFFFF00);
		i++;
	}
#endif
#if XXLVER == 1
	KClass *asterixHook = GetLvlObject(2, 28, 0);
	if(asterixHook) {
		float px = *(float*)((char*)asterixHook + 0x6c),
			  pz = *(float*)((char*)asterixHook + 0x74);
		drawlist->AddText(ImVec2(px*ts+tp[0], pz*ts+tp[1]), -1, "A");
	}
	KClass *obelixHook = GetLvlObject(2, 29, 0);
	if(obelixHook) {
		float px = *(float*)((char*)obelixHook + 0x6c),
			  pz = *(float*)((char*)obelixHook + 0x74);
		drawlist->AddText(ImVec2(px*ts+tp[0], pz*ts+tp[1]), -1, "B");
	}
	drawlist->PopClipRect();
#endif
}

void EnumKHook(KHook *hook)
{
	int clcat = hook->getClassGroup(), clid = hook->getClassID();
	const char *name = getObjectName(hook);
	bool bb = ImGui::TreeNodeEx(hook, ImGuiTreeNodeFlags_Leaf | ((selectedObject == hook) ? ImGuiTreeNodeFlags_Selected : 0), "%s (%i,%i): %s", getClassName(clcat, clid), clcat, clid, name);
	if(ImGui::IsItemClicked()) {
		selectedObject = hook;
		//selNodeSize = ax_msize(hook);
	}
	if(bb)
		ImGui::TreePop();
	if(hook->nextHook)
		EnumKHook(hook->nextHook);
}

void EnumKGroup(KGroup *grp)
{
	int clcat = grp->getClassGroup(), clid = grp->getClassID();
	const char *name = getObjectName(grp);
	bool bb = ImGui::TreeNodeEx(grp, (selectedObject == grp) ? ImGuiTreeNodeFlags_Selected : 0, "%s (%i,%i): %s", getClassName(clcat, clid), clcat, clid, name);
	if(ImGui::IsItemClicked())
		selectedObject = grp;
	if(bb) {
		if(grp->child)
			EnumKGroup(grp->child);
		if(grp->hook)
			EnumKHook(grp->hook);
		ImGui::TreePop();
	}
	if(grp->next)
		EnumKGroup(grp->next);
}

void IGHookTree()
{
	if(!showHookTree) return;
	if(*(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel) == -1) return;
	KGroup *grproot = (KGroup*)GetLvlObject(4, 1, 0); // There's a ref in CKCoreManager too...
	ImGui::Begin("Hook tree", &showHookTree);
	EnumKGroup(grproot);
	ImGui::End();
}

#if XXLVER >= 2
void EnumKComparedData(CKComparedData *data)
{
	ImGui::BulletText("%s\n%s", getObjectName(data), getObjectName((KClass*)data->data));
}

void EnumKCondition(CKCondition *condition)
{
	bool isCombiner = condition->isSubclass(12, 139);
	bool isComparator = condition->isSubclass(12, 140);
	bool bb = ImGui::TreeNodeEx(condition, isCombiner ? 0 : 0 /*ImGuiTreeNodeFlags_Leaf*/, "[C] %s", getObjectName(condition));
	if(ImGui::IsItemClicked())
		selectedObject = condition;
	if(bb) {
		if(isCombiner) {
			CKCombiner *combiner = (CKCombiner*)condition;
			EnumKCondition(combiner->firstCondition);
		}
		if(isComparator) {
			CKComparator *comparator = (CKComparator*)condition;
			EnumKComparedData(comparator->left);
			EnumKComparedData(comparator->right);
		}
		ImGui::TreePop();
	}
	if(condition->next)
		EnumKCondition(condition->next);
}

void EnumKTriggerDomain(CKTriggerDomain *domain)
{
	bool bbd = ImGui::TreeNode(domain, "[D] %s", getObjectName(domain));
	if(ImGui::IsItemClicked())
		selectedObject = domain;
	if(bbd) {
		for(auto it = domain->subdomains.begin(); it != domain->subdomains.end(); it++)
			EnumKTriggerDomain(*it);
		for(auto it = domain->triggers.begin(); it != domain->triggers.end(); it++) {
			CKTrigger *trigger = *it;
			bool bbt = ImGui::TreeNodeEx(trigger, 0, "[T] %s", getObjectName(trigger));
			if(ImGui::IsItemClicked())
				selectedObject = trigger;
			if(bbt) {
				if(ImGui::Button("Try")) {
					for(size_t a = 0; a < trigger->numActions; a++) {
						KAction &action = trigger->actions[a];
						action.object->sendEvent(action.type, action.param);
					}
				}
				EnumKCondition(trigger->condition);
				for(size_t a = 0; a < trigger->numActions; a++) {
					KAction &action = trigger->actions[a];
					int grp = action.object->getClassGroup(), cid = action.object->getClassID();
					ImGui::BulletText("OBJ: %s (%i,%i): %s\nMSG: 0x%04X\nARG: %X", getClassName(grp, cid), grp, cid, getObjectName(action.object), action.type, action.param);
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}
#endif

void IGTriggerTree()
{
#if XXLVER >= 2
	if(!showTriggerTree) return;
	if(*(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel) == -1) return;
	CKSrvTrigger *srvTrigger = (CKSrvTrigger*)GetLvlObject(1, 18, 0);
	ImGui::Begin("Trigger Tree", &showTriggerTree);
	EnumKTriggerDomain(srvTrigger->rootDomain);
	ImGui::End();
#endif
}

void IGNewFrame()
{
	static bool show_demo_window = false;
	//static char clfndbox[128];
	
	ImGui_ImplDX9_NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	unsigned char *pixels; int width,height,bytes_per_pixel;
	
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	static bool showMainWindow = true;
	if(ImGui::IsKeyPressed(VK_F1))
		showMainWindow = !showMainWindow;
#if XXLVER == 1
	if(ImGui::IsKeyPressed(VK_F2)) {
		KGroup *trio = (KGroup*)GetLvlObject(4, 12, 0);
		if(trio) {
			trio->sendEvent(0x4800 | (*((char*)trio + 0x2D) ? 1 : 0), (void*)0);
		}
	}
	if(ImGui::IsKeyPressed(VK_F3)) {
		KClass *projBomb = GetLvlObject(12, 79, 0);
		//KHook *hkBoss = (KHook*)GetLvlObject(2, 177, 0);
		KHook *asterixHook = (KHook*)GetLvlObject(2, 28, 0);
		KHook *obelixHook = (KHook*)GetLvlObject(2, 29, 0);
		if(projBomb && asterixHook)
			SpawnBombFromAllHeroes(projBomb, nullptr, asterixHook, obelixHook);
	}
#endif
	ApplyCheats();

if(showMainWindow)
{
	ImGui::Begin("XXL Inspector");

	ImGui::Text(INSPECTOR_VERSION " by AdrienTD");

	ImGui::Checkbox("Scene Tree", &showSceneTree);
	ImGui::SameLine(); ImGui::Checkbox("Selected Object", &showSelectedObject);
	ImGui::SameLine(); ImGui::Checkbox("Level objects", &showObjectTree);
#if XXLVER >= 2
	ImGui::SameLine(); ImGui::Checkbox("Global objects", &showGameObjectsList);
#endif
	/*ImGui::SameLine();*/ ImGui::Checkbox("Hooks", &showHookTree);
	ImGui::SameLine(); ImGui::Checkbox("Cheats", &showCheats);
#if XXLVER == 1
	ImGui::SameLine(); ImGui::Checkbox("Crate detector", &showCrateDetector);
#endif
#if XXLVER >= 2
	ImGui::SameLine(); ImGui::Checkbox("Trigger tree", &showTriggerTree);
#endif
	ImGui::SameLine(); ImGui::Checkbox("ImGui Demo", &show_demo_window);
	ImGui::Separator();

	ImGui::Text("Current level: %i", *(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel));
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Go to level:"); ImGui::SameLine();
	static char nextlvlstr[64];
	bool newlvlenter = ImGui::InputText("##NextLVL", nextlvlstr, 63, ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();
	if(ImGui::Button("Go##LVL") || newlvlenter)
		*(int*)((char*)yellowPages->gameManager + m_CKGameManager_nextLevel) = atoi(nextlvlstr);
	//ImGui::Separator();

	char *levelobj = *(char**)((char*)yellowPages->gameManager + m_CKGameManager_levelObject);
	if(levelobj) {
		ImGui::Text("Current sector: %i", *(int*)(levelobj + m_CKLevel_currentSector));
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Go to sector:"); ImGui::SameLine();
		static char nextsecstr[64];
		bool newsecenter = ImGui::InputText("##NextSTR", nextsecstr, 63, ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SameLine();
		if(ImGui::Button("Go##STR") || newsecenter)
			*(int*)(levelobj + m_CKLevel_nextSector) = atoi(nextsecstr);
	}
	ImGui::Separator();

	// ImGui::InputText("Cl", clfndbox, 127);
	// ImGui::SameLine();
	// if(ImGui::Button("Go")) {
	// 	char copyofclfndbox[128];
	// 	strncpy(copyofclfndbox, clfndbox, 127); copyofclfndbox[127] = 0;
	// 	uint clgrp = strtoul(strguard(strtok(copyofclfndbox, " .,;:/-"), "0"), NULL, 10);
	// 	uint clid = strtoul(strguard(strtok(NULL, " .,;:/-"), "0"), NULL, 10);
	// 	uint iid = strtoul(strguard(strtok(NULL, " .,;:/-"), "0"), NULL, 10);
	// 	uint ref = clgrp | (clid << 6) | (iid << 17);
	// 	void* lm = yellowPages->loadingManager;
	// 	kfRefToPnt(lm, &ref);
	// 	clfndres = (void*)ref;
	// }

	ImGui::Text("Size: %ux%u", gameStartInfo->width, gameStartInfo->height);
	ImGui::Text("Load manager: %p", yellowPages->loadingManager); ImGui::SameLine();
	if(ImGui::SmallButton("Go##LM"))
		g_clfndres = yellowPages->loadingManager;
	ImGui::Text("Game manager: %p", yellowPages->gameManager); ImGui::SameLine();
	if(ImGui::SmallButton("Go##GM"))
		g_clfndres = yellowPages->gameManager;
	ImGui::Text("Yellow Pages: %p", yellowPages); ImGui::SameLine();
	if(ImGui::SmallButton("Go##YP"))
		g_clfndres = yellowPages;
	void *tim = *(void**)((char*)yellowPages->gameLoop + 8);
	ImGui::Text("Timer object: %p", tim); ImGui::SameLine();
	if(ImGui::SmallButton("Go##TO"))
		g_clfndres = tim;
	ImGui::Separator();

	ImGui::Text("Memory address: %p", g_clfndres);
	ImGui::AlignTextToFramePadding(); ImGui::Text("Go to:"); ImGui::SameLine();
	static uint nextmemadr = 0;
	bool memadrentered = ImGui::InputScalar("##memadr", ImGuiDataType_U32, &nextmemadr, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();
	if(ImGui::Button("Go##MEMADR") || memadrentered)
		g_clfndres = (void*)nextmemadr;
	ImGui::SameLine();
	if(ImGui::Button("Reset"))
		g_clfndres = 0;

	if(g_clfndres) {
		ImGui::Separator();
		static MemoryEditor memedit;
		static int memsize = 32;
		ImGui::InputInt("Size", &memsize);
		memedit.DrawContents(g_clfndres, memsize);
	}
	
	ImGui::End();

	IGSceneTree();
	IGSelectedObject();
	IGObjectTree();
	IGCheats();
	IGGameObjectsList();
	IGHookTree();
	IGTriggerTree();
	
	if(show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	IGCrateDetector();
}
	ImGui::EndFrame();

	//Sleep(50);
}

void IGRender()
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void IGDX9Reset()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

LRESULT __stdcall IGWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
}
