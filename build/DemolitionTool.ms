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
	faceIndices=#(), 
	bound = BoundingBox [0, 0, 0],
	center=[0, 0, 0]
)

struct JSONExporter
(
	fn formatVector3 fptr v = 
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
			formatVector3 fptr bound.size
		format "\t\t\t}\n" to:fptr
	),
	
	fn formatFaceGroup fptr faceGroupValue = 
	(
		format "\t\t{\n" to:fptr
			format "\t\t\t\"groupID\":%,\n" faceGroupValue.groupID to:fptr
			format "\t\t\t\"bound\":\n" to:fptr
			
			formatFaceBoundingBox fptr faceGroupValue.bound
		format "\t\t}\n" to:fptr
	),
	
	fn formatFaceGroups fptr faceGroups = 
	(
		groupIdx = 0
		
		format "\t\"faceGroups\":\n" to:fptr
		format "\t[\n" to:fptr
		
		for faceGroupKeyValue in faceGroups do
		(
			formatFaceGroup fptr faceGroupKeyValue.value

			groupIdx += 1;
			if(groupIdx<faceGroups.count) then
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
	
	fn createConvexHull meshObj numVertex hullScale borderScale as_Unreal_Collision = 
	(
		-- First, delete old hull
		(
			hull_name = meshObj.name + " hull"
			ucx_name = "UCX_" + meshObj.name
			
			old_hull = getnodebyname hull_name
			if old_hull != undefined then 
			(
				delete old_hull
			)

			old_hull = getnodebyname ucx_name
			if old_hull != undefined then
			(
				delete old_hull
			)
		)
		
		-- MassFX is able to generate the convex hull.  Here we add the modifier
		Mass_FX_Modifier = MassFX_RBody()
		addModifier meshObj Mass_FX_Modifier
		
		-- Assign the max vert and buffer value from the spinner to the modifier
		Mass_FX_Modifier.meshVerticesLimit = numVertex
		Mass_FX_Modifier.meshInflation = borderScale
		
		-- Now extract the mesh and delete the modifier
		Mass_FX_Modifier.RBMeshCustomNodeExtract 1
		deleteModifier meshObj 1
		
		-- Scale the hull
		hull = getnodebyname hull_name
		hull.pivot = hull.center
		hull.scale = [hullScale,hullScale,hullScale]
		
		-- Name the hull UCX_ selectedmesh if it is for an unreal collision box.
		if as_Unreal_Collision == True then
		(
			hull.name = ucx_name
		)

		-- delete hull
	)		

	----------------------------------------------------------
	,
	
	fn computeFaceGroups meshObj =
	(
		assert(meshObj!=undefined)

		currentGroup = -1
		
		faceIDArray = #()
		faceGroups = Dictionary(#integer)
		for i = 1 to meshObj.numfaces do
		(
			if(faceIDArray[i]==undefined) then
			(
				currentGroup = currentGroup + 1;
				faceGroups[currentGroup] = (FaceGroup currentGroup)
			)

			elementFaceIndices = meshop.GetElementsUsingFace meshObj i as Array
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
	,
		
	----------------------------------------------------
	fn computeSphere vertexPositions =
	(
	)
	,
		
	fn computeCapsule vertexPositions =
	(
		for vertexPosition in vertexPositions do
		(	
		)
	)
	,
	
	fn computeAABB vertexPositions =
	(
		minV = [999999, 999999, 999999]
		maxV = [-999999, -999999, -999999]

		for vertexPosition in vertexPositions do
		(
			minV = vector3Min minV vertexPosition
			maxV = vector3Max maxV vertexPosition
		)
		center = (maxV + minV) * 0.5
		size = maxV - minV
	
		return BoundingBox center size
	)
	,	
	
	fn computeConvexHull vertexPositions =
	(
		for vertexPosition in vertexPositions do
		(	
		)
	)
	,
	
	fn setFaceGroupBound meshObj faceGroupValue =
	(
		vertexPositions = #()
		for faceIdx in faceGroupValue.faceIndices do
		(		
			vertexIdxs = meshop.getFace meshObj faceIdx
		
			for v = 1 to vertexIdxs.count do
			(
				append vertexPositions (meshop.getVert meshObj vertexIdxs[v])
			)
		)
		
		--faceGroupValue.bound = computeSphere vertexPositions
		--faceGroupValue.bound = computeCapsule vertexPositions
		--faceGroupValue.bound = computeConvexHull vertexPositions
		faceGroupValue.bound = computeAABB vertexPositions
	)
	,

	fn setMeshFaceGroupBounds meshObj maxGroupCount faceGroups = 
	(
		for faceGroupKeyValue in faceGroups do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value

			setFaceGroupBound meshObj faceGroupValue
		)
		
		return true
	)
	,

	------------------------------------	
	fn setFaceGroupID meshObj vertexPerFace useMapChannel faceGroupValue =
	(
		for faceIdx in faceGroupValue.faceIndices do
		(		
			vertexIdxs = [(faceIdx-1) * vertexPerFace + 1, (faceIdx-1) * vertexPerFace + 2, (faceIdx-1) * vertexPerFace + 3]
			meshop.setMapFace meshObj useMapChannel faceIdx vertexIdxs
		
			for v = 1 to vertexIdxs.count do
			(
				p = meshop.getMapVert meshObj useMapChannel vertexIdxs[v]
				p.x = faceGroupValue.groupID
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
		
		for faceGroupKeyValue in faceGroups do
		(
			faceGroupKey = faceGroupKeyValue.key
			faceGroupValue = faceGroupKeyValue.value
			
			setFaceGroupID meshObj vertexPerFace useMapChannel faceGroupValue
		)
		
		return true
	)
	,
	
	----------------------------------------------------
	fn computeMeshFaceGroups meshObj useMapChannel maxGroupCount = 
	(
		faceGroups = computeFaceGroups meshObj
		if(faceGroups.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroups

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroups
		
		return true
	)
	,
	
	----------------------------------------------------
	fn convertToFacesIndices meshObj =
	(
		result = #()
		for face in meshObj.faces do
		(
			append result face.index
		)
		
		return result
	)
	,

	fn clearMeshFaceGroups meshObj useMapChannel maxGroupCount = 
	(
		faceGroups = Dictionary(#integer)
		faceGroups[0] = (FaceGroup 0 (convertToFacesIndices meshObj))
		if(faceGroups.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroups

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroups
		
		return true
	)
	,
	
	-- ================================================================================
	fn setCrossSectionFace meshObj useMapChannel faces isCrossSection = 
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
	
	fn addSelectedToCrossSection meshObj useMapChannel = 
	(
		return setCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 1
	)
	,
	
	fn removeSelectedFromCrossSection meshObj useMapChannel = 
	(
		return setCrossSectionFace meshObj useMapChannel meshObj.selectedFaces 0
	)
	,
	
	fn clearAllCrossSection meshObj useMapChannel = 
	(
		return setCrossSectionFace meshObj useMapChannel meshObj.faces 0
	),
	
	fn exportDemolition meshObj useMapChannel maxGroupCount = 
	(
		jsonFilePath = getSaveFileName types:"Json(*.json)|*.json"
		if(jsonFilePath==undefined) then
			return false
		
		faceGroups = computeFaceGroups meshObj
		if(faceGroups.count>=maxGroupCount) then
		(
			messageBox "Maximiun group count reached, Reduce Elements count of the mesh"
			
			return false
		)
		
		setMeshFaceGroupIDs meshObj useMapChannel maxGroupCount faceGroups

		setMeshFaceGroupBounds meshObj maxGroupCount faceGroups

		print faceGroups
		formatJSON jsonFilePath faceGroups
		
		return true
	)
)

demolitionTool = DemolitionTool()

macroScript DemolitionToolUI category:"Tencent" toolTip:"Demolition Tool"
(
	rollout DemolitionToolUI_Panel "DemolitionTool"
	(
		/*
		group "Convex Hull Params"
		(
			spinner convexHullVertexCountUI "Convex Hull Vertex Count:" range:[1,1000, 32] type:#integer
			spinner convexHullScalarUI "Convex Hull Scaler:" range:[0.1,1000, 1]
			spinner convexHullBorderSizeUI "Convex Hull Border Size:" range:[-1000,1000, 0]			
		
			button createConvexHullUI "Create Convex Hull"		
		)
		*/		
		/*
		on createConvexHullUI pressed do
		(
			demolitionTool.createConvexHull $ convexHullVertexCountUI.value convexHullScalarUI.value convexHullBorderSizeUI.value false
		)
		*/
				
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
		
		group "Export"
		(
			button exportDemolitionUI "Export Demolition"
		)

		on exportDemolitionUI pressed do
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

					if( not (demolitionTool.exportDemolition meshObj mapChannelUI.value 32)) then
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
				
		
		on computeMeshFaceGroupsUI pressed do
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

					if( not (demolitionTool.computeMeshFaceGroups meshObj mapChannelUI.value 32)) then
					(
						succeeded = false
						break
					)
				)

				if(succeeded) then
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
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
				
					if( not (demolitionTool.clearMeshFaceGroups meshObj mapChannelUI.value 32)) then
					(
						succeeded = false
						break
					)					
				)
				
				if(succeeded) then
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
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
				
					if( not (demolitionTool.addSelectedToCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						break
					)	
				)
				
				if(succeeded) then
					messageBox "Selected Faces are added as Cross Section, succeeded"
				else
					messageBox "Selected Faces are added as Cross Section, failed"
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
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
				
					if( not (demolitionTool.removeSelectedFromCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						break
					)	
				)
				
				if(succeeded) then
					messageBox "Selected Faces are removed from Cross Section, succeeded"
				else
					messageBox "Selected Faces are removed from Cross Section, failed"	
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
				succeeded = true
				for meshObj in $* do
				(
					convertToMesh meshObj
				
					if( not (demolitionTool.clearAllCrossSection meshObj mapChannelUI.value)) then
					(
						succeeded = false
						break
					)	
				)
				if(succeeded) then
					messageBox "all Faces are removed from Cross Section, succeeded"
				else
					messageBox "all Faces are removed from Cross Section, failed"
			)
		)
	)
	
	createDialog DemolitionToolUI_Panel height:300 width:400 pos:[150,200]
)