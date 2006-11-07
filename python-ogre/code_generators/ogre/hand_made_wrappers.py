import os
import environment

WRAPPER_DEFINITION_General = \
"""
boost::python::tuple 
GetOgreVersion () {
            return ( boost::python::make_tuple( Ogre::StringConverter::toString(OGRE_VERSION_MAJOR),
                                                Ogre::StringConverter::toString(OGRE_VERSION_MINOR),
                                                Ogre::StringConverter::toString(OGRE_VERSION_PATCH),
                                                OGRE_VERSION_NAME 
                                                ) );
}
boost::python::tuple 
GetPythonOgreVersion () {
            return ( boost::python::make_tuple( """ + environment.PythonOgreMajorVersion + """,
                                                """ + environment.PythonOgreMinorVersion + """,
                                                """ + environment.PythonOgrePatchVersion + """
                                                ) );
}
"""            


WRAPPER_DEFINITION_CombinedListener = \
"""
namespace Ogre {
    class CombinedListener : public Ogre::FrameListener, public Ogre::KeyListener,
                 public Ogre::MouseListener, public Ogre::MouseMotionListener
{
public:
    CombinedListener() {}
    virtual ~CombinedListener() {}
    
    // KeyListener methods
    virtual void keyClicked(Ogre::KeyEvent *e) {}
    virtual void keyPressed(Ogre::KeyEvent *e) {}
    virtual void keyReleased(Ogre::KeyEvent *e) {}
    
    // MouseListener methods
    virtual void mouseClicked(Ogre::MouseEvent *e) {}
    virtual void mousePressed(Ogre::MouseEvent *e) {}
    virtual void mouseReleased(Ogre::MouseEvent *e) {}
    virtual void mouseEntered(Ogre::MouseEvent *e) {}
    virtual void mouseExited(Ogre::MouseEvent *) {}
    
    // MouseMotionListener methods
    virtual void mouseMoved(Ogre::MouseEvent *e) {}
    virtual void mouseDragged(Ogre::MouseEvent *e) {}
    
    // FrameEvent methods
    virtual bool frameStarted (const FrameEvent &evt) {return true;}
    virtual bool frameEnded (const FrameEvent &evt) {return true;}
};
}
"""


WRAPPER_DEFINITION_ConfigFile = \
"""
// We don't currently support multimaps very well so to extract the resources from a config file
// we call this with the multimap and it extracts it into a list of tuples for us
boost::python::list
ConfigFile_getMultiMapSettings ( Ogre::ConfigFile& cf, Ogre::ConfigFile::SettingsMultiMap * settings ){
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    Ogre::String typeName, archName;
    boost::python::list retlist = boost::python::list();
    for (i = settings->begin(); i != settings->end(); ++i)
    {
        typeName = i->first;
        archName = i->second;
        retlist.append(boost::python::make_tuple(typeName, archName));
    }
    return ( retlist );
}
"""
WRAPPER_DEFINITION_RenderTarget = \
"""
static int
RenderTarget_getCustomAttributeWindowInt(Ogre::RenderTarget& rd, const std::string& name) {
    int value(0);
    rd.getCustomAttribute( name, &value );
    return value;
}
"""

WRAPPER_DEFINITION_RenderWindow = \
"""
boost::python::tuple 
RenderWindow_getMetricsIntWrapper( Ogre::RenderWindow& rw ) {
   unsigned int width, height, colourDepth;
   int left, top;
   rw.getMetrics(  width,  height, colourDepth,  left,  top );
   return ( boost::python::make_tuple(width, height, colourDepth, left, top));
}
"""
## Needed as boost doesn't handle this overload properly and always uses 'Plane' as the override - see renderToTexture demo
WRAPPER_DEFINITION_Frustum = \
"""
void
Frustum_enableCustomNearClipPlaneMP(Ogre::Frustum& Fr, Ogre::MovablePlane const * MP ) {
        Fr.enableCustomNearClipPlane ( MP ) ;
}
"""
WRAPPER_REGISTRATION_ConfigFile = \
"""
    def( "getMultiMapSettings", &::ConfigFile_getMultiMapSettings )
"""
WRAPPER_REGISTRATION_RenderTarget = \
"""
    def( "getCustomAttributeWindowInt", &::RenderTarget_getCustomAttributeWindowInt )
"""
WRAPPER_REGISTRATION_RenderWindow = \
"""
    def( "getMetricsInt", &::RenderWindow_getMetricsIntWrapper )
"""
WRAPPER_REGISTRATION_Frustum = \
"""
    def( "enableCustomNearClipPlaneMP", &::Frustum_enableCustomNearClipPlaneMP )
"""

WRAPPER_REGISTRATION_General = \
"""
    def( "GetOgreVersion", &GetOgreVersion);
    def( "GetPythonOgreVersion", &GetPythonOgreVersion);
    
"""


if environment.ogre.version != "CVS":
    ### need to override keylistener etc
    WRAPPER_DEFINITION_EventProcessor =\
    """
	void EventProcessor_addKeyListener(Ogre::EventProcessor& evp, Ogre::KeyListener  * l) {
	    //evp.addKeyListener ( l );
	    }
	void EventProcessor_addMouseListener(Ogre::EventProcessor& evp, Ogre::MouseListener  * m) {
	    //evp.addMouseListener (  m );
	    }

	"""     
    WRAPPER_REGISTRATION_EventProcessor = \
    """
        def( "addKeyListenerFunc", &::EventProcessor_addKeyListener );
        EventProcessor_exposer.def( "addMouseListenerFunc", &::EventProcessor_addMouseListener );
    """



def apply( mb ):
    rt = mb.class_( 'ConfigFile' )
    rt.add_declaration_code( WRAPPER_DEFINITION_ConfigFile )
    rt.add_registration_code( WRAPPER_REGISTRATION_ConfigFile )
    rt = mb.class_( 'RenderTarget' )
    rt.add_declaration_code( WRAPPER_DEFINITION_RenderTarget )
    rt.add_registration_code( WRAPPER_REGISTRATION_RenderTarget )
    rt = mb.class_( 'RenderWindow' )
    rt.add_declaration_code( WRAPPER_DEFINITION_RenderWindow )
    rt.add_registration_code( WRAPPER_REGISTRATION_RenderWindow )
    rt = mb.class_( 'Frustum' )
    rt.add_declaration_code( WRAPPER_DEFINITION_Frustum )
    rt.add_registration_code( WRAPPER_REGISTRATION_Frustum )
    if environment.ogre.version != "CVS":
        rt = mb.class_( 'EventProcessor' )
        rt.add_declaration_code( WRAPPER_DEFINITION_EventProcessor )
        rt.add_registration_code( WRAPPER_REGISTRATION_EventProcessor )
        ## now add support for the combined listener
        rt = mb.class_( 'CombinedListener' )
        rt.add_declaration_code( WRAPPER_DEFINITION_CombinedListener )
        
        
    mb.add_declaration_code( WRAPPER_DEFINITION_General )
    mb.add_registration_code( WRAPPER_REGISTRATION_General )
    

