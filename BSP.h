#ifndef _BSP_h_
#define _BSP_h_

#include "Polygon.h"
#include "Plane.h"

class BSP
{
public:
	//////////////////////////////////////////////////////////////////////////////////////////////////
	enum PolygonType
	{
		Coplanar = 0,
		Front = 1,
		Back = 2,
		Spanning = 3,
	};

	BSP();

	BSP(const std::vector<Polygon>& polygons);

	~BSP();

	BSP(const BSP& bsp);

	BSP& operator = (const BSP& bsp);

	void Invert();

	std::vector<Polygon> ClipPolygons(const std::vector<Polygon>& polygons) const;

	void ClipBy(const BSP& bsp);

	void ToPolygons(std::vector<Polygon>& polygons) const;

	void FromPolygons(const std::vector<Polygon>& polygons);
protected:
	void SplitPolygon(
		const Plane& splitPlane,
		const Polygon& polygon,
		std::vector<Polygon>& coplanarFrontPolygons,
		std::vector<Polygon>& coplanarBackPolygons,
		std::vector<Polygon>& frontPolygons,
		std::vector<Polygon>& backPolygons) const;

	void Concat(std::vector<Polygon>& dst, const std::vector<Polygon>& src) const;

	void GetPolygonsType(const Plane& splitPlane, const Polygon& polygon, int& coplanerCount, int& frontCount, int& backCount, int& spanCount) const;

	int GetSplitPlaneIdx(const std::vector<Polygon>& polygons) const;
private:

public:
protected:
private:
	Plane* nodeSplitPlane;
	std::vector<Polygon> nodePolygons;

	BSP* front;
	BSP* back;
};

#endif