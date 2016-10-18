#include "ModelManager.h"
#include "SkeletonNode.h"
#include <iostream>

#define FRAME_RATE_MODE FbxTime::eDefaultMode

using std::cout;
using std::endl;

ModelManager::ModelManager()
{
	RootNode(NULL);
	counter = 0;
}

ModelManager::~ModelManager()
{
	cout << "Model Manager destructor" << endl;
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

	fbxScene = FbxScene::Create(fbxManager, sceneName);

	if (!importer->Import(fbxScene)) {
		cout << "Importing failed!" << endl;
		cout << "Reason: " << importer->GetStatus().GetErrorString() << endl;
		exit(-1);
	}

	importer->Destroy();
}

void ModelManager::PrintTree()
{
	FbxNode* fbxRootNode = fbxScene->GetRootNode();
	RootNode(new SkeletonNode());
	if (fbxRootNode) {
		for (int i = 0; i < fbxRootNode->GetChildCount(); ++i)
			PrintNode(fbxRootNode->GetChild(i));
	}
}

void ModelManager::PrintAnimationStacks()
{
	int count = fbxScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < count; ++i) {
		FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(i);
		PrintAnimationStacks(animStack);
	}
}

void ModelManager::PrintAnimationStacks(FbxAnimStack* stack)
{
	int layerCount = stack->GetMemberCount<FbxAnimLayer>();
	std::cout << "\tStack has " << layerCount << " layers." << std::endl;

	for (int i = 0; i < layerCount; ++i) {
		FbxAnimLayer* layer = stack->GetMember<FbxAnimLayer>(i);

		std::cout << "\tAnimLayer: " << i << std::endl;
		PrintAnimationLayers(layer, fbxRootNode);
	}
}

void ModelManager::PrintAnimationLayers(FbxAnimLayer* layer, FbxNode* node)
{
	int childCount = node->GetChildCount();
	std::cout << "\t\tNode Name: " << node->GetName() << std::endl;
	// Call display channels
	PrintAnimationChannels(node, layer);
	std::cout << std::endl;
	// Loop through all the children and call everything again.
	for (int i = 0; i < childCount; ++i) {
		PrintAnimationLayers(layer, node->GetChild(i));
	}
}

void ModelManager::PrintAnimationChannels(FbxNode* node, FbxAnimLayer* layer)
{
	FbxAnimCurve* lAnimCurve = NULL;
	// Display general curves.
	lAnimCurve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		FBXSDK_printf("        TX\n");
		DisplayCurveKeys(lAnimCurve);
	}
	lAnimCurve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		FBXSDK_printf("        TY\n");
		DisplayCurveKeys(lAnimCurve);
	}
	lAnimCurve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		FBXSDK_printf("        TZ\n");
		DisplayCurveKeys(lAnimCurve);
	}

	lAnimCurve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		FBXSDK_printf("        RX\n");
		DisplayCurveKeys(lAnimCurve);
	}
	lAnimCurve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		FBXSDK_printf("        RY\n");
		DisplayCurveKeys(lAnimCurve);
	}
	lAnimCurve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		FBXSDK_printf("        RZ\n");
		DisplayCurveKeys(lAnimCurve);
	}

	lAnimCurve = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		FBXSDK_printf("        SX\n");
		DisplayCurveKeys(lAnimCurve);
	}
	lAnimCurve = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve)
	{
		FBXSDK_printf("        SY\n");
		DisplayCurveKeys(lAnimCurve);
	}
	lAnimCurve = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve)
	{
		FBXSDK_printf("        SZ\n");
		DisplayCurveKeys(lAnimCurve);
	}

}

void ModelManager::DisplayCurveKeys(FbxAnimCurve* curve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

	FbxTime   lKeyTime;
	float   lKeyValue;
	char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = curve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(curve->KeyGetValue(lCount));
		lKeyTime = curve->KeyGetTime(lCount);
		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		/*lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(curve->KeyGetInterpolation(lCount))];
		if ((curve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(curve->KeyGetConstantMode(lCount))];
		}
		else if ((curve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(curve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(curve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(curve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";*/
		//FBXSDK_printf(lOutputString);
		std::cout << lOutputString.Buffer() << std::endl;
	}
}

void ModelManager::DisplayListCurveKeys(FbxAnimCurve* curve, FbxProperty* pProperty)
{
	std::cout << "DISPLAYERLISTCURVEKEYS" << std::endl;
	FbxTime   lKeyTime;
	int     lKeyValue;
	char    lTimeString[256];
	FbxString lListValue;
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = curve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<int>(curve->KeyGetValue(lCount));
		lKeyTime = curve->KeyGetTime(lCount);

		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " (";
		lOutputString += pProperty->GetEnumValue(lKeyValue);
		lOutputString += ")";

		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}

void ModelManager::CreateTree()
{
	fbxRootNode = fbxScene->GetRootNode();
	RootNode(new SkeletonNode());
	counter = 0;
	if (fbxRootNode) {
		for (int i = 0; i < fbxRootNode->GetChildCount(); ++i)
			InsertNode(fbxRootNode->GetChild(i), RootNode());
	}
}

void ModelManager::PrintNode(FbxNode* fbxNode)
{
	PrintTabs();
	const char* nodeName = fbxNode->GetName();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();
	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();

	// Print the contents of the node.

	cout << "<node name = " << nodeName << ">" << endl;
	cout << "translation = (" << translation[0] << ", " << translation[1] << ", " << translation[2] << ")" << endl;
	cout << "rotation = (" << rotation[0] << ", " << rotation[1] << ", " << rotation[2] << ")" << endl;
	cout << "scaling = (" << scaling[0] << ", " << scaling[1] << ", " << scaling[2] << ")" << endl;
	cout << endl;
	cout << endl;
	numTabs++;

	// Recursively print the children.
	for (int j = 0; j < fbxNode->GetChildCount(); j++)
		PrintNode(fbxNode->GetChild(j));

	numTabs--;
	PrintTabs();
	printf("</node>\n");
}

void ModelManager::InsertNode(FbxNode* fbxNode, SkeletonNode* parent)
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
	FillAnimationTable(child, fbxNode);
	
	child->level = counter;

	counter++;
	// Recursively process other children
	for (int j = 0; j < fbxNode->GetChildCount(); j++)
		InsertNode(fbxNode->GetChild(j), child);
	counter--;

}

ModelManager* ModelManager::instance = NULL;

// Helper functions

void ModelManager::PrintTabs()
{
	for (int i = 0; i < numTabs; i++)
		printf("\t");
}

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

void ModelManager::FillAnimationTable(SkeletonNode* skeletonNode, FbxNode* fbxNode)
{
	// Assuming we have only one stack and layer in the model
	FbxAnimStack* animStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	FbxTimeSpan localSpan = animStack->GetLocalTimeSpan();
	FbxAnimLayer* layer = animStack->GetMember<FbxAnimLayer>(0);
	FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animStack->GetName());
	FbxTime durationTime = takeInfo->mLocalTimeSpan.GetDuration();
	double duration = takeInfo->mLocalTimeSpan.GetDuration().GetSecondDouble();
	double framerate = durationTime.GetFrameRate(FRAME_RATE_MODE);
	double mux = duration * framerate;
	int frametime = takeInfo->mLocalTimeSpan.GetDuration().GetFrameCount(FRAME_RATE_MODE);

	for (int i = 0; i <= frametime; ++i) {
		FbxTime frameTime;
		frameTime.SetFrame(i);
		FbxAMatrix transform = fbxNode->EvaluateLocalTransform(frameTime);
		mat4 glTransform;
		/*for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				glTransform[j][k] = transform[j][k];
			}
		}*/

		skeletonNode->Insert(i, transform);
	}
	
}
