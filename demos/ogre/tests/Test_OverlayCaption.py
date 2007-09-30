# This code is in the Public Domain
import  Ogre as ogre
import SampleFramework as sf
import ctypes, math,sys

class TestApplication(sf.Application):
    def _createScene(self):
        sceneManager = self.sceneManager
        
        overman = ogre.OverlayManager.getSingleton()
        statspanel = overman.getOverlayElement('Core/StatPanel', False)
        print statspanel
        print dir(statspanel)
        statspanel.caption=ogre.UTFString("This does work")
        statspanel.setCaption(ogre.UTFString("as does this") )
        
#         statspanel.caption="This is broken"
        statspanel.setCaption ("This is broken")
        
        sys.exit()        
        
if __name__ == '__main__':
    try:
        application = TestApplication()
        application.go()
    except ogre.Exception, e:
        print e