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
	if (!BuildFbxMeshes(fbxScene, fbxNodes, meshArrays))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
bool FBXMeshBuilder::BuildFbxMeshes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes, const std::vector<MeshArray>& meshArrays)
{
	for (size_t i = 0; i < fbxNodes.size(); i++)
	{
		FbxNode* fbxNode = fbxNodes[i];
		const MeshArray& meshes = meshArrays[i];

		for (size_t j = 0; j < meshes.size(); j++)
		{
			FbxString name(fbxNode->GetName());
			name += FbxString("_");
			name += (int)j;

			BuildFbxMesh(fbxScene, fbxNode, meshes[j], name);
		}
	}

	return true;
}

void FBXMeshBuilder::BuildFbxMesh(FbxScene* fbxScene, FbxNode* fbxNode, const Mesh& mesh, const FbxString& name)
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
	// copy Geometry Transform
	FbxNode* dstNode = FbxNode::Create(fbxScene, name);
	dstNode->SetNodeAttribute(dstMesh);
	fbxScene->GetRootNode()->AddChild(dstNode);
	auto t1 = fbxNode->GetGeometricTranslation(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	auto r1 = fbxNode->GetGeometricRotation(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	auto s1 = fbxNode->GetGeometricScaling(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	dstNode->SetGeometricTranslation(fbxsdk::FbxNode::EPivotSet::eSourcePivot, t1);
	dstNode->SetGeometricRotation(fbxsdk::FbxNode::EPivotSet::eSourcePivot, r1);
	dstNode->SetGeometricScaling(fbxsdk::FbxNode::EPivotSet::eSourcePivot, s1);

	////////////////////////////////////////////////////////
	// fill material 
	FillMaterial(fbxScene, dstMesh, fbxNode);

	////////////////////////////////////////////////////////
	// delete original mesh node
	fbxScene->GetRootNode()->RemoveChild(fbxNode);
}

void FBXMeshBuilder::FillColor(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, size_t ch)
{
	DataOptimizer<Color> optimizer(useOptimizer);
	for (size_t polyIdx = 0; polyIdx < mesh.GetPolygonCount(); polyIdx++)
		for (size_t edgeIdx = 0; edgeIdx < mesh.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			optimizer.Add(mesh.GetPolygonVertex(polyIdx, edgeIdx).colors[ch]);

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

void FBXMeshBuilder::FillUV(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, size_t ch)
{
	DataOptimizer<Vector2> optimizer(useOptimizer);
	for (size_t polyIdx = 0; polyIdx < mesh.GetPolygonCount(); polyIdx++)
		for (size_t edgeIdx = 0; edgeIdx < mesh.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			optimizer.Add(mesh.GetPolygonVertex(polyIdx, edgeIdx).uvs[ch]);

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

void FBXMeshBuilder::FillNormal(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, size_t ch)
{
	DataOptimizer<Vector3> optimizer(useOptimizer);
	for (size_t polyIdx = 0; polyIdx < mesh.GetPolygonCount(); polyIdx++)
		for (size_t edgeIdx = 0; edgeIdx < mesh.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			optimizer.Add(mesh.GetPolygonVertex(polyIdx, edgeIdx).normals[ch]);


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

void FBXMeshBuilder::FillTangent(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, size_t ch)
{
	DataOptimizer<Vector3> optimizer(useOptimizer);
	for (size_t polyIdx = 0; polyIdx < mesh.GetPolygonCount(); polyIdx++)
		for (size_t edgeIdx = 0; edgeIdx < mesh.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			optimizer.Add(mesh.GetPolygonVertex(polyIdx, edgeIdx).tangents[ch]);


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

void FBXMeshBuilder::FillBinormal(bool useOptimizer, FbxMesh* dstMesh, const Mesh& mesh, size_t ch)
{
	DataOptimizer<Vector3> optimizer(useOptimizer);
	for (size_t polyIdx = 0; polyIdx < mesh.GetPolygonCount(); polyIdx++)
		for (size_t edgeIdx = 0; edgeIdx < mesh.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			optimizer.Add(mesh.GetPolygonVertex(polyIdx, edgeIdx).binormals[ch]);


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
	for (size_t polyIdx = 0; polyIdx < mesh.GetPolygonCount(); polyIdx++)
		for (size_t edgeIdx = 0; edgeIdx < mesh.GetPolygonEdgesCount(polyIdx); edgeIdx++)
			optimizer.Add(mesh.GetPolygonVertex(polyIdx, edgeIdx).position);

	for (size_t i = 0; i < optimizer.GetDataCount(); i++)
	{
		const Vector3& p = optimizer.GetData(i);
		dstMesh->mControlPoints.Add(FbxVector4(p.X(), p.Y(), p.Z(), 0.0));
	}

	int vertexIdx = 0;
	for (size_t polyIdx = 0; polyIdx < mesh.GetPolygonCount(); polyIdx++)
	{
		dstMesh->BeginPolygon(mesh.GetPolygonMaterialIdx(polyIdx));

		for (size_t k = 0; k < mesh.GetPolygonEdgesCount(polyIdx); k++)
		{
			dstMesh->AddPolygon(optimizer.GetIndex(vertexIdx));

			vertexIdx++;
		}

		dstMesh->EndPolygon();
	}
}

void FBXMeshBuilder::FillMaterial(FbxScene* fbxScene, FbxMesh* dstMesh, FbxNode* fbxNode)
{
	FbxMesh* srcMesh = fbxNode->GetMesh();
	for (size_t i = 0; i < fbxNode->GetMaterialCount(); i++)
		dstMesh->GetNode()->AddMaterial(fbxNode->GetMaterial(i));

	// no need copy
	//dstMesh->GetNode()->AddMaterial(GetPhongMaterial(fbxScene, "crossSection"));
}

FbxSurfaceMaterial* FBXMeshBuilder::GetPhongMaterial(FbxScene* fbxScene, FbxString materialName)
{
	FbxString lMaterialName = materialName;
	FbxString lShadingName = "Phong";
	FbxDouble3 lBlack(0.0, 0.0, 0.0);
	FbxDouble3 lRed(1.0, 0.0, 0.0);
	FbxDouble3 lColor(1.0f, 1.0f, 1.0f);
	FbxSurfacePhong* material = FbxSurfacePhong::Create(fbxScene, lMaterialName.Buffer());

	// Generate primary and secondary colors.
	material->Emissive.Set(lBlack);
	material->Ambient.Set(lRed);
	material->Diffuse.Set(lColor);
	material->TransparencyFactor.Set(0.0);
	material->ShadingModel.Set(lShadingName);
	material->Shininess.Set(0.5);

	return material;
}