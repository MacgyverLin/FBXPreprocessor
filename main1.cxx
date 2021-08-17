#include "../Common/Common.h"
#include <vector>
#include <map>
#include <assert.h>

#define NUM_COLORS 2
#define NUM_UVS 8
#define NUM_NORMALS 1
#define NUM_TANGENTS 1
#define NUM_BINORMALS 1

#define NUM_VERTICES_PER_POLYGON 3

void CreateMaterials(FbxScene* pScene, FbxMesh* pMesh)
{
	int i;

	for (i = 0; i < 5; i++)
	{
		FbxString lMaterialName = "material";
		FbxString lShadingName = "Phong";
		lMaterialName += i;
		FbxDouble3 lBlack(0.0, 0.0, 0.0);
		FbxDouble3 lRed(1.0, 0.0, 0.0);
		FbxDouble3 lColor;
		FbxSurfacePhong* lMaterial = FbxSurfacePhong::Create(pScene, lMaterialName.Buffer());


		// Generate primary and secondary colors.
		lMaterial->Emissive.Set(lBlack);
		lMaterial->Ambient.Set(lRed);
		lColor = FbxDouble3(i > 2 ? 1.0 : 0.0,
			i > 0 && i < 4 ? 1.0 : 0.0,
			i % 2 ? 0.0 : 1.0);
		lMaterial->Diffuse.Set(lColor);
		lMaterial->TransparencyFactor.Set(0.0);
		lMaterial->ShadingModel.Set(lShadingName);
		lMaterial->Shininess.Set(0.5);

		//get the node of mesh, add material for it.
		FbxNode* lNode = pMesh->GetNode();
		if (lNode)
			lNode->AddMaterial(lMaterial);
	}
}

// Create a pyramid with materials. 
FbxNode* CreatePyramidWithMaterials(FbxScene* pScene, const char* pName)
{
	int i, j;
	FbxMesh* lMesh = FbxMesh::Create(pScene, pName);

	FbxVector4 lControlPoint0(-5, 0, 5);
	FbxVector4 lControlPoint1(5, 0, 5);
	FbxVector4 lControlPoint2(5, 0, -5);
	FbxVector4 lControlPoint3(-5, 0, -5);
	FbxVector4 lControlPoint4(0, 10, 0);

	FbxVector4 lNormalP0(0, 1, 0);
	FbxVector4 lNormalP1(0, 0.447, 0.894);
	FbxVector4 lNormalP2(0.894, 0.447, 0);
	FbxVector4 lNormalP3(0, 0.447, -0.894);
	FbxVector4 lNormalP4(-0.894, 0.447, 0);

	// Create control points.
	lMesh->InitControlPoints(16);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	lControlPoints[0] = lControlPoint0;
	lControlPoints[1] = lControlPoint1;
	lControlPoints[2] = lControlPoint2;
	lControlPoints[3] = lControlPoint3;
	lControlPoints[4] = lControlPoint0;
	lControlPoints[5] = lControlPoint1;
	lControlPoints[6] = lControlPoint4;
	lControlPoints[7] = lControlPoint1;
	lControlPoints[8] = lControlPoint2;
	lControlPoints[9] = lControlPoint4;
	lControlPoints[10] = lControlPoint2;
	lControlPoints[11] = lControlPoint3;
	lControlPoints[12] = lControlPoint4;
	lControlPoints[13] = lControlPoint3;
	lControlPoints[14] = lControlPoint0;
	lControlPoints[15] = lControlPoint4;

	// specify normals per control point.

	FbxGeometryElementNormal* lNormalElement = lMesh->CreateElementNormal();
	lNormalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
	lNormalElement->SetReferenceMode(FbxGeometryElement::eDirect);

	lNormalElement->GetDirectArray().Add(lNormalP0);
	lNormalElement->GetDirectArray().Add(lNormalP0);
	lNormalElement->GetDirectArray().Add(lNormalP0);
	lNormalElement->GetDirectArray().Add(lNormalP0);
	lNormalElement->GetDirectArray().Add(lNormalP1);
	lNormalElement->GetDirectArray().Add(lNormalP1);
	lNormalElement->GetDirectArray().Add(lNormalP1);
	lNormalElement->GetDirectArray().Add(lNormalP2);
	lNormalElement->GetDirectArray().Add(lNormalP2);
	lNormalElement->GetDirectArray().Add(lNormalP2);
	lNormalElement->GetDirectArray().Add(lNormalP3);
	lNormalElement->GetDirectArray().Add(lNormalP3);
	lNormalElement->GetDirectArray().Add(lNormalP3);
	lNormalElement->GetDirectArray().Add(lNormalP4);
	lNormalElement->GetDirectArray().Add(lNormalP4);
	lNormalElement->GetDirectArray().Add(lNormalP4);


	// Array of polygon vertices.
	int lPolygonVertices[] = { 0, 3, 2, 1,
		4, 5, 6,
		7, 8, 9,
		10, 11, 12,
		13, 14, 15 };

	// Set material mapping.
	FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	// Create polygons. Assign material indices.

	// Pyramid base.
	lMesh->BeginPolygon(0); // Material index.

	for (j = 0; j < 4; j++)
	{
		lMesh->AddPolygon(lPolygonVertices[j]); // Control point index.
	}

	lMesh->EndPolygon();

	// Pyramid sides.
	for (i = 1; i < 5; i++)
	{
		lMesh->BeginPolygon(i); // Material index.

		for (j = 0; j < 3; j++)
		{
			lMesh->AddPolygon(lPolygonVertices[4 + 3 * (i - 1) + j]); // Control point index.
		}

		lMesh->EndPolygon();
	}


	FbxNode* lNode = FbxNode::Create(pScene, pName);

	lNode->SetNodeAttribute(lMesh);

	CreateMaterials(pScene, lMesh);

	return lNode;
}

class Vertex
{
public:
	Vertex()
	{
	}

	Vertex(const Vertex& other)
	{
		memcpy(this, &other, sizeof(Vertex));
	}

	Vertex& operator = (const Vertex& other)
	{
		memcpy(this, &other, sizeof(Vertex));

		return *this;
	}

	FbxVector4 position;
	FbxColor colors[NUM_COLORS];
	FbxVector2 uvs[NUM_UVS];
	FbxVector4 normals[NUM_NORMALS];
	FbxVector4 tangents[NUM_TANGENTS];
	FbxVector4 binormals[NUM_BINORMALS];
};

class Polygon
{
public:
	Polygon()
	{
	}

	Polygon(const Polygon& other)
	{
		vertices = other.vertices;
	}

	Polygon& operator = (const Polygon& other)
	{
		vertices = other.vertices;

		return *this;
	}

	std::vector<Vertex> vertices;
};

void InfoMsg(FbxString msg)
{
	FBXSDK_printf("Info: " + msg);
}

void ErrorMsg(FbxString msg)
{
	FBXSDK_printf("Error: " + msg);
}

class Arguments
{
public:
	Arguments()
	{
		inputPath = "";
		outputPath = "out.fbx";
		unity = true;
		unreal = false;
	}

	FbxString inputPath;
	FbxString outputPath;
	bool unity;
	bool unreal;
};

Arguments processArgs(int argc, const char** argv)
{
	Arguments params;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (stricmp(&argv[i][1], "o") == 0 || stricmp(&argv[i][1], "out") == 0)
			{
				i++;
				params.outputPath = argv[i];
			}
			else if (stricmp(&argv[i][1], "engine") == 0)
			{
				i++;
				if (stricmp(argv[i], "unreal") == 0)
				{
					params.unreal = true;
				}
				else if (stricmp(argv[i], "unity") == 0)
				{
					params.unity = true;
				}
				else
				{
					params.unity = true;
				}
			}
		}
		else
		{
			params.inputPath = argv[i];
		}
	}

	return params;
}

void CollectMeshNode(FbxNode* node, std::vector<FbxNode* >& nodes)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if (node->GetNodeAttribute() == NULL)
	{
		FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (node->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		case FbxNodeAttribute::eMarker:
			// DisplayMarker(node);
			break;

		case FbxNodeAttribute::eSkeleton:
			// DisplaySkeleton(node);
			break;

		case FbxNodeAttribute::eMesh:
			nodes.push_back(node);
			break;

		case FbxNodeAttribute::eNurbs:
			// DisplayNurb(node);
			break;

		case FbxNodeAttribute::ePatch:
			// DisplayPatch(node);
			break;

		case FbxNodeAttribute::eCamera:
			// DisplayCamera(node);
			break;

		case FbxNodeAttribute::eLight:
			// DisplayLight(node);
			break;

		case FbxNodeAttribute::eLODGroup:
			// DisplayLodGroup(node);
			break;

		default:
			break;
		}
	}

	//DisplayUserProperties(pNode);
	//DisplayTarget(pNode);
	//DisplayPivotsAndLimits(pNode);
	//DisplayTransformPropagation(pNode);
	//DisplayGeometricTransform(pNode);

	for (i = 0; i < node->GetChildCount(); i++)
	{
		CollectMeshNode(node->GetChild(i), nodes);
	}
}

void CollectMeshNode(FbxScene* scene, std::vector<FbxNode* >& nodes)
{
	int i;
	FbxNode* node = scene->GetRootNode();

	if (node)
	{
		for (i = 0; i < node->GetChildCount(); i++)
		{
			CollectMeshNode(node->GetChild(i), nodes);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CollectPolygons(FbxMesh* mesh, std::vector<Polygon>& polygons)
{
	//int lPolygonCount = ;
	//char header[100];
	// DisplayString("    Polygons");
	mesh->ComputeBBox();

	polygons.resize(mesh->GetPolygonCount());

	int vertexId = 0;
	for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++)
	{
		Polygon& polygon = polygons[polygonIndex];

		// DisplayInt("        Polygon ", i);
		for (int polygonGroupIndex = 0; polygonGroupIndex < mesh->GetElementPolygonGroupCount(); polygonGroupIndex++)
		{
			FbxGeometryElementPolygonGroup* lePolgrp = mesh->GetElementPolygonGroup(polygonGroupIndex);
			switch (lePolgrp->GetMappingMode())
			{
			case FbxGeometryElement::eByPolygon:
				if (lePolgrp->GetReferenceMode() == FbxGeometryElement::eIndex)
				{
					//FBXSDK_sprintf(header, 100, "        Assigned to group: ");
					int polyGroupId = lePolgrp->GetIndexArray().GetAt(polygonIndex);
					//DisplayInt(header, polyGroupId);
					break;
				}
			default:
				// any other mapping modes don't make sense
				// DisplayString("        \"unsupported group assignment\"");
				//break;

				ErrorMsg("unsupported group assignment");
				return false;
			}
		}

		for (int polygonVertexIndex = 0; polygonVertexIndex < mesh->GetPolygonSize(polygonIndex); polygonVertexIndex++)
		{
			polygon.vertices.push_back(Vertex());

			Vertex& vertex = polygon.vertices.back();

			FbxVector4* lControlPoints = mesh->GetControlPoints();

			int lControlPointIndex = mesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
			if (lControlPointIndex < 0)
			{
				// DisplayString("            Coordinates: Invalid index found!");
				// continue;
				return false;
			}
			else
			{
				//Display3DVector("            Coordinates: ", lControlPoints[lControlPointIndex]);
				//printf("%i: %4.4f %4.4f %4.4f\n", vertexId, lControlPoints[lControlPointIndex][0], lControlPoints[lControlPointIndex][1], lControlPoints[lControlPointIndex][2]);
				vertex.position = lControlPoints[lControlPointIndex];
			}

			for (int layer = 0; layer < mesh->GetElementVertexColorCount() && layer < NUM_COLORS; layer++)
			{
				FbxGeometryElementVertexColor* leVtxc = mesh->GetElementVertexColor(layer);
				// FBXSDK_sprintf(header, 100, "            Color vertex: ");

				switch (leVtxc->GetMappingMode())
				{
				case FbxGeometryElement::eByControlPoint:
				{
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));

						vertex.colors[layer] = leVtxc->GetDirectArray().GetAt(lControlPointIndex);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
						//DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));

						int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
						vertex.colors[layer] = leVtxc->GetDirectArray().GetAt(id);
					}
					break;

					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					switch (leVtxc->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
						vertex.colors[layer] = leVtxc->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leVtxc->GetIndexArray().GetAt(vertexId);
						//DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));

						int id = leVtxc->GetIndexArray().GetAt(vertexId);
						vertex.colors[layer] = leVtxc->GetDirectArray().GetAt(id);
					}
					break;

					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for Colors
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for Colors
				case FbxGeometryElement::eNone:       // doesn't make much sense for Colors
				default:
					break;
				}
			}

			for (int layer = 0; layer < mesh->GetElementUVCount() && layer < NUM_UVS; ++layer)
			{
				FbxGeometryElementUV* leUV = mesh->GetElementUV(layer);
				// FBXSDK_sprintf(header, 100, "            Texture UV: ");

				switch (leUV->GetMappingMode())
				{
				case FbxGeometryElement::eByControlPoint:
				{
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));

						vertex.uvs[layer] = leUV->GetDirectArray().GetAt(lControlPointIndex);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						// int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						// Display2DVector(header, leUV->GetDirectArray().GetAt(id));

						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						vertex.uvs[layer] = leUV->GetDirectArray().GetAt(id);
					}
					break;

					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = mesh->GetTextureUVIndex(polygonIndex, polygonVertexIndex);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						// Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));

						vertex.uvs[layer] = leUV->GetDirectArray().GetAt(lTextureUVIndex);
					}
					break;

					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
				default:
					break;
				}
			}

			for (int layer = 0; layer < mesh->GetElementNormalCount(); ++layer)
			{
				FbxGeometryElementNormal* leNormal = mesh->GetElementNormal(layer);
				// FBXSDK_sprintf(header, 100, "            Normal: ");

				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));

						vertex.normals[layer] = leNormal->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						// int id = leNormal->GetIndexArray().GetAt(vertexId);
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(id));

						int id = leNormal->GetIndexArray().GetAt(vertexId);
						vertex.normals[layer] = leNormal->GetDirectArray().GetAt(id);
					}
					break;

					default:
						break; // other reference modes not shown here!
					}
				}

			}

			for (int layer = 0; layer < mesh->GetElementTangentCount(); ++layer)
			{
				FbxGeometryElementTangent* leTangent = mesh->GetElementTangent(layer);
				// FBXSDK_sprintf(header, 100, "            Tangent: ");

				if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leTangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));

						vertex.tangents[layer] = leTangent->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leTangent->GetIndexArray().GetAt(vertexId);
						//Display3DVector(header, leTangent->GetDirectArray().GetAt(id));

						int id = leTangent->GetIndexArray().GetAt(vertexId);
						vertex.tangents[layer] = leTangent->GetDirectArray().GetAt(id);
					}
					break;

					default:
						break; // other reference modes not shown here!
					}
				}
			}

			for (int layer = 0; layer < mesh->GetElementBinormalCount(); ++layer)
			{
				FbxGeometryElementBinormal* leBinormal = mesh->GetElementBinormal(layer);

				// FBXSDK_sprintf(header, 100, "            Binormal: ");
				if (leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leBinormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));

						vertex.binormals[layer] = leBinormal->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leBinormal->GetIndexArray().GetAt(vertexId);
						//Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));

						int id = leBinormal->GetIndexArray().GetAt(vertexId);
						vertex.binormals[layer] = leBinormal->GetDirectArray().GetAt(id);
					}
					break;

					default:
						break; // other reference modes not shown here!
					}
				}
			}
			vertexId++;
		} // for polygonSize
	} // for polygonCount

	/*
	//check visibility for the edges of the mesh
	for (int l = 0; l < mesh->GetElementVisibilityCount(); ++l)
	{
		FbxGeometryElementVisibility* leVisibility = mesh->GetElementVisibility(l);
		FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
		DisplayString(header);
		switch (leVisibility->GetMappingMode())
		{
		default:
			break;
			//should be eByEdge
		case FbxGeometryElement::eByEdge:
			//should be eDirect
			for (j = 0; j != mesh->GetMeshEdgeCount(); ++j)
			{
				DisplayInt("        Edge ", j);
				DisplayBool("              Edge visibility: ", leVisibility->GetDirectArray().GetAt(j));
			}

			break;
		}
	}
	DisplayString("");
	*/

	return true;
}

FbxNode* buildMesh(FbxScene* scene, std::vector<Polygon>& polygons, FbxNode* srcNode, const FbxString& name)
{
	FbxMesh* srcMesh = srcNode->GetMesh();
	FbxMesh* dstMesh = FbxMesh::Create(scene, name);

	////////////////////////////////////////////////////////
	// fill position
	dstMesh->InitControlPoints(polygons.size() * NUM_VERTICES_PER_POLYGON);
	FbxVector4* controlPoints = dstMesh->GetControlPoints();
	{
		for (size_t i = 0; i < polygons.size(); i++)
		{
			for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
				controlPoints[i * NUM_VERTICES_PER_POLYGON + k] = polygons[i].vertices[k].position;
		}
	}

	////////////////////////////////////////////////////////
	// fill color
	{
		for (size_t ch = 0; ch < NUM_COLORS; ch++)
		{
			FbxGeometryElementVertexColor* geometryElementVertexColor = dstMesh->CreateElementVertexColor();
			geometryElementVertexColor->SetMappingMode(FbxGeometryElement::eByControlPoint);
			geometryElementVertexColor->SetReferenceMode(FbxGeometryElement::eDirect);

			for (size_t i = 0; i < polygons.size(); i++)
			{
				for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
					geometryElementVertexColor->GetDirectArray().Add(polygons[i].vertices[k].colors[ch]);
			}
		}
	}

	////////////////////////////////////////////////////////
	// fill normal
	{
		for (size_t ch = 0; ch < NUM_NORMALS; ch++)
		{
			FbxGeometryElementNormal* geometryElementNormal = dstMesh->CreateElementNormal();
			geometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
			geometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

			for (size_t i = 0; i < polygons.size(); i++)
			{
				for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
					geometryElementNormal->GetDirectArray().Add(polygons[i].vertices[k].normals[ch]);
			}
		}
	}


	////////////////////////////////////////////////////////
	// fill uv
	{
		for (size_t ch = 0; ch < NUM_UVS; ch++)
		{
			FbxGeometryElementUV* geometryElementUV = dstMesh->CreateElementUV(FbxString("UV") + ((int)ch));
			geometryElementUV->SetMappingMode(FbxGeometryElement::eByControlPoint);
			geometryElementUV->SetReferenceMode(FbxGeometryElement::eDirect);

			for (size_t i = 0; i < polygons.size(); i++)
			{
				for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
					geometryElementUV->GetDirectArray().Add(polygons[i].vertices[k].uvs[ch]);
			}
		}
	}

	////////////////////////////////////////////////////////
	// fill tangent
	{
		for (size_t ch = 0; ch < NUM_TANGENTS; ch++)
		{
			FbxGeometryElementTangent* geometryElementTangent = dstMesh->CreateElementTangent();
			geometryElementTangent->SetMappingMode(FbxGeometryElement::eByControlPoint);
			geometryElementTangent->SetReferenceMode(FbxGeometryElement::eDirect);

			for (size_t i = 0; i < polygons.size(); i++)
			{
				for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
					geometryElementTangent->GetDirectArray().Add(polygons[i].vertices[k].tangents[ch]);
			}
		}
	}

	////////////////////////////////////////////////////////
	// fill binormal
	{
		for (size_t ch = 0; ch < NUM_BINORMALS; ch++)
		{
			FbxGeometryElementBinormal* geometryElementBinormal = dstMesh->CreateElementBinormal();
			geometryElementBinormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
			geometryElementBinormal->SetReferenceMode(FbxGeometryElement::eDirect);

			for (size_t i = 0; i < polygons.size(); i++)
			{
				for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
					geometryElementBinormal->GetDirectArray().Add(polygons[i].vertices[k].binormals[ch]);
			}
		}
	}

	////////////////////////////////////////////////////////
	// fill polygons
	FbxGeometryElementMaterial* geometryElementMaterial = dstMesh->CreateElementMaterial();
	geometryElementMaterial->SetMappingMode(FbxGeometryElement::eByPolygon);
	geometryElementMaterial->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	for (size_t i = 0; i < polygons.size(); i++)
	{
		//we won't use the default way of assigning textures, as we have
		//textures on more than just the default (diffuse) channel.
		dstMesh->BeginPolygon(i);

		for (size_t k = 0; k < NUM_VERTICES_PER_POLYGON; k++)
			dstMesh->AddPolygon(i * NUM_VERTICES_PER_POLYGON + k);

		dstMesh->EndPolygon();
	}

	////////////////////////////////////////////////////////
	// fill dstNode
	FbxNode* dstNode = FbxNode::Create(scene, name);
	dstNode->SetNodeAttribute(dstMesh);

	//CreateMaterials(scene, dstMesh);
	for (size_t i = 0; i < srcMesh->GetNode()->GetMaterialCount(); i++)
	{
		dstMesh->GetNode()->AddMaterial(srcMesh->GetNode()->GetMaterial(i));
	}

	return dstNode;
}

FbxNode* SliceMeshNode(FbxScene* scene, FbxNode* srcNode, bool positive)
{
	FbxMesh* srcMesh = srcNode->GetMesh();

	/*
	srcMesh->ComputeBBox();
	FbxDouble3 a = srcMesh->BBoxMin;
	FbxDouble3 b = srcMesh->BBoxMax;

	int c = 0;
	c = srcMesh->GetElementNormalCount();
	c = srcMesh->GetElementBinormalCount();
	c = srcMesh->GetElementTangentCount();
	c = srcMesh->GetElementMaterialCount();
	c = srcMesh->GetElementPolygonGroupCount();
	c = srcMesh->GetElementVertexColorCount();
	c = srcMesh->GetElementSmoothingCount();
	c = srcMesh->GetElementVertexCreaseCount();
	c = srcMesh->GetElementEdgeCreaseCount();
	c = srcMesh->GetElementHoleCount();
	c = srcMesh->GetElementUserDataCount();
	c = srcMesh->GetElementVisibilityCount();
	c = srcMesh->GetElementUVCount();
	*/

	std::vector<Polygon> polygons;
	CollectPolygons(srcMesh, polygons);

	return buildMesh(scene, polygons, srcNode, srcNode->GetName() + FbxString(positive ? "LeftSlice" : "RightSlice"));
}

void SliceMeshNodes(FbxScene* scene, std::vector<FbxNode* >& nodes)
{
	for (size_t i = 0; i < nodes.size(); i++)
	{
		FbxNode* left = SliceMeshNode(scene, nodes[i], true);
		nodes[i]->AddChild(left);

		FbxNode* right = SliceMeshNode(scene, nodes[i], false);
		nodes[i]->AddChild(right);
	}

	//scene->GetRootNode()->AddChild(CreatePyramidWithMaterials(scene, "1"));
}

bool ValidatePolygon(FbxMesh* mesh)
{
	for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++)
	{
		if (mesh->GetPolygonSize(polygonIndex) != NUM_VERTICES_PER_POLYGON)
		{
			ErrorMsg(FbxString("Number of Vertices in Polygon must be ") + NUM_VERTICES_PER_POLYGON + FbxString(", export FBX with Triangulation"));
			return false;
		}
	}

	return true;
}

bool ValidateMeshNode(FbxScene* scene, FbxNode* node)
{
	FbxMesh* mesh = node->GetMesh();
	if (mesh == nullptr)
		return false;

	return ValidatePolygon(mesh)
		&& (mesh->GetElementVertexColorCount() <= NUM_COLORS)
		&& (mesh->GetElementNormalCount() <= NUM_NORMALS)
		&& (mesh->GetElementBinormalCount() <= NUM_BINORMALS)
		&& (mesh->GetElementTangentCount() <= NUM_TANGENTS)
		&& (mesh->GetElementUVCount() <= NUM_UVS)
		// || (node->GetMesh()->GetElementMaterialCount() != NUM_NORMALS)
		// || (node->GetMesh()->GetElementPolygonGroupCount() != NUM_NORMALS)
		// || (node->GetMesh()->GetElementSmoothingCount() != NUM_NORMALS)

		// || node->GetMesh()->GetElementVertexCreaseCount()
		// || node->GetMesh()->GetElementEdgeCreaseCount()
		// || node->GetMesh()->GetElementHoleCount()
		// || node->GetMesh()->GetElementUserDataCount()
		// || node->GetMesh()->GetElementVisibilityCount()
		// || node->GetMesh()->GetElementUVCount()
		;

	/*
	int GetElementNormalCount() const;
int GetElementBinormalCount() const;
int GetElementTangentCount() const;
int GetElementMaterialCount() const;
int GetElementPolygonGroupCount() const;
int GetElementVertexColorCount() const;
int GetElementSmoothingCount() const;
int GetElementVertexCreaseCount() const;
int GetElementEdgeCreaseCount() const;
int GetElementHoleCount() const;
int GetElementUserDataCount() const;
int GetElementVisibilityCount() const;
int GetElementUVCount(FbxLayerElement::EType pTypeIdentifier=FbxLayerElement::eUnknown) const;
	*/
}

bool ValidateMeshNodes(FbxScene* scene, std::vector<FbxNode* >& nodes)
{
	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (!ValidateMeshNode(scene, nodes[i]))
			return false;
	}

	return true;
}

int main(int argc, const char** argv)
{
	FbxManager* sdkManager = NULL;
	FbxScene* scene = NULL;
	bool lResult;

	Arguments params = processArgs(argc, argv);

	// Prepare the FBX SDK.
	InitializeSdkObjects(sdkManager, scene);

	// Load the scene.
	lResult = LoadScene(sdkManager, scene, params.inputPath);
	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
		return -1;
	}

	std::vector<FbxNode* > meshNodes;
	CollectMeshNode(scene, meshNodes);

	if (!ValidateMeshNodes(scene, meshNodes))
		return false;

	SliceMeshNodes(scene, meshNodes);

	lResult = SaveScene(sdkManager, scene, params.outputPath);
	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while saving the scene...");
		return -1;
	}

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(sdkManager, lResult);

	return 0;
}