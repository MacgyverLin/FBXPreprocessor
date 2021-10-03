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
        for item in items:
            cmds.menuItem(label = item)

    def initializeMesh(self):
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
        print('exportFBXandJSON')

    def show(self):
        self.window = cmds.window(title="Demolishable Tool", iconName="Short Name")
        cmds.columnLayout(adjustableColumn=True)

        ###############
        cmds.frameLayout(label="Map Channel", collapsable=True)
        cmds.polyUVSet(allUVSets=True, q=1)
        cmds.columnLayout(columnAttach=("both", 5), rowSpacing=5, columnWidth=500)
        makeSpinner('Max Mesh Group', 32, 1, 32, fun0, enable=False)

        #makeSpinner('Map Channel', 3, 1, 8, fun1)
        polyUVSets = cmds.polyUVSet( query=True, allUVSets=True)
        makeOptionMenu('Map Channel', polyUVSets, setMapChannel)
        cmds.button(label="Initialize Mesh", command="initializeMesh()")
        cmds.setParent( '..' )
        cmds.setParent( '..' )

        ###############
        cmds.frameLayout(label="Mesh Face Group", collapsable=True)
        cmds.columnLayout(columnAttach=("both", 5), rowSpacing=5, columnWidth=500)
        cmds.button(label="Compute By Mesh Element", command="computeByMeshElement()")
        makeSpinner('Set ID:', 1, 1, 32, fun2)
        makeButtonSpinner('Select ID', 3, 1, 8, fun3, fun4)
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
        
demolishableTool = DemolishableTool('1', '2')
demolishableTool.show()