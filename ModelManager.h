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
	void PrintAnimationStacks();
	void PrintAnimationStacks(FbxAnimStack* stack);
	void PrintAnimationLayers(FbxAnimLayer* layer, FbxNode* node);
	void PrintAnimationChannels(FbxNode* node, FbxAnimLayer* layer);
	void DisplayCurveKeys(FbxAnimCurve* curve);
	void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty);
	void CreateTree();
	void PrintNode(FbxNode* fbxNode);
	void InsertNode(FbxNode* fbxNode, SkeletonNode* parent);
	SkeletonNode* RootNode() const { return rootNode; }
	void RootNode(SkeletonNode* val) { rootNode = val; }
private:
	// Var
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

	// Singleton things
	ModelManager();
	ModelManager(ModelManager const &) {};
	ModelManager& operator=(ModelManager const &) {};
	static ModelManager* instance;

	int counter = 0;


	static int InterpolationFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
		if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
		if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
		return 0;
	}

	static int ConstantmodeFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
		if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
		return 0;
	}

	static int TangentmodeFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
		if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
		if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
		if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
		if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
		if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
		return 0;
	}

	static int TangentweightFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
		if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
		if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
		return 0;
	}

	static int TangentVelocityFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
		if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
		if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
		return 0;
	}

};

