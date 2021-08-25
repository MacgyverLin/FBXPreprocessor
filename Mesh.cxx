#include "Mesh.h"
#include "Common.h"
#include <map>
#include <algorithm>
//////////////////////////////////////////////////////
Mesh::Mesh(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
	: polygons()
	, verticesOptimizer()
	, edgeVertexOptimizer()
{
	Clear(colorChannelCount_, uvChannelCount_, normalChannelCount_, tangentChannelCount_, binormalChannelCount_);
}

Mesh::~Mesh()
{
}

Mesh::Mesh(const Mesh& other)
{
	isClosed = other.isClosed;

	maxGroupIdx = other.maxMaterialIdx;
	maxMaterialIdx = other.maxMaterialIdx;
	verticesCount = other.verticesCount;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;

	polygons = other.polygons;
	verticesOptimizer = other.verticesOptimizer;
	edgeVertexOptimizer = other.edgeVertexOptimizer;
}

Mesh& Mesh::operator = (const Mesh& other)
{
	isClosed = other.isClosed;

	maxGroupIdx = other.maxMaterialIdx;
	maxMaterialIdx = other.maxMaterialIdx;
	verticesCount = other.verticesCount;

	colorChannelCount = other.colorChannelCount;
	uvChannelCount = other.uvChannelCount;
	normalChannelCount = other.normalChannelCount;
	tangentChannelCount = other.tangentChannelCount;
	binormalChannelCount = other.binormalChannelCount;

	aabb = other.aabb;

	polygons = other.polygons;
	verticesOptimizer = other.verticesOptimizer;
	edgeVertexOptimizer = other.edgeVertexOptimizer;

	return *this;
}

bool Mesh::IsClosed() const
{
	return isClosed;
}

int Mesh::GetMaxGroupIdx() const
{
	return maxGroupIdx;
}

int Mesh::GetMaxMaterialIdx() const
{
	return maxMaterialIdx;
}

int Mesh::GetVerticesCount() const
{
	return verticesCount;
}

int Mesh::GetColorChannelCount() const
{
	return colorChannelCount;
}

int Mesh::GetUVChannelCount() const
{
	return uvChannelCount;
}

int Mesh::GetNormalChannelCount() const
{
	return normalChannelCount;
}

int Mesh::GetTangentChannelCount() const
{
	return tangentChannelCount;
}

int Mesh::GetBinormalChannelCount() const
{
	return binormalChannelCount;
}

const AABB& Mesh::GetAABB() const
{
	return aabb;
}

size_t Mesh::GetPolygonCount() const
{
	return polygons.size();
}

int Mesh::GetPolygonMaterialIdx(size_t polyIdx) const
{
	return polygons[polyIdx].GetMaterialIdx();
}

int Mesh::GetPolygonEdgesCount(size_t polyIdx) const
{
	return polygons[polyIdx].GetEdgesCount();
}

const Edge& Mesh::GetPolygonEdge(size_t polyIdx, size_t edgeIdx) const
{
	return polygons[polyIdx].GetEdge(edgeIdx);
}

const Vertex& Mesh::GetPolygonVertex(size_t polyIdx, size_t vertexIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, vertexIdx);

	return verticesOptimizer.GetData(edge.GetVertexIdx());
}

const Vector3& Mesh::GetPolygonEdgeStartPosition(size_t polyIdx, size_t edgeIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, edgeIdx);

	return edgeVertexOptimizer.GetData(edge.GetStartIdx());
}

const Vector3& Mesh::GetPolygonEdgeEndPosition(size_t polyIdx, size_t edgeIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, edgeIdx);

	return edgeVertexOptimizer.GetData(edge.GetEndIdx());
}

const int& Mesh::GetPolygonEdgeAdjacentPolygonIdx(size_t polyIdx, size_t edgeIdx) const
{
	const Edge& edge = GetPolygonEdge(polyIdx, edgeIdx);

	return edge.GetAdjacentPolygonIdx();
}

const int& Mesh::GetPolygonGroupID(size_t polyIdx) const
{
	return polygons[polyIdx].GetGroupID();
}

const Plane& Mesh::GetPolygonPlane(size_t polyIdx) const
{
	return polygons[polyIdx].GetPlane();
}

const AABB& Mesh::GetPolygonAABB(size_t polyIdx) const
{
	return polygons[polyIdx].GetAABB();
}

void Mesh::SetPolygonEdgeAdjacentPolygonIdx(size_t polyIdx, size_t edgeIdx, size_t adjacentPolygonIdx)
{
	return polygons[polyIdx].SetEdgeAdjacentPolygonIdx(edgeIdx, adjacentPolygonIdx);
}

void Mesh::SetPolygonGroupID(size_t polyIdx, int groupID)
{
	if (maxGroupIdx < groupID)
		maxGroupIdx = groupID;

	return polygons[polyIdx].SetGroupID(groupID);
}

void Mesh::Begin(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
{
	isClosed = false;

	maxGroupIdx = 0;
	maxMaterialIdx = 0;
	verticesCount = 0;

	colorChannelCount = colorChannelCount_;
	uvChannelCount = uvChannelCount_;
	normalChannelCount = normalChannelCount_;
	tangentChannelCount = tangentChannelCount_;
	binormalChannelCount = binormalChannelCount_;

	aabb = AABB();

	polygons.clear();
	verticesOptimizer.Clear();
	edgeVertexOptimizer.Clear();
}

void Mesh::BeginPolygon(int groupIdx_, int materialIdx_)
{
	if (maxGroupIdx < groupIdx_)
		maxGroupIdx = groupIdx_;

	if (maxMaterialIdx < materialIdx_)
		maxMaterialIdx = materialIdx_;

	polygons.push_back(Polygon());
	polygons.back().Begin(groupIdx_, materialIdx_);
}

void Mesh::AddPolygonVertices(const std::vector<Vertex>& vertices)
{
	for (auto& v : vertices)
	{
		AddPolygonVertex(v);
	}
}

void Mesh::AddPolygonVertex(const Vertex& vertex)
{
	Polygon& polygon = polygons.back();

	polygon.Add
	(
		verticesOptimizer.Add(vertex),
		edgeVertexOptimizer.Add(vertex.position),
		-1
	);

	verticesCount++;
}

void Mesh::EndPolygon()
{
	Polygon& polygon = polygons.back();

	polygon.End(verticesOptimizer, edgeVertexOptimizer);
}

void Mesh::End()
{
	SortPolygonsByMaterialIdx();

	ComputeAABB();

	isClosed = ComputePolygonsAdjacency();

	Slice(Plane());
}

void Mesh::SortPolygonsByMaterialIdx()
{
	std::sort(polygons.begin(), polygons.end(), [&](const Polygon& p0, const Polygon& p1) -> bool { return p0.GetMaterialIdx() < p1.GetMaterialIdx();  });
}

void Mesh::ComputeAABB()
{
	if (polygons.size() == 0)
	{
		aabb = AABB(Vector3::Zero, Vector3::Zero);
	}
	else
	{
		aabb = AABB(polygons[0].GetAABB());
		for (size_t i = 0; i < polygons.size(); i++)
			aabb += polygons[i].GetAABB();
	}
}

#include <experimental/map>
bool Mesh::ComputeClosed()
{
	std::map<int, int> isolatedEdges;

	DataOptimizer<Edge> edgeOptimizer;
	for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
	{
		for (size_t edgeIdx = 0; edgeIdx < GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Edge edge = GetPolygonEdge(polyIdx, edgeIdx);
			bool normalized = edge.IsNormalized();
			if (!normalized)
				edge.Flip();

			int edgeId = edgeOptimizer.Add(edge);
			if (normalized)
				isolatedEdges[edgeId] += 1;
			else
				isolatedEdges[edgeId] -= 1;
		}
	}

	std::experimental::erase_if(isolatedEdges, [](const auto& item) { return item.second == 0; });

	if (isolatedEdges.size() == 0)
		Debug::Verbose("Mesh is closed\n");

	return isolatedEdges.size() == 0;
}

bool Mesh::ComputePolygonsAdjacency()
{
	return ComputePolygonsAdjacency(std::bind(&Mesh::SetPolygonEdgeAdjacentPolygonIdx, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

bool Mesh::ComputePolygonsAdjacency(std::function<void(int, int, int)> setAdjacentCB)
{
	bool meshClosed = true;

	std::map<int, int> postiiveSidePolygons;
	std::map<int, int> negativeSidePolygons;
	DataOptimizer<Edge> edgeOptimizer;
	for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
	{
		for (size_t edgeIdx = 0; edgeIdx < GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Edge edge = GetPolygonEdge(polyIdx, edgeIdx);
			bool normalized = edge.IsNormalized();
			if (!normalized)
				edge.Flip();

			int edgeId = edgeOptimizer.Add(edge);
			if (normalized)
				postiiveSidePolygons[edgeId] = polyIdx;
			else
				negativeSidePolygons[edgeId] = polyIdx;
		}
	}


	for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
	{
		for (size_t edgeIdx = 0; edgeIdx < GetPolygonEdgesCount(polyIdx); edgeIdx++)
		{
			Edge edge = GetPolygonEdge(polyIdx, edgeIdx);
			bool normalized = edge.IsNormalized();
			if (!normalized)
				edge.Flip();

			int edgeId = edgeOptimizer.GetIndex(edge);
			if (normalized)
			{
				std::map<int, int>::iterator itr = negativeSidePolygons.find(edgeId);
				if (itr != negativeSidePolygons.end())
				{
					setAdjacentCB(polyIdx, edgeIdx, itr->second);
					// SetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx, itr->second);
				}
			}
			else
			{
				std::map<int, int>::iterator itr = postiiveSidePolygons.find(edgeId);
				if (itr != postiiveSidePolygons.end())
				{
					setAdjacentCB(polyIdx, edgeIdx, itr->second);
					// SetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx, itr->second);
				}
			}

			Debug::Info("%d\n", GetPolygonEdge(polyIdx, edgeIdx).GetAdjacentPolygonIdx());

			if (GetPolygonEdge(polyIdx, edgeIdx).GetAdjacentPolygonIdx() == -1)
			{
				meshClosed = false;
			}
		}
	}

	if (meshClosed)
		Debug::Verbose("Mesh is closed\n");

	return meshClosed;
}

void Mesh::Slice(const Plane& plane)
{
	///////////////////////////////////////////////////
	// assume mesh is closed

	// group
	Vector3 center = GetAABB().GetCenter(true);
	//Vector3 normal = GetAABB().GetMajorAxis(true);
	Vector3 normal = Vector3::UnitY;
	Plane cutPlane(center, normal);

	for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
	{
		Vector3 polyCenter = GetPolygonAABB(polyIdx).GetCenter();

		if (cutPlane.WhichSide(polyCenter))
		{
			SetPolygonGroupID(polyIdx, 0);
		}
		else
		{
			SetPolygonGroupID(polyIdx, 1);
		}
	}


	for (int groupID = 0; groupID <= GetMaxGroupIdx(); groupID++)
	{
		Debug::Verbose("Polygon Group=%d\n", groupID);
		Debug::Verbose("{\n");

		std::multimap<int, Edge> isolatedEdges;
		{
			// if polygon neighbour ID is not myself, save the edge in the map
			for (size_t polyIdx = 0; polyIdx < GetPolygonCount(); polyIdx++)
			{
				const Polygon& polygon = polygons[polyIdx];
				if (groupID != GetPolygonGroupID(polyIdx))
					continue;

				for (size_t edgeIdx = 0; edgeIdx < GetPolygonEdgesCount(polyIdx); edgeIdx++)
				{
					int polygonEdgeAdjacentPolygonIdx = GetPolygonEdgeAdjacentPolygonIdx(polyIdx, edgeIdx);
					const Polygon& adjacentPolygon = polygons[polygonEdgeAdjacentPolygonIdx];

					if (polygon.GetGroupID() != adjacentPolygon.GetGroupID())
					{
						const Edge& edge = GetPolygonEdge(polyIdx, edgeIdx);
						isolatedEdges.insert(std::pair<int, Edge>(edge.GetStartIdx(), edge));
					}
				}
			}
		}

		typedef std::vector<Edge> Loop;
		std::vector<Loop> loops;
		{
			bool newLoop = true;

			std::multimap<int, Edge>::iterator currentEdge = isolatedEdges.begin();
			while (isolatedEdges.size())
			{
				if (newLoop)
				{
					loops.push_back(Loop());
					newLoop = false;
				}

				Loop& currentLoop = loops.back();

				currentLoop.push_back(currentEdge->second);
				isolatedEdges.erase(currentEdge);

				if (currentLoop.back().GetEndIdx() == currentLoop.front().GetStartIdx()) // loop itself
				{
					newLoop = true;
					currentEdge = isolatedEdges.begin();
				}
				else
				{
					int nextIdx = currentLoop.back().GetEndIdx();
					currentEdge = isolatedEdges.find(currentLoop.back().GetEndIdx());
				}
			}
		}

		for (auto& loop : loops)
		{
			Debug::Verbose("\tLoop\n");
			Debug::Verbose("\t{\n");
			for (int i = 0; i < loop.size(); i++)
			{
				Debug::Verbose("\t\t(%d,%d)", loop[i].GetStartIdx(), loop[i].GetEndIdx());

				if (i == 0)
				{
					if (loop[loop.size() - 1].GetEndIdx() == loop[0].GetStartIdx())
						Debug::Verbose(" *");
				}
				else
				{
					if (loop[i - 1].GetEndIdx() == loop[i].GetStartIdx())
						Debug::Verbose(" *");
				}

				Debug::Verbose("  \n");
			}
			Debug::Verbose("\t}\n");
		}

		Debug::Verbose("}\n\n");

		/*
		for (auto& loop : loops)
		{
			BeginPolygon(GetMaxGroupIdx()+1, GetMaxMaterialIdx() + 1);
			for (int i = 0; i < loop.size(); i++)
			{
				const Edge& edge = loop[i];

				const Vertex& vertex = verticesOptimizer.GetData(edge.GetVertexIdx());
				AddPolygonVertex(vertex);
			}
			EndPolygon();
		}
		*/
	}
}

#if 0
	void Mesh::UpdateIsolatedEdges()
	{
		Debug::Info("===================================================\n");
		class EdgeVertex
		{
		public:
			EdgeVertex(const Edge& edge_, const Vertex& v0_, const Vertex& v1_)
			{
				edge = edge_;
				v0 = v0_;
				v1 = v1_;
			}

			void Flip()
			{
				edge.Flip();
				std::swap(v0, v1);
			}

			void Print(const char* tab, int i)
			{
				Debug::Info("%sEdge %d:(%d->%d), (%.4f, %.4f, %.4f)->(%.4f, %.4f, %.4f)", tab, i, edge.StartIdx(), edge.EndIdx(),
					v0.position.X(), v0.position.Y(), v0.position.Z(),
					v1.position.X(), v1.position.Y(), v1.position.Z());
			}

			Edge edge;
			Vertex v0;
			Vertex v1;
		};

		std::vector<EdgeVertex> isolatedEdges;
		for (size_t i = 0; i < polygons.size(); i++)
		{
			const Polygon& polygon = polygons[i];
			for (size_t j = 0; j < polygon.GetVerticesCount(); j++)
			{
				const Adjacency& adjacency = polygon.GetAdjacency(j);

				bool notPairedEdge = ((adjacency.PolygonIdx() == -1) || (polygons[i].GetGroupIdx() != polygons[adjacency.PolygonIdx()].GetGroupIdx()));
				if (notPairedEdge)
				{
					bool flipped = adjacency.EdgeFlipped();

					const Vertex& v0 = polygon.GetVertex((j + 0) % polygon.GetVerticesCount());
					const Vertex& v1 = polygon.GetVertex((j + 1) % polygon.GetVerticesCount());

					EdgeVertex edge
					(
						edgeOptimizer.GetData(adjacency.EdgeIndex()),
						flipped ? v0 : v1,
						flipped ? v1 : v0
					);
					if (adjacency.EdgeFlipped())
						edge.Flip();

					isolatedEdges.push_back(edge);
				}
			}
		}

		typedef std::vector<EdgeVertex> Loop;
		std::vector<Loop> loops;
		if (isolatedEdges.size())
		{
			Debug::Info("Mesh with isolated Edges\n");
			for (size_t i = 0; i < isolatedEdges.size(); i++)
			{
				isolatedEdges[i].Print("\t", i);
				Debug::Info("\n");
			}

			bool newLoop = true;

			int i = 0;
			do
			{
				if (newLoop)
				{
					loops.push_back(Loop());
					newLoop = false;
					Debug::Info("\tBegin Loop %d\n", (int)loops.size());
				}

				Loop& currentLoop = loops.back();
				if (currentLoop.empty() || (isolatedEdges[i].edge.StartIdx() == currentLoop.back().edge.EndIdx()))
				{
					currentLoop.push_back(isolatedEdges[i]);

					isolatedEdges[i].Print("\t\t", (int)(currentLoop.size() - 1));

					if (currentLoop.size() > 1)
					{
						if (currentLoop.back().edge.StartIdx() == currentLoop[currentLoop.size() - 2].edge.EndIdx())
							Debug::Info(" *");
					}
					Debug::Info("\n");

					isolatedEdges.erase(isolatedEdges.begin() + i);

					if (currentLoop.front().edge.StartIdx() == currentLoop.back().edge.EndIdx())
					{
						Debug::Info("\tEnd Loop %d\n", (int)(loops.size()));
						newLoop = true;
					}
				}
				else
				{
					i++;
				}

				if (i >= isolatedEdges.size())
					i = 0;
			} while (isolatedEdges.size());
		}
		else
		{
			Debug::Info("Mesh without isolated Edge\n");
		}
	}

	void Mesh::Test()
	{
		std::vector<int> counters(edgeOptimizer.GetDataCount());
		for (size_t i = 0; i < polygons.size(); i++)
		{
			const Polygon& polygon = polygons[i];
			for (auto& adjacency : polygon.GetAdjacencies())
				counters[adjacency.EdgeIndex()] += (adjacency.EdgeFlipped() ? 1 : -1);
		}

		Debug::Info("================================================\n");
		std::vector<Edge> isolatedEdges;
		for (size_t i = 0; i < counters.size(); i++)
		{
			if (counters[i] != 0)
			{
				Edge edge = edgeOptimizer.GetData(i);
				if (counters[i] < 0)
					edge.Flip();

				isolatedEdges.push_back(edge);
			}
		}

		typedef std::vector<Edge> Loop;
		std::vector<Loop> loops;
		if (isolatedEdges.size())
		{
			Debug::Info("Mesh with isolated Edges\n");
			for (size_t i = 0; i < isolatedEdges.size(); i++)
			{
				Debug::Info("\tEdge %d:(%d->%d)\n", i, isolatedEdges[i].StartIdx(), isolatedEdges[i].EndIdx());
			}

			bool newLoop = true;

			int i = 0;
			do
			{
				if (newLoop)
				{
					loops.push_back(Loop());
					newLoop = false;
					Debug::Info("\tBegin Loop %d\n", loops.size());
				}

				Loop& currentLoop = loops.back();
				if (currentLoop.empty() || (isolatedEdges[i].StartIdx() == currentLoop.back().EndIdx()))
				{
					currentLoop.push_back(isolatedEdges[i]);
					Debug::Info("\t\tEdge %d:(%d %d)", currentLoop.size() - 1, isolatedEdges[i].StartIdx(), isolatedEdges[i].EndIdx());
					if (currentLoop.size() > 1)
					{
						if (currentLoop.back().StartIdx() == currentLoop[currentLoop.size() - 2].EndIdx())
							Debug::Info(" *");
					}
					Debug::Info("\n");

					isolatedEdges.erase(isolatedEdges.begin() + i);

					if (currentLoop.front().StartIdx() == currentLoop.back().EndIdx())
					{
						Debug::Info("\tEnd Loop %d\n", loops.size());
						newLoop = true;
					}
				}
				else
				{
					i++;
				}

				if (i >= isolatedEdges.size())
					i = 0;
			} while (isolatedEdges.size());
		}
		else
		{
			Debug::Info("Mesh without isolated Edge\n");
		}
	}
#endif

	void Mesh::Clear(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
	{
		isClosed = false;

		maxGroupIdx = 0;
		maxMaterialIdx = 0;
		verticesCount = 0;

		colorChannelCount = colorChannelCount_;
		uvChannelCount = uvChannelCount_;
		normalChannelCount = normalChannelCount_;
		tangentChannelCount = tangentChannelCount_;
		binormalChannelCount = binormalChannelCount_;
		aabb = AABB(Vector3::Zero, Vector3::Zero);

		polygons.clear();
		verticesOptimizer.Clear();
		edgeVertexOptimizer.Clear();
	}

	void Mesh::Flip()
	{
		for (size_t i = 0; i < polygons.size(); i++)
			polygons[i].Flip();
	}

	bool Mesh::IsEmpty() const
	{
		return polygons.size() == 0;
	}