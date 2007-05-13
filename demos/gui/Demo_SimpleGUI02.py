## /*
## -----------------------------------------------------------------------------
## This source file is part of OGRE
## (Object-oriented Graphics Rendering Engine)
## For the latest info, see http:##www.ogre3d.org/

## Copyright (c) 2000-2007 The OGRE Team
## Also see acknowledgements in Readme.html

## You may use this sample code for anything you like, it is not covered by the
## LGPL like the rest of the engine.
## -----------------------------------------------------------------------------
## */

## /*
## -----------------------------------------------------------------------------
## Filename:    SimpleGUIDemo.h
## Description: A place for me to try out stuff with OGRE.
## -----------------------------------------------------------------------------
## */

import ogre.renderer.OGRE as Ogre
import ogre.io.OIS as ois
import ogre.gui.SimpleGUI as gui
import SampleFramework as sf

class CallBack ( gui.MemberFunctionSlot ):
    """ Callback class for user events in SimpleGUI"""
    def __init__(self, function=""):
        gui.MemberFunctionSlot.__init__(self)
        self.function=function
    def execute(self, args):
        if self.function:
            self.function(args)
            
class GuiFrameListener ( sf.FrameListener, ois.MouseListener, ois.KeyListener ):
    ## NB using buffered input, this is the only change
    def __init__( self, win,  cam):
        sf.FrameListener.__init__(self, win, cam, True, True, True) 
        ois.KeyListener.__init__(self)
        ois.MouseListener.__init__(self)
        self.mShutdownRequested = False
        self.robotAnimationState=None
        self.Mouse.setEventCallback(self)
        self.Keyboard.setEventCallback(self)

    ## Tell the frame listener to exit at the end of the next frame
    def requestShutdown(self):
        self.mShutdownRequested = True
    
    def setAnimationState(self, as):
        self.robotAnimationState = as

    def frameStarted(self, evt):
        if (self.robotAnimationState != None): self.robotAnimationState.addTime(evt.timeSinceLastFrame)

        gm = gui.GUIManager.getSingletonPtr()
        if( gm != None ): gm.injectTime(evt.timeSinceLastFrame)

        s = ""
        if(gm.getMouseOverWidget() != None): s = gm.getMouseOverWidget().getInstanceName()  
        gm.getWindow("Mouse Over Window").getTextBox(0).setText(s)

        return sf.FrameListener.frameStarted(self,evt)

    def frameEnded(self, evt):
        if (self.mShutdownRequested):
            return False
        else:
            return sf.FrameListener.frameEnded(self,evt)
    ##----------------------------------------------------------------##
    def mouseMoved( self, arg ):
        ms= arg.get_state()
        gui.GUIManager.getSingleton().injectMouseMove( ms.X.rel, ms.Y.rel )
        return True

    ##----------------------------------------------------------------##
    def mousePressed( self, arg,  _id ):
        b = gui.MouseButtonID.values[_id]
        gui.GUIManager.getSingleton().injectMouseButtonDown(b)
        return True

    ##----------------------------------------------------------------##
    def mouseReleased( self, arg, _id ):
        b = gui.MouseButtonID.values[_id]
        gui.GUIManager.getSingleton().injectMouseButtonUp(b)
        return True

    ##----------------------------------------------------------------##
    def  keyPressed( self, arg ):
        if( arg.key == ois.KC_ESCAPE ):
            self.ShutdownRequested = True
        ## Now convert from OIS keycode to SimpleGUI one..            
        k = gui.KeyCode.values[arg.key]
        gui.GUIManager.getSingleton().injectKeyDown( k )
        gui.GUIManager.getSingleton().injectChar( chr(arg.text) )
        return True

    ##----------------------------------------------------------------##
    def keyReleased( self, arg ):
        k = gui.KeyCode.values[arg.key]
        gui.GUIManager.getSingleton().injectKeyDown( k )
        return True



class SimpleGUIDemoApp (sf.Application):
    ## Just override the mandatory create scene method
    def _createScene(self):
        self.mDebugDisplayShown=True
        ## Set ambient light
        self.sceneManager.setAmbientLight((0.5, 0.5, 0.5))

        ## Create a SkyBox
        self.sceneManager.setSkyBox(True,"Examples/EveningSkyBox")

        ## Create a light
        l = self.sceneManager.createLight("MainLight")
        ## Accept default settings: point light, white diffuse, just set position
        ## NB I could attach the light to a SceneNode if I wanted it to move automatically with
        ##  other objects, but I don't
        l.setPosition(20,80,50)

        self.mGUIManager = gui.GUIManager(self.renderWindow.getWidth(),self.renderWindow.getHeight())

        self.robot = self.sceneManager.createEntity("ChuckNorris", "robot.mesh")

        robotNode = self.sceneManager.getRootSceneNode().createChildSceneNode()
        robotNode.attachObject(self.robot)
        robotNode.yaw(Ogre.Radian(Ogre.Degree(-90)))
        robotNode.scale = (1.25,1.25,1.25)
        
        ##  This ensures the camera doesn't move when we move the cursor..
        #3 However we do the same thing by setting MenuMode to True in the frameListerner further down
#         self.camera.setAutoTracking(True, robotNode)

        plane = Ogre.Plane( Ogre.Vector3.UNIT_Y, 0 )
        Ogre.MeshManager.getSingleton().createPlane("ground",
           Ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME, plane,
           1500,1500,20,20,True,1,5,5,Ogre.Vector3.UNIT_Z)
        ground = self.sceneManager.createEntity( "GroundEntity", "ground" )
        self.sceneManager.getRootSceneNode().createChildSceneNode().attachObject(ground)
        ground.setMaterialName("Examples/Rockwall")
        ground.setCastShadows(False)

        self.camera.setPosition(0,125,500)
        self.camera.pitch(Ogre.Radian(Ogre.Degree(-15)))

        ## Setup Render To Texture for preview window
        self.rttTex = self.root.getRenderSystem().createRenderTexture( "self.rttTex",\
                        512, 512, Ogre.TextureType.TEX_TYPE_2D, Ogre.PixelFormat.PF_R8G8B8 )
        ## From CEGUI example.  The camera position doesn't fit the robot setup, so I changed it some.
        rttCam = self.sceneManager.createCamera("RttCam")
        camNode = self.sceneManager.getRootSceneNode().createChildSceneNode("rttCamNode")
        camNode.attachObject(rttCam)
        rttCam.setPosition(0,75,225)
        ##rttCam.setVisible(True)

        v = self.rttTex.addViewport( rttCam )
        ## Alternatively, use the main camera for the self.rttText, imitating the main screen
        ##Viewport *v = self.rttTex.addViewport( self.camera )
        v.setOverlaysEnabled( False )
        v.setClearEveryFrame( True )
        v.setBackgroundColour( Ogre.ColourValue.Black )

        self.createGUI()
        
    def MakeCallback ( self, function ):
        cb = CallBack()
        cb.function = function
        self.callbacks.append( cb )
        return cb
        
    def createGUI(self):
        self.callbacks=[]
        self.mGUIManager.createMouseCursor((0.05,0.05),"sgui.pointer")

        ## Main Menu and it's MenuLists
        menuWindow = self.mGUIManager.createEmptyWindow((0,0,1,0.04))
        menuWindow.setZOrder(600)
        topMenu = menuWindow.createFullSizeMenu("sgui.menu")
        
        fileList = topMenu.addMenuList("File",(0,0,0.075,1),"sgui.menubutton",0.1,"sgui.menulist","sgui.listitem.highlight")
        exitListItem = fileList.addListItem("Exit")
        
        exitListItem.addEventHandler(gui.Widget.Event.SGUI_MOUSE_BUTTON_UP,self.MakeCallback (self.evtHndlr_exitListItem ) )
        
        cameraList = topMenu.addMenuList("Camera Properties",Ogre.Vector4(0.08,0,0.2,1),"sgui.menubutton",0.15,"sgui.menulist","sgui.listitem.highlight")
        pointListItem = cameraList.addListItem("Point")
        pointListItem.addImage(Ogre.Vector4(0,0,0.2,1),"demo.pointmode")
        pointListItem.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_CameraPoint) )
        wireframeListItem = cameraList.addListItem("Wire Frame")
        wireframeListItem.addImage(Ogre.Vector4(0,0,0.2,1),"demo.wireframemode")
        wireframeListItem.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_CameraWireFrame) )
        solidListItem = cameraList.addListItem("Solid")
        solidListItem.addImage(Ogre.Vector4(0,0,0.2,1),"demo.solidmode")
        solidListItem.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_CameraSolid) )
 
        viewportList = topMenu.addMenuList("Progress Bar Color",Ogre.Vector4(0.285,0,0.2,1),"sgui.menubutton",0.2,"sgui.menulist","sgui.listitem.highlight")
        redListItem = viewportList.addListItem("Red")
        redListItem.addImage(Ogre.Vector4(0.1,0.1,0.8,0.8),"demo.listitem.red")
        redListItem.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_pbRed) )
        greenListItem = viewportList.addListItem("Green")
        greenListItem.addImage(Ogre.Vector4(0.1,0.1,0.8,0.8),"demo.listitem.green")
        greenListItem.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_pbGreen) )
        blueListItem = viewportList.addListItem("Blue")
        blueListItem.addImage(Ogre.Vector4(0.1,0.1,0.8,0.8),"demo.listitem.blue")
        blueListItem.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_pbBlue) )

        otherList = topMenu.addMenuList("Other",Ogre.Vector4(0.490,0,0.1,1),"sgui.menubutton",0.2,"sgui.menulist","sgui.listitem.highlight")
        textColorListItem = otherList.addListItem("Text Color")
        textColorListItem.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_setTextWhite) )
        tcProperties = textColorListItem.addNStateButton(Ogre.Vector4(0.8,0.1,0.175,0.9))
        tcProperties.addState("OpenProperties","demo.listitem.textproperties")
        tcProperties.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_showSetTextDialog) )
        RenderStatsListItem = otherList.addListItem("Render Stats")
        toggleRenderStats = RenderStatsListItem.addNStateButton(Ogre.Vector4(0.8,0.1,0.175,0.9))
        toggleRenderStats.addState("checked","sgui.button.state2")
        toggleRenderStats.addState("unchecked","sgui.button.state1")
        toggleRenderStats.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_toggleDebugDisplay))

        ## Logos
        logoImage = self.mGUIManager.createEmptyWindow("Logo_Image_Window",Ogre.Vector4(0.02,0.07,0.3,0.3)).createFullSizeImage("demo.ogrelogo")
        logoLabel = self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.075,0.4,0.15,0.05)).createFullSizeLabel("sgui.label")
        logoLabel.setText("Click Me >")
        imageToggleButton = self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.225,0.4,0.05,0.05)).createFullSizeNStateButton()
        imageToggleButton.addState("OgreLogo","sgui.button.state1")
        imageToggleButton.addState("SimpleGUILogo","sgui.button.state2")

        ## RTT Example Use
        rttImage = self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.75,0.07,0.2,0.15)).createFullSizeImage("self.rttTex",True)
        ninjaWindow = self.mGUIManager.createWindow(Ogre.Vector4(0.725,0.25,0.25,0.15),"sgui.window")
        ninjaWindow.hideTitlebar()
        animToggleButton = ninjaWindow.createNStateButton(Ogre.Vector4(0.05,0.033,0.9,0.3))
        ## populate NStateButton with States - robot animations
        casi = self.robot.getAllAnimationStates().getAnimationStateIterator()
        state = 0
        while( casi.hasMoreElements() ):
            animName = casi.getNext().getAnimationName()
            animToggleButton.addState("State"+str(state),"sgui.button",animName)
            if state == 0:
                self.robotAnimationState = self.robot.getAnimationState(animName)
                self.robotAnimationState.setEnabled(True)
                self.robotAnimationState.setTimePosition(0.0)
                self.robotAnimationState.setLoop(True)
            state+=1
#         animToggleButton.addOnStateChangedEventHandler(self.evtHndlr_changeAnimations)
        animToggleButton.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_changeAnimations) )
        
        hurtButton = ninjaWindow.createButton(Ogre.Vector4(0.05,0.36,0.9,0.3),"sgui.button")
        hurtButton.setText("Hurt")
        hurtButton.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_hurt) )
        healButton = ninjaWindow.createButton(Ogre.Vector4(0.05,0.69,0.9,0.3),"sgui.button")
        healButton.setText("Heal")
        healButton.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_heal) )

        ## Progress Bar Setup
        lbW1 = self.mGUIManager.createEmptyWindow((0.4,0.5125,0.07,0.07))
        lbW1.setZOrder(600)
        self.lifeBarLabel = lbW1.createFullSizeLabel("")
        self.lifeBarLabel.setText("HP")
        lbW2 = self.mGUIManager.createEmptyWindow((0.55,0.5125,0.07,0.07))
        lbW2.setZOrder(600)
        self.lifeBarValueLabel = lbW2.createFullSizeLabel("")
#         self.lifeBarValueLabel.setText("100")
        self.lifeBar = self.mGUIManager.createEmptyWindow((0.4,0.55,0.2,0.03)).createFullSizeProgressBar("sgui.progressbar.green")
        self.lifeBar.setProgress(0.5)
        self.lifeBarValueLabel.setText("50")
        
        ## Combobox
# #         listbox = self.mGUIManager.
        
        
        ## Mouse Over window
        mouseOverWindow = self.mGUIManager.createWindow("Mouse Over Window",Ogre.Vector4(0.7,0.7,0.3,0.1),"sgui.window")
        mouseOverWindow.hideTitlebar()
        mouseOverLabel = mouseOverWindow.createLabel(Ogre.Vector4(0,0,1,0.5),"sgui.label")
        mouseOverLabel.setText("Mouse Over Widget:")
        mouseOverTB = mouseOverWindow.createTextBox(Ogre.Vector4(0,0.5,1,0.5),"sgui.textbox")
        mouseOverTB.setReadOnly(True)

        ## Login Portion
        self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.02,0.6,0.2,0.05)).createFullSizeLabel("sgui.label").setText("User Name:")
        self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.02,0.65,0.2,0.05)).createFullSizeLabel("sgui.label").setText("Password:")
        self.usernameTB = self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.225,0.6,0.2,0.05)).createFullSizeTextBox("sgui.textbox")
        self.passwordTB = self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.225,0.65,0.2,0.05)).createFullSizeTextBox("sgui.textbox")
        self.passwordTB.maskUserInput('*')
        loginButton = self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.125,0.7,0.25,0.07)).createFullSizeButton("sgui.button")
        loginButton.setText("Login")
        loginButton.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_login) )
        self.loginResultLabel = self.mGUIManager.createEmptyWindow(Ogre.Vector4(0.0,0.77,0.5,0.05)).createFullSizeLabel("")

        ## Set Text Window
        self.stWindow = self.mGUIManager.createWindow(Ogre.Vector4(0.7,0.45,0.3,0.2),"sgui.window",False)
        self.stWindow.setText("Set Text Color:")
        self.stWindow.createLabel(Ogre.Vector4(0.05,0.3,0.3,0.25),"sgui.label").setText("Color:")
        colorCB = self.stWindow.createComboBox(Ogre.Vector4(0.4,0.3,0.55,0.25),"sgui.combobox","sgui.listitem.highlight")
        colorCB.addListItem("Red")
        colorCB.addListItem("Green")
        colorCB.addListItem("Blue")
        colorCB.addListItem("Black")
        colorCB.addListItem("White")
        setTextButton = self.stWindow.createButton(Ogre.Vector4(0.05,0.6,0.9,0.3),"sgui.button")
        setTextButton.setText("Apply")
        setTextButton.addEventHandler(gui.Widget.SGUI_MOUSE_BUTTON_UP,self.MakeCallback(self.evtHndlr_setTextColor) )

    def evtHndlr_exitListItem(self, args):
        ## check if left mouse button is down
        if(args.button == gui.MB_Left):
            self.frameListener.requestShutdown()
        return True

    def evtHndlr_changeAnimations(self,args):
        ## disable previous animation
        self.robotAnimationState.setEnabled(False)
        self.robotAnimationState.setWeight(0.0)

        self.robotAnimationState = self.robot.getAnimationState(args.widget.getCurrentState().getText())
        self.robotAnimationState.setEnabled(True)
        self.robotAnimationState.setTimePosition(0.0)
        self.robotAnimationState.setWeight(1.0)
        self.robotAnimationState.setLoop(True)

        self.frameListener.setAnimationState(self.robotAnimationState)
        return True

    def evtHndlr_heal(self, args):
        currentProgress = self.lifeBar.getProgress()
        random = Ogre.Math.RangeRandom(currentProgress,1.0)
        self.lifeBar.setProgress(random)
        self.lifeBarValueLabel.setText(str(int(random * 100)))
        return True

    def evtHndlr_hurt(self, args):
        if args.button == gui.MB_Left: 
            currentProgress = self.lifeBar.getProgress()
            random = Ogre.Math.RangeRandom(0.0,currentProgress)
            self.lifeBar.setProgress(random)
            self.lifeBarValueLabel.setText(str(int(random * 100)))
        return True

    def evtHndlr_pbRed(self,args):
        self.lifeBar.setMaterial("sgui.progressbar.red")
        return False

    def evtHndlr_pbGreen(self, args):
        self.lifeBar.setMaterial("sgui.progressbar.green")
        return True

    def evtHndlr_pbBlue(self, args):
        self.lifeBar.setMaterial("sgui.progressbar.blue")
        return True

    def evtHndlr_login(self, args):
        if( self.usernameTB.getText() == self.passwordTB.getText() ) : s = "Login Successful."
        else: s = "Username and/or Password do not match."
        self.loginResultLabel.setText(s)
        return True

    def evtHndlr_toggleDebugDisplay(self, args):
        self.mDebugDisplayShown = not self.mDebugDisplayShown
        self.frameListener.showDebugOverlay(self.mDebugDisplayShown)
        return True

    def evtHndlr_setTextWhite(self, args):
        self.stWindow.setTextColor(Ogre.ColourValue.White)
        return True

    def evtHndlr_showSetTextDialog(self, args):
        self.stWindow.setPosition(0.7,0.45)
        self.stWindow.show()
        return True
        
    def evtHndlr_CameraPoint(self, args):
        self.camera.polygonMode=Ogre.PM_POINTS
        return True
    
    def evtHndlr_CameraWireFrame(self, args):
        self.camera.polygonMode=Ogre.PM_WIREFRAME
        return True
        
    def evtHndlr_CameraSolid(self, args):
        self.camera.polygonMode=Ogre.PM_SOLID
        return True

    def evtHndlr_setTextColor(self, args):
        s = self.stWindow.getComboBox(0).getText()
        
        if( s == "Red" ): c = Ogre.ColourValue.Red
        elif( s == "Green" ): c = Ogre.ColourValue.Green
        elif( s == "Blue" ): c = Ogre.ColourValue.Blue
        elif( s == "Black" ): c = Ogre.ColourValue.Black
        elif( s == "White" ): c = Ogre.ColourValue.White

        self.stWindow.setTextColor(c)
        return True
            
    def _createFrameListener(self):
        self.frameListener= GuiFrameListener(self.renderWindow, self.camera)
        ## This ensures the mouse doesn't move the camera..
        self.frameListener.setMenuMode ( True )
        self.root.addFrameListener(self.frameListener)
        self.frameListener.setAnimationState(self.robotAnimationState)

    def handleQuit(self,e):
        self.frameListener.requestShutdown()
        return True
 
if __name__ == '__main__':
    try:
        application = SimpleGUIDemoApp()
        application.go()
    except Ogre.OgreException, e:
        print e