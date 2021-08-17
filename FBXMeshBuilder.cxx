#include "FBXMeshBuilder.h"

FBXMeshBuilder::FBXMeshBuilder()
{
}

FBXMeshBuilder::~FBXMeshBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////////
bool FBXMeshBuilder::Build(FbxScene* fbxScene, std::vector<FbxNode*>& fbxNodes, std::vector<MeshArray>& precutMeshArrays)
{
	if (!ConvertMeshesTriangleFanToTriangles(precutMeshArrays))
		return false;

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
	dstMesh->InitControlPoints(mesh.polygons.size() * NUM_VERTICES_PER_POLYGON);
	FbxVector4* controlPoints = dstMesh->GetControlPoints();
	{
		for (size_t i = 0; i < mesh.polygons.size(); i++)
		{
			for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
			{
				const Vector3& p = mesh.polygons[i].vertices[k].position;
				controlPoints[i * NUM_VERTICES_PER_POLYGON + k] = FbxVector4(p.X(), p.Y(), p.Z(), 0.0);
			}
		}
	}
}

void FBXMeshBuilder::FillColor(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementVertexColor* geometryElementVertexColor = dstMesh->CreateElementVertexColor();
	geometryElementVertexColor->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementVertexColor->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
		{
			const Color& c = mesh.polygons[i].vertices[k].colors[ch];
			geometryElementVertexColor->GetDirectArray().Add(FbxColor(c[0], c[1], c[2], c[3]));
		}
	}
}

void FBXMeshBuilder::FillNormal(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementNormal* geometryElementNormal = dstMesh->CreateElementNormal();
	geometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
		{
			const Vector3& n = mesh.polygons[i].vertices[k].normals[ch];
			geometryElementNormal->GetDirectArray().Add(FbxVector4(n.X(), n.Y(), n.Z(), 0.0f));
		}
	}
}

void FBXMeshBuilder::FillUV(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementUV* geometryElementUV = dstMesh->CreateElementUV(FbxString("UV") + ((int)(ch + 1)));
	geometryElementUV->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementUV->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
		{
			const Vector2& uv = mesh.polygons[i].vertices[k].uvs[ch];
			geometryElementUV->GetDirectArray().Add(FbxVector2(uv.X(), uv.Y()));
		}
	}
}

void FBXMeshBuilder::FillTangent(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementTangent* geometryElementTangent = dstMesh->CreateElementTangent();
	geometryElementTangent->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementTangent->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
		{
			const Vector3& t = mesh.polygons[i].vertices[k].tangents[ch];
			geometryElementTangent->GetDirectArray().Add(FbxVector4(t.X(), t.Y(), t.Z(), 0.0f));
		}
	}
}

void FBXMeshBuilder::FillBinormal(FbxMesh* dstMesh, const Mesh& mesh, int ch)
{
	FbxGeometryElementBinormal* geometryElementBinormal = dstMesh->CreateElementBinormal();
	geometryElementBinormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	geometryElementBinormal->SetReferenceMode(FbxGeometryElement::eDirect);

	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
		{
			const Vector3& b = mesh.polygons[i].vertices[k].binormals[ch];
			geometryElementBinormal->GetDirectArray().Add(FbxVector4(b.X(), b.Y(), b.Z(), 0.0f));
		}
	}
}

void FBXMeshBuilder::FillPolygon(FbxMesh* dstMesh, const Mesh& mesh)
{
	for (size_t i = 0; i < mesh.polygons.size(); i++)
	{
		dstMesh->BeginPolygon(mesh.polygons[i].materialIdx);

		for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
			dstMesh->AddPolygon(i * NUM_VERTICES_PER_POLYGON + k);

		dstMesh->EndPolygon();
	}
}

void FBXMeshBuilder::FillMaterial(FbxMesh* dstMesh, FbxNode* fbxNode)
{
	FbxMesh* srcMesh = fbxNode->GetMesh();
	for (size_t i = 0; i < srcMesh->GetNode()->GetMaterialCount(); i++)
		dstMesh->GetNode()->AddMaterial(srcMesh->GetNode()->GetMaterial(i));
}

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

	////////////////////////////////////////////////////////
	// fill color
	for (size_t ch = 0; ch < NUM_COLORS; ch++)
		FillColor(dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill normal
	for (size_t ch = 0; ch < NUM_NORMALS; ch++)
		FillNormal(dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill uv
	for (size_t ch = 0; ch < NUM_UVS; ch++)
		FillUV(dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill tangent
	for (size_t ch = 0; ch < NUM_TANGENTS; ch++)
		FillTangent(dstMesh, mesh, ch);

	////////////////////////////////////////////////////////
	// fill binormal
	for (size_t ch = 0; ch < NUM_BINORMALS; ch++)
		FillBinormal(dstMesh, mesh, ch);

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

void FBXMeshBuilder::ConvertTriangleFanToTriangles(Mesh& mesh)
{
	std::vector<Polygon> oldPolygons = mesh.polygons;
	mesh.polygons.clear();

	for (size_t i = 0; i < oldPolygons.size(); i++)
		ConvertTriangleFanToTriangles(oldPolygons[i], mesh.polygons);
}

void FBXMeshBuilder::ConvertTriangleFanToTriangles(const Polygon& polygon, std::vector<Polygon>& polygons)
{
	if (polygon.vertices.size() < 3)
		return;

	int startIdx = polygons.size();
	polygons.resize(polygons.size() + polygon.vertices.size() - 2);
	for (size_t i = 0; i < polygon.vertices.size() - 2; i++)
	{
		polygons[startIdx + i].materialIdx = polygon.materialIdx;

		polygons[startIdx + i].vertices.resize(3);
		polygons[startIdx + i].vertices[0] = polygon.vertices[0];
		polygons[startIdx + i].vertices[1] = polygon.vertices[i + 1];
		polygons[startIdx + i].vertices[2] = polygon.vertices[i + 2];
	}
}