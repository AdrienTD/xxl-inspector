///////////////////////////
/// CSX (Class Serialization eXtraction)
///////////////////////////

#include "global.h"
#include "csx.h"
#include "imgui/imgui.h"
#include "imgui_memory_editor.h"

#include <algorithm>
#include <list>
#include <numeric>
#include <string>



///////////////////////////
// Code injection part
///////////////////////////


// FILE* g_csxFile;

std::map<void*, CSXReader> g_csx_readers;
std::map<KClass*, std::shared_ptr<CSXObject>> g_csx_objects;
std::map<uint32_t, CSXClass> g_csx_classes;

std::shared_ptr<CSXObject> g_csx_currentObj;
void* g_csx_nextIsRef = nullptr;

bool canBeFloat(void* data) {
	// reject denormals (note that 0.0f which has lowest exponent is often used!)
	return *(uint32_t*)data & 0x7F80'0000;
}


void __stdcall onBeforeDeserialize(KClass* obj) {
	int clcat = obj->getClassGroup(), clid = obj->getClassID();
	// fprintf(g_csxFile, "\nObject %s (%i %i)\n", getClassName(clcat, clid), clcat, clid);

	uint32_t clfid = clcat | (clid << 6);
	if(!g_csx_classes.count(clfid)) {
		CSXClass cc;
		cc.kClassFullID = clfid;
		g_csx_classes[clfid] = std::move(cc);
	}
	auto cobj = std::make_shared<CSXObject>();
	cobj->kObject = obj;
	cobj->cClass = &g_csx_classes[clfid];
	g_csx_objects[obj] = cobj;
	g_csx_classes[clfid].objects.push_back(cobj);
	g_csx_currentObj = cobj;
}

void __stdcall onAfterDeserialize(KClass* obj) {
	int clcat = obj->getClassGroup(), clid = obj->getClassID();
	// fprintf(g_csxFile, "END Object %s (%i %i)\n", getClassName(clcat, clid), clcat, clid);
	g_csx_currentObj = nullptr;
}

// hook before calling KClass::deserialize
void naked hook_40AF6F() {
	__asm {
		pushad
		push esi
		call onBeforeDeserialize
		popad
		
		mov ecx, esi
		call [edx+0x24]

		push esi
		call onAfterDeserialize

		mov eax, 0x40AF74
		jmp eax
	}
}

// hook before calling KClass::deserialize (2nd)
void naked hook_40AFEE() {
	__asm {
		pushad
		push edi
		call onBeforeDeserialize
		popad
		
		mov ecx, edi
		call [edx+0x24]

		push edi
		call onAfterDeserialize

		mov eax, 0x40AFF3
		jmp eax
	}
}

void __stdcall onReadBytes(void* caller, void* out, int size) {
	//fprintf(g_csxFile, "  %p: Read %i bytes -> %p\n", caller, size, out);
}

void __stdcall onBytesRead(void* caller, void* out, int size) {
	// fprintf(g_csxFile, "  %p: Read %4i bytes ->", caller, size);
	// //fprintf(g_csxFile, "      ");
	// int cnt = std::min(size, 16);
	// for(int i = 0; i < cnt; ++i) {
	// 	fprintf(g_csxFile, " %02X", ((uint8_t*)out)[i]);
	// }
	// if(size == 4 && canBeFloat(out))
	// 	fprintf(g_csxFile, " (float)");
	// fprintf(g_csxFile, "\n");

	if(g_csx_currentObj) {
		if(g_csx_nextIsRef)
			caller = g_csx_nextIsRef;
		if(!g_csx_readers.count(caller)) {
			CSXReader cr;
			cr.address = caller;
			if(g_csx_nextIsRef)
				cr.knownType = TypeRef;
			g_csx_readers[caller] = std::move(cr);
		}

		auto ri = std::make_unique<CSXReadInst>();
		ri->object = g_csx_currentObj;
		ri->reader = &g_csx_readers[caller];
		ri->data.resize(size);
		memcpy(ri->data.data(), out, size);
		ri->reader->readInsts.push_back(ri.get());
		g_csx_currentObj->reads.push_back(std::move(ri));
	}

	g_csx_nextIsRef = nullptr;
}

// hook for reading from file
void naked vftrep_622350() {
	__asm {
		pushad
		push [esp+32+4+4] // pushad + retaddr + arg[0]
		push [esp+32+4+4]
		push [esp+32+4+4]
		call onReadBytes
		popad

		mov eax, 0x401C60
		push [esp+4+4]
		push [esp+4+4]
		call eax

		pushad
		push [esp+32+4+4]
		push [esp+32+4+4]
		push [esp+32+4+4]
		call onBytesRead
		popad

		ret 8
	}
}

void __stdcall onReadReference(void* caller, KFile* file, void** out) {
	// fprintf(g_csxFile, "  %p: Read REF -> %p\n", caller, out);
	if(!g_csx_nextIsRef)
		g_csx_nextIsRef = caller;
}

// hook for reading reference
void naked vftrep_622844() {
	__asm {
		pushad
		push [esp+32+4+4] // pushad + retaddr + arg[0]
		push [esp+32+4+4]
		push [esp+32+4+4]
		call onReadReference
		popad
		mov eax, 0x40D410
		push [esp+4+4]
		push [esp+4+4]
		call eax
		ret 8
	}
}

void __stdcall onReadReferenceList(void* caller, KFile* file, void** out, int count) {
	// fprintf(g_csxFile, "  %p: Read REF -> %p\n", caller, out);
	if(!g_csx_nextIsRef)
		g_csx_nextIsRef = caller;
}

// hook for reading reference list
void naked vftrep_622848() {
	__asm {
		pushad
		push [esp+32+4+8] // pushad + retaddr + 2 first arguments
		push [esp+32+4+8]
		push [esp+32+4+8]
		push [esp+32+4+8]
		call onReadReferenceList
		popad
		mov eax, 0x40D440
		push [esp+4+8]
		push [esp+4+8]
		push [esp+4+8]
		call eax
		ret 12
	}
}

void naked hook_47CA00() {
	__asm {
		pushad
		push [esp+32+4]
		push ecx
		push [esp+32+8]
		call onReadReference
		popad

		mov eax, [ecx+0x10]
		test eax, eax
		mov edx, 0x47CA05
		jmp edx
	}
}

void naked hook_447D00() {
	__asm {
		pushad
		push [esp+32+4+4]
		push [esp+32+4+4]
		push ecx
		push [esp+32+12]
		call onReadReferenceList
		popad

		mov eax, [ecx+0x10]
		test eax, eax
		mov edx, 0x447D05
		jmp edx
	}
}

void InitCSX() {
	SetImmediateJump((void*)0x40AF6F, (uint)hook_40AF6F);
	SetImmediateJump((void*)0x40AFEE, (uint)hook_40AFEE);
	SetMemProtection((void*)0x6222C8, PAGE_READWRITE);
	*(void**)0x622350 = vftrep_622350;
	*(void**)0x622844 = vftrep_622844;
	*(void**)0x622848 = vftrep_622848;
	SetImmediateJump((void*)0x47CA00, (uint)hook_47CA00);
	SetImmediateJump((void*)0x447D00, (uint)hook_447D00);
	//g_csxFile = fopen("C:\\Users\\Adrien\\Desktop\\csx.txt", "w");
}


///////////////////////////
// GUI Part
///////////////////////////


template <typename T>
T gcd(T a, T b) {
	while(a != b) {
		if(a > b) a = a - b;
		else b = b - a;
	}
	return a;
}

template <typename T>
T ngcd(T a, T b) {
	if(!a) return b;
	if(!b) return a;
	return gcd(a, b);
}

static constexpr const char* dataTypeNames[] = {
	"Unk",
	"Int8",
	"Int16",
	"Int32",
	"Float",
	"ObjRef",
	"PostRef",
	"Vector3",
	"Event",
	"StringSize",
	"String",
	"MarkerIndex"
};

std::list<std::string> genCodes;

std::string GetClassShortname(const char* clname) {
	std::string str;
	size_t len = strlen(clname);
	for(size_t i = 0; i < len; ++i)
		if(clname[i] >= 'A' && clname[i] <= 'Z')
			str += clname[i] - 'A' + 'a';
	return str;
}

void GenerateCode(CSXObject* cobj, CSXReadInst* notBefore) {
	static const std::string cpptypes[] = {
		"?", "uint8_t", "uint16_t", "int32_t", "float", "kobjref<CKObject>", "KPostRef", "Vector3", "EventNode", "std::string", "std::string", "MarkerIndex"
	};
	static const std::string kclcatNames[15] = {
		"CKManager", "CKService", "CKHook",
		"CKHookLife", "CKGroup", "CKGroupLife",
		"CKComponent", "CKCameraBase", "CKCinematicNode",
		"CKDictionary", "CKAnyGeometry", "CKSceneNode",
		"CKLogic", "CKGraphical", "CKError"
	};
	std::string header, impl;
	char buf[256];

	CSXClass* cl = cobj->cClass;
	int clcat = cl->kClassFullID & 63;
	int clid = cl->kClassFullID >> 6;
	const char* clname = getClassName(clcat, clid);
	std::string clShortName = GetClassShortname(clname);
	sprintf_s(buf, "struct %s : CKMRSubclass<%s, CKReflectableSomething, %i> {\n", clname, clname, clid);
	header += buf;

	sprintf_s(buf, "void %s::reflectMembers2(MemberListener& r, KEnvironment* kenv) {\n", clname);
	impl += buf;

	int unknum = 0;
	bool skip = true;
	for(auto& ri : cobj->reads) {
		if(skip && ri.get() != notBefore)
			continue;
		skip = false;

		CSXReader* rd = ri->reader;
		if(rd->type == TypeString)
			continue;
		header += '\t';
		switch(rd->arrayCount) {
			case 0:
				header += "std::vector<" + cpptypes[rd->type] + '>';
				break;
			case 1:
				if(rd->type == TypeRef) {
					header += "kobjref<";
					// find best parameter type (could be better)
					int bestcat = -1, bestid = -1;
					bool firstTime = true;
					for(CSXReadInst* ar : rd->readInsts) {
						uint32_t ref = *(uint32_t*)ar->data.data();
						if(ref == 0xFFFFFFFF)
							continue;
						int refcat = ref & 63;
						int refid = (ref >> 6) & 2047;
						if(firstTime) {
							bestcat = refcat; bestid = refid;
							firstTime = false;
						} else {
							if(refcat != bestcat) bestcat = -1;
							if(refid != bestid) bestid = -1;
							if(bestcat == -1 && bestid == -1)
								break;
						}
					}
					if(bestcat != -1 && bestid != -1)
						header += getClassName(bestcat, bestid);
					else if(bestcat != -1)
						header += kclcatNames[bestcat];
					else
						header += "CKObject";
					header += '>';
				} else {
					header += cpptypes[rd->type];
				}
				break;
			default:
				header += "std::array<" + cpptypes[rd->type] + ", " + std::to_string(rd->arrayCount) + '>';
				break;
		}
		std::string varname = clShortName + "Unk" + std::to_string(unknum++);
		header += ' ' + varname + ";\n";
		impl += "\tr.reflect(" + varname + ", \"" + varname + '\"';
		if(rd->type == TypeEvent)
			impl += ", this";
		impl += ");\n";
	}
	header += "\tvoid reflectMembers2(MemberListener& r, KEnvironment* kenv);\n";
	header += "};\n";
	impl += "};\n";

	genCodes.push_back(header + impl);
}

void GuessDataType(CSXReader* reader) {
	auto sizemm = std::minmax_element(reader->readInsts.begin(), reader->readInsts.end(), [](CSXReadInst* l, CSXReadInst* r) {return l->data.size() < r->data.size();});
	size_t sizemin = (*sizemm.first)->data.size();
	size_t sizemax = (*sizemm.second)->data.size();
	size_t sizegcd = std::accumulate(reader->readInsts.begin(), reader->readInsts.end(), (size_t)0, [](size_t l, CSXReadInst* r) {return ngcd(r->data.size(), l);});

	if(reader->knownType == TypeRef) {
		reader->type = CSXDataType::TypeRef;
		reader->arrayCount = (sizemin == sizemax) ? (sizemin / 4) : 0;
	}
	else if(reader->address == (void*)0x40D425) {
		reader->type = CSXDataType::TypeRef;
		reader->arrayCount = 1;
	}
	else if(reader->address == (void*)0x40D460) {
		reader->type = CSXDataType::TypeRef;
		reader->arrayCount = (sizemin == sizemax) ? (sizemin / 4) : 0;
	}
	else if(reader->address == (void*)0x402144) {
		reader->type = CSXDataType::TypeStringSize;
		reader->arrayCount = 1;
	}
	else if(reader->address == (void*)0x4021B6) {
		reader->type = CSXDataType::TypeString;
		reader->arrayCount = 1;
	}
	else if(reader->address == (void*)0x474AC3) {
		reader->type = CSXDataType::TypeMarkerIndex;
		reader->arrayCount = 1;
	}
	else if(sizemax == 0) {
		reader->type = CSXDataType::TypeUnknown;
		reader->arrayCount = 1;
	}
	else {
		// if(sizegcd == 1) {
		// 	reader->type = CSXDataType::TypeI8;
		// }
		// else if(sizegcd == 2) {
		// 	reader->type = CSXDataType::TypeI16;
		// }
		// else if(sizegcd == 4) {
		// 	reader->type = CSXDataType::TypeI32;
		// }
		// reader->arrayCount = (sizemin == sizemax) ? (sizemin / sizegcd) : 0;
		// check floats
		if((sizegcd & 3) == 0) {
			reader->arrayCount = (sizemin == sizemax) ? (sizemin / 4) : 0;
			for(CSXReadInst* ri : reader->readInsts) {
				uint8_t* dat = ri->data.data();
				size_t len = ri->data.size();
				uint32_t* fldat = (uint32_t*)dat;
				size_t flnum = len / 4;
				if(std::any_of(fldat, fldat + flnum, [](uint32_t f) { auto m = f & 0x7F80'0000; return (f != 0) & (m == 0 || m == 0x7F80'0000);})) {
					reader->type = CSXDataType::TypeI32;
					return;
				}
			}
			reader->type = CSXDataType::TypeFloat;
			return;
		}
		if((sizegcd & 1) == 0) {
			reader->arrayCount = (sizemin == sizemax) ? (sizemin / 2) : 0;
			for(CSXReadInst* ri : reader->readInsts) {
				uint8_t* dat = ri->data.data();
				size_t len = ri->data.size();
				uint16_t* u16dat = (uint16_t*)dat;
				size_t u16num = len / 2;
				if(std::any_of(u16dat, u16dat + u16num, [](uint16_t f) { return f == 0xFFF8; })) {
					reader->type = CSXDataType::TypeEvent;
					return;
				}
			}
			reader->type = CSXDataType::TypeI16;
			return;
		}
		reader->arrayCount = (sizemin == sizemax) ? sizemin : 0;
		reader->type = CSXDataType::TypeI8;
	}
}

std::string GetDataTypeString(CSXReader* reader) {
	std::string str = dataTypeNames[reader->type];
	if(reader->arrayCount != 1) {
		if(reader->arrayCount == 0)
			str = "vector<" + str + '>';
		else
			str += '[' + std::to_string(reader->arrayCount) + ']';
	}
	return str;
}

std::string GetReadInstValueString(CSXReadInst* ri) {
	CSXDataType dt = ri->reader->type;
	bool isArray = ri->reader->arrayCount != 1;
	const uint8_t* datptr = ri->data.data();
	std::string str;
	if(isArray)
		str += '[';
	for(int i = 0; i < 8; ++i) {
		if(datptr - ri->data.data() >= ri->data.size())
			break;
		if(i != 0)
			str += ',';
		switch(dt) {
			case TypeI8:
				str += std::to_string(*datptr);
				datptr += 1;
				break;
			case TypeI16:
			case TypeStringSize:
				str += std::to_string(*(uint16_t*)datptr);
				datptr += 2;
				break;
			case TypeI32:
			case TypeMarkerIndex:
				str += std::to_string(*(uint32_t*)datptr);
				datptr += 4;
				break;
			case TypeFloat:
				str += std::to_string(*(float*)datptr);
				datptr += 4;
				break;
			case TypeRef: {
				uint32_t objid = *(uint32_t*)datptr;
				if(objid == 0xFFFFFFFF)
					str += "null";
				else
					str += getClassName(objid & 63, (objid >> 6) & 2047);
				datptr += 4;
				break;
			}
			case TypeVector3:
				str += '(' + std::to_string(*(float*)datptr) + ',' + std::to_string(*(float*)(datptr + 4)) + ',' + std::to_string(*(float*)(datptr + 8)) + ')';
				datptr += 12;
				break;
			case TypeEvent:
				str += std::to_string(*(uint16_t*)datptr >> 3) + ':' + std::to_string(*(uint16_t*)datptr & 7);
				datptr += 2;
				break;
			case TypeString:
				str.append((char*)datptr, ri->data.size());
				datptr += ri->data.size();
				break;
			default:
				str += '?';
		}
	}
	if(datptr - ri->data.data() < ri->data.size())
		str += ", ...";
	if(isArray)
		str += ']';
	return str;
}

void IGClassSerialXtract(KClass* selectedObject) {
	if(!showCSX) return;

	static constexpr const char* longDataTypeNames[] = {
		"Unknown",
		"Integer 8 bits",
		"Integer 16 bits",
		"Integer 32 bits",
		"Float",
		"Object reference",
		"Post reference",
		"Vector3",
		"Event node",
		"String size",
		"String",
		"Marker index"
	};

	static CSXReadInst* curRI = nullptr;
	static CSXReadInst* hoveredRI = nullptr;

	ImGui::Begin("CSX");
	//ImGui::Text("Hello!");
	// ImGui::Columns(2);
	// ImGui::BeginChild("ObjectSelect");
	// for(auto& kv : g_csx_classes) {
	// 	int clcat = kv.first & 63;
	// 	int clid = kv.first >> 6;
	// 	if(ImGui::TreeNode(&kv.second, "%s (%i %i)", getClassName(clcat, clid), clcat, clid)) {
	// 		for(auto& pobj : kv.second.objects) {
	// 			if(ImGui::TreeNodeEx(&pobj, ImGuiTreeNodeFlags_Leaf, "%p", pobj->kObject)) {
	// 				ImGui::TreePop();
	// 			}
	// 		}
	// 		ImGui::TreePop();
	// 	}
	// }
	// ImGui::EndChild();
	if(ImGui::Button("Guess")) {
		for(auto& kv : g_csx_readers) {
			CSXReader* reader = &kv.second;
			GuessDataType(reader);
		}
	}
	ImGui::SameLine();
	if(ImGui::Button("Generate code")) {
		if(g_csx_objects.count(selectedObject)) {
			CSXObject* cobj = g_csx_objects.at(selectedObject).get();
			GenerateCode(cobj, curRI);
		}
	}
	ImGui::Columns(2);
	ImGui::BeginChild("RISelect");
	if(g_csx_objects.count(selectedObject)) {
		CSXObject* cobj = g_csx_objects.at(selectedObject).get();
		bool noHovered = true;
		for(auto& ri : cobj->reads) {
			ImGui::PushID(ri.get());
			bool clicked = ImGui::Selectable("##select", curRI == ri.get());
			if(ImGui::IsItemHovered()) {
				hoveredRI = ri.get();
				noHovered = false;
			}
			ImGui::SameLine();
			bool isref = ri->reader->address == (void*)0x40D425 || ri->reader->address == (void*)0x40D460;
			bool samecaller = hoveredRI ? (ri->reader->address == hoveredRI->reader->address) : false;
			if(samecaller)
				ImGui::PushStyleColor(ImGuiCol_Text, 0xFFFF00FF);
			ImGui::Text("%p : %5zi %s %s %s", ri->reader->address, ri->data.size(), GetDataTypeString(ri->reader).c_str(), isref ? "[REF]" : "", GetReadInstValueString(ri.get()).c_str());
			if(clicked) {
				curRI = ri.get();
			}
			if(samecaller)
				ImGui::PopStyleColor();
			ImGui::PopID();
		}
		if(noHovered)
			hoveredRI = nullptr;
	}
	ImGui::EndChild();
	ImGui::NextColumn();
	ImGui::BeginChild("RIInfo");
	if(curRI) {
		static MemoryEditor me;
		ImGui::BeginChild("Hex", ImVec2(0.0f, 100.0f));
		me.DrawContents(curRI->data.data(), curRI->data.size());
		ImGui::EndChild();
		ImGui::Separator();

		CSXReader* reader = curRI->reader;
		ImGui::Combo("Data Type", (int*)&reader->type, dataTypeNames, std::size(dataTypeNames));
		ImGui::InputInt("Array count", &reader->arrayCount);
		ImGui::Text("--- STATS ---");
		auto sizemm = std::minmax_element(reader->readInsts.begin(), reader->readInsts.end(), [](CSXReadInst* l, CSXReadInst* r) {return l->data.size() < r->data.size();});
		CSXReadInst *sizemin = *sizemm.first, *sizemax = *sizemm.second;
		size_t sizegcd = std::accumulate(reader->readInsts.begin(), reader->readInsts.end(), (size_t)0, [](size_t l, CSXReadInst* r) {return ngcd(r->data.size(), l);});
		ImGui::Text("Min size: %5zi\nMax size: %5zi\nSize GCD: %5zi", sizemin->data.size(), sizemax->data.size(), sizegcd);
		
		ImGui::Separator();

		ImGui::BeginChild("RIList");
		for(CSXReadInst* ri : reader->readInsts) {
			ImGui::PushID(ri);
			bool clicked = ImGui::Selectable("##select", curRI == ri);
			if(ImGui::IsItemVisible()) {
				ImGui::SameLine();
				int clcat = ri->object->cClass->kClassFullID & 63;
				int clid = ri->object->cClass->kClassFullID >> 6;
				ImGui::Text("%s %3zi : %5zi %s", getClassName(clcat, clid), ri->object->reads.size(), ri->data.size(), GetReadInstValueString(ri).c_str());
			}
			if(clicked) {
				curRI = ri;
			}
			ImGui::PopID();
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
	ImGui::End();

	auto toDelete = genCodes.end();
	for(auto it = genCodes.begin(); it != genCodes.end(); ++it) {
		bool open = true;
		ImGui::Begin(std::to_string((intptr_t)&*it).c_str(), &open);
		//ImGui::TextUnformatted(it->data(), it->data() + it->size());
		ImGui::PushItemWidth(-1.0f);
		ImGui::InputTextMultiline("##TextEdit", (char*)it->data(), it->size(), ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::End();
		if(!open)
			toDelete = it;
	}
	if(toDelete != genCodes.end())
		genCodes.erase(toDelete);
}
