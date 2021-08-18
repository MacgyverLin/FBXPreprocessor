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
bool FBXMeshBuilder::Build(FbxScene* fbxScene, std::vector<FbxNode*>& fbxNodes, std::vector<MeshArray>& precutMeshArrays)
{
	//if (!ConvertMeshesTriangleFanToTriangles(precutMeshArrays))
		//return false;

	if (!BuildFbxMeshes(fbxScene, fbxNodes, precutMeshArrays))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
bool FBXMeshBuilder::BuildFbxMeshes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes, std::vector<MeshArray>& precutMeshArrays)
{
	for (size_t i = 0; i < fbxNodes.size(); i++)
	{
		for (size_t j = 0; j < precutMeshArrays[i].size(); j++)
		{
			FbxString name(fbxNodes[i]->GetName());
			name += FbxString("_");
			name += (int)j;

			FbxNode* newMeshNode = BuildFbxMesh(fbxScene, fbxNodes[i], precutMeshArrays[i][j], name);
			fbxNodes[i]->AddChild(newMeshNode);
		}
	}

	return true;
}

/*
TODO:
	1) indexed done, not check

	2) mesh.colorChannelCount, mesh.normalChannelCount...
	for (size_t ch = 0; ch < NUM_COLORS; ch++)...
	for (size_t ch = 0; ch < NUM_NORMALS; ch++)...

	3) Split plane Selection

	4) indexed position
*/

FbxNode* FBXMeshBuilder::BuildFbxMesh(FbxScene* fbxScene, FbxNode* fbxNode, const Mesh& mesh, const FbxString& name)
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
	FillPosition(dstMesh, mesh);
	FillPolygon(dstMesh, mesh);

	////////////////////////////////////////////////////////
	// fill material
	FbxNode* dstNode = FbxNode::Create(fbxScene, name);
	dstNode->SetNodeAttribute(dstMesh);

	FillMaterial(dstMesh, fbxNode);

	return dstNode;
}

void FBXMeshBuilder::FillPosition(FbxMesh* dstMesh, const Mesh& mesh)
{
	int vertexIdx = 0;
	
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			const Vector3& p = mesh.GetPolygon(i).GetVertex(k).position;

			dstMesh->mControlPoints.Add(FbxVector4(p.X(), p.Y(), p.Z(), 0.0));

			vertexIdx++;
		}
	}
}

/*
void FBXMeshBuilder::FillColor(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementVertexColor* geometryElementVertexColor = dstMesh->CreateElementVertexColor();
	geometryElementVertexColor->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementVertexColor->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			const Color& c = mesh.GetPolygon(i).GetVertex(k).colors[ch];
			geometryElementVertexColor->GetDirectArray().Add(FbxColor(c[0], c[1], c[2], c[3]));
		}
	}
}
*/

void FBXMeshBuilder::FillColor(bool useBatch, FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexBatcher<Color> batcher(useBatch);
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).colors[ch]);

	FbxGeometryElementVertexColor* geometryElementVertexColor = dstMesh->CreateElementVertexColor();
	geometryElementVertexColor->SetMappingMode(FbxGeometryElement::eByControlPoint);
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
	int v = mesh.GetVerticesCount();
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			batcher.Add(mesh.GetPolygon(i).GetVertex(k).normals[ch]);


	FbxGeometryElementNormal* geometryElementNormal = dstMesh->CreateElementNormal();
	geometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
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
	geometryElementTangent->SetMappingMode(FbxGeometryElement::eByControlPoint);
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
	geometryElementBinormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
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

void FBXMeshBuilder::FillPolygon(FbxMesh* dstMesh, const Mesh& mesh)
{
	int vertexIdx = 0;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		dstMesh->BeginPolygon(mesh.GetPolygon(i).materialIdx);

		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			dstMesh->AddPolygon(vertexIdx);

			vertexIdx++;
		}

		dstMesh->EndPolygon();
	}
}

void FBXMeshBuilder::FillMaterial(FbxMesh* dstMesh, FbxNode* fbxNode)
{
	FbxMesh* srcMesh = fbxNode->GetMesh();
	for (size_t i = 0; i < srcMesh->GetNode()->GetMaterialCount(); i++)
		dstMesh->GetNode()->AddMaterial(srcMesh->GetNode()->GetMaterial(i));
}

/////////////////////////////////////////////////////////////////////////////////
bool FBXMeshBuilder::ConvertMeshesTriangleFanToTriangles(std::vector<MeshArray>& slicedMeshArrays)
{
	for (size_t i = 0; i < slicedMeshArrays.size(); i++)
	{
		for (size_t j = 0; j < slicedMeshArrays[i].size(); j++)
		{
			ConvertTriangleFanToTriangles(slicedMeshArrays[i][j]);
		}
	}

	return true;
}