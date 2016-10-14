#pragma once
#include <fbxsdk.h>
#include <glm.hpp>

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
	void CreateTree();
	void PrintNode(FbxNode* fbxNode);
	void InsertNode(FbxNode* fbxNode, SkeletonNode* parent);
	SkeletonNode* RootNode() const { return rootNode; }
	void RootNode(SkeletonNode* val) { rootNode = val; }
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
	void ConvertFbxDouble3ToGlmVec3(FbxDouble3 const & fbxVec3, glm::vec3 & floatVector);
	void ConvertFbxDouble4ToGlmVec3(FbxVector4 const & fbxVec3, glm::vec3 & floatVector);

	// Singleton things
	ModelManager();
	ModelManager(ModelManager const &) {};
	ModelManager& operator=(ModelManager const &) {};
	static ModelManager* instance;

	int counter = 0;
};

