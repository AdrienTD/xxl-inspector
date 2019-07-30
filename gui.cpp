
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
	showHookTree = false;
KClass *selectedObject = 0;
int selNodeSize = 32;

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

void IGCheats()
{
	if(!showCheats) return;
	if(*(int*)((char*)yellowPages->gameManager + m_CKGameManager_currentLevel) == -1) return;
	ImGui::Begin("Cheats", &showCheats);

	void *tim = *(void**)((char*)yellowPages->gameLoop + 8);
	ImGui::DragFloat("Game speed", (float*)((char*)tim + 0x10), 0.1f);

#if XXLVER == 1
	KClass *asterixHook = GetLvlObject(2, 28, 0);
	if(asterixHook) {
		ImGui::DragFloat3("Asterix Pos", (float*)((char*)asterixHook + 0x6c), 0.3f);
	}
	KClass *obelixHook = GetLvlObject(2, 29, 0);
	if(obelixHook) {
		ImGui::DragFloat3("Obelix Pos", (float*)((char*)obelixHook + 0x6c), 0.3f);
	}
	KClass *savegame = GetLvlObject(12, 100, 0);
	if(savegame)
		if(ImGui::Button("Infinite potion"))
			*(float*)((char*)savegame + 0x9C) = HUGE_VAL;
	if(asterixHook)
		if(ImGui::Button("Place Asterix at Egypt skip")) {
			*(float*)((char*)asterixHook + 0x6c) = 278;
			*(float*)((char*)asterixHook + 0x70) = 12;
			*(float*)((char*)asterixHook + 0x74) = 247;
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

void IGObjElement(KClass *obj, int a, int csz = 0)
{
	char *name = lvlObjectNames[yellowPages->loadingManager->currentSector][obj];
	if(!name) name = lvlObjectNames[0][obj];
	if(!name) name = "?";
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
	char *name = lvlObjectNames[yellowPages->loadingManager->currentSector][node];
	if(!name) name = lvlObjectNames[0][node];
	if(!name) name = "?";
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
		if(obj->isSubclass(0x14b)) // CSGMovable
			if(ImGui::CollapsingHeader("Movable")) {
				CNode *node = (CNode*)obj;
				ImGui::DragFloat3("Local  Position", (float*)&(node->matrix->c[12]));
				ImGui::DragFloat3("Global Position", (float*)&(node->matrix->d[12]));
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
		if(ImGui::Button("Go to selected address"))
			selectedObject = *(KClass**)((char*)obj + memedit.DataPreviewAddr);
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
	KClass *crateGroup = 0, *firstCrateHook = 0;
#if XXLVER == 1
	asterix = GetLvlObject(2, 28, 0);
	if(asterix)
		axGround = *(KClass**)((char*)asterix + 0x280);
	crateGroup = GetLvlObject(4, 60, 1);
	if(crateGroup)
		firstCrateHook = *(KClass**)((char*)crateGroup + 0x20);
#endif

	static float ts = 5, tp[2] = {10,10};
	ImGui::Begin("Crate detector", &showCrateDetector);
	ImGui::DragFloat("Scale", &ts);
	ImGui::DragFloat2("Translate", tp);
	ImDrawList *drawlist = ImGui::GetWindowDrawList();
	ImGui::End();
	drawlist->PushClipRectFullScreen();
	//drawlist->AddLine(ImVec2(0,0), ImVec2(64,64), -1);
#if XXLVER <= 2
	for(uint i = 0; i < GetNumLvlClassInstances(2, 112); i++) {
		KClass *hook = GetLvlObject(2, 112, i);
		CNode *branch = *(CNode**)((char*)hook + ((XXLVER==2)?0x1C:0x10));
		KClass *crateGround = *(KClass**)((char*)hook + 0x50);
		//if(logfile) fprintf(logfile, "branch: %p\n", branch);
		float x = branch->matrix->c[12], z = branch->matrix->c[14];
		char s[2] = {*(char*)((char*)hook + 8) + '0', 0};
		ImVec2 drawpos(floor(x*ts+tp[0]), floor(z*ts+tp[1]));
		drawlist->AddText(drawpos, ImGui::GetColorU32(GetNodeColor(branch)), s);
		if(hook == firstCrateHook)
			drawlist->AddRect(drawpos, ImVec2(drawpos.x+10,drawpos.y+14), 0xFF00FF00);
		if(crateGround == axGround)
			drawlist->AddRect(drawpos, ImVec2(drawpos.x+8,drawpos.y+12), 0xFFFFFF00);
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
	char *name = lvlObjectNames[yellowPages->loadingManager->currentSector][hook];
	if(!name) name = lvlObjectNames[0][hook];
	if(!name) name = "?";
	bool bb = ImGui::TreeNodeEx(hook, ImGuiTreeNodeFlags_Leaf, "%s (%i,%i): %s", getClassName(clcat, clid), clcat, clid, name);
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
	char *name = lvlObjectNames[yellowPages->loadingManager->currentSector][grp];
	if(!name) name = lvlObjectNames[0][grp];
	if(!name) name = "?";
	bool bb = ImGui::TreeNode(grp, "%s (%i,%i): %s", getClassName(clcat, clid), clcat, clid, name);
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

void IGNewFrame()
{
	static bool show_demo_window = false;
	static char clfndbox[128];
	static void* clfndres = 0;
	
	ImGui_ImplDX9_NewFrame();
	ImGuiIO& io = ImGui::GetIO();
	unsigned char *pixels; int width,height,bytes_per_pixel;
	
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	ImGui::Begin("XXL Inspector");

	ImGui::Text("v0.1 by AdrienTD");

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
		clfndres = yellowPages->loadingManager;
	ImGui::Text("Game manager: %p", yellowPages->gameManager); ImGui::SameLine();
	if(ImGui::SmallButton("Go##GM"))
		clfndres = yellowPages->gameManager;
	ImGui::Text("Yellow Pages: %p", yellowPages); ImGui::SameLine();
	if(ImGui::SmallButton("Go##YP"))
		clfndres = yellowPages;
	void *tim = *(void**)((char*)yellowPages->gameLoop + 8);
	ImGui::Text("Timer object: %p", tim); ImGui::SameLine();
	if(ImGui::SmallButton("Go##TO"))
		clfndres = tim;
	ImGui::Separator();

	ImGui::Text("Memory address: %p", clfndres);
	ImGui::AlignTextToFramePadding(); ImGui::Text("Go to:"); ImGui::SameLine();
	static uint nextmemadr = 0;
	bool memadrentered = ImGui::InputScalar("##memadr", ImGuiDataType_U32, &nextmemadr, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();
	if(ImGui::Button("Go##MEMADR") || memadrentered)
		clfndres = (void*)nextmemadr;
	ImGui::SameLine();
	if(ImGui::Button("Reset"))
		clfndres = 0;

	if(clfndres) {
		ImGui::Separator();
		static MemoryEditor memedit;
		static int memsize = 32;
		ImGui::InputInt("Size", &memsize);
		memedit.DrawContents(clfndres, memsize);
	}
	
	ImGui::End();

	IGSceneTree();
	IGSelectedObject();
	IGObjectTree();
	IGCheats();
	IGGameObjectsList();
	IGHookTree();
	
	if(show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	IGCrateDetector();
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
