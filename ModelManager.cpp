#include "ModelManager.h"
#include "EventManager.h"
#include "SkeletonNode.h"
#include "VQS.h"

#include <glfw3.h>
#include <iostream>

#define FRAME_RATE_MODE FbxTime::eDefaultMode
#define PI 3.14159265359f
#define TRANSLATE_AMOUNT 1.0f

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

	AnimationDefinition* definition = new AnimationDefinition();
	FbxScene* fbxScene = FbxScene::Create(fbxManager, sceneName);
	definition->fbxScene = fbxScene;
	definitionMap.insert(std::pair<std::string, AnimationDefinition*>(sceneName, definition));

	if (!importer->Import(fbxScene)) {
		cout << "Importing failed!" << endl;
		cout << "Reason: " << importer->GetStatus().GetErrorString() << endl;
		exit(-1);
	}

	importer->Destroy();
}

void ModelManager::CreateTree(std::string const & modelName)
{
	AnimationDefinition* definition = definitionMap[modelName];
	FbxScene* fbxScene = definition->fbxScene;
	FbxNode* fbxRootNode = fbxScene->GetRootNode();
	SkeletonNode* rootNode = new SkeletonNode();
	definition->rootNode = rootNode;

	// Get Animation Duration
	FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStack->GetName());
	int frametime = static_cast<int>(takeInfo->mLocalTimeSpan.GetDuration().GetFrameCount(FRAME_RATE_MODE));
	definition->animationDurationInFrames = frametime;

	if (fbxRootNode) {
		for (int i = 0; i < fbxRootNode->GetChildCount(); ++i)
			InsertNode(fbxRootNode->GetChild(i), fbxScene, rootNode);
	}

	// If there is a spline, relocate the node
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

	// Fill animation table of the child with time-animation pairs
	FillAnimationTable(child, scene, fbxNode);

	// Recursively process other children
	for (int j = 0; j < fbxNode->GetChildCount(); j++)
		InsertNode(fbxNode->GetChild(j), scene, child);

}

void ModelManager::ScaleSkeleton(std::string const & sceneName, float scalingFactor)
{
	AnimationDefinition* definition = definitionMap[sceneName];
	definition->rootNode->ScaleSkeleton(scalingFactor);
}

void ModelManager::ChangeSkeletonColor(glm::vec3 const & colorValue, bool byLevel /*= false*/)
{
	if (byLevel)
		currentAnimation->rootNode->ColorSkeletonByLevel(colorValue);
	else
		currentAnimation->rootNode->ColorSkeletonUniformly(colorValue);
}

void ModelManager::Update()
{
	vec3 & rootTranslate = currentAnimation->rootNode->transformVQS.translate;
	if(EventManager::IsKeyTriggered(GLFW_KEY_UP)){
		rootTranslate.z -= TRANSLATE_AMOUNT;
	}
	if (EventManager::IsKeyTriggered(GLFW_KEY_DOWN)) {
		rootTranslate.z += TRANSLATE_AMOUNT;
	}
	if (EventManager::IsKeyTriggered(GLFW_KEY_RIGHT)) {
		rootTranslate.x += TRANSLATE_AMOUNT;
	}
	if (EventManager::IsKeyTriggered(GLFW_KEY_LEFT)) {
		rootTranslate.x -= TRANSLATE_AMOUNT;
	}
	
	if (EventManager::IsKeyTriggered(GLFW_KEY_T)) {
		Quaternion & rootRotate = currentAnimation->rootNode->transformVQS.rotate;
		VQS & vqs = currentAnimation->rootNode->transformVQS;
		vec3 forward(0.0f, 0.0f, 1.0f);
		vec3 rotated = vqs * forward;
		vec4 vectorForm = rootRotate.VectorForm();
		vec3 eulerform = rootRotate.EulerForm();
		vec3 rotate(0.0f, 90.0f, 0.0f);
		//Quaternion rotationQuat(rotate * angleMultiplication);
		Quaternion rotationQuat(vec3(0.0f, 1.0f, 0.0f), 90.0f * angleMultiplication);

		rootRotate = rotationQuat * rootRotate;
	}

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
	int frametime = static_cast<int>(takeInfo->mLocalTimeSpan.GetDuration().GetFrameCount(FRAME_RATE_MODE));

	for (int i = 0; i <= frametime; ++i) {
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
		Quaternion rotationQuat(rotate * angleMultiplication);

		// Create a VQS for that frame and store it in the map
		skeletonNode->Insert(i, VQS(translate, rotationQuat));
	}
	
}
