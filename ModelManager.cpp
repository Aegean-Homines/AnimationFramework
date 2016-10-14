#include "ModelManager.h"
#include "SkeletonNode.h"
#include <iostream>

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

void ModelManager::CreateTree()
{
	FbxNode* fbxRootNode = fbxScene->GetRootNode();
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

	// Print the node's attributes.
	for (int i = 0; i < fbxNode->GetNodeAttributeCount(); i++)
		PrintAttribute(fbxNode->GetNodeAttributeByIndex(i));

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
	FbxDouble3 fbxTranslation = fbxNode->LclTranslation.Get();
	FbxDouble3 fbxRotation = fbxNode->LclRotation.Get();
	FbxDouble3 fbxScaling = fbxNode->LclScaling.Get();

	FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform();
	FbxVector4 transform = localTransform.GetT();
	FbxVector4 rotate = localTransform.GetR();
	FbxVector4 scaling = localTransform.GetS();
	vec3 translation;
	ConvertFbxDouble4ToGlmVec3(transform, translation);
	vec3 rotation;
	ConvertFbxDouble4ToGlmVec3(rotate, rotation);
	vec3 scale;
	ConvertFbxDouble4ToGlmVec3(scaling, scale);
	

	// convert into my format
	/*vec3 translation;
	ConvertFbxDouble3ToGlmVec3(fbxTranslation, translation);
	vec3 rotation;
	ConvertFbxDouble3ToGlmVec3(fbxRotation, rotation);
	vec3 scale;
	ConvertFbxDouble3ToGlmVec3(fbxScaling, scale);*/

	// Add a new node to the parent
	SkeletonNode* child = parent->AddSkeletonNode(translation, rotation, scale, MeshType::CUBE, nodeName);
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

FbxString ModelManager::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

void ModelManager::PrintAttribute(FbxNodeAttribute* attribute)
{
	if (!attribute) return;

	FbxString typeName = GetAttributeTypeName(attribute->GetAttributeType());
	FbxString attrName = attribute->GetName();
	PrintTabs();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
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
