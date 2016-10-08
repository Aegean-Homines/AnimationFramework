#include "ModelManager.h"
#include "SkeletonNode.h"
#include <iostream>

using std::cout;
using std::endl;
ModelManager::ModelManager()
{
	rootNode = NULL;
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
	if (fbxRootNode) {
		for (int i = 0; i < fbxRootNode->GetChildCount(); ++i)
			PrintNode(fbxRootNode->GetChild(i));
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
