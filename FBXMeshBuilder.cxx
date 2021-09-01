#include "FBXMeshBuilder.h"
#include "DataOptimizer.h"

FBXMeshBuilder::FBXMeshBuilder()
{
}

FBXMeshBuilder::~FBXMeshBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////////
/*
bool FBXMeshBuilder::TriangulateMeshArrays(std::vector<MeshArray>& meshArrays)
{
	for (size_t i = 0; i < meshArrays.size(); i++)
	{
		MeshArray& meshes = meshArrays[i];

		for (size_t j = 0; j < meshes.size(); j++)
		{
			Triangulate(meshes[j]);
		}
	}

	return true;
}
*/

/////////////////////////////////////////////////////////////////////////////////
bool FBXMeshBuilder::Build(FbxScene* fbxScene, std::vector<FbxNode*>& fbxNodes, const std::vector<MeshArray>& meshArrays)
{
	int sceneMaxMaterialIdx = ComputeSceneMaxMaterialIdx(meshArrays);

	if (!BuildFbxMeshes(fbxScene, fbxNodes, sceneMaxMaterialIdx, meshArrays))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
bool FBXMeshBuilder::BuildFbxMeshes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes, int sceneMaxMaterialIdx_, const std::vector<MeshArray>& meshArrays)
{
	for (size_t i = 0; i < fbxNodes.size(); i++)
	{
		FbxNode* fbxNode = fbxNodes[i];
		const MeshArray& meshes = meshArrays[i];

		for (size_t j = 0; j < meshes.size(); j++)
		{
			FbxString name = fbxNode->GetName();
			name += "_";
			name += (int)j;

			BuildFbxMesh(fbxScene, fbxNode, sceneMaxMaterialIdx_, meshes[j], name);
		}
	}

	return true;
}

void FBXMeshBuilder::BuildFbxMesh(FbxScene* fbxScene, FbxNode* fbxNode, int sceneMaxMaterialIdx_, const Mesh& mesh, const FbxString& name)
{
	////////////////////////////////////////////////////////
	// create Mesh
	FbxMesh* dstMesh = FbxMesh::Create(fbxScene, name);

	////////////////////////////////////////////////////////
	// fill material
	{
		FbxGeometryElementMaterial* geometryElementMaterial = dstMesh->CreateElementMaterial();
		geometryElementMaterial->SetMappingMode(FbxGeometryElement::eByPolygon);
		geometryElementMaterial->SetReferenceMode(FbxGeometryElement::eIndexToDirect);
	}

	////////////////////////////////////////////////////////
	// fill position
	bool useOptimizer = true;
	bool useChannelCount = true;
	////////////////////////////////////////////////////////
	// fill color
	for (size_t ch = 0; ch < (useChannelCount ? mesh.GetColorChannelCount() : NUM_COLORS); ch++)
		FillColor(useOptimizer, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill normal
	for (size_t ch = 0; ch < (useChannelCount ? mesh.GetNormalChannelCount() : NUM_NORMALS); ch++)
		FillNormal(useOptimizer, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill uv
	for (size_t ch = 0; ch < (useChannelCount ? mesh.GetUVChannelCount() : NUM_UVS); ch++)
		FillUV(useOptimizer, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill tangent
	for (size_t ch = 0; ch < (useChannelCount ? mesh.GetTangentChannelCount() : NUM_TANGENTS); ch++)
		FillTangent(useOptimizer, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill binormal
	for (size_t ch = 0; ch < (useChannelCount ? mesh.GetBinormalChannelCount() : NUM_BINORMALS); ch++)
		FillBinormal(useOptimizer, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill polygons
	FillPolygon(useOptimizer, dstMesh, mesh);

	////////////////////////////////////////////////////////
	// Copy Geometry Transform
	FbxNode* dstNode = FbxNode::Create(fbxScene, name);
	dstNode->SetNodeAttribute(dstMesh); 
	fbxNode->AddChild(dstNode);

	auto t2 = fbxNode->LclTranslation.Get();
	auto r2 = fbxNode->LclRotation.Get();
	auto s2 = fbxNode->LclScaling.Get();
	auto t1 = fbxNode->GetGeometricTranslation(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	auto r1 = fbxNode->GetGeometricRotation(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	auto s1 = fbxNode->GetGeometricScaling(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	dstNode->SetGeometricTranslation(fbxsdk::FbxNode::EPivotSet::eSourcePivot, t1);
	dstNode->SetGeometricRotation(fbxsdk::FbxNode::EPivotSet::eSourcePivot, r1);
	dstNode->SetGeometricScaling(fbxsdk::FbxNode::EPivotSet::eSourcePivot, s1);

	////////////////////////////////////////////////////////
	// fill material 
	BuildMaterial(fbxNode, dstNode, sceneMaxMaterialIdx_);
}

void FBXMeshBuilder::FillColor(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	DataOptimizer<Color> optimizer(useOptimizer);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).colors[ch]);

	FbxGeometryElementVertexColor* geometryElementVertexColor = dstMesh->CreateElementVertexColor();
	geometryElementVertexColor->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementVertexColor->SetReferenceMode((useOptimizer ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < optimizer.GetDataCount(); i++)
	{
		const Color& c = optimizer.GetData(i);
		geometryElementVertexColor->GetDirectArray().Add(FbxColor(c[0], c[1], c[2], c[3]));
	}

	if (useOptimizer)
	{
		for (size_t i = 0; i < optimizer.GetIndicesCount(); i++)
		{
			int idx = optimizer.GetIndex(i);
			geometryElementVertexColor->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillUV(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	DataOptimizer<Vector2> optimizer(useOptimizer);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).uvs[ch]);

	FbxGeometryElementUV* geometryElementUV = dstMesh->CreateElementUV(FbxString("UV") + ((int)(ch + 1)));
	geometryElementUV->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementUV->SetReferenceMode((useOptimizer ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < optimizer.GetDataCount(); i++)
	{
		const Vector2& uv = optimizer.GetData(i);
		geometryElementUV->GetDirectArray().Add(FbxVector2(uv.X(), uv.Y()));
	}

	if (useOptimizer)
	{
		for (size_t i = 0; i < optimizer.GetIndicesCount(); i++)
		{
			int idx = optimizer.GetIndex(i);
			geometryElementUV->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillNormal(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	DataOptimizer<Vector3> optimizer(useOptimizer);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).normals[ch]);

	FbxGeometryElementNormal* geometryElementNormal = dstMesh->CreateElementNormal();
	geometryElementNormal->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementNormal->SetReferenceMode((useOptimizer ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < optimizer.GetDataCount(); i++)
	{
		const Vector3& n = optimizer.GetData(i);
		geometryElementNormal->GetDirectArray().Add(FbxVector4(n.X(), n.Y(), n.Z(), 0.0f));
	}

	if (useOptimizer)
	{
		for (size_t i = 0; i < optimizer.GetIndicesCount(); i++)
		{
			int idx = optimizer.GetIndex(i);
			geometryElementNormal->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillTangent(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	DataOptimizer<Vector3> optimizer(useOptimizer);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).tangents[ch]);

	FbxGeometryElementTangent* geometryElementTangent = dstMesh->CreateElementTangent();
	geometryElementTangent->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementTangent->SetReferenceMode((useOptimizer ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < optimizer.GetDataCount(); i++)
	{
		const Vector3& t = optimizer.GetData(i);
		geometryElementTangent->GetDirectArray().Add(FbxVector4(t.X(), t.Y(), t.Z(), 0.0f));
	}

	if (useOptimizer)
	{
		for (size_t i = 0; i < optimizer.GetIndicesCount(); i++)
		{
			int idx = optimizer.GetIndex(i);
			geometryElementTangent->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillBinormal(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	DataOptimizer<Vector3> optimizer(useOptimizer);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).binormals[ch]);

	FbxGeometryElementBinormal* geometryElementBinormal = dstMesh->CreateElementBinormal();
	geometryElementBinormal->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementBinormal->SetReferenceMode((useOptimizer ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < optimizer.GetDataCount(); i++)
	{
		const Vector3& b = optimizer.GetData(i);
		geometryElementBinormal->GetDirectArray().Add(FbxVector4(b.X(), b.Y(), b.Z(), 0.0f));
	}

	if (useOptimizer)
	{
		for (size_t i = 0; i < optimizer.GetIndicesCount(); i++)
		{
			int idx = optimizer.GetIndex(i);
			geometryElementBinormal->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillPolygon(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh)
{
	DataOptimizer<Vector3> optimizer(useOptimizer);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).position);

	for (size_t i = 0; i < optimizer.GetDataCount(); i++)
	{
		const Vector3& p = optimizer.GetData(i);
		dstMesh->mControlPoints.Add(FbxVector4(p.X(), p.Y(), p.Z(), 0.0));
	}

	int vertexIdx = 0;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		dstMesh->BeginPolygon(mesh.GetPolygon(i).GetMaterialIdx());

		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			dstMesh->AddPolygon(optimizer.GetIndex(vertexIdx));

			vertexIdx++;
		}

		dstMesh->EndPolygon();
	}
}

int FBXMeshBuilder::ComputeSceneMaxMaterialIdx(const std::vector<MeshArray>& meshArrays_)
{
	int sceneMaxMaterialIdx = 0;

	for (auto& meshArray : meshArrays_)
	{
		for (auto& mesh : meshArray)
		{
			if (sceneMaxMaterialIdx < mesh.GetMaxMaterialIdx())
				sceneMaxMaterialIdx = mesh.GetMaxMaterialIdx();
		}
	}

	return sceneMaxMaterialIdx;
}

void FBXMeshBuilder::BuildMaterial(FbxNode* fbxNode, FbxNode* dstNode, int sceneMaxMaterialIdx_)
{
	std::vector<FbxString> names(fbxNode->GetMaterialCount());
	for (int i = 0; i < fbxNode->GetMaterialCount(); i++)
		names[i] = fbxNode->GetMaterial(i)->GetName();

	dstNode->RemoveAllMaterials();

	for (int i = 0; i < sceneMaxMaterialIdx_; i++)
	{
		AddMaterial(dstNode, names[i]);
	}

	AddMaterial(dstNode, "Cross Section");
}

FbxSurfaceMaterial* FBXMeshBuilder::AddMaterial(FbxNode* fbxNode, FbxString materialName)
{
	FbxString lMaterialName = materialName;
	FbxString lShadingName = "Phong";
	FbxDouble3 lBlack(0.0, 0.0, 0.0);
	FbxDouble3 lRed(1.0, 0.0, 0.0);
	FbxDouble3 lColor(1.0f, 1.0f, 1.0f);
	FbxSurfacePhong* material = FbxSurfacePhong::Create(fbxNode, lMaterialName.Buffer());

	// Generate primary and secondary colors.
	material->Emissive.Set(lBlack);
	material->Ambient.Set(lRed);
	material->Diffuse.Set(lColor);
	material->TransparencyFactor.Set(0.0);
	material->ShadingModel.Set(lShadingName);
	material->Shininess.Set(0.5);

	return material;
}