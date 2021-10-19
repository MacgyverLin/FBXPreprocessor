import maya.cmds as cmds

class DemolishableTool:
    empCount = 0

    def __init__(self, name, salary):
        self.name = name
        self.salary = salary
        self.window = None
        DemolishableTool.empCount += 1

    def fun0(self):
        print('fun0')
        
    def fun1(self):
        print('fun1')

    def fun2(self):
        print('fun2')

    def fun3(self):
        print('fun3')

    def fun4(self):
        print('fun4')
        
    def getSelectedNode(self):
        return cmds.ls(long=True, selection=True, type='dagNode')

    def messageBox(self, msg):
        cmds.confirmDialog(message=msg, button=["ok"])
        
    def noticeMessage(self, succeeded, msg, reason):
        s = ""

        if(not reason==""):
            reason = ", " + reason
				
        if(succeeded):
            s = "Succeded in {}{}\n".format(msg, reason)
        else:
            s = "Failed To {}{}\n".format(msg, reason)

        self.messageBox(s)
		
    def makeSpinner(self, caption, value, minValue, maxValue, cbIntField, enable=True):
        cmds.rowLayout(numberOfColumns=3, columnWidth3=(100, 100, 300), columnAlign3=("left", "left", "left"))
        cmds.text(caption)
        spin = cmds.intField(changeCommand = (cbIntField.__name__ + '(' + ')'), v=value, enable=enable)
        cmds.intSlider( min=minValue, max=maxValue, value=value, step=1, width=200, enable=enable)
        cmds.setParent( '..' )
        return spin

    def makeButtonSpinner(self, caption, value, minValue, maxValue, cbButton, cbIntField, enable = True):
        cmds.rowLayout(numberOfColumns=3, columnWidth3=(100, 100, 300), columnAlign3=("left", "left", "left"))
        but = cmds.button(label=caption, command=(cbButton.__name__ + '(' + ')'), enable=enable)
        spin = cmds.intField(changeCommand = (cbIntField.__name__ + '(' + ')'), v = value, enable=enable)
        cmds.intSlider( min=minValue, max=maxValue, value=value, step=1, width=200 )
        cmds.setParent( '..' )
        return

    def setMapChannel(self, item):
        print(item)
        
    def makeOptionMenu(self, caption, items, cbItem):
        cmds.optionMenu(label=caption, changeCommand = cbItem)
        if(not items==None):
            for item in items:
                cmds.menuItem(label = item)

    def show(self):
        self.window = cmds.window(title="Demolishable Tool", iconName="Short Name")
        cmds.columnLayout(adjustableColumn=True)

        ###############
        cmds.frameLayout(label="Map Channel", collapsable=True)
        cmds.polyUVSet(allUVSets=True, q=1)
        cmds.columnLayout(columnAttach=("both", 5), rowSpacing=5, columnWidth=500)
        self.maxMeshGroupUI = self.makeSpinner('Max Mesh Group', 32, 1, 32, self.fun0, enable=False)
        self.mapChannelUI = self.makeSpinner('map Channel', 3, 1, 8, self.fun1)

        #makeSpinner('Map Channel', 3, 1, 8, fun1)
        polyUVSets = cmds.polyUVSet( query=True, allUVSets=True)
        self.makeOptionMenu('Map Channel', polyUVSets, self.setMapChannel)
        cmds.button(label="Initialize Mesh", command="self.initializeMeshUI()")
        cmds.setParent( '..' )
        cmds.setParent( '..' )

        ###############
        cmds.frameLayout(label="Mesh Face Group", collapsable=True)
        cmds.columnLayout(columnAttach=("both", 5), rowSpacing=5, columnWidth=500)
        cmds.button(label="Compute By Mesh Element", command="computeByMeshElement()")
        self.makeSpinner('Set ID:', 1, 1, 32, self.fun2)
        self.makeButtonSpinner('Select ID', 3, 1, 8, self.fun3, self.fun4)
        cmds.checkBox(label='Clear Selection', v = True )
        cmds.setParent( '..' )
        cmds.setParent( '..' )

        ###############
        cmds.frameLayout(label="Cross Section", collapsable=True)
        cmds.columnLayout(columnAttach=("both", 5), rowSpacing=5, columnWidth=500)
        cmds.button(label="Select Cross Section Face", command="selectCrossSectionFace()")
        cmds.button(label="Add Selected Faces To Cross Section", command="addSelectedFacesToCrossSection()")
        cmds.button(label="Remove Selected Faces To Cross Section", command="removeSelectedFacesToCrossSection()")
        cmds.button(label="Remove All Faces To Cross Section", command="removeAllFacesToCrossSection()")
        cmds.setParent( '..' )
        cmds.setParent( '..' )

        ###############
        cmds.frameLayout(label="Export", collapsable=True)
        cmds.columnLayout(columnAttach=("both", 5), rowSpacing=5, columnWidth=500)
        cmds.button(label="Export FBX and JSON", command="exportFBXandJSON()")
        cmds.setParent( '..' )
        cmds.setParent( '..' )

        cmds.showWindow( self.window )
        
    def initializeMesh(self, meshObj, useMapChannel, maxGroupCount):
        print('initializeMesh')

    def computeByMeshElement(self):
        print('computeByMeshElement')

    def selectCrossSectionFace(self):
        print('selectCrossSectionFace')

    def addSelectedFacesToCrossSection(self):
        print('addSelectedFacesToCrossSection')

    def removeSelectedFacesToCrossSection(self):
        print('removeSelectedFacesToCrossSection')

    def removeAllFacesToCrossSection(self):
        print('removeAllFacesToCrossSection')

    def exportFBXandJSON(self):
        # cmds.polySphere()
        # cmds.addAttr( longName='DemolishableData', dataType='string' )
        # cmds.setAttr( 'pSphere1.DemolishableData', '{"faceGroups":[{"groupID":0,"density":1,"bound":{"center":{"x":0.0,"y":0.0,"z":0.5},"size":{"x":0.5,"y":0.5,"z":0.5}}}]}', type='string')
        print('exportFBXandJSON')

    def isMeshType(self, meshObj):
		return cmds.objectType(meshObj, isType="mesh")
        
	def initializeMeshUI(self):
		selection = self.getSelectedNode()

		succeeded = True
		reason = ""

		if not len(selection)==1:
			succeeded = False
			reason = "Please select at 1 and only 1 mesh."
    
			self.noticeMessage(succeeded, "InitializeMesh", reason)
		else:
			for meshObj in selection:
				print(meshObj)
				if(not self.isMeshType(meshObj)):
					succeeded = False
					reason = "selected Mesh is not and Editable Mesh."
					continue

				if( not (self.initializeMesh(meshObj, mapChannelUI.value, maxMeshGroupUI.value))):
					succeeded = False
					reason = "initializeMesh failed."
					continue

				self.noticeMessage(succeeded, "InitializeMesh", reason)

demolishableTool = DemolishableTool('1', '2')
demolishableTool.initializeMeshUI()