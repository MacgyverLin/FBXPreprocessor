#include "FBXMeshBuilder.h"

template<class T>
class VertexBatcher
{
friend class FBXMeshBuilder;
public:
	VertexBatcher(bool batch_)
		: batch(batch_)
	{
	}

	~VertexBatcher()
	{
	}

	void Add(const T& v)
	{
		if (batch)
		{
			std::map<T, int>::iterator itr = verticesMap.find(v);

			int idx;
			if (itr == verticesMap.end())
			{
				idx = vertices.size();

				verticesMap[v] = idx;
				vertices.push_back(v);
			}
			else
			{
				idx = itr->second;
			}

			indices.push_back(idx);
		}
		else
		{
			int idx = vertices.size();

			vertices.push_back(v);
			indices.push_back(idx);
		}
	}
private:
	std::map<T, int> verticesMap;

	std::vector<T> vertices;
	std::vector<int> indices;

	bool batch;
};

FBXMeshBuilder::FBXMeshBuilder()
{
}

FBXMeshBuilder::~FBXMeshBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////////
bool FBXMeshBuilder::FixMaterialOrderMeshArrays(std::vector<MeshArray>& meshArrays)
{
	for (size_t i = 0; i < meshArrays.size(); i++)
	{
		MeshArray& meshes = meshArrays[i];

		for (size_t j = 0; j < meshes.size(); j++)
		{
			FixMaterialOrder(meshes[j]);
		}
	}

	return true;
}

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
	bool useBatch = true;
	bool useChannelCount = true;
	////////////////////////////////////////////////////////
	// fill color
	for (size_t ch = 0; ch < (useChannelCount ? mesh.colorChannelCount : NUM_COLORS); ch++)
		FillColor(useBatch, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill normal
	for (size_t ch = 0; ch < (useChannelCount ? mesh.normalChannelCount : NUM_NORMALS); ch++)
		FillNormal(useBatch, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill uv
	for (size_t ch = 0; ch < (useChannelCount ? mesh.uvChannelCount : NUM_UVS); ch++)
		FillUV(useBatch, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill tangent
	for (size_t ch = 0; ch < (useChannelCount ? mesh.tangentChannelCount : NUM_TANGENTS); ch++)
		FillTangent(useBatch, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill binormal
	for (size_t ch = 0; ch < (useChannelCount ? mesh.binormalChannelCount : NUM_BINORMALS); ch++)
		FillBinormal(useBatch, dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill polygons
	FillPolygon(useBatch, dstMesh, mesh);

	////////////////////////////////////////////////////////
	// fill material 
	FbxNode* dstNode = FbxNode::Create(fbxScene, name);
	dstNode->SetNodeAttribute(dstMesh); 
	fbxNode->AddChild(dstNode);

	////////////////////////////////////////////////////////
	// copy Geometry Transform
	auto t1 = fbxNode->GetGeometricTranslation(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	auto r1 = fbxNode->GetGeometricRotation(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	auto s1 = fbxNode->GetGeometricScaling(fbxsdk::FbxNode::EPivotSet::eSourcePivot);
	dstNode->SetGeometricTranslation(fbxsdk::FbxNode::EPivotSet::eSourcePivot, t1);
	dstNode->SetGeometricRotation(fbxsdk::FbxNode::EPivotSet::eSourcePivot, r1);
	dstNode->SetGeometricScaling(fbxsdk::FbxNode::EPivotSet::eSourcePivot, s1);

	FillMaterial(dstMesh, fbxNode);
}

void FBXMeshBuilder::FillColor(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexBatcher<Color> batcher(useBatch);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).colors[ch]);

	FbxGeometryElementVertexColor* geometryElementVertexColor = dstMesh->CreateElementVertexColor();
	geometryElementVertexColor->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementVertexColor->SetReferenceMode((useBatch ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < batcher.vertices.size(); i++)
	{
		const Color& c = batcher.vertices[i];
		geometryElementVertexColor->GetDirectArray().Add(FbxColor(c[0], c[1], c[2], c[3]));
	}

	if (useBatch)
	{
		for (size_t i = 0; i < batcher.indices.size(); i++)
		{
			int idx = batcher.indices[i];
			geometryElementVertexColor->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillUV(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexBatcher<Vector2> batcher(useBatch);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).uvs[ch]);

	FbxGeometryElementUV* geometryElementUV = dstMesh->CreateElementUV(FbxString("UV") + ((int)(ch + 1)));
	geometryElementUV->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementUV->SetReferenceMode((useBatch ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < batcher.vertices.size(); i++)
	{
		const Vector2& uv = batcher.vertices[i];
		geometryElementUV->GetDirectArray().Add(FbxVector2(uv.X(), uv.Y()));
	}

	if (useBatch)
	{
		for (size_t i = 0; i < batcher.indices.size(); i++)
		{
			int idx = batcher.indices[i];
			geometryElementUV->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillNormal(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexBatcher<Vector3> batcher(useBatch);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).normals[ch]);

	FbxGeometryElementNormal* geometryElementNormal = dstMesh->CreateElementNormal();
	geometryElementNormal->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementNormal->SetReferenceMode((useBatch ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < batcher.vertices.size(); i++)
	{
		const Vector3& n = batcher.vertices[i];
		geometryElementNormal->GetDirectArray().Add(FbxVector4(n.X(), n.Y(), n.Z(), 0.0f));
	}

	if (useBatch)
	{
		for (size_t i = 0; i < batcher.indices.size(); i++)
		{
			int idx = batcher.indices[i];
			geometryElementNormal->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillTangent(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexBatcher<Vector3> batcher(useBatch);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).tangents[ch]);

	FbxGeometryElementTangent* geometryElementTangent = dstMesh->CreateElementTangent();
	geometryElementTangent->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementTangent->SetReferenceMode((useBatch ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < batcher.vertices.size(); i++)
	{
		const Vector3& t = batcher.vertices[i];
		geometryElementTangent->GetDirectArray().Add(FbxVector4(t.X(), t.Y(), t.Z(), 0.0f));
	}

	if (useBatch)
	{
		for (size_t i = 0; i < batcher.indices.size(); i++)
		{
			int idx = batcher.indices[i];
			geometryElementTangent->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillBinormal(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexBatcher<Vector3> batcher(useBatch);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).binormals[ch]);

	FbxGeometryElementBinormal* geometryElementBinormal = dstMesh->CreateElementBinormal();
	geometryElementBinormal->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	geometryElementBinormal->SetReferenceMode((useBatch ? FbxGeometryElement::eIndexToDirect : FbxGeometryElement::eDirect));

	for (size_t i = 0; i < batcher.vertices.size(); i++)
	{
		const Vector3& b = batcher.vertices[i];
		geometryElementBinormal->GetDirectArray().Add(FbxVector4(b.X(), b.Y(), b.Z(), 0.0f));
	}

	if (useBatch)
	{
		for (size_t i = 0; i < batcher.indices.size(); i++)
		{
			int idx = batcher.indices[i];
			geometryElementBinormal->GetIndexArray().Add(idx);
		}
	}
}

void FBXMeshBuilder::FillPolygon(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh)
{
	VertexBatcher<Vector3> batcher(useBatch);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).position);

	for (size_t i = 0; i < batcher.vertices.size(); i++)
	{
		const Vector3& p = batcher.vertices[i];
		dstMesh->mControlPoints.Add(FbxVector4(p.X(), p.Y(), p.Z(), 0.0));
	}

	int vertexIdx = 0;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		dstMesh->BeginPolygon(mesh.GetPolygon(i).GetMaterialIdx());

		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			dstMesh->AddPolygon(batcher.indices[vertexIdx]);

			vertexIdx++;
		}

		dstMesh->EndPolygon();
	}
}

void FBXMeshBuilder::FillMaterial(FbxMesh* dstMesh, FbxNode* fbxNode)
{
	FbxMesh* srcMesh = fbxNode->GetMesh();
	int a = fbxNode->GetMaterialCount();
	for (size_t i = 0; i < fbxNode->GetMaterialCount(); i++)
		dstMesh->GetNode()->AddMaterial(fbxNode->GetMaterial(i));
}