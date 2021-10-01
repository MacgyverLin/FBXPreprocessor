/*
	running from commandline
	https://knowledge.autodesk.com/support/3ds-max/getting-started/caas/CloudHelp/cloudhelp/2021/ENU/3DSMax-Basics/files/GUID-BCB04DEC-7967-4091-B980-638CFDFE47EC-htm.html	
*/
struct BoundingCapsule
(
	center, 
	height=2, 
	radius=0.5
)

struct BoundingBox
(
	center,
	size = [1, 1, 1]
)
	
struct FaceGroup
(
	groupID, 
	density = 1,
	bound = BoundingBox [0, 0, 0],
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
		vunity = toUnityVector3 v3dsmax
		format "\t\t\t\t{\n" to:fptr
			format "\t\t\t\t\t\"x\":%,\n" vunity.x to:fptr
			format "\t\t\t\t\t\"y\":%,\n" vunity.y to:fptr
			format "\t\t\t\t\t\"z\":%\n" vunity.z to:fptr
		format "\t\t\t\t}\n" to:fptr
	),
	
	fn formatVector3Unreal fptr v3dsmax = 
	(
		vunreal = toUnrealVector3 v3dsmax
		format "\t\t\t\t{\n" to:fptr
			format "\t\t\t\t\t\"x\":%,\n" vunreal.x to:fptr
			format "\t\t\t\t\t\"y\":%,\n" vunreal.y to:fptr
			format "\t\t\t\t\t\"z\":%\n" vunreal.z to:fptr
		format "\t\t\t\t}\n" to:fptr
	),

	fn formatVector3 fptr v = 
	(
		formatVector3Unity fptr v
		--formatVector3Unreal fptr v
	),	
	
	fn formatSize fptr v = 
	(
		format "\t\t\t\t{\n" to:fptr
			format "\t\t\t\t\t\"x\":%,\n" v.x to:fptr
			format "\t\t\t\t\t\"y\":%,\n" v.y to:fptr
			format "\t\t\t\t\t\"z\":%\n" v.z to:fptr
		format "\t\t\t\t}\n" to:fptr
	),		
	
	fn formatFaceBoundingBox fptr bound = 
	(
		format "\t\t\t{\n" to:fptr
			format "\t\t\t\t\"center\":\n" to:fptr
			formatVector3 fptr bound.center
			format "\t\t\t\t,\n" to:fptr
		
			format "\t\t\t\t\"size\":\n" to:fptr
			formatSize fptr bound.size
		format "\t\t\t}\n" to:fptr
	),
	
	fn formatFaceGroup fptr faceGroup = 
	(
		format "\t\t{\n" to:fptr
			format "\t\t\t\"groupID\":%,\n" faceGroup.groupID to:fptr
			format "\t\t\t\"density\":%,\n" faceGroup.density to:fptr
			format "\t\t\t\"bound\":\n" to:fptr
			
			formatFaceBoundingBox fptr faceGroup.bound
		format "\t\t}\n" to:fptr
	),
	
	fn formatFaceGroupMap fptr faceGroupMap = 
	(
		format "\t\"faceGroups\":\n" to:fptr
		format "\t[\n" to:fptr
		
		groupCount = 0
		for faceGroupKeyValue in faceGroupMap do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value
			
			formatFaceGroup fptr faceGroupValue

			groupCount += 1;
			if(groupCount < faceGroupMap.count) then
				format "\t\t,\n" to:fptr
			else
				format "" to:fptr
		)
		
		format "\t]\n" to:fptr
	),	
	
	
	fn formatDemolitionData fptr faceGroupMap = 
	(
		format "{\n" to:fptr
		
		formatFaceGroupMap fptr faceGroupMap
		
		format "}\n" to:fptr
		
		return true
	)
)
jsonExporter = JSONExporter()

struct DemolitionTool
(
	/*
	running from commandline
	https://knowledge.autodesk.com/support/3ds-max/getting-started/caas/CloudHelp/cloudhelp/2021/ENU/3DSMax-Basics/files/GUID-BCB04DEC-7967-4091-B980-638CFDFE47EC-htm.html	
	*/
	
	----------------------------------------------------------
	fn formatJSON filename faceGroupMap = 
	(
		fptr = openFile filename mode:"wt"
		if(fptr==undefined) then
			return false

		jsonExporter.formatDemolitionData fptr faceGroupMap
		
		close fptr
		
		return true
	),
	
	----------------------------------------------------------
	fn validMapChannel meshObj useMapChannel =
	(
		maxChannel = meshop.getNumMaps meshObj
		
		if(useMapChannel>=maxChannel) then
		(
			-- format "Map Channel % is allocated.\n" (useMapChannel+1)
			
			vertexPerFace = 3
			numFaces = meshop.getNumFaces meshObj			
			
			meshop.setNumMaps meshObj (useMapChannel+1) keep:true
		
			meshop.buildMapFaces meshObj useMapChannel keep:true

			meshop.setNumMapVerts meshObj useMapChannel (numFaces * vertexPerFace) keep:true
			
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
			-- format "Map Channel %  has already allocated.\n" (useMapChannel+1)
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
	
	fn computeAABB vertexPositions =
	(
		minV = [999999, 999999, 999999]
		maxV = [-999999, -999999, -999999]
		
		center = [0, 0, 0]
		for vertexPosition in vertexPositions do
		(
			center += vertexPosition
			
			minV = vector3Min minV vertexPosition
			maxV = vector3Max maxV vertexPosition
		)
		center /= vertexPositions.count
		size = maxV - center
	
		return BoundingBox center size
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
		faceGroupMap = computeFaceGroupMapByMeshElements meshObj
		if(faceGroupMap.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroupMap

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroupMap
		
		return true
	)
	,	
	
	fn selectFacesByGroupID meshObj useMapChannel groupID clearSelectedFaces =
	(
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
		vertexPerFace = 3
	
		setFaceGroupID meshObj vertexPerFace useMapChannel (convertFacesToFaceIndices meshObj.selectedfaces) groupID

		selectFacesByGroupID meshObj useMapChannel groupID true
		
		max modify mode

		subObjectLevel = 4
		
		return true
	)
	,

	---------------------------------------------------------	
	fn selectFacesByCrossSectionID meshObj useMapChannel crossSectionID clearSelectedFaces = 
	(
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
		setMeshCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 1
		
		max modify mode

		subObjectLevel = 4
		
		return true
	)
	,
	
	fn removeSelectedFacesFromCrossSection meshObj useMapChannel = 
	(
		setMeshCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 0
		
		max modify mode

		subObjectLevel = 4		
		
		return true
	)
	,
	
	fn removeAllFacesFromCrossSection meshObj useMapChannel = 
	(
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
		-------------------------------------------------------------------------
		group "MapChannel"
		(
			spinner maxMeshGroupUI "Max Mesh Group:" range:[16,32,32] type:#integer enabled:false  width:100
			spinner mapChannelUI "Map Channel:" range:[1,8,3] type:#integer enabled:true  width:100
			button initializeMeshUI "Initialize Mesh" width:200 
			
			button testUI "testUI" width:200 
		)		
		
		on testUI pressed do
		(
			/*
			openEdgeIndices = meshop.getOpenEdges $ as Array
			
			for openEdgeIdx in openEdgeIndices do
			(
				v = meshop.getVertsUsingEdge $ openEdgeIdx as Array
				--format "%\n" v
			)
			
			v = $.verts.count				
			e = $.edges.count / 2
			f = $.faces.count
			x = v - e + f
			g = (2 - x) / 2
				
			format "V %\n" v
			format "E %\n" e
			format "F %\n" f
			format "Euler characteristic %\n" x
			format "Genus %\n" g
			*/
			
			p = convertToPoly $
			if(p.capHoles #Edge flags: 2) then
				messageBox "cap ok"
			else
				messageBox "cap failed"
			p = convertToMesh $
		)
		
		on initializeMeshUI pressed do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)
					if( not (demolitionTool.initializeMesh sel mapChannelUI.value maxMeshGroupUI.value)) then
					(
						succeeded = false
						break
					)	
				)
				
				if(succeeded) then
					messageBox "Succeed in Initialize Mesh"
				else
					messageBox "Succeed in Initialize Mesh"
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
		
		on setIDValueUI entered val val2 do
		(
			succeeded = true
		
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.setSelectedFacesGroupID meshObj mapChannelUI.value (setIDValueUI.value-1))) then
					(
						succeeded = false
						break
					)	
				)
			)
			
			selectIDValueUI.value = setIDValueUI.value
			
			subobjectLevel = 4
		)
		
		on selectIDValueUI changed val do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.selectFacesByGroupID meshObj mapChannelUI.value (selectIDValueUI.value-1) clearSelectionUI.checked)) then
					(
						succeeded = false
						break
					)	
				)
			)
		)			

		on selectIDButtonUI pressed do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.selectFacesByGroupID meshObj mapChannelUI.value (selectIDValueUI.value-1) clearSelectionUI.checked)) then
					(
						succeeded = false
						break
					)	
				)
			)
		)	
		
		on computeFaceGroupByMeshElementUI pressed do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.computeFaceGroupByMeshElement meshObj mapChannelUI.value maxMeshGroupUI.value)) then
					(
						succeeded = false
						break
					)	
				)
				
				if(succeeded) then
					messageBox "Succeed in Compute FaceGroup By Mesh Element"
				else
					messageBox "Succeed in Compute FaceGroup By Mesh Element"
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
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.selectFacesByCrossSectionID meshObj mapChannelUI.value 1 crossSectionClearSelectionUI.checked)) then
					(
						succeeded = false
						break
					)	
				)
			)
		)
		
		on addSelectedFacesToCrossSectionUI pressed do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.addSelectedFacesToCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						break
					)	
				)
				
				if(succeeded) then
					messageBox "Succeed in Add Selected Faces from Cross Section"
				else
					messageBox "Failed to Add Selected Faces from Cross Section"
			)
		)
		
		on removeSelectedFacesFromCrossSectionUI pressed do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.removeSelectedFacesFromCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						break
					)	
				)
				
				if(succeeded) then
					messageBox "Succeed in Remove Selected Faces from Cross Section"
				else
					messageBox "Failed to Remove Selected Faces from Cross Section"
			)
		)
		
		on removeAllFacesFromCrossSectionUI pressed do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						break
					)					
					if( not (demolitionTool.removeAllFacesFromCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						break
					)	
				)

				if(succeeded) then
					messageBox "Succeed in Remove All Faces from Cross Section"
				else
					messageBox "Failed to Remove All Faces from Cross Section"	
			)
		)
		
		-------------------------------------------------------------------------		
		group "Export"
		(
			button exportFBXAndJsonUI "Export FBX and JSON" width:200
		)

		on exportFBXAndJsonUI pressed do
		(
			if(not selection.count==1) then
			(
				messageBox "Please select at 1 and only 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in selection do
				(
					if((classof meshObj)==Editable_Mesh) then
					(
						succeeded = false
						return false
					)
					
					fbxFilePath = getSaveFileName types:"FBX(*.fbx)|*.fbx"
					if(fbxFilePath==undefined) then
					(
						messageBox "Please select a fbx file to save"
			
						return false
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
		
					--if( not (demolitionTool.exportFBXAndJson1 meshObj mapChannelUI.value maxMeshGroupUI.value fbxFilePath jsonFilePath)) then
					if( not (demolitionTool.exportFBXAndJson meshObj mapChannelUI.value maxMeshGroupUI.value fbxFilePath jsonFilePath)) then
					(
						succeeded = false
						break
					)
				)

				if(succeeded) then
					messageBox "Export Demolition succeeded."
				else
					messageBox "Export Demolition failed."
			)
		)
	)
	
	createDialog DemolitionToolUI_Panel height:500 width:400 pos:[150,200]
)