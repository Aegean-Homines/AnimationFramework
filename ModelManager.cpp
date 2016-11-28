#include "ModelManager.h"
#include "EventManager.h"
#include "SplineManager.h"
#include "SkeletonNode.h"
#include "Skeleton.h"
#include "VQS.h"

#include <glfw3.h>
#include <iostream>
#include <string>

#define FRAME_RATE_MODE FbxTime::eDefaultMode
#define PI 3.14159265359f
#define TRANSLATE_AMOUNT 1.0f
#define NO_MODEL_NODE_CHAIN_LENGTH 6

using std::cout;
using std::endl;

const float angleMultiplication = (PI / 180.0f);

ModelManager* ModelManager::instance = NULL;

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
	cout << "Model Manager destructor" << endl;
	for (AnimationDefinitionMap::iterator iter = definitionMap.begin(); iter != definitionMap.end(); ++iter) {
		delete (iter->second);
	}

	fbxManager->Destroy();
	delete instance;
}

ModelManager* ModelManager::Instance()
{
	if (!instance)
		instance = new ModelManager();
	return instance;
}

void ModelManager::CreateFbxManager()
{
	fbxManager = FbxManager::Create();
}

void ModelManager::CreateFbxScene(const char* sceneName, const char* fileName)
{
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(fbxManager, "");
	
	if (!importer->Initialize(fileName, -1, fbxManager->GetIOSettings())) {
		cout << "Call to FbxImporter::Initialize() failed." << endl;
		cout << "Error returned: " << importer->GetStatus().GetErrorString() << "\n" << endl;
		exit(-1);
	}


	FbxScene* fbxScene = FbxScene::Create(fbxManager, sceneName);
	currentAnimation->fbxScene = fbxScene;


	if (!importer->Import(fbxScene)) {
		cout << "Importing failed!" << endl;
		cout << "Reason: " << importer->GetStatus().GetErrorString() << endl;
		exit(-1);
	}

	importer->Destroy();
}

void ModelManager::InitializeModel(std::string const & modelName, std::string const & modelFileName,float scale /*= 1.0f*/, glm::vec3 const & color /*= vec3(1.0f)*/)
{
	AnimationDefinition* definition = new AnimationDefinition();
	definitionMap.insert(std::pair<std::string, AnimationDefinition*>(modelName, definition));
	CurrentAnimation(modelName);

	if (modelName == "") {
		CreateTree();
	}
	else {
		CreateFbxScene(modelName.c_str(), modelFileName.c_str());
		CreateTree(modelName);
	}
	
	ScaleSkeleton(modelName, scale);
	ChangeSkeletonColor(color);
}

void ModelManager::SetIK(bool isIK)
{
	currentAnimation->isUsingIK = isIK;
}

void ModelManager::CreateTree(std::string const & modelName)
{
	AnimationDefinition* definition = definitionMap[modelName];
	FbxScene* fbxScene = definition->fbxScene;
	FbxNode* fbxRootNode = fbxScene->GetRootNode();
	Skeleton* skeleton = new Skeleton();
	definition->skeleton = skeleton;

	SkeletonNode* rootNode = new SkeletonNode();
	skeleton->RegisterJoint(rootNode);

	// Get Animation Duration
	FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStack->GetName());
	FbxTime frametime = takeInfo->mLocalTimeSpan.GetDuration();
	definition->animationDuration = frametime;

	if (fbxRootNode) {
		for (int i = 0; i < fbxRootNode->GetChildCount(); ++i)
			InsertNode(fbxRootNode->GetChild(i), fbxScene, rootNode);
	}
	else {
		CreateTree();
	}

}

void ModelManager::CreateTree()
{
	Skeleton* skeleton = new Skeleton();
	currentAnimation->skeleton = skeleton;
	SkeletonNode* rootNode = new SkeletonNode();
	skeleton->RegisterJoint(rootNode);

	// Get Animation Duration
	currentAnimation->animationDuration.SetMilliSeconds(1000);

	rootNode->Name("NoModel");

	InsertNode(1, rootNode);

	/*rootNode->CalculateAllTransforms(0.0f);
	rootNode->MoveAllToWorldSpace();*/

}

void ModelManager::InsertNode(FbxNode* fbxNode, FbxScene* scene, SkeletonNode* parent)
{
	FbxNodeAttribute* attribute = fbxNode->GetNodeAttribute();
	if (attribute)
	{
		FbxNodeAttribute::EType attributeType = attribute->GetAttributeType();
		if (attributeType != FbxNodeAttribute::eSkeleton)
			return;
	}
	else {
		return;
	} 
	// Get data from fbx
	const char* nodeName = fbxNode->GetName();
	SkeletonNode* child = parent->AddSkeletonNode(MeshType::CUBE, nodeName);
	currentAnimation->skeleton->RegisterJoint(child);

	// Fill animation table of the child with time-animation pairs
	FillAnimationTable(child, scene, fbxNode);

	// Recursively process other children
	for (int j = 0; j < fbxNode->GetChildCount(); j++)
		InsertNode(fbxNode->GetChild(j), scene, child);

}

void ModelManager::InsertNode(int currentLevel, SkeletonNode* parent)
{
	if (currentLevel > NO_MODEL_NODE_CHAIN_LENGTH) {
		return;
	}

	// Custom name
	const char* nodeName = (parent->Name() + std::to_string(currentLevel)).c_str();
	SkeletonNode* child = parent->AddSkeletonNode(MeshType::CUBE, nodeName);
	currentAnimation->skeleton->RegisterJoint(child);
	// Goes upwards always
	VQS & parentVQS = parent->globalVQS;
	vec3 childTranslate(0.0f);
	childTranslate.y = 0.5f;
	child->localVQS = VQS(childTranslate, Quaternion());
	child->initialVQS = child->localVQS;
	child->ToWorldSpace();

	// Recursively process other children
	InsertNode(++currentLevel, child);

}

glm::vec3 const & ModelManager::GetCurrentModelPosition()
{
	Skeleton* skeleton = currentAnimation->skeleton;
	if (skeleton && skeleton->GetRoot()) {
		return skeleton->GetRoot()->globalVQS.translate;
	}

	std::cout << "Model doesn't have a root node" << std::endl;
	return vec3(0.0f);
}

void ModelManager::ScaleSkeleton(std::string const & sceneName, float scalingFactor)
{
	AnimationDefinition* definition = definitionMap[sceneName];
	Skeleton* skeleton = definition->skeleton;
	if (skeleton && skeleton->GetRoot())
		skeleton->GetRoot()->ScaleSkeleton(scalingFactor);
}

void ModelManager::ChangeSkeletonColor(glm::vec3 const & colorValue, bool byLevel /*= false*/)
{
	Skeleton* skeleton = currentAnimation->skeleton;
	if (skeleton && skeleton->GetRoot()) {
		if (byLevel)
			skeleton->GetRoot()->ColorSkeletonByLevel(colorValue);
		else
			skeleton->GetRoot()->ColorSkeletonUniformly(colorValue);
	}

}

void ModelManager::ResetSkeleton()
{
	Skeleton* skeleton = currentAnimation->skeleton;
}

// Helper functions

void ModelManager::ConvertFbxDouble3ToGlmVec3(FbxDouble3 const & fbxVec3, glm::vec3 & floatVector)
{
	floatVector[0] = static_cast<float>(fbxVec3[0]);
	floatVector[1] = static_cast<float>(fbxVec3[1]);
	floatVector[2] = static_cast<float>(fbxVec3[2]);
}

void ModelManager::ConvertFbxDouble4ToGlmVec3(FbxVector4 const & fbxVec4, glm::vec3 & floatVector)
{
	floatVector[0] = static_cast<float>(fbxVec4[0]);
	floatVector[1] = static_cast<float>(fbxVec4[1]);
	floatVector[2] = static_cast<float>(fbxVec4[2]);
}

void ModelManager::FillAnimationTable(SkeletonNode* skeletonNode, FbxScene* scene, FbxNode* node)
{
	// Assuming we have only one stack and layer in the model
	// I can actually send these from the root node but at this point
	// I think I just don't care as long as I have an animation on the screen
	FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(0);
	FbxTimeSpan localSpan = animStack->GetLocalTimeSpan();
	FbxAnimLayer* layer = animStack->GetMember<FbxAnimLayer>(0);
	FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStack->GetName());
	int frameCount = static_cast<int>(takeInfo->mLocalTimeSpan.GetDuration().GetFrameCount(FRAME_RATE_MODE));

	for (int i = 0; i <= frameCount; ++i) {
		FbxTime frameTime;
		frameTime.SetFrame(i);
		// We can get the resulting transform matrix directly from the fbx file
 		FbxAMatrix transform = node->EvaluateLocalTransform(frameTime);

		// Get current translation values
		FbxDouble3 fbxTranslate, fbxRotate, fbxScale;
		fbxTranslate = transform.GetT();
		fbxRotate = transform.GetR();
		fbxScale = transform.GetS();

		// Convert fbx vector to our belowed glm vectors
		vec3 translate, rotate;
		ConvertFbxDouble3ToGlmVec3(fbxTranslate, translate);
		ConvertFbxDouble3ToGlmVec3(fbxRotate, rotate);

		// Rotation to quaternion
		Quaternion rotationQuat(rotate);

		// Create a VQS for that frame and store it in the map
		int timeInMilliseconds = static_cast<int>(frameTime.GetMilliSeconds());
		skeletonNode->Insert(timeInMilliseconds, VQS(translate, rotationQuat));
	}
	
}
