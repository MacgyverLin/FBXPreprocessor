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
	
	fn setMeshGroupID obj useMapChannel maxGroupCount = 
	(
		faceGroupMap = computeFaceGroupMap obj
		
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		-- print maxChannel
		-- print useMapChannel
		-- print numFaces
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true
		
		vertexPerFace = 3
		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for faceGroup in faceGroupMap do
		(
			print faceGroup.key
			for faceIdx in faceGroup.value do
			(
				vertexIds = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
				meshop.setMapFace obj useMapChannel faceIdx vertexIds
				
				vertexIdxs = (meshop.getMapFace obj useMapChannel faceIdx)
				for v = 1 to vertexIdxs.count do
				(
					vertexIdx = vertexIdxs[v]
					
					p = meshop.getMapVert obj useMapChannel vertexIdx
					p.x = mod faceGroup.key maxGroupCount
					meshop.setMapVert obj useMapChannel vertexIdx p
				)
			)
		)
		
		return true
	)
	
	fn markInternalFace obj useMapChannel = 
	(
		faceGroupMap = computeFaceGroupMap obj
		
		maxChannel = meshop.getNumMaps obj
		numFaces = meshop.getNumFaces obj
		
		-- print maxChannel
		-- print useMapChannel
		-- print numFaces
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps obj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces obj useMapChannel keep:true
		
		vertexPerFace = 3
		meshop.setNumMapVerts obj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for faceIdx = 1 to numFaces do
		(
			vertexIds = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace obj useMapChannel faceIdx vertexIds
			
			vertexIdxs = (meshop.getMapFace obj useMapChannel faceIdx)
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert obj useMapChannel vertexIdxs[v]
				p.y = 0
				meshop.setMapVert obj useMapChannel vertexIdxs[v] p
			)			
		)
		
		for face in obj.selectedFaces do
		(
			faceIdx = face.index
			
			vertexIds = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace obj useMapChannel faceIdx vertexIds
			
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
	
	group "MapChannel"
	(
		spinner mapChannelUI "Map Channel:" range:[1,8,3] type:#integer enabled:true
	)
		
	group "Set GroupID"
	(
		button setGroupIDUI "Compute"
	)
	
	group "Mark Internal Face"
	(
		button markInternalFaceUI "Mark"
	)	

	on setGroupIDUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			setMeshGroupID obj mapChannelUI.value 16

			messageBox "Export successful"
			
			--filePath = getSaveFileName types:"Json(*.json)|*.json"
			--filePath = "C:/Users/guanronlin/Desktop/3.json"
			--if(exportJSON filePath faceGroupMap) then
				--messageBox "Export successful"
			--else
				--messageBox "Export not successful"
		)
	)
	
	on markInternalFaceUI pressed do
	(
		if($==undefined) then
		(
			messageBox "Please select at least 1 mesh."
		)
		else
		(
			obj = $

			convertToMesh obj
			
			markInternalFace obj mapChannelUI.value

			messageBox "Export successful"
			
			--filePath = getSaveFileName types:"Json(*.json)|*.json"
			--filePath = "C:/Users/guanronlin/Desktop/3.json"
			--if(exportJSON filePath faceGroupMap) then
				--messageBox "Export successful"
			--else
				--messageBox "Export not successful"
		)
	)
)