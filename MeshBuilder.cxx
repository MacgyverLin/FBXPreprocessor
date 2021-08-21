#include "MeshBuilder.h"

MeshBuilder::MeshBuilder()
{
}

MeshBuilder::~MeshBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder::Copy(const std::vector<Mesh>& meshes, std::vector<MeshArray>& meshArrays)
{
	meshArrays.resize(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshArrays[i].resize(1);
		meshArrays[i][0] = meshes[i];
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder::Build(FbxScene* fbxScene, std::vector<FbxNode*>& fbxNodes, std::vector<Mesh>& meshes)
{
	CollectMeshNode(fbxScene, fbxNodes);

	if (!ValidateMeshNodes(fbxScene, fbxNodes))
		return false;

	if (!BuildMeshes(fbxScene, fbxNodes, meshes))
		return false;

	return true;
}

void MeshBuilder::CollectMeshNode(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes)
{
	int i;
	FbxNode* fbxNode = fbxScene->GetRootNode();

	if (fbxNode)
	{
		for (i = 0; i < fbxNode->GetChildCount(); i++)
		{
			CollectMeshNode(fbxNode->GetChild(i), fbxNodes);
		}
	}
}

void MeshBuilder::CollectMeshNode(FbxNode* fbxNode, std::vector<FbxNode* >& fbxNodes)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if (fbxNode->GetNodeAttribute() == NULL)
	{
		FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (fbxNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		case FbxNodeAttribute::eMesh:
			fbxNodes.push_back(fbxNode);
			break;

		case FbxNodeAttribute::eMarker:
		case FbxNodeAttribute::eSkeleton:
		case FbxNodeAttribute::eNurbs:
		case FbxNodeAttribute::ePatch:
		case FbxNodeAttribute::eCamera:
		case FbxNodeAttribute::eLight:
		case FbxNodeAttribute::eLODGroup:
		default:
			break;
		}
	}

	//DisplayUserProperties(pNode);
	//DisplayTarget(pNode);
	//DisplayPivotsAndLimits(pNode);
	//DisplayTransformPropagation(pNode);
	//DisplayGeometricTransform(pNode);

	for (i = 0; i < fbxNode->GetChildCount(); i++)
	{
		CollectMeshNode(fbxNode->GetChild(i), fbxNodes);
	}
}

/////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder::ValidateMeshNode(FbxScene* fbxScene, FbxNode* fbxNode)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();
	if (fbxMesh == nullptr)
		return false;

	return ValidatePolygon(fbxMesh)
		&& (fbxMesh->GetElementVertexColorCount() <= NUM_COLORS)
		&& (fbxMesh->GetElementNormalCount() <= NUM_NORMALS)
		&& (fbxMesh->GetElementBinormalCount() <= NUM_BINORMALS)
		&& (fbxMesh->GetElementTangentCount() <= NUM_TANGENTS)
		&& (fbxMesh->GetElementUVCount() <= NUM_UVS)
		// || (fbxMesh->GetElementMaterialCount() != NUM_NORMALS)
		// || (fbxMesh->GetElementPolygonGroupCount() != NUM_NORMALS)
		// || (fbxMesh->GetElementSmoothingCount() != NUM_NORMALS)

		// || fbxMesh->GetElementVertexCreaseCount()
		// || fbxMesh->GetElementEdgeCreaseCount()
		// || fbxMesh->GetElementHoleCount()
		// || fbxMesh->GetElementUserDataCount()
		// || fbxMesh->GetElementVisibilityCount()
		// || fbxMesh->GetElementUVCount()
		;
}

bool MeshBuilder::ValidatePolygon(FbxMesh* fbxMesh)
{
	for (int polygonIndex = 0; polygonIndex < fbxMesh->GetPolygonCount(); polygonIndex++)
	{
		if (fbxMesh->GetPolygonSize(polygonIndex) != NUM_VERTICES_PER_POLYGON)
		{
			ErrorMsg("Number of Vertices in Polygon must be %d, try re-export FBX with Triangulation on", NUM_VERTICES_PER_POLYGON);
			return false;
		}
	}

	return true;
}

bool MeshBuilder::ValidateMeshNodes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes)
{
	for (size_t i = 0; i < fbxNodes.size(); i++)
	{
		if (!ValidateMeshNode(fbxScene, fbxNodes[i]))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder::BuildMeshes(FbxScene* fbxScene, std::vector<FbxNode* >& fbxNodes, std::vector<Mesh >& meshes)
{
	meshes.resize(fbxNodes.size());

	for (size_t i = 0; i < fbxNodes.size(); i++)
	{
		if (!BuildMesh(fbxNodes[i], meshes[i]))
			return false;
	}

	return true;
}

bool MeshBuilder::BuildMaterial(FbxNode* fbxNode, int polygonIndex, int& materialIdx)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	for (int materialIndex = 0; materialIndex < fbxMesh->GetElementMaterialCount(); materialIndex++)
	{
		FbxGeometryElementMaterial* leMaterial = fbxMesh->GetElementMaterial(materialIndex);
		switch (leMaterial->GetMappingMode())
		{
		case FbxGeometryElement::eByPolygon:
		case FbxGeometryElement::eAllSame:
		{
			switch (leMaterial->GetReferenceMode())
			{
			case FbxGeometryElement::eIndexToDirect:
			case FbxGeometryElement::eDirect:
			{
				materialIdx = leMaterial->GetIndexArray().GetAt(polygonIndex);
			}
			break;

			case FbxGeometryElement::eIndex:
			default:
				break; // other reference modes not shown here!
			}
		}
		break;

		case FbxGeometryElement::eNone:
		case FbxGeometryElement::eByControlPoint:
		case FbxGeometryElement::eByPolygonVertex:
		case FbxGeometryElement::eByEdge:
		default:
			// any other mapping modes don't make sense
			// DisplayString("        \"unsupported group assignment\"");

			ErrorMsg("unsupported material idx assignment");
			return false;
		}
	}

	return true;
}

bool MeshBuilder::BuildPosition(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId)
{
	assert(lControlPointIndex >= 0);
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	FbxVector4* lControlPoints = fbxMesh->GetControlPoints();
	vertex.position = lControlPoints[lControlPointIndex];

	return true;
}

bool MeshBuilder::BuildColor(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId)
{
	assert(lControlPointIndex >= 0);
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	int colorChannelCount = std::min(fbxMesh->GetElementVertexColorCount(), NUM_COLORS);
	for (int layer = 0; layer < colorChannelCount; layer++)
	{
		FbxGeometryElementVertexColor* leVtxc = fbxMesh->GetElementVertexColor(layer);
		// FBXSDK_sprintf(header, 100, "            Color vertex: ");

		Color& color = vertex.colors[layer];

		switch (leVtxc->GetMappingMode())
		{
			case FbxGeometryElement::eByControlPoint:
			{
				switch (leVtxc->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						// DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));

						color = leVtxc->GetDirectArray().GetAt(lControlPointIndex);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
						//DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));

						int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
						color = leVtxc->GetDirectArray().GetAt(id);
					}
					break;

					case FbxGeometryElement::eIndex:
					default:
					{
						ErrorMsg("unsupported leVtxc->GetReferenceMode() in %s, line: %s\n", __FILE__, __LINE__);
					}
					return false;
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
						color = leVtxc->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leVtxc->GetIndexArray().GetAt(vertexId);
						//DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));

						int id = leVtxc->GetIndexArray().GetAt(vertexId);
						color = leVtxc->GetDirectArray().GetAt(id);
					}
					break;

					case FbxGeometryElement::eIndex:
					default:
					{
						ErrorMsg("unsupported leVtxc->GetReferenceMode() in %s, line: %s\n", __FILE__, __LINE__);
					}
					return false;
				}
			}
			break;

			case FbxGeometryElement::eNone:
			case FbxGeometryElement::eByPolygon:
			case FbxGeometryElement::eByEdge:
			case FbxGeometryElement::eAllSame:
			default:
			{
				ErrorMsg("unsupported leVtxc->GetMappingMode() in %s, line: %s\n", __FILE__, __LINE__);
			}
			return false;
		}
	}

	return true;
}

bool MeshBuilder::BuildUV(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int polygonIndex, int polygonVertexIndex)
{
	assert(lControlPointIndex >= 0);
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	int uvChannelCount = std::min(fbxMesh->GetElementUVCount(), NUM_UVS);
	for (int layer = 0; layer < uvChannelCount; ++layer)
	{
		FbxGeometryElementUV* leUV = fbxMesh->GetElementUV(layer);
		// FBXSDK_sprintf(header, 100, "            Texture UV: ");

		Vector2& uv = vertex.uvs[layer];

		switch (leUV->GetMappingMode())
		{
			case FbxGeometryElement::eByControlPoint:
			{
				switch (leUV->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						// Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));

						uv = leUV->GetDirectArray().GetAt(lControlPointIndex);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						// int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						// Display2DVector(header, leUV->GetDirectArray().GetAt(id));

						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						uv = leUV->GetDirectArray().GetAt(id);
					}
					break;

					case FbxGeometryElement::eIndex:
					default:
					{
						ErrorMsg("unsupported leUV->GetReferenceMode() in %s, line: %s\n", __FILE__, __LINE__);
					}
					return false;
				}
			}
			break;

			case FbxGeometryElement::eByPolygonVertex:
			{
				int lTextureUVIndex = fbxMesh->GetTextureUVIndex(polygonIndex, polygonVertexIndex);
				switch (leUV->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						// Display2DVector(header, leUV->GetDirectArray().GetAt(lTextureUVIndex));

						uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);
					}
					break;

					case FbxGeometryElement::eIndex:
					default:
					{
						ErrorMsg("unsupported leUV->GetReferenceMode() in %s, line: %s\n", __FILE__, __LINE__);
					}
					return false;
				}
			}
			break;

			case FbxGeometryElement::eNone:
			case FbxGeometryElement::eByPolygon:
			case FbxGeometryElement::eByEdge:
			case FbxGeometryElement::eAllSame:
			default:
			{
				ErrorMsg("unsupported leUV->GetMappingMode() in %s, line: %s\n", __FILE__, __LINE__);
			}
			return false;
		}
	}

	return true;
}

bool MeshBuilder::BuildNormal(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId)
{
	assert(lControlPointIndex >= 0);
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	int normalChannelCount = std::min(fbxMesh->GetElementNormalCount(), NUM_NORMALS);
	for (int layer = 0; layer < normalChannelCount; ++layer)
	{
		FbxGeometryElementNormal* leNormal = fbxMesh->GetElementNormal(layer);
		// FBXSDK_sprintf(header, 100, "            Normal: ");

		Vector3& normal = vertex.normals[layer];

		switch (leNormal->GetMappingMode())
		{
			case FbxGeometryElement::eByPolygonVertex:
			{
				switch (leNormal->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));

						normal = leNormal->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						// int id = leNormal->GetIndexArray().GetAt(vertexId);
						// Display3DVector(header, leNormal->GetDirectArray().GetAt(id));

						int id = leNormal->GetIndexArray().GetAt(vertexId);
						normal = leNormal->GetDirectArray().GetAt(id);
					}
					break;

					case FbxGeometryElement::eIndex:
					default:
					{
						ErrorMsg("unsupported leNormal->GetReferenceMode() in %s, line: %s\n", __FILE__, __LINE__);
					}
					return false;
				}
			}
			break;

			case FbxGeometryElement::eNone:
			case FbxGeometryElement::eByControlPoint:
			case FbxGeometryElement::eByPolygon:
			case FbxGeometryElement::eByEdge:
			case FbxGeometryElement::eAllSame:
			default:
			{
				ErrorMsg("unsupported leNormal->GetMappingMode() in %s, line: %s\n", __FILE__, __LINE__);
			}
			return false;
		}
	}

	return true;
}

bool MeshBuilder::BuildTangent(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId)
{
	assert(lControlPointIndex >= 0);
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	int tangentChannelCount = std::min(fbxMesh->GetElementTangentCount(), NUM_TANGENTS);
	for (int layer = 0; layer < tangentChannelCount; ++layer)
	{
		FbxGeometryElementTangent* leTangent = fbxMesh->GetElementTangent(layer);
		// FBXSDK_sprintf(header, 100, "            Tangent: ");

		Vector3& tangent = vertex.tangents[layer];

		switch (leTangent->GetMappingMode())
		{
			case FbxGeometryElement::eByPolygonVertex:
			{
				switch (leTangent->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leTangent->GetDirectArray().GetAt(vertexId));

						tangent = leTangent->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leTangent->GetIndexArray().GetAt(vertexId);
						//Display3DVector(header, leTangent->GetDirectArray().GetAt(id));

						int id = leTangent->GetIndexArray().GetAt(vertexId);
						tangent = leTangent->GetDirectArray().GetAt(id);
					}
					break;

					case FbxGeometryElement::eIndex:
					default:
					{
						ErrorMsg("unsupported leTangent->GetReferenceMode() in %s, line: %s\n", __FILE__, __LINE__);
					}
					return false;
				}
			}
			break;

			case FbxGeometryElement::eNone:
			case FbxGeometryElement::eByControlPoint:
			case FbxGeometryElement::eByPolygon:
			case FbxGeometryElement::eByEdge:
			case FbxGeometryElement::eAllSame:
			default:
			{
				ErrorMsg("unsupported leTangent->GetMappingMode() in %s, line: %s\n", __FILE__, __LINE__);
			}
			return false;
		}
	}

	return true;
}

bool MeshBuilder::BuildBinormal(FbxNode* fbxNode, Mesh& mesh, Vertex& vertex, int lControlPointIndex, int vertexId)
{
	assert(lControlPointIndex >= 0);
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	int binormalChannelCount = std::min(fbxMesh->GetElementBinormalCount(), NUM_BINORMALS);
	for (int layer = 0; layer < binormalChannelCount; ++layer)
	{
		FbxGeometryElementBinormal* leBinormal = fbxMesh->GetElementBinormal(layer);

		Vector3& binormal = vertex.binormals[layer];

		// FBXSDK_sprintf(header, 100, "            Binormal: ");
		switch (leBinormal->GetMappingMode())
		{
			case FbxGeometryElement::eByPolygonVertex:
			{
				switch (leBinormal->GetReferenceMode())
				{
					case FbxGeometryElement::eDirect:
					{
						// Display3DVector(header, leBinormal->GetDirectArray().GetAt(vertexId));

						binormal = leBinormal->GetDirectArray().GetAt(vertexId);
					}
					break;

					case FbxGeometryElement::eIndexToDirect:
					{
						//int id = leBinormal->GetIndexArray().GetAt(vertexId);
						//Display3DVector(header, leBinormal->GetDirectArray().GetAt(id));

						int id = leBinormal->GetIndexArray().GetAt(vertexId);
						binormal = leBinormal->GetDirectArray().GetAt(id);
					}
					break;

					case FbxGeometryElement::eIndex:
					default:
					{
						ErrorMsg("unsupported leBinormal->GetReferenceMode() in %s, line: %s\n", __FILE__, __LINE__);
					}
					return false;
				}
			}
			break;

			case FbxGeometryElement::eNone:
			case FbxGeometryElement::eByControlPoint:
			case FbxGeometryElement::eByPolygon:
			case FbxGeometryElement::eByEdge:
			case FbxGeometryElement::eAllSame:
			default:
			{
				ErrorMsg("unsupported leBinormal->GetMappingMode() in %s, line: %s\n", __FILE__, __LINE__);
			}
			return false;
		}
	}

	return true;
}

bool MeshBuilder::BuildMesh(FbxNode* fbxNode, Mesh& mesh)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();
	fbxMesh->ComputeBBox();
	FbxDouble3 aabbMin = fbxMesh->BBoxMin;
	FbxDouble3 aabbMax = fbxMesh->BBoxMax;

	int colorChannelCount = std::min(fbxMesh->GetElementVertexColorCount(), NUM_COLORS);
	int uvChannelCount = std::min(fbxMesh->GetUVLayerCount(), NUM_UVS);
	int normalChannelCount = std::min(fbxMesh->GetElementNormalCount(), NUM_NORMALS);
	int tangentChannelCount = std::min(fbxMesh->GetElementTangentCount(), NUM_TANGENTS);
	int binormalChannelCount = std::min(fbxMesh->GetElementBinormalCount(), NUM_BINORMALS);

	mesh.Begin(colorChannelCount, uvChannelCount, normalChannelCount, tangentChannelCount, binormalChannelCount);

	int vertexId = 0;
	for (int polygonIndex = 0; polygonIndex < fbxMesh->GetPolygonCount(); polygonIndex++)
	{
		int materialIdx = 0;
		if (!BuildMaterial(fbxNode, polygonIndex, materialIdx))
			return false;

		Polygon polygon;
		polygon.Begin(materialIdx);

		for (int polygonVertexIndex = 0; polygonVertexIndex < fbxMesh->GetPolygonSize(polygonIndex); polygonVertexIndex++)
		{
			Vertex vertex;

			int lControlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
			if (lControlPointIndex < 0)
			{
				// DisplayString("            Coordinates: Invalid index found!");
				// continue;
				return false;
			}

			////////////////////////////////////////////////
			// collect position
			if (!BuildPosition(fbxNode, mesh, vertex, lControlPointIndex, vertexId))
				return false;

			////////////////////////////////////////////////
			// collect color
			if (!BuildColor(fbxNode, mesh, vertex, lControlPointIndex, vertexId))
				return false;

			/////////////////////////////////
			// collect uv
			if (!BuildUV(fbxNode, mesh, vertex, lControlPointIndex, polygonIndex, polygonVertexIndex))
				return false;

			/////////////////////////////////
			// collect normal
			if (!BuildNormal(fbxNode, mesh, vertex, lControlPointIndex, vertexId))
				return false;

			/////////////////////////////////
			// collect tangent
			if (!BuildTangent(fbxNode, mesh, vertex, lControlPointIndex, vertexId))
				return false;

			/////////////////////////////////
			// collect binormal
			if (!BuildBinormal(fbxNode, mesh, vertex, lControlPointIndex, vertexId))
				return false;

			polygon.Add(vertex);

			vertexId++;
		}
		polygon.End();

		mesh.Add(polygon);
	}

	//mesh.SetAABB(AABB(Vector3((float)aabbMin[0], (float)aabbMin[1], (float)aabbMin[2]), Vector3((float)aabbMax[0], (float)aabbMax[1], (float)aabbMax[2])));
	mesh.End();

	/*
	//check visibility for the edges of the fbxMesh
	for (int l = 0; l < fbxMesh->GetElementVisibilityCount(); ++l)
	{
		FbxGeometryElementVisibility* leVisibility = fbxMesh->GetElementVisibility(l);
		FBXSDK_sprintf(header, 100, "    Edge Visibility : ");
		DisplayString(header);
		switch (leVisibility->GetMappingMode())
		{
		default:
			break;
			//should be eByEdge
		case FbxGeometryElement::eByEdge:
			//should be eDirect
			for (j = 0; j != fbxMesh->GetMeshEdgeCount(); ++j)
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