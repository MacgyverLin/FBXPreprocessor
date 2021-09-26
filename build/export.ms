utility ExportFBXWithElement "Export Fbx With Element"
(
	/*
	running from commandline
	https://knowledge.autodesk.com/support/3ds-max/getting-started/caas/CloudHelp/cloudhelp/2021/ENU/3DSMax-Basics/files/GUID-BCB04DEC-7967-4091-B980-638CFDFE47EC-htm.html	
	*/
	fn computeFaceGroupMap obj =
	(
		assert(obj!=undefined)

		currentGroup = -1
		
		faceIDArray = #()
		faceGroupMap = Dictionary(#integer)
		for i = 1 to obj.numfaces do
		(
			if(faceIDArray[i]==undefined) then
			(
				currentGroup = currentGroup + 1;
				faceGroupMap[currentGroup] = #()
			)

			elementFaceIndices = meshop.GetElementsUsingFace obj i as Array
			for j = 1 to elementFaceIndices.count do
			(
				elementFaceIndex = elementFaceIndices[j]
				if(faceIDArray[elementFaceIndex]==undefined) then
				(
					faceIDArray[elementFaceIndex] = currentGroup
					appendIfUnique faceGroupMap[currentGroup] elementFaceIndex
				)
			)
		)
		
		for faceID in faceIDArray do
		(
			--format "% %\n" faceIdx faceIDArray[faceIdx]
		)
				
		for faceGroup in faceGroupMap do
		(
			--format "group:%, count:%, values:%\n" faceGroup.key faceGroup.value.count faceGroup.value
		)
		
		return faceGroupMap
	)
	
	fn exportJSON filename faceGroupMap = 
	(
		fptr = openFile filename mode:"wt"
		if(fptr==undefined) then
			return false
	
		groupIdx = 0

		format "[\n" to:fptr
		for faceGroup in faceGroupMap do
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
			if(groupIdx<faceGroupMap.count) then
			format "," to:fptr
					
			format "\n" to:fptr
		)
		
		format "]\n" to:fptr
		
		close fptr
		
		return true
	)
	
	fn setFaceGroupID vertexPerFace faceIdx useMapChannel groupID =
	(
		vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
		meshop.setMapFace obj useMapChannel faceIdx vertexIdxs
		
		for v = 1 to vertexIdxs.count do
		(
			p = meshop.getMapVert obj useMapChannel vertexIdxs[v]
			p.x = groupID
			meshop.setMapVert obj useMapChannel vertexIdxs[v] p
		)
	)
	
	fn computeMeshGroupID obj useMapChannel maxGroupCount = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		faceGroupMap = computeFaceGroupMap obj
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true

		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		print faceGroupMap.count
		for faceGroup in faceGroupMap do
		(
			groupID = (mod faceGroup.key maxGroupCount)
			format "% %\n"groupID faceGroup.value
			for faceIdx in faceGroup.value do
			(
				setFaceGroupID vertexPerFace faceIdx useMapChannel groupID
			)
		)
		
		return true
	)
	
	
	fn addCrossSectionFace obj useMapChannel = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		faceGroupMap = computeFaceGroupMap obj
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true
		
		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for face in obj.selectedFaces do
		(
			faceIdx = face.index
			
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace obj useMapChannel faceIdx vertexIdxs
			
			vertexIdxs = (meshop.getMapFace obj useMapChannel faceIdx)
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert obj useMapChannel vertexIdxs[v]
				p.y = 1
				meshop.setMapVert obj useMapChannel vertexIdxs[v] p
			)			
		)		
		
		
		return true
	)
	
	fn removeCrossSectionFace obj useMapChannel = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		faceGroupMap = computeFaceGroupMap obj
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true
		
		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for face in obj.selectedFaces do
		(
			faceIdx = face.index
			
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace obj useMapChannel faceIdx vertexIdxs
			
			vertexIdxs = (meshop.getMapFace obj useMapChannel faceIdx)
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert obj useMapChannel vertexIdxs[v]
				p.y = 0
				meshop.setMapVert obj useMapChannel vertexIdxs[v] p
			)			
		)		
		
		return true
	)
	
	
	fn clearAllCrossSectionFace obj useMapChannel = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		faceGroupMap = computeFaceGroupMap obj
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true
		
		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for faceIdx = 1 to numFaces do
		(
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace obj useMapChannel faceIdx vertexIdxs
			
			vertexIdxs = (meshop.getMapFace obj useMapChannel faceIdx)
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert obj useMapChannel vertexIdxs[v]
				p.y = 0
				meshop.setMapVert obj useMapChannel vertexIdxs[v] p
			)			
		)
		
		return true
	)		
	
	group "MapChannel"
	(
		spinner mapChannelUI "Map Channel:" range:[1,8,3] type:#integer enabled:true
	)
		
	group "GroupID"
	(
		button groupIDComputeUI "Compute"
	)
	
	group "Cross Section Face"
	(
		button crossSectionFaceAddSelectedUI "Add Selected Face"
		button crossSectionFaceRemoveSelectedUI "Remove Selected Face"
		button crossSectionFaceClearAllUI "Clear All"
	)

	on groupIDComputeUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			computeMeshGroupID obj mapChannelUI.value 16

			messageBox "Compute successful"
		)
	)
	
	on crossSectionFaceAddSelectedUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			addCrossSectionFace obj mapChannelUI.value

			messageBox "Export successful"
		)
	)
	
	on crossSectionFaceRemoveSelectedUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			removeCrossSectionFace obj mapChannelUI.value

			messageBox "Export successful"
		)
	)
	
	on crossSectionFaceClearAllUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			clearAllCrossSectionFace obj mapChannelUI.value

			messageBox "Export successful"
		)
	)		
)