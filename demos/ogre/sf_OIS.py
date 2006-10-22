# This code is in the Public Domain
# designed for the CVS/OIS verison of Ogre
import Ogre as ogre
import OIS as OIS

def getPluginPath():
    """Return the absolute path to a valid plugins.cfg file.""" 
    import sys
    import os
    import os.path
    
    paths = [os.path.join(os.getcwd(), 'plugins.cfg'),
             '/etc/OGRE/plugins.cfg',
             os.path.join(os.path.dirname(os.path.abspath(__file__)),
                              'plugins.cfg')]
    for path in paths:
        if os.path.exists(path):
            return path

    sys.stderr.write("\n"
        "** Warning: Unable to locate a suitable plugins.cfg file.\n"
        "** Warning: Please check your ogre installation and copy a\n"
        "** Warning: working plugins.cfg file to the current directory.\n\n")
    raise ogre.Exception(0, "can't locate the 'plugins.cfg' file", "")
    
class Application(object):
    "This class is the base for an Ogre application."
    debugText=""

    def __init__(self):
        self.frameListener = None
        self.root = None
        self.camera = None
        self.renderWindow = None
        self.sceneManager = None

    def __del__(self):
        "Clear variables, this should not actually be needed."
        del self.camera
        del self.sceneManager
        del self.frameListener
        del self.root
        del self.renderWindow

    def go(self):
        "Starts the rendering loop."
        if not self._setUp():
            return
        if self._isPsycoEnabled():
            self._activatePsyco()
        self.root.startRendering()


    def _setUp(self):
        """This sets up the ogre application, and returns false if the user
        hits "cancel" in the dialog box."""
        self.root = ogre.Root(getPluginPath())
        self.root.setFrameSmoothingPeriod (5.0)

        self._setUpResources()
        if not self._configure():
            return False
        
        self._chooseSceneManager()
        self._createCamera()
        self._createViewports()

        ogre.TextureManager.getSingleton().setDefaultNumMipmaps (5)

        self._createResourceListener()
        self._loadResources()

        self._createScene()
        self._createFrameListener()
        return True

    def _setUpResources(self):
        """This sets up Ogre's resources, which are required to be in
        resources.cfg."""
        config = ogre.ConfigFile()
#         print dir (config)
        config.load('resources.cfg', '', False )
#         t = config.SectionIterator
#         print "\n\n......\n", t,"\n\n"
#         while (t.hasMoreElements()):
#             y = t.getNext()
#             print y
#             for setting in y:
#                 print "Setting: ", setting
#         print "\n\n......\n", t,"\n\n"
#         for sectionName, mm in config.SectionIterator:
#             print sectionName, mm
            #if mm:
                #for key, path in mm.items():
                    #ogre.ResourceGroupManager.getSingleton().addResourceLocation(path, key, sectionName)
                    
                    
                    
                    
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/OgreCore.zip", "Zip", "Bootstrap", False)
#        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/andy", "FileSystem", "Bootstrap", False)
#        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/andy/textures", "FileSystem", "Bootstrap", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/fonts", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/materials/programs", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/materials/scripts", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/materials/textures", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/models", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/overlays", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/particle", "FileSystem", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/cubemap.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/cubemapsJS.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/dragon.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/fresneldemo.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/ogretestmap.zip", "Zip", "General", False)
        ogre.ResourceGroupManager.getSingleton().addResourceLocation("../media/packs/skybox.zip", "Zip", "General", False)

        #for section, key, path in config.values:
        #    ogre.ResourceGroupManager.getSingleton().addResourceLocation(path, key, section)
        #print dir(config.getSectionIterator())          
        #for sectionName, mm in config.getSectionIterator():
            #if mm:
                #for key, path in mm.items():
                    #print "--------------"
                    #print key, path 
                    
    def _createResourceListener(self):
        """This method is here if you want to add a resource listener to check
        the status of resources loading."""
        pass

    def _loadResources(self):
        """This loads all initial resources.  Redefine this if you do not want
        to load all resources at startup."""
        ogre.ResourceGroupManager.getSingleton().initialiseAllResourceGroups()

    def _configure(self):
        """This shows the config dialog and creates the renderWindow."""
        carryOn = self.root.showConfigDialog()
        if carryOn:
            self.renderWindow = self.root.initialise(True, "OGRE Render Window")
        return carryOn

    def _chooseSceneManager(self):
        """Chooses a default SceneManager."""
        #typedef uint16 SceneTypeMask;
        #md=ogre.SceneManagerMetaData()
        #md.sceneTypeMask=ogre.ST_GENERIC
        #print dir(self.root)    
        self.sceneManager = self.root.createSceneManager(ogre.ST_GENERIC,"ExampleSMInstance")

    def _createCamera(self):
        """Creates the camera."""        
        self.camera = self.sceneManager.createCamera('PlayerCam')
        self.camera.setPosition(ogre.Vector3(0, 0, 500))
        self.camera.lookAt(ogre.Vector3(0, 0, -300))
        self.camera.NearClipDistance = 5

    def _createViewports(self):
        """Creates the Viewport."""
        self.viewport = self.renderWindow.addViewport(self.camera)
        self.viewport.BackgroundColour = ogre.ColourValue(0,0,0)
        
    def _createScene(self):
        """Creates the scene.  Override this with initial scene contents."""
        pass

    def _createFrameListener(self):
        """Creates the FrameListener."""
        #,self.frameListener, self.frameListener.Mouse 
        self.frameListener = FrameListener(self.renderWindow, self.camera)
        self.frameListener.showDebugOverlay(True)
        self.root.addFrameListener(self.frameListener)

    def _isPsycoEnabled(self):
        """Override this function and return True to turn on Psyco"""
        return False

    def _activatePsyco(self):        
       """Import Psyco if available"""
       try:
           import psyco
           psyco.full()
       except ImportError:
           pass


class FrameListener(ogre.FrameListener, ogre.WindowEventListener):
    """A default frame listener, which takes care of basic mouse and keyboard
    input."""
      
    def __init__(self, renderWindow, camera, bufferedKeys = False, bufferedMouse = True, bufferedJoy = False):
        ogre.FrameListener.__init__(self)
        ogre.WindowEventListener.__init__(self)
        self.camera = camera
        self.renderWindow = renderWindow
        self.statisticsOn = True
        self.numScreenShots = 0
        self.timeUntilNextToggle = 0
        self.sceneDetailIndex = 0
        self.moveScale = 0.0
        self.rotationScale = 0.0
        self.translateVector = ogre.Vector3(0.0,0.0,0.0)
        self.filtering = ogre.TFO_BILINEAR
        self.showDebugOverlay(True)
        self.moveSpeed = 100.0
        self.rotationSpeed = 8.0
        self.displayCameraDetails = False
        self.bufferedKeys = bufferedKeys
        self.bufferedMouse = bufferedMouse
        self.bufferedJoy = bufferedJoy
        self.MenuMode = False   # lets understand a simple menu function
        self._setupInput()
        
    def __del__ (self ):
      ogre.WindowEventUtilities.removeWindowEventListener(self.renderWindow, self)
      self.windowClosed(self.renderWindow);

    def _setupInput(self):
        # ignore buffered input
           
         windowHnd = self.renderWindow.getCustomAttributeWindowInt("WINDOW")
         im = OIS.createPythonInputSystem( windowHnd )
         
         #pl = OIS.ParamList()
         #windowHndStr = str ( windowHnd)
         #pl.insert("WINDOW", windowHndStr)
         #im = OIS.InputManager.createInputSystem( pl )
         
         #Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
         self.Keyboard = im.createInputObjectKeyboard( OIS.OISKeyboard, self.bufferedKeys )
         self.Mouse = im.createInputObjectMouse( OIS.OISMouse, self.bufferedMouse )
         try :
            self.Joy = im.createInputObjectJoyStick( OIS.OISJoyStick, bufferedJoy )
         except:
            self.Joy = False
         
         #Set initial mouse clipping size
         self.windowResized(self.renderWindow)
         
         self.showDebugOverlay(True)
         
         #Register as a Window listener
         ogre.WindowEventUtilities.addWindowEventListener(self.renderWindow, self);
         
    def windowResized (self, rw):
         [width, height, depth, left, top] = rw.getMetricsInt()  # Note the wrapped function as default needs unsigned int's
         ms = self.Mouse.getMouseState()
         ms.width = width
         ms.height = height
         
    def windowClosed(self, rw):
      #Only close for window that created OIS (mWindow)
      if( rw == self.renderWindow ):
         im = OIS.InputManager.getSingletonPtr()
         if( im ):
            im.destroyInputObjectMouse( self.Mouse )
            im.destroyInputObjectKeyboard( self.Keyboard )
            if self.Joy:
                im.destroyInputObjectJoyStick( self.Joy )
            im.destroyInputSystem()
            
    def frameStarted(self, frameEvent):
        if(self.renderWindow.isClosed()):
            return False
        
        ##Need to capture/update each device - this will also trigger any listeners
        self.Keyboard.capture()    
        self.Mouse.capture()
        if( self.Joy ):
            self.Joy.capture()
    
        ##bool buffJ = (mJoy) ? mJoy->buffered() : true;
    
        if self.timeUntilNextToggle >= 0:
            self.timeUntilNextToggle -= frameEvent.timeSinceLastFrame
    
        if frameEvent.timeSinceLastFrame == 0:
            self.moveScale = 1
            self.rotationScale = 0.1
        else:
            self.moveScale = self.moveSpeed * frameEvent.timeSinceLastFrame
            self.rotationScale = self.rotationSpeed * frameEvent.timeSinceLastFrame
    
        self.rotationX = ogre.Radian(0.0)
        self.rotationY = ogre.Radian(0.0)
        self.translateVector = ogre.Vector3(0.0, 0.0, 0.0)
        if not self._processUnbufferedKeyInput(frameEvent):
            return False
        
        if not self.MenuMode:   # if we are in Menu mode we don't move the camera..
            self._processUnbufferedMouseInput(frameEvent)
        
        self._moveCamera()
        return True

    def frameEnded(self, frameEvent):
        self._updateStatistics()
        return True

    def showDebugOverlay(self, show):
        """Turns the debug overlay (frame statistics) on or off."""
        overlay = ogre.OverlayManager.getSingleton().getByName('Core/DebugOverlay')
        if overlay is None:
            raise ogre.Exception(111, "Could not find overlay Core/DebugOverlay", "SampleFramework.py")
        if show:
            overlay.show()
        else:
            overlay.hide()

    def _processUnbufferedKeyInput(self, frameEvent):
        if self.Keyboard.isKeyDown(OIS.KC_A):
            self.translateVector.x = -self.moveScale

        if self.Keyboard.isKeyDown(OIS.KC_D):
            self.translateVector.x = self.moveScale

        if self.Keyboard.isKeyDown(OIS.KC_UP) or self.Keyboard.isKeyDown(OIS.KC_W):
            self.translateVector.z = -self.moveScale

        if self.Keyboard.isKeyDown(OIS.KC_DOWN) or self.Keyboard.isKeyDown(OIS.KC_S):
            self.translateVector.z = self.moveScale

        if self.Keyboard.isKeyDown(OIS.KC_PGUP):
            self.translateVector.y = self.moveScale

        if self.Keyboard.isKeyDown(OIS.KC_PGDOWN):
            self.translateVector.y = - self.moveScale

        if self.Keyboard.isKeyDown(OIS.KC_RIGHT):
            self.rotationX = - self.rotationScale

        if self.Keyboard.isKeyDown(OIS.KC_LEFT):
            self.rotationX = self.rotationScale

        if self.Keyboard.isKeyDown(OIS.KC_ESCAPE) or self.Keyboard.isKeyDown(OIS.KC_Q):
            return False

        if( self.Keyboard.isKeyDown(OIS.KC_F) and self.timeUntilNextToggle <= 0 ): 
             self.statisticsOn = not self.statisticsOn
             self.showDebugOverlay(self.statisticsOn)
             self.timeUntilNextToggle = 1

        if self.Keyboard.isKeyDown(OIS.KC_T) and self.timeUntilNextToggle <= 0:
            if self.filtering == ogre.TFO_BILINEAR:
                self.filtering = ogre.TFO_TRILINEAR
                self.Aniso = 1
            elif self.filtering == ogre.TFO_TRILINEAR:
                self.filtering = ogre.TFO_ANISOTROPIC
                self.Aniso = 8
            else:
                self.filtering = ogre.TFO_BILINEAR
                self.Aniso = 1

            ogre.MaterialManager.getSingleton().setDefaultTextureFiltering(self.filtering)
            ogre.MaterialManager.getSingleton().setDefaultAnisotropy(self.Aniso)
            self.showDebugOverlay(self.statisticsOn)
            self.timeUntilNextToggle = 1
        
        if self.Keyboard.isKeyDown(OIS.KC_SYSRQ) and self.timeUntilNextToggle <= 0:
            path = 'screenshot_%d.png' % self.numScreenShots
            self.numScreenShots += 1
            self.renderWindow.writeContentsToFile(path)
            Application.debugText = 'screenshot taken: ' + path
            self.timeUntilNextToggle = 0.5
        
        if self.Keyboard.isKeyDown(OIS.KC_R) and self.timeUntilNextToggle <= 0:
            detailsLevel = [ ogre.PM_SOLID,
                             ogre.PM_WIREFRAME,
                             ogre.PM_POINTS ]
            self.sceneDetailIndex = (self.sceneDetailIndex + 1) % len(detailsLevel)
            self.camera.polygonMode=detailsLevel[self.sceneDetailIndex]
            self.timeUntilNextToggle = 0.5
            
        if self.Keyboard.isKeyDown(OIS.KC_F) and self.timeUntilNextToggle <= 0:
            self.statisticsOn = not self.statisticsOn
            self.showDebugOverlay(self.statisticsOn)
            self.timeUntilNextToggle = 1
        
        if self.Keyboard.isKeyDown(OIS.KC_P) and self.timeUntilNextToggle <= 0:
            self.displayCameraDetails = not self.displayCameraDetails
            if not self.displayCameraDetails:
                Application.debugText = ""
                
        if self.displayCameraDetails:
            # Print camera details
            pos = self.camera.getDerivedPosition()
            o = self.camera.getDerivedOrientation()
            Application.debugText = "P: %.3f %.3f %.3f O: %.3f %.3f %.3f %.3f"  \
                        % (pos.x,pos.y,pos.z, o.w,o.x,o.y,o.z)
        return True        
        
    def _isToggleKeyDown(self, keyCode, toggleTime = 1.0):
        if self.Keyboard.isKeyDown(keyCode)and self.timeUntilNextToggle <=0:
            self.timeUntilNextToggle = toggleTime
            return True
        return False

    def _processUnbufferedMouseInput(self, frameEvent):
        ms = self.Mouse.getMouseState()
        if ms.buttonDown( OIS.MB_Right ):
            self.translateVector.x += ms.relX * 0.13
            self.translateVector.y -= ms.relY * 0.13
        else:
            self.rotationX = ogre.Radian(- ms.relX * 0.13)
            self.rotationY = ogre.Radian(- ms.relY * 0.13)

#         if self.inputDevice.getMouseRelativeZ() > 0:
#             self.translateVector.z = - self.moveScale * 8.0
#         if self.inputDevice.getMouseRelativeZ() < 0:
#             self.translateVector.z = self.moveScale * 8.0
#          pass

    def _moveCamera(self):
        self.camera.yaw(self.rotationX)
        self.camera.pitch(self.rotationY)
        self.camera.moveRelative(self.translateVector)

    def _updateStatistics(self):
        statistics = self.renderWindow
        self._setGuiCaption('Core/AverageFps', 'Average FPS: %f' % statistics.getAverageFPS())
        self._setGuiCaption('Core/CurrFps', 'Current FPS: %f' % statistics.getLastFPS())
        self._setGuiCaption('Core/BestFps',
                             'Best FPS: %f %d ms' % (statistics.getBestFPS(), statistics.getBestFrameTime()))
        self._setGuiCaption('Core/WorstFps',
                             'Worst FPS: %f %d ms' % (statistics.getWorstFPS(), statistics.getWorstFrameTime()))
        self._setGuiCaption('Core/NumTris', 'Triangle Count: %d' % statistics.getTriangleCount())
        self._setGuiCaption('Core/DebugText', Application.debugText)

    def _setGuiCaption(self, elementName, text):
        element = ogre.OverlayManager.getSingleton().getOverlayElement(elementName, False)
        element.setCaption(text)
