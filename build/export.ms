utility ExportFBXWithElement "Export Fbx With Element"
(
	/*
	running from commandline
	https://knowledge.autodesk.com/support/3ds-max/getting-started/caas/CloudHelp/cloudhelp/2021/ENU/3DSMax-Basics/files/GUID-BCB04DEC-7967-4091-B980-638CFDFE47EC-htm.html	
	*/
	fn exportJSON filename faceGroups = 
	(
		fptr = openFile filename mode:"wt"
		if(fptr==undefined) then
			return false
	
		groupIdx = 0

		format "[\n" to:fptr
		for faceGroup in faceGroups do
		(
			format "\t{\n" to:fptr
			
				format "\t\t\"groupID\":%,\n" faceGroup.key to:fptr
			
					format "\t\t\"faceIndices\":[" to:fptr
					for i = 1 to faceGroup.value.count do
					(
						format "%" faceGroup.value[i] to:fptr
						if(i<faceGroup.value.count) then
						format ", " to:fptr
					)

				format "]\n" to:fptr
						
			format "\t}" to:fptr

			groupIdx += 1;						
			if(groupIdx<faceGroups.count) then
			format "," to:fptr
					
			format "\n" to:fptr
		)
		
		format "]\n" to:fptr
		
		close fptr
		
		return true
	)
	
	-------------------------------------------------------------------------
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
	
	fn computeAABBTest obj = 
	(
		format "------------\n"
		
		minV = obj.min
		maxV = obj.max
		center = (obj.min + obj.max) / 2
		
		format "% % %\n" center minV maxV
		
		return true
	)	
	
	fn computeAABB obj = 
	(
		format "------------\n"
		
		minV = [999999, 999999, 999999]
		maxV = [-999999, -999999, -999999]
		center = [0, 0, 0]
		vertexcount = 0
		
		for face in obj.faces do
		(
			faceIdx = face.index
			vertexIdxs = (meshop.getFace obj faceIdx)
			
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getVert obj vertexIdxs[v]
				center = center + p
				vertexcount = vertexcount + 1
				
				minV = vector3Min minV p
				maxV = vector3Max maxV p
			)
		)
		
		if(vertexcount) then
			center = center / vertexcount;
		
		format "% % %\n" center minV maxV
		
		return true
	)
	
	struct BoundingCapsule
	(
		center, 
		height=2, 
		radius=0.5
	)
	
	struct FaceGroup
	(
		groupID, 
		faceIndices=#(), 
		bound=(BoundingCapsule [0, 0, 0])
	)
  
	fn computeFaceGroups obj =
	(
		assert(obj!=undefined)

		currentGroup = -1
		
		faceIDArray = #()
		faceGroups = Dictionary(#integer)
		for i = 1 to obj.numfaces do
		(
			if(faceIDArray[i]==undefined) then
			(
				currentGroup = currentGroup + 1;
				faceGroups[currentGroup] = (FaceGroup currentGroup)
			)

			elementFaceIndices = meshop.GetElementsUsingFace obj i as Array
			for j = 1 to elementFaceIndices.count do
			(
				elementFaceIndex = elementFaceIndices[j]
				if(faceIDArray[elementFaceIndex]==undefined) then
				(
					faceIDArray[elementFaceIndex] = currentGroup
					appendIfUnique faceGroups[currentGroup].faceIndices elementFaceIndex
				)
			)
		)
		
		return faceGroups
	)
		
	----------------------------------------------------
	fn computeCapsule vertexPositions =
	(
		for vertexPosition in vertexPositions do
		(	
		)
	)
	
	fn computeOBB vertexPositions =
	(
		for vertexPosition in vertexPositions do
		(	
		)
	)	
	
	fn computeConvexHull vertexPositions =
	(
		for vertexPosition in vertexPositions do
		(	
		)
	)		
	
	fn setFaceGroupBound obj faceGroupValue =
	(
		vertexPositions = #()
		for faceIdx in faceGroupValue.faceIndices do
		(		
			vertexIdxs = meshop.getFace obj faceIdx
		
			for v = 1 to vertexIdxs.count do
			(
				append vertexPositions (meshop.getVert obj vertexIdxs[v])
			)
		)
		
		faceGroupValue.bound = (computeCapsule vertexPositions)
		faceGroupValue.bound = (computeConvexHull vertexPositions)
		faceGroupValue.bound = (computeOBBs vertexPositions)
	)

	fn setMeshFaceGroupBounds obj maxGroupCount faceGroups = 
	(
		for faceGroupKeyValue in faceGroups do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value

			setFaceGroupBound obj faceGroupValue
		)
		
		return true
	)

	------------------------------------	
	fn setFaceGroupID obj vertexPerFace useMapChannel faceGroupValue =
	(
		for faceIdx in faceGroupValue.faceIndices do
		(		
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace obj useMapChannel faceIdx vertexIdxs
		
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert obj useMapChannel vertexIdxs[v]
				p.x = faceGroupValue.groupID
				meshop.setMapVert obj useMapChannel vertexIdxs[v] p
			)
		)
	)

	fn setMeshFaceGroupIDs obj useMapChannel maxGroupCount faceGroups = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true

		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for faceGroupKeyValue in faceGroups do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value
			
			setFaceGroupID obj vertexPerFace useMapChannel faceGroupValue
		)
		
		return true
	)
	
	----------------------------------------------------
	fn computeMeshFaceGroups obj useMapChannel maxGroupCount = 
	(
		faceGroups = computeFaceGroups obj
		if(faceGroups.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs obj useMapChannel maxGroupCount faceGroups
		
		setMeshFaceGroupBounds obj maxGroupCount faceGroups
		
		return true
	)
	
	----------------------------------------------------
	fn convertToFacesIndices obj =
	(
		result = #()
		for face in obj.faces do
		(
			append result face.index
		)
		
		return result
	)

	fn clearMeshFaceGroups obj useMapChannel maxGroupCount = 
	(
		faceGroups = Dictionary(#integer)
		faceGroups[0] = (FaceGroup 0 (convertToFacesIndices obj))
		if(faceGroups.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs obj useMapChannel maxGroupCount faceGroups
			
		setMeshFaceGroupBounds obj maxGroupCount faceGroups
		
		return true
	)	
	
	-- ================================================================================
	fn setCrossSectionFace obj useMapChannel faces isCrossSection = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true
		
		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for face in faces do
		(
			faceIdx = face.index
			
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace obj useMapChannel faceIdx vertexIdxs
			
			vertexIdxs = (meshop.getMapFace obj useMapChannel faceIdx)
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert obj useMapChannel vertexIdxs[v]
				p.y = isCrossSection
				meshop.setMapVert obj useMapChannel vertexIdxs[v] p
			)			
		)		
		
		return true
	)	
	
	fn addSelectedToCrossSection obj useMapChannel = 
	(
		return setCrossSectionFace obj useMapChannel obj.selectedFaces 1
	)
	
	fn removeSelectedFromCrossSection obj useMapChannel = 
	(
		return setCrossSectionFace obj useMapChannel obj.selectedFaces 0
	)
	
	fn clearAllCrossSection obj useMapChannel = 
	(
		return setCrossSectionFace obj useMapChannel obj.faces 0
	)		
	
	-------------------------------------------------------------------------
	group "MapChannel"
	(
		spinner mapChannelUI "Map Channel:" range:[1,8,3] type:#integer enabled:true
	)
		
	group "Mesh Face Groups"
	(
		button computeMeshFaceGroupsUI "Compute Face Groups"
		button clearMeshFaceGroupsUI "Clear Face Groups"
	)
	
	group "Cross Section"
	(
		button addSelectedToCrossSectionUI "Add Selected Face"
		button removeSelectedFromCrossSectionUI "Remove Selected Face"
		button clearAllCrossSectionUI "Clear All"
	)
	
	on computeMeshFaceGroupsUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			if( (computeMeshFaceGroups obj mapChannelUI.value 16)) then
				messageBox "Compute Mesh Face Groups succeeded."
			else
				messageBox "Compute Mesh Face Groups failed."
		)
	)
	
	on clearMeshFaceGroupsUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			if( (clearMeshFaceGroups obj mapChannelUI.value 16)) then
				messageBox "Clear Mesh Face Groups succeeded."
			else
				messageBox "Clear Mesh Face Groups failed."			
		)
	)	
	
	on addSelectedToCrossSectionUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			addSelectedToCrossSection obj mapChannelUI.value

			messageBox "Selected Faces are added as Cross Section"
		)
	)
	
	on removeSelectedFromCrossSectionUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			removeSelectedFromCrossSection obj mapChannelUI.value

			messageBox "Selected Faces are removed from Cross Section"
		)
	)
	
	on clearAllCrossSectionUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			clearAllCrossSection obj mapChannelUI.value

			messageBox "all Faces are removed from Cross Section"
		)
	)		
)