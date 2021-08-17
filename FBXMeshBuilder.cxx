#include "FBXMeshBuilder.h"

template<class T>
class VertexOptimizer
{
friend class FBXMeshBuilder;
public:
	VertexOptimizer()
	{
	}

	~VertexOptimizer()
	{
	}

	int Add(T v)
	{
		std::map<T, int>::iterator itr = verticesMap.find(v);
		if (itr != verticesMap.end())
		{
			return itr->second;
		}
		else
		{
			int idx = vertices.size();

			verticesMap[v] = idx;
			
			vertices.push_back(v);
			indices.push_back(idx);

			return idx;
		}
	}
private:
	std::map<T, int> verticesMap;

	std::vector<T> vertices;
	std::vector<int> indices;
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

void FBXMeshBuilder::FillDirectColor(FbxMesh* dstMesh, const Mesh& mesh, int ch)
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

void FBXMeshBuilder::FillDirectNormal(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementNormal* geometryElementNormal = dstMesh->CreateElementNormal();
	geometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			const Vector3& n = mesh.GetPolygon(i).GetVertex(k).normals[ch];
			geometryElementNormal->GetDirectArray().Add(FbxVector4(n.X(), n.Y(), n.Z(), 0.0f));
		}
	}
}

void FBXMeshBuilder::FillDirectUV(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementUV* geometryElementUV = dstMesh->CreateElementUV(FbxString("UV") + ((int)(ch + 1)));
	geometryElementUV->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementUV->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			const Vector2& uv = mesh.GetPolygon(i).GetVertex(k).uvs[ch];
			geometryElementUV->GetDirectArray().Add(FbxVector2(uv.X(), uv.Y()));
		}
	}
}

void FBXMeshBuilder::FillDirectTangent(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementTangent* geometryElementTangent = dstMesh->CreateElementTangent();
	geometryElementTangent->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementTangent->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			const Vector3& t = mesh.GetPolygon(i).GetVertex(k).tangents[ch];
			geometryElementTangent->GetDirectArray().Add(FbxVector4(t.X(), t.Y(), t.Z(), 0.0f));
		}
	}
}

void FBXMeshBuilder::FillDirectBinormal(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementBinormal* geometryElementBinormal = dstMesh->CreateElementBinormal();
	geometryElementBinormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementBinormal->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
	{
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
		{
			const Vector3& b = mesh.GetPolygon(i).GetVertex(k).binormals[ch];
			geometryElementBinormal->GetDirectArray().Add(FbxVector4(b.X(), b.Y(), b.Z(), 0.0f));
		}
	}
}

void FBXMeshBuilder::FillIndexedColor(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexOptimizer<Color> optimizer;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).colors[ch]);


	FbxGeometryElementVertexColor* geometryElementVertexColor = dstMesh->CreateElementVertexColor();
	geometryElementVertexColor->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementVertexColor->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	for (size_t i = 0; i < optimizer.vertices.size(); i++)
	{
		const Color& c = optimizer.vertices[i];
		geometryElementVertexColor->GetDirectArray().Add(FbxColor(c[0], c[1], c[2], c[3]));
	}

	for (size_t i = 0; i < optimizer.indices.size(); i++)
	{
		int idx = optimizer.indices[i];
		geometryElementVertexColor->GetIndexArray().Add(idx);
	}
}

void FBXMeshBuilder::FillIndexedNormal(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexOptimizer<Vector3> optimizer;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).normals[ch]);


	FbxGeometryElementNormal* geometryElementNormal = dstMesh->CreateElementNormal();
	geometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementNormal->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	for (size_t i = 0; i < optimizer.vertices.size(); i++)
	{
		const Vector3& n = optimizer.vertices[i];
		geometryElementNormal->GetDirectArray().Add(FbxVector4(n.X(), n.Y(), n.Z(), 0.0f));
	}

	for (size_t i = 0; i < optimizer.indices.size(); i++)
	{
		int idx = optimizer.indices[i];
		geometryElementNormal->GetIndexArray().Add(idx);
	}
}

void FBXMeshBuilder::FillIndexedUV(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexOptimizer<Vector2> optimizer;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).uvs[ch]);


	FbxGeometryElementUV* geometryElementUV = dstMesh->CreateElementUV(FbxString("UV") + ((int)(ch + 1)));
	geometryElementUV->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementUV->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	for (size_t i = 0; i < optimizer.vertices.size(); i++)
	{
		const Vector2& uv = optimizer.vertices[i];
		geometryElementUV->GetDirectArray().Add(FbxVector2(uv.X(), uv.Y()));
	}

	for (size_t i = 0; i < optimizer.indices.size(); i++)
	{
		int idx = optimizer.indices[i];
		geometryElementUV->GetIndexArray().Add(idx);
	}
}

void FBXMeshBuilder::FillIndexedTangent(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexOptimizer<Vector3> optimizer;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).tangents[ch]);


	FbxGeometryElementTangent* geometryElementTangent = dstMesh->CreateElementTangent();
	geometryElementTangent->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementTangent->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	for (size_t i = 0; i < optimizer.vertices.size(); i++)
	{
		const Vector3& t = optimizer.vertices[i];
		geometryElementTangent->GetDirectArray().Add(FbxVector4(t.X(), t.Y(), t.Z(), 0.0f));
	}

	for (size_t i = 0; i < optimizer.indices.size(); i++)
	{
		int idx = optimizer.indices[i];
		geometryElementTangent->GetIndexArray().Add(idx);
	}
}

void FBXMeshBuilder::FillIndexedBinormal(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	VertexOptimizer<Vector3> optimizer;
	for (size_t i = 0; i < mesh.GetPolygonCount(); i++)
		for (size_t k = 0; k < mesh.GetPolygon(i).GetVerticesCount(); k++)
			optimizer.Add(mesh.GetPolygon(i).GetVertex(k).binormals[ch]);


	FbxGeometryElementBinormal* geometryElementBinormal = dstMesh->CreateElementBinormal();
	geometryElementBinormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementBinormal->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	for (size_t i = 0; i < optimizer.vertices.size(); i++)
	{
		const Vector3& b = optimizer.vertices[i];
		geometryElementBinormal->GetDirectArray().Add(FbxVector4(b.X(), b.Y(), b.Z(), 0.0f));
	}

	for (size_t i = 0; i < optimizer.indices.size(); i++)
	{
		int idx = optimizer.indices[i];
		geometryElementBinormal->GetIndexArray().Add(idx);
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
	FillPosition(dstMesh, mesh);

	bool indexed = false;
	////////////////////////////////////////////////////////
	// fill color
	for (size_t ch = 0; ch < NUM_COLORS; ch++)
	{
		if(indexed)
			FillIndexedColor(dstMesh, mesh, ch);
		else
			FillDirectColor(dstMesh, mesh, ch);
	}

	////////////////////////////////////////////////////////
	// fill normal
	for (size_t ch = 0; ch < NUM_NORMALS; ch++)
	{
		if (indexed)
			FillIndexedNormal(dstMesh, mesh, ch);
		else
			FillDirectNormal(dstMesh, mesh, ch);
	}

	////////////////////////////////////////////////////////
	// fill uv
	for (size_t ch = 0; ch < NUM_UVS; ch++)
	{
		if (indexed)
			FillIndexedUV(dstMesh, mesh, ch);
		else
			FillDirectUV(dstMesh, mesh, ch);
	}

	////////////////////////////////////////////////////////
	// fill tangent
	for (size_t ch = 0; ch < NUM_TANGENTS; ch++)
	{
		if (indexed)
			FillIndexedTangent(dstMesh, mesh, ch);
		else
			FillDirectTangent(dstMesh, mesh, ch);
	}

	////////////////////////////////////////////////////////
	// fill binormal
	for (size_t ch = 0; ch < NUM_BINORMALS; ch++)
	{
		if (indexed)
			FillIndexedBinormal(dstMesh, mesh, ch);
		else
			FillDirectBinormal(dstMesh, mesh, ch);
	}

	////////////////////////////////////////////////////////
	// fill polygons
	FillPolygon(dstMesh, mesh);

	////////////////////////////////////////////////////////
	// fill material
	FbxNode* dstNode = FbxNode::Create(fbxScene, name);
	dstNode->SetNodeAttribute(dstMesh);

	FillMaterial(dstMesh, fbxNode);

	return dstNode;
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