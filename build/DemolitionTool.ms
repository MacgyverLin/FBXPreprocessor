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
	
	fn formatFaceGroups fptr faceGroups = 
	(
		groupIdx = 0
		
		format "\t\"faceGroups\":\n" to:fptr
		format "\t[\n" to:fptr
		
		for i = 1 to faceGroups.count do
		(
			formatFaceGroup fptr faceGroups[i]

			groupIdx += 1;
			if(groupIdx < faceGroups.count) then
				format "\t\t,\n" to:fptr
			else
				format "" to:fptr
		)
		
		format "\t]\n" to:fptr
	),	
	
	
	fn formatDemolitionData fptr faceGroups = 
	(
		format "{\n" to:fptr
		
		formatFaceGroups fptr faceGroups
		
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
	
	fn formatJSON filename faceGroups = 
	(
		fptr = openFile filename mode:"wt"
		if(fptr==undefined) then
			return false

		jsonExporter.formatDemolitionData fptr faceGroups
		
		close fptr
		
		return true
	),
	
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
	
	----------------------------------------------------------
	fn computeFaceGroups meshObj =
	(
		assert(meshObj!=undefined)

		currentGroup = -1
		
		faceIDArray = #()
		faceGroupMap = Dictionary(#integer)
		for i = 1 to meshObj.numfaces do
		(
			if(faceIDArray[i]==undefined) then
			(
				currentGroup = currentGroup + 1;
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
		
		faceGroups = #()
		for faceGroupKeyValue in faceGroupMap do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value

			faceGroups[faceGroupKey+1] = faceGroupValue
		)
		
		for i = 1 to faceGroups.count do
		(
			format "% % %\n" i faceGroups[i].groupID faceGroups[i]
		)
		
		return faceGroups
	)
	,
		
	----------------------------------------------------
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
		meshInvTransform = (inverse $.transform)
		
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

	fn setMeshFaceGroupBounds meshObj maxGroupCount faceGroups = 
	(
		for i = 1 to faceGroups.count do
		(
			setFaceGroupBound meshObj faceGroups[i]
		)
		
		return true
	)
	,

	------------------------------------	
	fn setFaceGroupID meshObj vertexPerFace useMapChannel faceIndices groupID =
	(
		for faceIdx in faceIndices do
		(		
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace meshObj useMapChannel faceIdx vertexIdxs
		
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert meshObj useMapChannel vertexIdxs[v]
				p.x = groupID
				meshop.setMapVert meshObj useMapChannel vertexIdxs[v] p
			)
		)
	)
	,

	fn setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroups = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps meshObj
		numFaces = meshop.getNumFaces meshObj
		
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps meshObj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces meshObj useMapChannel keep:true

		meshop.setNumMapVerts meshObj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for i = 1 to faceGroups.count do
		(
			setFaceGroupID meshObj vertexPerFace useMapChannel faceGroups[i].faceIndices faceGroups[i].groupID
		)
		
		return true
	)
	,
	
	-------------------------------------------------------------------
	-- internal Face
	fn setMeshCrossSectionFace meshObj useMapChannel faces isCrossSection = 
	(
		vertexPerFace = 3
		maxChannel = meshop.getNumMaps meshObj
		numFaces = meshop.getNumFaces meshObj
		
		if(useMapChannel>=maxChannel) then
			meshop.setNumMaps meshObj (useMapChannel+1) keep:true
		
		meshop.buildMapFaces meshObj useMapChannel keep:true
		
		meshop.setNumMapVerts meshObj useMapChannel (numFaces * vertexPerFace) keep:true
		
		for face in faces do
		(
			faceIdx = face.index
			
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace meshObj useMapChannel faceIdx vertexIdxs
			
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
	fn initializeMesh meshObj useMapChannel maxGroupCount = 
	(
		faceGroups = computeFaceGroups meshObj
		if(faceGroups.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroups

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroups
		
		setMeshCrossSectionFace meshObj useMapChannel meshObj.faces 0
		
		return true
	)
	,
	
	fn addSelectedFacesToCrossSection meshObj useMapChannel = 
	(
		setMeshCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 1
	)
	,
	
	fn removeAllFacesFromCrossSection meshObj useMapChannel = 
	(
		setMeshCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 0
	)
	,
	
	fn removeSelectedFacesFromCrossSection meshObj useMapChannel = 
	(
		setMeshCrossSectionFace meshObj useMapChannel meshObj.faces 0
	),
	
	fn exportFBXAndJson meshObj useMapChannel maxGroupCount fbxFilePath jsonFilePath = 
	(
		faceGroups = computeFaceGroups meshObj
		if(faceGroups.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroups

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroups

		if(not exportFile fbxFilePath using:FBXEXP) then
		(
			messageBox "Failed to Export fbx file"
			return false
		)		
		else
		(
			messageBox "Success to Export fbx file"
		)
		
		if(not formatJSON jsonFilePath faceGroups) then
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
		)		
		
		-------------------------------------------------------------------------		
		group "Mesh Face Groups"
		(
		)
		
		on initializeMeshUI pressed do
		(
			if($==undefined) then
			(
				messageBox "Please select at least 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
					
					if( not (demolitionTool.initializeMesh meshObj mapChannelUI.value maxMeshGroupUI.value)) then
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
		group "Cross Section"
		(
			button addSelectedFacesToCrossSectionUI "Add Selected Face" width:200
			button removeSelectedFacesFromCrossSectionUI "Remove Selected Face" width:200
			button removeAllFacesFromCrossSectionUI "Clear All" width:200
		)
		
		on addSelectedFacesToCrossSectionUI pressed do
		(
			if($==undefined) then
			(
				messageBox "Please select at least 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
				
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
			if($==undefined) then
			(
				messageBox "Please select at least 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
				
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
			if($==undefined) then
			(
				messageBox "Please select at least 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
				
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
			button exportFBXAndJsonUI "Export FBX and JSON"
		)

		on exportFBXAndJsonUI pressed do
		(
			if($==undefined) then
			(
				messageBox "Please select at least 1 mesh."
			)
			else
			(
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj

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
	
	createDialog DemolitionToolUI_Panel height:300 width:400 pos:[150,200]
)