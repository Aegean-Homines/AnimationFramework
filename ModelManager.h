#pragma once
#include <fbxsdk.h>
#include <unordered_map>
#include <glm.hpp>

class SkeletonNode;

struct AnimationDefinition {
	FbxScene* fbxScene;
	SkeletonNode* rootNode;
	FbxTime animationDuration;

	AnimationDefinition(FbxScene* fbxScene = nullptr, SkeletonNode* rootNode = nullptr, int animationDuration = 0)
		:
		fbxScene(fbxScene), rootNode(rootNode), animationDuration(animationDuration)
	{}
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

	// recursively create the tree
	void CreateTree(std::string const & modelName);

	// Recursive part of creating the tree of nodes
	void InsertNode(FbxNode* fbxNode, FbxScene* scene, SkeletonNode* parent);

	AnimationDefinition* CurrentAnimation() const { return currentAnimation; }
	void CurrentAnimation(AnimationDefinition* val) { currentAnimation = val; }
	AnimationDefinition* GetAnimationDefinition(std::string const & sceneName) { return definitionMap[sceneName]; }
	void CurrentAnimation(std::string const &  sceneName) { currentAnimation = definitionMap[sceneName]; }

	void ScaleSkeleton(std::string const & sceneName, float scalingFactor);
	void ChangeSkeletonColor(glm::vec3 const & colorValue, bool byLevel = false);

	// Function for handling input etc for the animation
	void Update();
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

