#pragma once
#include <fbxsdk.h>
#include <unordered_map>
#include <glm.hpp>

class Skeleton;
class SkeletonNode;

struct AnimationDefinition {
public:
	AnimationDefinition(FbxScene* fbxScene = nullptr, Skeleton* skeleton = nullptr, int animationDuration = 0, bool isUsingIK = false)
		:
		fbxScene(fbxScene), skeleton(skeleton), animationDuration(animationDuration), isUsingIK(isUsingIK)
	{}
	FbxScene* fbxScene;
	Skeleton* skeleton;
	FbxTime animationDuration;
	bool isUsingIK;
};

typedef std::unordered_map<std::string, AnimationDefinition*> AnimationDefinitionMap;

class ModelManager
{
public:
	~ModelManager();
	// Singleton things
	static ModelManager* Instance();

	// Methods
	void CreateFbxManager();
	void CreateFbxScene(const char* sceneName, const char* fileName);
	void InitializeModel(std::string const & modelName, std::string const & modelFileName, float scale = 1.0f, glm::vec3 const & color = glm::vec3(1.0f));
	void SetIK(bool isIK);
	// recursively create the tree
	void CreateTree(std::string const & modelName);
	void CreateTree();

	// Recursive part of creating the tree of nodes
	void InsertNode(FbxNode* fbxNode, FbxScene* scene, SkeletonNode* parent);
	void InsertNode(int currentLevel, SkeletonNode* parent);

	AnimationDefinition* CurrentAnimation() const { return currentAnimation; }
	void CurrentAnimation(AnimationDefinition* val) { currentAnimation = val; }
	AnimationDefinition* GetAnimationDefinition(std::string const & sceneName) { return definitionMap[sceneName]; }
	void CurrentAnimation(std::string const &  sceneName) { currentAnimation = definitionMap[sceneName]; }
	glm::vec3 const & GetCurrentModelPosition();

	void ScaleSkeleton(std::string const & sceneName, float scalingFactor);
	void ChangeSkeletonColor(glm::vec3 const & colorValue, bool byLevel = false);
	void ResetSkeleton();
private:
	// Variables
	FbxManager* fbxManager;
	AnimationDefinition* currentAnimation;
	AnimationDefinitionMap definitionMap;

	// Helper methods
	void ConvertFbxDouble3ToGlmVec3(FbxDouble3 const & fbxVec3, glm::vec3 & floatVector);
	void ConvertFbxDouble4ToGlmVec3(FbxVector4 const & fbxVec3, glm::vec3 & floatVector);
	void FillAnimationTable(SkeletonNode* skeletonNode, FbxScene* scene, FbxNode* node);
	// #JustSingletonThings
	ModelManager();
	ModelManager(ModelManager const &) {};
	ModelManager& operator=(ModelManager const &) {};
	static ModelManager* instance;

};

