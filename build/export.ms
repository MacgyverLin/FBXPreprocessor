utility ExportFBXWithElement "Export Fbx With Element"
(
	/*
	running from commandline
	https://knowledge.autodesk.com/support/3ds-max/getting-started/caas/CloudHelp/cloudhelp/2021/ENU/3DSMax-Basics/files/GUID-BCB04DEC-7967-4091-B980-638CFDFE47EC-htm.html	
	*/
	fn exportJSON1 filename faceGroupMap = 
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
	
	fn exportJSON filename faceGroupMap = 
	(
		groupIdx = 0

		format "[\n"
		for faceGroup in faceGroupMap do
		(
			format "\t{\n"
			
				format "\t\t\"groupID\":%,\n" faceGroup.key
			
					format "\t\t\"faceIndices\":["
					for i = 1 to faceGroup.value.count do
					(
						format "%" faceGroup.value[i]
						if(i<faceGroup.value.count) then
							format ", "
					)

				format "]\n"
						
			format "\t}"

			groupIdx += 1;						
			if(groupIdx<faceGroupMap.count) then
				format ","
					
			format "\n"
		)
		
		format "]\n"

		return true
	)
		
	
	fn GetFaceGroupMap obj =
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
		
		for faceIdx = 1 to faceIDArray.count do
		(
			--format "% %\n" faceIdx faceIDArray[faceIdx]
		)
				
		for faceGroup in faceGroupMap do
		(
			--format "group:%, count:%, values:%\n" faceGroup.key faceGroup.value.count faceGroup.value
		)
		
		return faceGroupMap
	)
		
	group "Export"
	(
		button export "Export"
		spinner mapChannel "Map Channel:" range:[1,8,2] type:#integer

		on export pressed do
		(
			if($==undefined) then
			(
				messageBox "Export UnSucessful"
			)
			else
			(
				convertToMesh $
				faceGroupMap = GetFaceGroupMap $
				
				--filePath = getSaveFileName types:"Json(*.json)|*.json"
				filePath = "C:/Users/guanronlin/Desktop/3.json"
				if(exportJSON filePath faceGroupMap) then
					messageBox "Export Sucessful"
			)
		)
	)
)