#include "Mesh.h"
#include "BSP.h"
#include <algorithm>

//////////////////////////////////////////////////////
Mesh::Mesh(int colorChannelCount_, int uvChannelCount_, int normalChannelCount_, int tangentChannelCount_, int binormalChannelCount_)
	: polygons()
	, positionOptimizer()
	, edgeOptimizer()
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
	positionOptimizer = other.positionOptimizer;
	edgeOptimizer = other.edgeOptimizer;
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
	positionOptimizer = other.positionOptimizer;
	edgeOptimizer = other.edgeOptimizer;

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

const Polygon& Mesh::GetPolygon(int i) const
{
	return polygons[i];
}

const std::vector<Polygon>& Mesh::GetPolygons() const
{
	return polygons;
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
	positionOptimizer.Clear();
	edgeOptimizer.Clear();
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

void Mesh::BeginPolygons(const std::vector<Polygon>& polys)
{
	for (size_t i = 0; i < polys.size(); i++)
	{
		BeginPolygon(polys[i]);
		if (i != polys.size() - 1)
			EndPolygon();
	}
}

void Mesh::BeginPolygon(const Polygon& polygon)
{
	BeginPolygon(polygon.GetGroupIdx(), polygon.GetMaterialIdx());

	AddPolygonVertices(polygon.GetVertices());
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

	polygon.Add(vertex);

	verticesCount++;
}

void Mesh::EndPolygon()
{
	Polygon& polygon = polygons.back();

	polygon.End(positionOptimizer, edgeOptimizer);
}

void Mesh::End()
{
	SortPolygonsByMaterialIdx();

	UpdateAABB();

	isClosed = UpdatePolygonsAdjacency();

	UpdateIsolatedEdges();
}

void Mesh::SortPolygonsByMaterialIdx()
{
	std::sort(polygons.begin(), polygons.end(), [&](const Polygon& p0, const Polygon& p1) -> bool { return p0.GetMaterialIdx() < p1.GetMaterialIdx();  });
}

void Mesh::UpdateAABB()
{
	if (polygons.size()==0)
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

bool Mesh::UpdatePolygonsAdjacency()
{
	std::map<int, int> edgeAdjPolygons0;
	std::map<int, int> edgeAdjPolygons1;

	for (size_t i = 0; i < polygons.size(); i++)
	{
		const Polygon& polygon = polygons[i];
		for (auto& adjacency : polygon.adjacencies)
		{
			if (!adjacency.EdgeFlipped())
			{
				edgeAdjPolygons0[adjacency.EdgeIndex()] = i;
			}
			else
			{
				edgeAdjPolygons1[adjacency.EdgeIndex()] = i;
			}
		}
	}

	bool result = true;
	for (size_t i = 0; i < polygons.size(); i++)
	{
		Polygon& polygon = polygons[i];
		for (auto& adjacency : polygon.adjacencies)
		{
			if (!adjacency.EdgeFlipped())
			{
				std::map<int, int>::iterator itr = edgeAdjPolygons1.find(adjacency.EdgeIndex());
				if (itr != edgeAdjPolygons1.end())
					adjacency.PolygonIdx() = itr->second;							 // found an opposite edge
			}
			else
			{
				std::map<int, int>::iterator itr = edgeAdjPolygons0.find(adjacency.EdgeIndex());
				if (itr != edgeAdjPolygons0.end())
					adjacency.PolygonIdx() = edgeAdjPolygons0[adjacency.EdgeIndex()]; // found an opposite edge
			}

			if (adjacency.PolygonIdx() == -1) // cannot found and opposite edge
			{
				result = false;
			}
		}
	}

	return result;
}

void Mesh::UpdateIsolatedEdges()
{
	printf("===================================================\n");
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
			printf("%sEdge %d:(%d->%d), (%.4f, %.4f, %.4f)->(%.4f, %.4f, %.4f)", tab, i, edge.StartIdx(), edge.EndIdx(), 
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
		printf("Mesh with isolated Edges\n");
		for (size_t i = 0; i < isolatedEdges.size(); i++)
		{
			isolatedEdges[i].Print("\t", i);
			printf("\n");
		}

		bool newLoop = true;

		int i = 0;
		do
		{
			if (newLoop)
			{
				loops.push_back(Loop());
				newLoop = false;
				printf("\tBegin Loop %d\n", (int)loops.size());
			}

			Loop& currentLoop = loops.back();
			if (currentLoop.empty() || (isolatedEdges[i].edge.StartIdx() == currentLoop.back().edge.EndIdx()))
			{
				currentLoop.push_back(isolatedEdges[i]);
				
				isolatedEdges[i].Print("\t\t", (int)(currentLoop.size() - 1));

				if (currentLoop.size() > 1)
				{
					if (currentLoop.back().edge.StartIdx() == currentLoop[currentLoop.size() - 2].edge.EndIdx())
						printf(" *");
				}
				printf("\n");

				isolatedEdges.erase(isolatedEdges.begin() + i);

				if (currentLoop.front().edge.StartIdx() == currentLoop.back().edge.EndIdx())
				{
					printf("\tEnd Loop %d\n", (int)(loops.size()));
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
		printf("Mesh without isolated Edge\n");
	}
}

void Mesh::Test()
{
	std::vector<int> counters(edgeOptimizer.GetDataCount());
	for (size_t i = 0; i < polygons.size(); i++)
	{
		const Polygon &polygon = polygons[i];
		for (auto& adjacency : polygon.GetAdjacencies())
			counters[adjacency.EdgeIndex()] += (adjacency.EdgeFlipped() ? 1 : -1);
	}

	printf("================================================\n");
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
		printf("Mesh with isolated Edges\n");
		for (size_t i = 0; i < isolatedEdges.size(); i++)
		{
			printf("\tEdge %d:(%d->%d)\n", i, isolatedEdges[i].StartIdx(), isolatedEdges[i].EndIdx());
		}

		bool newLoop = true;

		int i = 0;
		do
		{
			if (newLoop)
			{
				loops.push_back(Loop());
				newLoop = false;
				printf("\tBegin Loop %d\n", loops.size());
			}

			Loop& currentLoop = loops.back();
			if (currentLoop.empty() || (isolatedEdges[i].StartIdx() == currentLoop.back().EndIdx()))
			{
				currentLoop.push_back(isolatedEdges[i]);
				printf("\t\tEdge %d:(%d %d)", currentLoop.size()-1, isolatedEdges[i].StartIdx(), isolatedEdges[i].EndIdx());
				if (currentLoop.size() > 1)
				{
					if (currentLoop.back().StartIdx() == currentLoop[currentLoop.size() - 2].EndIdx())
						printf(" *");
				}
				printf("\n");

				isolatedEdges.erase(isolatedEdges.begin() + i);

				if (currentLoop.front().StartIdx() == currentLoop.back().EndIdx())
				{
					printf("\tEnd Loop %d\n", loops.size());
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
		printf("Mesh without isolated Edge\n");
	}
}

/*
void Mesh::UpdateIsolatedEdges()
{
	std::vector<int> counters(edgeOptimizer.GetDataCount());
	for (size_t i = 0; i <polygons.size(); i++)
	{
		for (auto& edge : polygons[i].GetEdges())
			counters[edge.Index()] += (edge.isFlipped() ? 1 : -1);
	}

	std::vector<Edge> edges;
	for (size_t i = 0; i < counters.size(); i++)
	{
		if (counters[i] != 0)
		{
			Edge edge = edgeOptimizer.GetData(i);
			if (counters[i] < 0)
				edge.Flip();

			edges.push_back(edge);
			printf("%d %d\n", edge[0], edge[1]);
		}
	}

	typedef std::vector<Edge> Loop;
	std::vector<Loop> loops;
	if (edges.size())
	{
		bool newLoop = true;
		
		int i = 0;
		do
		{
			if (newLoop)
			{
				loops.push_back(Loop());
				newLoop = false;
				printf("new loop ------------------------------------\n");
			}

			Loop& currentLoop = loops.back();
			if(currentLoop.empty() || (edges[i].StartIdx() == currentLoop.back().EndIdx()) )
			{
				currentLoop.push_back(edges[i]);
				printf("(%d %d), ", edges[i].StartIdx(), edges[i].EndIdx());

				edges.erase(edges.begin() + i);

				if (currentLoop.front().StartIdx()== currentLoop.back().EndIdx())
				{
					printf("\nloop closed ------------------------------------");
					// loop closed
					newLoop = true;
				}
			}
			else
			{
				i++;
			}

			if (i >= edges.size())
				i = 0;
		} while (edges.size());
	}
}
*/

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
	positionOptimizer.Clear();
	edgeOptimizer.Clear();
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

/*
void Triangulate(Mesh& mesh)
{
	Mesh oldMesh = mesh;

	mesh.Begin(oldMesh.GetColorChannelCount(), oldMesh.GetUVChannelCount(), oldMesh.GetNormalChannelCount(), oldMesh.GetTangentChannelCount(), oldMesh.GetBinormalChannelCount());
	{
		for (size_t i = 0; i < oldMesh.GetPolygonCount(); i++)
		{
			const Polygon& oldPolygon = oldMesh.GetPolygon(i);

			for (size_t j = 0; j < oldPolygon.GetVerticesCount() - 2; j++)
			{
				mesh.BeginPolygon(oldPolygon.GetGroupIdx(), oldPolygon.GetMaterialIdx());

				mesh.AddPolygonVertex(oldPolygon.GetVertex(0));
				mesh.AddPolygonVertex(oldPolygon.GetVertex(j + 1));
				mesh.AddPolygonVertex(oldPolygon.GetVertex(j + 2));

				mesh.EndPolygon();
			}
		}
	}
	mesh.End();
}
*/

Mesh Intersect(const Mesh& m0, const Mesh& m1)
{
	Mesh result;

	if (!m0.IsEmpty() && !m1.IsEmpty())
	{
		////////////////////////////////////////////
		// BSP
		BSP a(m0.GetPolygons());
		BSP b(m1.GetPolygons());

		a.Invert();
		b.ClipBy(a);		// find polygon of b inside ~a
		b.Invert();
		a.ClipBy(b);		// find polygon of a inside ~b

		b.ClipBy(a);		// remove b polygons coplanar with a

		// join polygons in a, b
		// a.FromPolygons(b.ToPolygons());
		std::vector<Polygon> joinPolygons;
		b.ToPolygons(joinPolygons);
		a.FromPolygons(joinPolygons);

		a.Invert();			// ~(~A | ~B)

		std::vector<Polygon> resultPolygons;
		a.ToPolygons(resultPolygons);

		////////////////////////////////////////////
		// output
		result.Begin(m0.GetColorChannelCount(), m0.GetUVChannelCount(), m0.GetNormalChannelCount(), m0.GetTangentChannelCount(), m0.GetBinormalChannelCount());

		result.BeginPolygons(resultPolygons);
		result.EndPolygon();

		result.End();
	}
	else if (m0.IsEmpty() && !m1.IsEmpty())
	{
		result = Mesh();
	}
	else if (!m0.IsEmpty() && m1.IsEmpty())
	{
		result = Mesh();
	}
	else// if (m0.IsEmpty() && m1.IsEmpty())
	{
		result = Mesh();
	}

	return result;
}