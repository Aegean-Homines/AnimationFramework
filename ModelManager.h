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

	// Debugging stuff
	void PrintTree();
	void PrintAnimationStacks();
	void PrintAnimationStacks(FbxAnimStack* stack);
	void PrintAnimationLayers(FbxAnimLayer* layer, FbxNode* node);
	void PrintAnimationChannels(FbxNode* node, FbxAnimLayer* layer);
	void DisplayCurveKeys(FbxAnimCurve* curve);
	void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty);

	// recursively create the tree
	void CreateTree();
	void PrintNode(FbxNode* fbxNode);

	// Recursive part of creating the tree of nodes
	void InsertNode(FbxNode* fbxNode, SkeletonNode* parent);

	// Getter & Setter
	SkeletonNode* RootNode() const { return rootNode; }
	void RootNode(SkeletonNode* val) { rootNode = val; }
private:
	// Variables
	// Supports only one model (I think) so
	// #TODO Generalize this if you find time (don't think so pal)
	FbxManager* fbxManager;
	FbxScene* fbxScene;
	FbxNode* fbxRootNode;
	SkeletonNode* rootNode;
	int numTabs = 0; // for printing

	// Helper methods
	void PrintTabs();
	void ConvertFbxDouble3ToGlmVec3(FbxDouble3 const & fbxVec3, glm::vec3 & floatVector);
	void ConvertFbxDouble4ToGlmVec3(FbxVector4 const & fbxVec3, glm::vec3 & floatVector);
	void FillAnimationTable(SkeletonNode* skeletonNode, FbxNode* node);
	// #JustSingletonThings
	ModelManager();
	ModelManager(ModelManager const &) {};
	ModelManager& operator=(ModelManager const &) {};
	static ModelManager* instance;

	int counter = 0;

};

