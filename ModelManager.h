#pragma once
#include <fbxsdk.h>

class SkeletonNode;

class ModelManager
{
public:
	~ModelManager();
	// Singleton things
	static ModelManager* Instance();

	// Methods
	void CreateFbxManager();
	void CreateFbxScene(const char* sceneName, const char* fileName);
	void PrintTree();
	void PrintNode(FbxNode* fbxNode);
private:
	// Var
	FbxManager* fbxManager;
	FbxScene* fbxScene;
	SkeletonNode* rootNode;
	int numTabs = 0; // for printing

	// Helper methods
	void PrintTabs();
	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
	void PrintAttribute(FbxNodeAttribute* attribute);

	// Singleton things
	ModelManager();
	ModelManager(ModelManager const &) {};
	ModelManager& operator=(ModelManager const &) {};
	static ModelManager* instance;
};

