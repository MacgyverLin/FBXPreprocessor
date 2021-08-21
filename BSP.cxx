#include "BSP.h"

BSP::BSP()
	: nodeSplitPlane(nullptr)
	, nodePolygons()
	, front(nullptr)
	, back(nullptr)
{
}

BSP::BSP(const std::vector<Polygon>& polygons)
	: nodeSplitPlane(nullptr)
	, nodePolygons()
	, front(nullptr)
	, back(nullptr)
{
	FromPolygons(polygons);
}

BSP::~BSP()
{
	if (nodeSplitPlane)
	{
		delete nodeSplitPlane;
		nodeSplitPlane = nullptr;
	}

	if (front)
	{
		delete front;
		front = nullptr;
	}

	if (back)
	{
		delete back;
		back = nullptr;
	}
}

BSP::BSP(const BSP& bsp)
	: nodeSplitPlane(bsp.nodeSplitPlane)
	, nodePolygons(bsp.nodePolygons)
	, front(bsp.front)
	, back(bsp.back)
{
}

BSP& BSP::operator = (const BSP& bsp)
{
	this->nodeSplitPlane = bsp.nodeSplitPlane;
	this->nodePolygons = bsp.nodePolygons;
	this->front = bsp.front;
	this->back = bsp.back;

	return *this;
}

void BSP::Invert()
{
	for (int i = 0; i < this->nodePolygons.size(); i++)
		this->nodePolygons[i].Flip();

	if (this->nodeSplitPlane)
		this->nodeSplitPlane->Flip();

	if (this->front)
		this->front->Invert();

	if (this->back)
		this->back->Invert();

	std::swap(this->front, this->back);
}

std::vector<Polygon> BSP::ClipPolygons(const std::vector<Polygon>& polygons) const
{
	if (!this->nodeSplitPlane)
		return polygons;

	std::vector<Polygon> frontPolygons;
	std::vector<Polygon> backPolygons;
	for (int i = 0; i < polygons.size(); i++)
		SplitPolygon(*nodeSplitPlane, polygons[i], frontPolygons, backPolygons, frontPolygons, backPolygons);

	if (this->front)
		frontPolygons = this->front->ClipPolygons(frontPolygons);

	if (this->back)
		backPolygons = this->back->ClipPolygons(backPolygons);
	else
		backPolygons.clear();

	Concat(frontPolygons, backPolygons);
	return frontPolygons;
}

void BSP::ClipBy(const BSP& bsp)
{
	this->nodePolygons = bsp.ClipPolygons(this->nodePolygons);

	if (this->front)
		this->front->ClipBy(bsp);
	if (this->back)
		this->back->ClipBy(bsp);
}

void BSP::ToPolygons(std::vector<Polygon>& polygons) const
{
	Concat(polygons, this->nodePolygons);

	if (this->front)
		this->front->ToPolygons(polygons);

	if (this->back)
		this->back->ToPolygons(polygons);
}

void BSP::FromPolygons(const std::vector<Polygon>& polygons)
{
	if (polygons.size() == 0)
		return;

	int splitPlaneIdx = -1;
	if (!this->nodeSplitPlane)
	{
		splitPlaneIdx = GetSplitPlaneIdx(polygons);
		this->nodeSplitPlane = new Plane(polygons[splitPlaneIdx].GetPlane());

		this->nodePolygons.push_back(polygons[splitPlaneIdx]);
	}

	std::vector<Polygon> frontPolygons;
	std::vector<Polygon> backPolygons;
	for (int i = 0; i < polygons.size(); i++)
	{
		if (i != splitPlaneIdx)
			SplitPolygon(*nodeSplitPlane, polygons[i], this->nodePolygons, this->nodePolygons, frontPolygons, backPolygons);
	}

	if (frontPolygons.size())
	{
		if (!this->front)
			this->front = new BSP();

		this->front->FromPolygons(frontPolygons);
	}

	if (backPolygons.size())
	{
		if (!this->back)
			this->back = new BSP();

		this->back->FromPolygons(backPolygons);
	}
}

void BSP::SplitPolygon(
	const Plane& splitPlane,
	const Polygon& polygon,
	std::vector<Polygon>& coplanarFrontPolygons,
	std::vector<Polygon>& coplanarBackPolygons,
	std::vector<Polygon>& frontPolygons,
	std::vector<Polygon>& backPolygons) const
{
	int polygonType = 0;
	std::vector<int> types(polygon.GetVerticesCount());
	for (int i = 0; i < polygon.GetVerticesCount(); i++)
	{
		float d = splitPlane.DistanceTo(polygon.GetVertex(i).position);
		int type = 0;
		if (d < -Math::ZeroTolerance)
			type = PolygonType::Back;
		else if (d > Math::ZeroTolerance)
			type = PolygonType::Front;
		else
			type = PolygonType::Coplanar;

		polygonType |= type;
		types[i] = type;
	}

	std::vector<Vertex> frontVertices;
	std::vector<Vertex> backVertices;

	switch (polygonType)
	{
	case PolygonType::Coplanar:
		(splitPlane.Normal().Dot(polygon.GetPlane().Normal()) > 0 ? coplanarFrontPolygons : coplanarBackPolygons).push_back(polygon);
		break;
	case PolygonType::Front:
		frontPolygons.push_back(polygon);
		break;
	case PolygonType::Back:
		backPolygons.push_back(polygon);
		break;
	case PolygonType::Spanning:
		for (int i = 0; i < polygon.GetVerticesCount(); i++)
		{
			int j = (i + 1) % polygon.GetVerticesCount();
			int ti = types[i];
			int tj = types[j];
			const Vertex& vi = polygon.GetVertex(i);
			const Vertex& vj = polygon.GetVertex(j);
			if (ti != PolygonType::Back)
				frontVertices.push_back(vi);
			if (ti != PolygonType::Front)
				backVertices.push_back(vi);
			if ((ti | tj) == PolygonType::Spanning)
			{
				const Vector3& normal = splitPlane.Normal();
				float t = (splitPlane.D() - normal.Dot(vi.position)) / normal.Dot(vj.position - vi.position);
				Vertex vc = Lerp(vi, vj, t);

				frontVertices.push_back(vc);
				backVertices.push_back(vc);
			}
		}

		if (frontVertices.size() >= 3)
			frontPolygons.push_back(Polygon(polygon.GetMaterialIdx(), frontVertices));

		if (backVertices.size() >= 3)
			backPolygons.push_back(Polygon(polygon.GetMaterialIdx(), backVertices));
		break;
	}
}

void BSP::Concat(std::vector<Polygon>& dst, const std::vector<Polygon>& src) const
{
	dst.insert(dst.end(), src.begin(), src.end());
}

void BSP::GetPolygonsType(const Plane& splitPlane, const Polygon& polygon, int& coplanerCount, int& frontCount, int& backCount, int& spanCount) const
{
	coplanerCount = 0;
	frontCount = 0;
	backCount = 0;
	spanCount = 0;

	int polygonType = 0;
	std::vector<int> types(polygon.GetVerticesCount());
	for (int i = 0; i < polygon.GetVerticesCount(); i++)
	{
		float d = splitPlane.DistanceTo(polygon.GetVertex(i).position);
		int type = 0;
		if (d < -Math::ZeroTolerance)
			type = PolygonType::Back;
		else if (d > Math::ZeroTolerance)
			type = PolygonType::Front;
		else
			type = PolygonType::Coplanar;

		polygonType |= type;
		types[i] = type;
	}

	switch (polygonType)
	{
	case PolygonType::Coplanar:
		coplanerCount++;
		break;
	case PolygonType::Front:
		frontCount++;
		break;
	case PolygonType::Back:
		backCount++;
		break;
	case PolygonType::Spanning:
		spanCount++;
		break;
	}
}


int BSP::GetSplitPlaneIdx(const std::vector<Polygon>& polygons) const
{
	return (int)Math::RangeRandom(0, (float)polygons.size()-1);
	/*
	int coplanerCount = 0;
	int frontCount = 0;
	int backCount = 0;
	int spanCount = 0;

	int minSpan = 0x7fffffff;
	int minSpanIdx = 0;
	int minDiff = 0x7fffffff;
	int minDiffIdx = 0;
	for (int i = 0; i < polygons.size()-1; i++)
	{
		Plane p = GetPlane(polygons[i]);
		for (int j = i+1; j < polygons.size(); j++)
		{
			GetPolygonsType(p, polygons[j], coplanerCount, frontCount, backCount, spanCount);
			if (minSpan > spanCount)
			{
				minSpanIdx = i;
				minSpan = spanCount;

				if (minSpanIdx != 0)
				{
					minSpanIdx = minSpanIdx;
				}
			}

			int diff = abs(frontCount - backCount);
			if (minDiff > diff)
			{
				minDiffIdx = i;
				minDiff = diff;
			}
		}
	}

	return minSpanIdx;
	// return minDiffIdx;
	*/
}