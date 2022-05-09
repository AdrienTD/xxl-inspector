///////////////////////////
/// CSX ///

#pragma once

#include <memory>

enum CSXDataType : int {
	TypeUnknown,
	TypeI8,
	TypeI16,
	TypeI32,
	TypeFloat,
	TypeRef,
	TypePostRef,
	TypeVector3,
	TypeEvent,
	TypeStringSize,
	TypeString,
	TypeMarkerIndex
};

struct CSXReader;
struct CSXReadInst;
struct CSXObject;
struct CSXClass;

struct CSXReader {
	void* address;
	CSXDataType knownType = CSXDataType::TypeUnknown;

	// TBD
	CSXDataType type = CSXDataType::TypeUnknown;
	int arrayCount = 1;
	bool varying = false;

	std::vector<CSXReadInst*> readInsts;
};

struct CSXReadInst {
	std::shared_ptr<CSXObject> object;
	CSXReader* reader;
	std::vector<uint8_t> data;
};

struct CSXObject {
	KClass* kObject;
	CSXClass* cClass;
	std::vector<std::unique_ptr<CSXReadInst>> reads;
};

struct CSXClass {
	uint32_t kClassFullID;
	std::vector<std::shared_ptr<CSXObject>> objects;
};

extern std::map<void*, CSXReader> g_csx_readers;
extern std::map<KClass*, std::shared_ptr<CSXObject>> g_csx_objects;
extern std::map<uint32_t, CSXClass> g_csx_classes;
extern bool showCSX;

void InitCSX();
void IGClassSerialXtract(KClass* selectedObject);