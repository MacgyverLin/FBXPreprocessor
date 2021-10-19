/*
	running from commandline
	https://knowledge.autodesk.com/support/3ds-max/getting-started/caas/CloudHelp/cloudhelp/2021/ENU/3DSMax-Basics/files/GUID-BCB04DEC-7967-4091-B980-638CFDFE47EC-htm.html	
*/
struct BoundingBox
(
	center,
	size = [1, 1, 1]
)

struct BoundingCapsule
(
	center,
	height = 1,
	radius = 1,
	direction = [1, 0, 0]
)

struct FaceGroup
(
	groupID, 
	density = 1,
	bound = BoundingBox [0, 0, 0],
	boundCapsule = BoundingCapsule [0, 0, 0],
	faceIndices=#()
)

struct JSONExporter
(
	fn toUnityVector3 v3dsmax =
	(
		vunity = [-v3dsmax.x, v3dsmax.y, v3dsmax.z]
		
		return vunity
	),
	
	fn toUnrealVector3 v3dsmax =
	(
		vunreal = [v3dsmax.x, v3dsmax.y, v3dsmax.z]
		
		return vunreal
	),
			
	fn formatVector3Unity fptr v3dsmax = 
	(
		v = toUnityVector3 v3dsmax
		format "{" to:fptr
			format "\"x\":%,\"y\":%,\"z\":%" v.x v.y v.z to:fptr
		format "}" to:fptr
	),
	
	fn formatVector3Unreal fptr v3dsmax = 
	(
		v = toUnrealVector3 v3dsmax
		format "{" to:fptr
			format "\"x\":%,\"y\":%,\"z\":%" v.x v.y v.z to:fptr
		format "}" to:fptr
	),

	fn formatVector3 fptr v = 
	(
		formatVector3Unity fptr v
		--formatVector3Unreal fptr v
	),	
	
	fn formatSize fptr v = 
	(
		format "{" to:fptr
			format "\"x\":%,\"y\":%,\"z\":%" v.x v.y v.z to:fptr
		format "}" to:fptr
	),		
	
	fn formatFaceGroupBoundingBox fptr bound = 
	(
		format "{" to:fptr
			format "\"center\":" to:fptr
			formatVector3 fptr bound.center
			format "," to:fptr
		
			format "\"size\":" to:fptr
			formatSize fptr bound.size
		format "}" to:fptr
	),
	
	fn formatFaceGroupBoundingCapsule fptr boundCapsule = 
	(
		format "{" to:fptr
			format "\"center\":" to:fptr
			formatVector3 fptr boundCapsule.center
			format "," to:fptr
		
			format "\"height\": %" boundCapsule.height to:fptr
			format "," to:fptr
		
			format "\"radius\": %" boundCapsule.radius to:fptr
			format "," to:fptr
		
			format "\"direction\":" to:fptr
			formatVector3 fptr boundCapsule.direction	
		format "}" to:fptr
	),	

	fn formatFaceGroup fptr faceGroup = 
	(
		format "{" to:fptr
			format "\"groupID\":%," faceGroup.groupID to:fptr
			
			format "\"density\":%," faceGroup.density to:fptr
			
			format "\"bound\":" to:fptr
			formatFaceGroupBoundingBox fptr faceGroup.bound
			format "," to:fptr
		
			format "\"boundCapsule\":" to:fptr
			formatFaceGroupBoundingCapsule fptr faceGroup.boundCapsule
		format "}" to:fptr
	),
	
	fn formatFaceGroupMap fptr faceGroupMap = 
	(
		format "\"faceGroups\":" to:fptr
		format "[" to:fptr
		
		groupCount = 0
		for faceGroupKeyValue in faceGroupMap do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value
			
			formatFaceGroup fptr faceGroupValue

			groupCount += 1;
			if(groupCount < faceGroupMap.count) then
				format "," to:fptr
			else
				format "" to:fptr
		)
		
		format "]" to:fptr
	),	
	
	
	fn formatJSON fptr faceGroupMap = 
	(
		format "{" to:fptr
		
		formatFaceGroupMap fptr faceGroupMap
		
		format "}" to:fptr
		
		return true
	)
)
jsonExporter = JSONExporter()

struct DemolitionTool
(
	----------------------------------------------------------
	fn exportJSONToStream fptr faceGroupMap = 
	(
		jsonExporter.formatJSON fptr faceGroupMap
		
		return true
	),
	
	fn exportJSON filename faceGroupMap = 
	(
		fptr = openFile filename mode:"wt"
		if(fptr==undefined) then
			return false

		exportJSONToStream fptr faceGroupMap
		
		close fptr
		
		return true
	),

	----------------------------------------------------------
	fn validMapChannel meshObj useMapChannel =
	(
		maxChannel = meshop.getNumMaps meshObj
		vertexPerFace = 3
		numFaces = meshop.getNumFaces meshObj			
		
		if(useMapChannel>=maxChannel) then
		(
			--format "Map Channel % is allocated.\n" (useMapChannel+1)
			
			meshop.setNumMaps meshObj (useMapChannel+1) keep:true
			meshop.buildMapFaces meshObj useMapChannel keep:true
		)
		else
		(
			--format "Map Channel %  has already allocated.\n" (useMapChannel+1)
		)
		
		totalVertex = (numFaces * vertexPerFace)
		if(totalVertex > meshop.getNumMapVerts meshObj useMapChannel) then
		(
			--format "Map Channel %, vertices is not enough, resize to %.\n" (useMapChannel+1) totalVertex
			
			meshop.setNumMapVerts meshObj useMapChannel totalVertex keep:true
			
			for faceIdx = 1 to numFaces do
			(
				vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
				meshop.setMapFace meshObj useMapChannel faceIdx vertexIdxs

				for v = 1 to vertexIdxs.count do
					meshop.setMapVert meshObj useMapChannel vertexIdxs[v] [0, 0, 0]
			)			
		)
		else
		(
			--format "Map Channel %, vertices is enough, size = %.\n" (useMapChannel+1) totalVertex
		)
	)
	,
	
	fn computeFaceGroupMapByMeshElements meshObj =
	(
		currentGroup = -1
		
		faceIDArray = #()
		faceGroupMap = Dictionary(#integer)
		for i = 1 to meshObj.numfaces do
		(
			if(faceIDArray[i]==undefined) then
			(
				currentGroup = currentGroup + 1
				faceGroupMap[currentGroup] = (FaceGroup currentGroup)
			)

			elementFaceIndices = meshop.GetElementsUsingFace meshObj i as Array
			for j = 1 to elementFaceIndices.count do
			(
				elementFaceIndex = elementFaceIndices[j]
				if(faceIDArray[elementFaceIndex]==undefined) then
				(
					faceIDArray[elementFaceIndex] = currentGroup
					appendIfUnique faceGroupMap[currentGroup].faceIndices elementFaceIndex
				)
			)
		)
	
		return faceGroupMap
	)
	,	
	
	fn computeFaceGroupArrayByMeshElements meshObj =
	(
		faceGroupMap = computeFaceGroupMapByMeshElements meshObj
		
		faceGroups = #()
		for faceGroupKeyValue in faceGroupMap do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value

			faceGroups[faceGroupKey+1] = faceGroupValue
		)
		
		for i = 1 to faceGroups.count do
		(
			-- format "% % %\n" i faceGroups[i].groupID faceGroups[i]
		)
		
		return faceGroups
	)
	,	
	
	fn collectFaceGroupMap meshObj useMapChannel =
	(
		validMapChannel meshObj useMapChannel
			
		vertexPerFace = 3
		faceGroupMap = Dictionary(#integer)
		for face in meshObj.faces do
		(		
			faceIdx = face.index
			
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			
			-- sanity check
			allVertexSameGroupID = true
			faceGroupID = -1
			for v = 1 to vertexIdxs.count-1 do
			(
				p0 = meshop.getMapVert meshObj useMapChannel vertexIdxs[v+0]
				p1 = meshop.getMapVert meshObj useMapChannel vertexIdxs[v+1]
				if( not (p0.x == p1.x) ) then
				(
					allVertexSameGroupID = false
					faceGroupID = -1
				)
				
				faceGroupID = int(p0.x)
			)
			if(not allVertexSameGroupID) then
			(
				messageBox "The map vertices is corrupted."
				return false
			)

			if(allVertexSameGroupID) then
			(
				if(faceGroupMap[faceGroupID]==undefined) then
				(
					faceGroupMap[faceGroupID] = (FaceGroup faceGroupID)
				)
				
				appendIfUnique faceGroupMap[faceGroupID].faceIndices faceIdx
			)
		)	
		
		return faceGroupMap
	)
	,
	
	fn collectFaceGroupArray meshObj useMapChannel =
	(
		faceGroupMap = collectFaceGroupMap meshObj useMapChannel
		
		faceGroups = #()
		for faceGroupKeyValue in faceGroupMap do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value
			
			faceGroups[faceGroupKey+1] = faceGroupValue
		)
		
		for i = 1 to faceGroups.count do
		(
			if(not faceGroups[i]==undefined) then
				format "% % %\n" i faceGroups[i].groupID faceGroups[i]
		)
		
		return faceGroups
	)
	,		
	
	fn convertFacesToFaceIndices faces = 
	(
		faceIndices = #()
		for face in faces do
		(
			append faceIndices face.index
		)
		
		return faceIndices
	)
	,	
	
	-----------------------------------------------------------------------------------------
	-- Bound
	fn vector3Max v1 v2 = 
	(
		result = [0, 0, 0]
		if(v1.x > v2.x) then
			result.x  = v1.x
		else
			result.x  = v2.x
		
		if(v1.y > v2.y) then
			result.y  = v1.y
		else
			result.y  = v2.y		
		
		if(v1.z > v2.z) then
			result.z  = v1.z
		else
			result.z  = v2.z		
		
		return result
	)
	,
	
	fn vector3Min v1 v2 = 
	(
		result = [0, 0, 0]
		if(v1.x < v2.x) then
			result.x  = v1.x
		else
			result.x  = v2.x
		
		if(v1.y < v2.y) then
			result.y  = v1.y
		else
			result.y  = v2.y		
		
		if(v1.z < v2.z) then
			result.z  = v1.z
		else
			result.z  = v2.z		
		
		return result
	)
	,
	
	fn computeCentroid vertexPositions =
	(
		minV = [999999, 999999, 999999]
		maxV = [-999999, -999999, -999999]
		
		centroid = [0, 0, 0]
		for vertexPosition in vertexPositions do
		(
			centroid += vertexPosition
			
			minV = vector3Min minV vertexPosition
			maxV = vector3Max maxV vertexPosition
		)
		centroid /= vertexPositions.count
		
		return centroid
	)
	,	
	
	fn computeAABB vertexPositions =
	(
		minV = [999999, 999999, 999999]
		maxV = [-999999, -999999, -999999]
		
		centroid = [0, 0, 0]
		for vertexPosition in vertexPositions do
		(
			centroid += vertexPosition
			
			minV = vector3Min minV vertexPosition
			maxV = vector3Max maxV vertexPosition
		)
		centroid /= vertexPositions.count
		
		size = maxV - centroid
	
		return BoundingBox centroid size
	)
	,

	-----------------------------------------------------------------	
	fn makeLine pointA pointB pointC =
	(
		ss = SplineShape pos:pointA
		addNewSpline ss
		addKnot ss 1 #corner #line pointA
		addKnot ss 1 #corner #line pointB
		addKnot ss 1 #corner #line pointC
		updateShape ss
		ss
	)
	,
	
	fn makeCapsule cap =
	(
		rotAxis = normalize (cross cap.direction [0, 0, 1])
		rotAngle = acos (dot cap.direction [0, 0, 1])
		--a = angleaxis rotAngle rotAxis
		--format "%\n" a
		trans = angleaxis rotAngle rotAxis as Matrix3
		trans[4] = cap.center + cap.direction * (-cap.height * 0.5)
		
		c = Capsule radius:cap.radius height:cap.height heighttype:0 sides:16 heightsegs:1 smooth:on sliceon:off slicefrom:0 sliceto:0 mapcoords:on isSelected:off
		c.transform = trans
		c.wirecolor = color 115 115 115
		
		converttoMesh c
		--bestline = makeLine (cap.center + cap.direction * -cap.height * 0.5) cap.center (cap.center + cap.direction * cap.height * 0.5)
		--bestline.wirecolor = color 115 115 115
	)
	,
	
	fn findMajorAxis dir =
	(
		--return [1, 0, 0]
		
		if(dir.x>dir.y and dir.x>dir.z) then
			return [1, 0, 0]
		else if(dir.y>dir.x and dir.y>dir.z) then
			return [0, 1, 0]
		else
			return [0, 0, 1]
	)
	,
		
	fn isVertexInCapsule vertex cap = 
	(
		halfHeight = (cap.height - (cap.radius * 2.0)) * 0.5
		dist = dot (vertex - cap.center) cap.direction
		
		if(dist > halfHeight) then
		(
			dist = halfHeight
		)
		else if(dist < -halfHeight) then
		(
			dist = -halfHeight
		)
		
		projVertex = (cap.center + dist * cap.direction)
		d = length (vertex - projVertex)
		
		return d <= cap.radius
	)
	,
	
	fn isAllVerticesInCapsule vertices cap = 
	(
		for i = 1 to vertices.count do
		(
			if( not(isVertexInCapsule vertices[i] cap) ) then
			(
				return false
			)
		)
		
		return true
	)
	,	
	
	-- https://zalo.github.io/blog/line-fitting/
	fn computeBoundingCapsule vertices  = 
	(
		centroid = computeCentroid vertices
		
		/* best fitting line*/
		direction = [0, 0, 0]
		for i = 1 to vertices.count do
		(
			direction = vertices[i] - centroid
		)
		direction /= vertices.count
		direction = normalize direction
		/* best fitting line
		iterations = 30
		direction = [0, 0, 0]
		for i = 1 to iterations do
		(
			nextDirection = [0, 0, 0]			
			for i = 1 to vertices.count do
			(
				centeredPoint = vertices[i] - centroid;
				nextDirection = nextDirection + (dot centeredPoint direction) * centeredPoint
			)
			direction = normalize nextDirection
		)
		*/
		direction = findMajorAxis direction
		
		/* find min max mid distance along direction */
		minH = 0
		maxH = 0
		radius = 0
		for i = 1 to vertices.count do
		(
			v = vertices[i] - centroid
			h = (dot v direction)
			r = length (v - h * direction)
			
			if((i==1) or (maxH<h)) then
			(
				maxH = h
			)
			if((i==1) or (minH>h)) then
			(
				minH = h
			)
			if((i==1) or (radius<r)) then
			(
				radius = r
			)
		)
		
		midH = (minH + maxH) * 0.5
		--format "% % %\n" minH midH maxH
		centroid = centroid + direction * midH
		minH = minH - midH
		maxH = maxH - midH
		--format "% % %\n" minH midH maxH
		
		minHeight = radius * 2.0
		maxHeight = radius * 2.0 + (maxH - minH)
		height = maxHeight
		cap = BoundingCapsule center:centroid height:height radius:radius direction:direction
		
		t = 1.0
		while(t > 0.05) do
		(
			if(isAllVerticesInCapsule vertices cap) then
			(
				t -= 0.05
				height = minHeight + (maxHeight - minHeight) * t
				cap = BoundingCapsule center:centroid height:height radius:radius direction:direction
				
				--format "isAllVerticesInCapsule % %\n" t height				
			)
			else
			(
				t += 0.05
				height = minHeight + (maxHeight - minHeight) * t
				cap = BoundingCapsule center:centroid height:height radius:radius direction:direction
				
				--format "not isAllVerticesInCapsule % %\n" t height				
				exit
			)
		)
		
		/* binary search best radius */
		/*
		minHeight = radius * 2.0
		maxHeight = radius * 2.0 + (maxH - minH)
		height = maxHeight
		
		loopCount = 50
		while( (minHeight < maxHeight) and (loopCount>0) ) do
		(
			midHeight = minHeight + (maxHeight - minHeight) * 0.5
			cap = BoundingCapsule center:centroid height:midHeight radius:radius direction:direction		
			
			if((isAllVerticesInCapsule vertices cap)) then
			(
				maxHeight = midHeight
				
				format "isAllVerticesInCapsule % %\n" minHeight maxHeight
			)
			else
			(
				minHeight = midHeight
				
				format "not isAllVerticesInCapsule % %\n" minHeight maxHeight
			)
			
			loopCount -= 1
		)
		*/
		
		/* show capsule and best fit line*/
		-- makeCapsule cap

		return cap
	)
	,
	
	fn testComputeBoundingCapsule vertices = 
	(
		computeBoundingCapsule vertices

		return true
	)
	,
	
	fn setFaceGroupBound meshObj faceGroup =
	(
		meshInvTransform = (inverse meshObj.transform)
		
		vertexPositions = #()
		for faceIdx in faceGroup.faceIndices do
		(		
			vertexIdxs = meshop.getFace meshObj faceIdx
		
			for v = 1 to vertexIdxs.count do
			(
				vertexPosition = (in coordsys world meshop.getVert meshObj vertexIdxs[v]) * meshInvTransform
				append vertexPositions vertexPosition
			)
		)

		faceGroup.bound = computeAABB vertexPositions
		faceGroup.boundCapsule = computeBoundingCapsule vertexPositions
	)
	,

	fn setMeshFaceGroupBounds meshObj maxGroupCount faceGroupMap = 
	(
		for faceGroupKeyValue in faceGroupMap do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value
			
			setFaceGroupBound meshObj faceGroupValue
		)
		
		return true
	)
	,
	
	-----------------------------------------------------------------------------------------
	--- Face Group
	fn setFaceGroupID meshObj vertexPerFace useMapChannel faceIndices groupID =
	(
		validMapChannel meshObj useMapChannel
		
		for faceIdx in faceIndices do
		(		
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert meshObj useMapChannel vertexIdxs[v]
				p.x = groupID
				meshop.setMapVert meshObj useMapChannel vertexIdxs[v] p
			)
		)
	)
	,

	fn setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroupMap = 
	(
		validMapChannel meshObj useMapChannel
		
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps meshObj
		numFaces = meshop.getNumFaces meshObj
				
		for faceGroupKeyValue in faceGroupMap do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value
			
			setFaceGroupID meshObj vertexPerFace useMapChannel faceGroupValue.faceIndices faceGroupValue.groupID
		)
		
		return true
	)
	,
	
	fn getFaceIndicesByGroupID meshObj useMapChannel groupID faceIndices  =
	(
		validMapChannel meshObj useMapChannel
		
		vertexPerFace = 3
		
		for face in meshObj.faces do
		(		
			faceIdx = face.index
			
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			
			-- sanity check
			allVertexSameGroupID = true
			faceGroupID = -1
			for v = 1 to vertexIdxs.count-1 do
			(
				p0 = meshop.getMapVert meshObj useMapChannel vertexIdxs[v+0]
				p1 = meshop.getMapVert meshObj useMapChannel vertexIdxs[v+1]
				if( not (p0.x == p1.x) ) then
				(
					allVertexSameGroupID = false
					faceGroupID = -1
				)
				
				faceGroupID = p0.x
			)
			if(not allVertexSameGroupID) then
			(
				messageBox "The map vertices is corrupted."
				return false
			)

			if(allVertexSameGroupID and faceGroupID==groupID) then
			(
				appendIfUnique faceIndices faceIdx
			)
		)	
		
		return faceIndices		
	)
	,
	
	fn getFaceIndicesByCrossSectionID meshObj useMapChannel crossSectionID faceIndices  =
	(
		validMapChannel meshObj useMapChannel
		
		vertexPerFace = 3
		
		for face in meshObj.faces do
		(		
			faceIdx = face.index
			
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			
			-- sanity check
			allVertexSameCrossSectionID= true
			faceCrossSectionID = -1
			for v = 1 to vertexIdxs.count-1 do
			(
				p0 = meshop.getMapVert meshObj useMapChannel vertexIdxs[v+0]
				p1 = meshop.getMapVert meshObj useMapChannel vertexIdxs[v+1]
				if( not (p0.y == p1.y) ) then
				(
					allVertexSameCrossSectionID = false
					faceCrossSectionID = -1
				)
				
				faceCrossSectionID = p0.y
			)
			if(not allVertexSameCrossSectionID) then
			(
				messageBox "The map vertices is corrupted."
				return false
			)

			if(allVertexSameCrossSectionID and faceCrossSectionID==crossSectionID) then
			(
				appendIfUnique faceIndices faceIdx
			)
		)	
		
		return faceIndices		
	)	
	
	,	
	
	-----------------------------------------------------------------------------------------
	--- Cross Section Face
	fn setMeshCrossSectionFace meshObj useMapChannel faces isCrossSection = 
	(
		vertexPerFace = 3
		validMapChannel meshObj useMapChannel
		
		for face in faces do
		(
			faceIdx = face.index
			
			vertexIdxs = (meshop.getMapFace meshObj useMapChannel faceIdx)
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert meshObj useMapChannel vertexIdxs[v]
				p.y = isCrossSection
				meshop.setMapVert meshObj useMapChannel vertexIdxs[v] p
			)
		)
		
		return true
	)
	,
	
	-------------------------------------------------------------------
	-- UI callback handler
	fn initializeMesh meshObj useMapChannel maxGroupCount = 
	(
		max modify mode
		subObjectLevel = 0

		faceGroupMap = Dictionary(#integer)
		
		faceGroupMap[0] = FaceGroup 0 density:1 faceIndices:(convertFacesToFaceIndices meshObj.faces) bound:(BoundingBox [0, 0, 0]) 
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroupMap

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroupMap
		
		setMeshCrossSectionFace meshObj useMapChannel meshObj.faces 0

		return true
	)
	,

	------------------------------------------------------------------
	fn computeFaceGroupByMeshElement meshObj useMapChannel maxGroupCount = 
	(
		max modify mode
		subObjectLevel = 0
		
		faceGroupMap = computeFaceGroupMapByMeshElements meshObj
		if(faceGroupMap.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroupMap

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroupMap
		
		max modify mode
		subObjectLevel = 4
		
		return true
	)
	,	
	
	fn selectFacesByGroupID meshObj useMapChannel groupID clearSelectedFaces =
	(
		max modify mode
		subObjectLevel = 0
		
		if(clearSelectedFaces) then
			meshObj.selectedFaces = getFaceIndicesByGroupID meshObj useMapChannel groupID #()
		else
			meshObj.selectedFaces = getFaceIndicesByGroupID meshObj useMapChannel groupID (convertFacesToFaceIndices meshObj.selectedfaces)

		max modify mode
		subObjectLevel = 4
		
		return true
	)
	,	

	fn setSelectedFacesGroupID meshObj useMapChannel groupID =
	(
		max modify mode
		subObjectLevel = 0
		
		vertexPerFace = 3
	
		setFaceGroupID meshObj vertexPerFace useMapChannel (convertFacesToFaceIndices meshObj.selectedfaces) groupID

		--selectFacesByGroupID meshObj useMapChannel groupID true
		
		max modify mode
		subObjectLevel = 4
		
		return true
	)
	,	
	---------------------------------------------------------	
	fn selectFacesByCrossSectionID meshObj useMapChannel crossSectionID clearSelectedFaces = 
	(
		max modify mode
		subObjectLevel = 0
		
		if(clearSelectedFaces) then
		(
			meshObj.selectedFaces = getFaceIndicesByCrossSectionID meshObj useMapChannel crossSectionID #()
		)
		else
		(
			meshObj.selectedFaces = getFaceIndicesByCrossSectionID meshObj useMapChannel crossSectionID (convertFacesToFaceIndices meshObj.selectedfaces)
		)
		
		max modify mode
		subObjectLevel = 4
		
		return true
	)
	,
	
	fn addSelectedFacesToCrossSection meshObj useMapChannel = 
	(
		max modify mode
		subObjectLevel = 0
		
		setMeshCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 1
		
		max modify mode
		subObjectLevel = 4		
		
		return true
	)
	,
	
	fn removeSelectedFacesFromCrossSection meshObj useMapChannel = 
	(
		max modify mode
		subObjectLevel = 0
		
		setMeshCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 0
		
		max modify mode
		subObjectLevel = 4		
		
		return true
	)
	,
	
	fn removeAllFacesFromCrossSection meshObj useMapChannel = 
	(
		max modify mode
		subObjectLevel = 0
		
		setMeshCrossSectionFace meshObj useMapChannel meshObj.faces 0
		
		max modify mode
		subObjectLevel = 4
		
		return true
	)
	,
	
	fn exportFBXAndJson meshObj useMapChannel maxGroupCount fbxFilePath jsonFilePath = 
	(
		faceGroupMap = collectFaceGroupMap meshObj useMapChannel
		if(faceGroupMap.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupBounds meshObj maxGroupCount faceGroupMap

		ss = stringStream ""
		exportJSONToStream ss faceGroupMap
		--format "%\n" (ss as string)
		
		setUserPropVal meshObj "DemolishableData" (ss as string) quoteString:false
		
		attr = attributes attr (parameters main (DemolishableData type:#string))
		custAttributes.add meshObj attr
		meshObj.DemolishableData = ss
		
		if(not exportFile fbxFilePath #noPrompt using:FBXEXP) then
		(
			messageBox "Failed to Export fbx file"
			return false
		)		
		else
		(
			messageBox "Success to Export fbx file"
		)

		/*
		if(not exportJson jsonFilePath faceGroupMap) then
		(
			messageBox "Failed to Export Json file"
			return false
		)
		else
		(
			messageBox "Success to Export Json file"
		)
		*/
		
		return true
	),
	
	
	fn exportFBXAndJsonUIOld meshObj useMapChannel maxGroupCount fbxFilePath jsonFilePath = 
	(
		faceGroupMap = computeFaceGroupMapByMeshElements meshObj
		if(faceGroupMap.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroupMap

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroupMap

		if(not exportFile fbxFilePath using:FBXEXP) then
		(
			messageBox "Failed to Export fbx file"
			return false
		)		
		else
		(
			messageBox "Success to Export fbx file"
		)
		
		if(not formatJSON jsonFilePath faceGroupMap) then
		(
			messageBox "Failed to Export Json file"
			return false
		)
		else
		(
			messageBox "Success to Export Json file"
		)		
		
		return true
	)	
)

demolitionTool = DemolitionTool()
	
macroScript DemolitionToolUI category:"Tencent" toolTip:"Demolition Tool"
(
	rollout DemolitionToolUI_Panel "DemolitionTool"
	(
		fn noticeMessage succeeded msg reason =
		(
			s = stringStream ""
			
			if(not reason=="") then
				reason = ", " + reason
			
			if(succeeded) then
				format "Succeded in %%\n" msg reason to:s
			else
				format "Failed To %%\n" msg reason to:s			
			
			messageBox s
		)

		button testUI "testUI" width:200
		
		on testUI pressed do
		(
			succeeded = true
			reason = ""

			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "InitializeMesh" reason	
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						continue
					)

					vertices = for i = 1 to (meshop.getNumVerts meshObj) collect (in coordsys world meshop.getVert meshObj i)
					if( not (demolitionTool.testComputeBoundingCapsule vertices)) then
					(
						succeeded = false
						reason = "computeBestFitCapsule failed."
						continue
					)
				)
				
				noticeMessage succeeded "computeBestFitCapsule" reason	
			)			
		)
		
		-------------------------------------------------------------------------
		group "MapChannel"
		(
			spinner maxMeshGroupUI "Max Mesh Group:" range:[16,32,32] type:#integer enabled:false  width:100
			spinner mapChannelUI "Map Channel:" range:[1,8,3] type:#integer enabled:true  width:100
			button initializeMeshUI "Initialize Mesh" width:200 
		)		

		on initializeMeshUI pressed do
		(
			succeeded = true
			reason = ""

			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "InitializeMesh" reason	
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						continue
					)
					
					if( not (demolitionTool.initializeMesh meshObj mapChannelUI.value maxMeshGroupUI.value)) then
					(
						succeeded = false
						reason = "initializeMesh failed."
						continue
					)
				)
				
				noticeMessage succeeded "InitializeMesh" reason	
			)
		)
		
		-------------------------------------------------------------------------		
		group "Mesh Face Groups"
		(
			button computeFaceGroupByMeshElementUI "Compute by Mesh Element" width:200 
			
			label setIDLabelUI "Set ID:" across:2 align:#right
			spinner setIDValueUI range:[1,32,1] type:#integer align:#left
			
			button selectIDButtonUI "Select ID:" across:2 align:#right
			spinner selectIDValueUI range:[1,32,1] type:#integer align:#left
			checkbox clearSelectionUI "Clear Selection" width:200 align:#center checked:true
		)
		
		on computeFaceGroupByMeshElementUI pressed do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Compute FaceGroup By Mesh Element" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)					
					if( not (demolitionTool.computeFaceGroupByMeshElement meshObj mapChannelUI.value maxMeshGroupUI.value)) then
					(
						succeeded = false
						reason = "computeFaceGroupByMeshElement failed."

						continue
					)	
				)
				
				noticeMessage succeeded "Compute FaceGroup By Mesh Element" reason
			)
		)	
				
		
		on setIDValueUI entered val val2 do
		(
			succeeded = true
			reason = ""

			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Set Selected Faces GroupID" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)

					if( not (demolitionTool.setSelectedFacesGroupID meshObj mapChannelUI.value (setIDValueUI.value-1))) then
					(
						succeeded = false
						reason = "setSelectedFacesGroupID failed."
						
						continue
					)	
				)

				if(not succeeded) then
					noticeMessage succeeded "Set Selected Faces GroupID" reason

				if(succeeded) then
				(
					selectIDValueUI.value = setIDValueUI.value
					subobjectLevel = 4
				)
			)
		)
		
		on selectIDValueUI entered val val2 do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Select Faces By GroupID" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)
					if( not (demolitionTool.selectFacesByGroupID meshObj mapChannelUI.value (selectIDValueUI.value-1) clearSelectionUI.checked)) then
					(
						succeeded = false
						reason = "selectFacesByGroupID failed."
						
						continue
					)
				)
				
				if(not succeeded) then
					noticeMessage succeeded "Select Faces By GroupID" reason
			)
		)			

		on selectIDButtonUI pressed do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Select Faces By GroupID" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)
					if( not (demolitionTool.selectFacesByGroupID meshObj mapChannelUI.value (selectIDValueUI.value-1) clearSelectionUI.checked)) then
					(
						succeeded = false
						reason = "selectFacesByGroupID failed."
						
						continue
					)
				)
				
				if(not succeeded) then
					noticeMessage succeeded "Select Faces By GroupID" reason
			)
		)	
		
		-------------------------------------------------------------------------
		group "Cross Section"
		(
			button selectFacesByCrossSectionIDUI "Select CrossSection Faces" width:230
			button addSelectedFacesToCrossSectionUI "Add Selected Faces To CrossSection" width:230
			button removeSelectedFacesFromCrossSectionUI "Remove Selected Faces From CrossSection" width:230
			button removeAllFacesFromCrossSectionUI "Remove All Faces From CrossSection" width:230
			checkbox crossSectionClearSelectionUI "Clear Selection" width:200 align:#center checked:true
		)
		
		on selectFacesByCrossSectionIDUI pressed do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Select CrossSection Faces" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)					
					if( not (demolitionTool.selectFacesByCrossSectionID meshObj mapChannelUI.value 1 crossSectionClearSelectionUI.checked)) then
					(
						succeeded = false
						reason = "selectFacesByCrossSectionID failed."
						
						continue
					)	
				)
				
				if(not succeeded) then
					noticeMessage succeeded "Select CrossSection Faces" reason
			)
		)
		
		on addSelectedFacesToCrossSectionUI pressed do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Add Selected Faces To CrossSection" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)					
					if( not (demolitionTool.addSelectedFacesToCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						reason = "addSelectedFacesToCrossSection failed."

						continue
					)	
				)
				
				--if(not succeeded) then
				noticeMessage succeeded "Add Selected Faces To CrossSection" reason
			)
		)
		
		on removeSelectedFacesFromCrossSectionUI pressed do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Remove Selected Faces From CrossSection" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)					
					if( not (demolitionTool.removeSelectedFacesFromCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						reason = "removeSelectedFacesFromCrossSection failed."
						
						continue
					)	
				)
				
				--if(not succeeded) then
				noticeMessage succeeded "Remove Selected Faces From CrossSection" reason
			)
		)
		
		on removeAllFacesFromCrossSectionUI pressed do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Remove All Faces From CrossSection" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)					
					if( not (demolitionTool.removeAllFacesFromCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						reason = "removeAllFacesFromCrossSection failed."
						
						continue
					)	
				)

				--if(not succeeded) then
				noticeMessage succeeded "Remove All Faces From CrossSection" reason
			)
		)
		
		-------------------------------------------------------------------------		
		group "Export"
		(
			button exportFBXAndJsonUI "Export FBX and JSON" width:200
		)

		on exportFBXAndJsonUI pressed do
		(
			succeeded = true
			reason = ""
			
			if(not selection.count==1) then
			(
				succeeded = false
				reason = "Please select at 1 and only 1 mesh."
				
				noticeMessage succeeded "Export FBX And Json" reason
			)
			else
			(
				for meshObj in selection do
				(
					if(not ((classof meshObj)==Editable_mesh)) then
					(
						succeeded = false
						reason = "selected Mesh is not and Editable Mesh."
						
						continue
					)
					
					fbxFilePath = getSaveFileName types:"FBX(*.fbx)|*.fbx" historyCategory:"Demolition Exporter"
					if(fbxFilePath==undefined) then
					(
						succeeded = false
						reason = "Please select a fbx file to save"

						continue
					)
					
					/*
					jsonFilePath = getSaveFileName types:"JSON(*.json)|*.json"
					if(jsonFilePath==undefined) then
					(
						messageBox "Please select a json file to save"
			
						return false
					)
					*/
					jsonFilePath = (getFilenamePath fbxFilePath) + (getFilenameFile fbxFilePath) + ".json"
		
					if( not (demolitionTool.exportFBXAndJson meshObj mapChannelUI.value maxMeshGroupUI.value fbxFilePath jsonFilePath)) then
					(
						succeeded = false
						reason = "exportFBXAndJson failed."

						continue
					)
				)

				noticeMessage succeeded "Export FBX And Json" reason
			)
		)
	)
	
	createDialog DemolitionToolUI_Panel height:500 width:400 pos:[150,200]
)