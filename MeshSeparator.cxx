#include "MeshSeparator.h"
#include "Polygon.h"

class EdgedPolygon
{
public:
	EdgedPolygon()
	{
	}

	~EdgedPolygon()
	{
	}

	int sharedEdge(const EdgedPolygon& other)
	{
		for (size_t i = 0; i < this->edges.size(); i++)
		{
			for (size_t j = 0; j < other.edges.size(); i++)
			{
				if (edges[i] == -other.edges[j])
				{
					return abs(edges[i]);
				}
			}
		}

		return -1;
	}
private:
	int materialIdx;
	std::vector<int> edges;
};

MeshSeparator::MeshSeparator()
{
}

MeshSeparator::~MeshSeparator()
{
}

bool MeshSeparator::Process(const std::vector<Mesh>& srcMeshes, std::vector<Mesh>& resultMeshes)
{
	resultMeshes.resize(srcMeshes.size());
	for (size_t i = 0; i < srcMeshes.size(); i++)
	{
		if (!Process(srcMeshes[i], resultMeshes[i]))
			return false;
	}

	return true;
}

bool MeshSeparator::Process(const Mesh& srcMesh, Mesh& resultMesh)
{
	resultMesh = srcMesh;

	while (srcMesh.polygons.size())
	{
	};

	return true;
}