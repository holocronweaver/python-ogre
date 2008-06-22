import os
import sys
import getpass
import datetime

##
##  Jan 2007 modification
##
##  This will now look for external config files.
## 
## first it will look for PythonOgreConfig_<username>.py
##
## if that fails it will look for PythonOgreConfig_<platform>.py
##
## otherwise it will use the values you enter below...
##

_LOGGING_ON = False
_PreCompiled = True

##
## set this to True if you compiled Ogre with Threads enabled
##
_USE_THREADS = False

def log ( instring ):
    if _LOGGING_ON:
        print __file__, "LOG::", instring

def isWindows():
    if os.name == 'nt': 
        return True
    return False
            
def isLinux():
    if os.name == 'posix' and os.sys.platform != 'darwin':
        return True
    return False 
       
def isMac():
    if os.sys.platform == 'darwin':
        return True
    return False
    
                    
PythonOgreMajorVersion = "1"
PythonOgreMinorVersion = "1" # change to 0.7 due to lowercase properties
PythonOgrePatchVersion = "0"


##
## these should be fine with auto create - however override them as necessary
##
PATH_Python = os.path.dirname( sys.executable )
## I want a version string 2.4 or 2.5 etc
PythonVersionString = str(sys.version_info[0]) + '.' + str(sys.version_info[1])

if os.name =='nt': 
    python_include_dirs = os.path.join ( PATH_Python, 'include')
    python_lib_dirs = os.path.join ( PATH_Python, 'libs' )
##
## for linux or darwin (MAC OS X)
else:
    if os.sys.platform <> 'darwin':
        ## It's linux of some sort
        python_include_dirs = os.path.join ( '/usr/include/python'+PythonVersionString,'')
        python_lib_dirs = os.path.join ( '/usr/lib/python'+PythonVersionString, 'libs' )
    else:
        ## it's Mac OS X
        pathlist = PATH_Python.split('/')
        version = pathlist[pathlist.index('Versions')+1]
        python_include_dirs = '/'.join(pathlist[:pathlist.index('Versions')+2]) + '/include/python' + version
        python_lib_dirs = ''

root_dir = os.path.abspath(os.path.dirname(__file__) )## The root directory is where this module is located

sys.path.append( os.path.join( root_dir, 'common_utils' ) )
shared_ptr_dir = os.path.join( root_dir, 'shared_ptr' )
include_dir = os.path.join( root_dir, 'include' )
generated_dir_name = 'generated' 
package_dir_name = 'packages'+ "_" + PythonVersionString 
generated_dir = os.path.join( root_dir, generated_dir_name )
declarations_cache_dir = os.path.join( root_dir, 'code_generators', 'cache' )

_UserName = getpass.getuser()
_ConfigSet = False
_SystemType = os.name ## nt or posix or mac
_PlatformType = sys.platform ## win32, ??

##
## Now we load the user specific stuff
##

# log ( "Config.PATH_Python: %s, UserName: %s, SystemType: %s, Root_dir: %s" % (Config.PATH_Python, _UserName, _SystemType, root_dir) )
try:
    s = 'PythonOgreConfig_' + _UserName ## + '.py'
    Config = __import__(  s  )
    _ConfigSet = True
    log ( "Loaded Config (based on username) from %s" % (s ))
except:
    try:
        s= 'PythonOgreConfig_' + _SystemType ## + '.py'
        
        Config = __import__(  s  )
        _ConfigSet = True
        log ( "Loaded Config (based on systemtype) from %s" % (s))
    except:
        pass   

if not _ConfigSet:
    log ( "  Going to use internal config setting")
    ## These need to be adjusted based upon your system
    ##
    ## PLEASE use an external PythonOgreConfig_<username>.py to hold these value !!!!
    ## 
    print "\n\n You DO need to create a PythonOgreConfig_%s.py file with config details" % ( _SystemType)
    sys.exit(-1)
    
        
######################
downloadPath = os.path.abspath("downloads") 
wget = "wget -c -nc "
tar = "tar "
gzip = "gzip "

if isWindows():
    mv = "move /Y "
    rm = "del /Q "
    cp = "copy /Y "
    unzip = "unzip "
    cvs = "cvs -z3 -q  "
    svn = "svn "
else:
    mv = "mv "
    rm = "rm -f "
    cp = "cp -f "
    unzip = "unzip "
    cvs = "cvs -z3 -q  "
    svn = "svn "
    #
## BIG assumption about where you want things put    
ROOT = os.path.join(os.getcwd(),'root' )
PREFIX = os.path.join(os.getcwd(),'root', 'usr' )


def unTarGz ( base, source ):
    """ a complete hack to cope with untar gziping a source file in the downloads directory into it's own directory
    Needed as the GNU tools for windows don't handle tar forking gzip or working on files that are in
    different directories
    """
    ret =[
        [0,cp + os.path.join(downloadPath, base +".tar.gz") + " " + os.getcwd(), '' ],
        [0,gzip + " -f -d " + os.path.join(os.getcwd(), base + ".tar.gz"), ''],
        [0,tar + " xf " + base + ".tar", '' ],
        [0,rm + base + ".tar", '']
        ]
    return ret

    


####################################################
##
## These are the base tools and libraries 
##
####################################################
class cg:
    pythonModule = False
    active = True
    if isLinux():
        base = 'Cg-1.5_Aug2007_x86'
        source = [
                    [wget, " http://developer.download.nvidia.com/cg/Cg_1.5/1.5.0/0022/"+base+".tar.gz",downloadPath]
                 ]
                 
        buildCmds =  [
                    [0,"mkdir -p gccxml-build", ''],
                    [0,"cmake ../gccxml -DCMAKE_INSTALL_PREFIX:PATH=%s" % PREFIX,os.path.join(os.getcwd(),'gccxml-build')],
                    [0,"make",  ,os.path.join(os.getcwd(),'gccxml-build')],
                    [0,"make install", ,os.path.join(os.getcwd(),'gccxml-build')]
                    ]   
                         
class gccxml:
    pythonModule = False
    active = True
    if isLinux():
        base = 'gccxml'
        source = [
                    [cvs, " -d :pserver:anoncvs@www.gccxml.org:/cvsroot/GCC_XML co "+base,'']
                 ]
                 
        buildCmds =  [
                    [0,tar + " xvzf "+ os.path.join(downloadPath,base) + ".tar.gz --overwrite", ROOT],   # unpack it directly into 'our' root
                    ]    
                    
                                        
class zziplib:
    pythonModule = False
    active = True
    if isLinux():
        base = 'zziplib-0.13.49'
        source = [
                    [wget, " http://downloads.sourceforge.net/zziplib/"+base+".tar.bz2",downloadPath],
                 ]
                 
        buildCmds =  [
                    [0,tar + " xjf "+ os.path.join(downloadPath,base) + ".tar.bz2 --overwrite", ''],  
                    [0,"aclocal", os.path.join(os.getcwd(), base) ],
                    [0,"./configure --prefix=%s --datadir=%s/share --includedir=%s/include " % (PREFIX,PREFIX,PREFIX), os.path.join(os.getcwd(), base) ],
                    [0,"make" ,os.path.join(os.getcwd(), base) ],
                    [0,"make install",os.path.join(os.getcwd(), base) ]
                    ]    
                    
                        
class freeimage:
    pythonModule = False
    active = True
    if isLinux():
        base = 'FreeImage'
        source = [
                    [wget, " http://prdownloads.sourceforge.net/freeimage/FreeImage393.zip",downloadPath],
                 ]
                 
        buildCmds =  [
                    [0, unzip + " -q -o " + os.path.join(downloadPath,base) + "393.zip", ''],
                    ##
                    # Note the use of a single command that does multiple sed's 
                    ##
                    [0, """sed --in-place -s 's|/usr/include|\$(DESTINC)|' Makefile.*\n\
                        sed --in-place -s 's|/usr/lib|\$(DESTLIB)|' Makefile.*\n\
                        sed --in-place -s 's|-o root -g root||' Makefile.*\n\
                        sed --in-place -s 's|ldconfig||' Makefile.*""", os.path.join(os.getcwd(), base) ],
                    [0, "make", os.path.join(os.getcwd(), base) ],
                    [0, "make DESTINC=%s/include DESTLIB=%s/lib install " % (PREFIX, PREFIX) ,os.path.join(os.getcwd(), base) ]
                    ]    
                    
                              
class cmake:
    pythonModule = False
    active = True
    if isLinux():
        base = 'cmake-2.4.6-Linux-i386'
        source = [
                    [wget, "http://www.cmake.org/files/v2.4/"+base+".tar.gz",downloadPath],
                 ]
                 
        buildCmds =  [
                    [0,tar + " xzf "+ os.path.join(downloadPath,base) + ".tar.gz --overwrite", ''],   # unpack it
                    [0,cp + " * %s -r" % PREFIX, os.path.join (os.getcwd(), base) ]   # copy to our bin area
                    ]
                        
class scons:
    pythonModule = False
    active = True
    base = 'scons-0.97.0d20070918'
    source = [
                [wget, "http://downloads.sourceforge.net/scons/"+base+".tar.gz",downloadPath],
             ]
             
    # the utils in Windows don't handle paths or tar spawing gzip hence the work arounds             
    if isLinux():
        buildCmds =  [
                [0, tar + " zxf " + os.path.join(downloadPath,base)+".tar.gz --overwrite",'' ],
                [0,"python setup.py install  --prefix=%s" % PREFIX , os.path.join (os.getcwd(), base) ]
                ]
    
    else:
        buildCmds =  unTarGz( base, downloadPath ) +\
                [
                [0,"python setup.py install  --prefix=%s" % PREFIX , os.path.join (os.getcwd(), base) ]
                ]
             
class boost:    ## also included bjam
    active = True
    version = "3.4"
    pythonModule = False
    ModuleName = ""
    base = 'boost_1_34_1'
        
    myLibraryPaths = [ 'boost/bin.v2/libs/python2.5/build/msvc-8.0/release/threading-multi' ]
    myLibraries = [ 'boost_python-vc80-mt-1_35']
    
        
    if isLinux():
        bjambase = 'boost-jam-3.1.16'
        source = [
             [wget,'http://downloads.sourceforge.net/boost/boost-jam-3.1.16.tgz', downloadPath],
             [wget,'http://downloads.sourceforge.net/boost/boost_1_34_1.tar.gz',downloadPath]
             ]   
        buildCmds  = [
                ## first handle bjam
                [0, tar + ' zxf ' + os.path.join(downloadPath, bjambase) + '.tgz --overwrite', ''],
                [0,"./build.sh gcc", os.path.join(os.getcwd(), bjambase )],
                [0,cp + " bjam %s/bin" % PREFIX, os.path.join(os.getcwd(), bjambase, "bin.linuxx86" )], ## may need to chang eon 64 bit systems
                
                ## and now boost
                [0, tar + ' zxf ' + os.path.join(downloadPath, base) + '.tar.gz', ''],
                [0,'chmod -R +rw ' + base, '' ],
                [0,cp + ' -r '+os.path.join('python-ogre','boost') + '* ' + base , ''],  # need to overwrite the boost with our files
                [0,'sed -i s/BJAM_CONFIG=\"\"/BJAM_CONFIG=release/ boost_1_34_1/configure', '' ],
                [0,"sed -i s/'BOOST_PYTHON_MAX_ARITY 15'/'BOOST_PYTHON_MAX_ARITY 19'/ boost_1_34_1/boost/python/detail/preprocessor.hpp", ''],
                [0,"./configure --with-libraries=python --prefix=%s"  % PREFIX, os.path.join(os.getcwd(), base )],
                [0,'make', os.path.join(os.getcwd(), base )],
                [0,'make install', os.path.join(os.getcwd(), base )],
                ]
                
    if isWindows():
        bjambase = 'boost-jam-3.1.16-1-ntx86'
        source = [
             [wget,'http://downloads.sourceforge.net/boost/boost-jam-3.1.16-1-ntx86.zip',downloadPath] ,
             [wget,'http://downloads.sourceforge.net/boost/boost_1_34_1.tar.gz',downloadPath]
             ] 
                         
        buildCmds =  unTarGz( base, downloadPath ) +\
                [
                [0,cp + ' -r '+os.path.join('python-ogre','boost') + '* ' + base , ''],  # need to overwrite the boost with our files
                [0,'sed -i s/BJAM_CONFIG=\"\"/BJAM_CONFIG=release/ boost_1_34_1/configure', '' ],
                [0,"sed -i s/'BOOST_PYTHON_MAX_ARITY 15'/'BOOST_PYTHON_MAX_ARITY 19'/ boost_1_34_1/boost/python/detail/preprocessor.hpp", ''],
                [0, unzip + " " + os.path.join ( downloadPath, bjambase) + ".zip", ''],
                ]
                             
    BuildCmds = []    # commands to build the library with
       
    
    
####################################################
##
## Now for the actual Python-Ogre Modules
##
####################################################
    
class ogre:
    active = True
    pythonModule = True
    version = "1.4"
    myName='ogre'
    ModuleName='OGRE'
    cflags = ""
    moduleParentLocation = "renderer"
    parent = "ogre/renderer"
    dependsOn = ['boost']
    myLibraryPaths = []
    myLibraries = ['OgreMain']
    libraries = myLibraries
    
#     for mod in dependsOn:
#         libraries += mod.libraries   
#     
    
    if os.name =='nt': 
        # requirements to build a precompiled header on the fly
        if _PreCompiled:
            pchstop = 'python_ogre_precompiled.h'
            pchbuild = 'buildpch.cpp'
            pchincludes = ['python_ogre_precompiled.h']
#         
        libs=[Config.LIB_Boost, 'OgreMain' ] #,  'OgreGUIRenderer', 'CEGUIBase']
        lib_dirs = [ Config.PATH_LIB_Boost
                    ,  Config.PATH_LIB_Ogre_CEGUIRenderer
                    , Config.PATH_LIB_Ogre_OgreMain
                    , Config.PATH_LIB_Ogre_Dependencies #needed for ceguibase.lib etc
                    , Config.PATH_LIB_CEGUI
                     ]
        include_dirs = [ Config.PATH_Boost 
                    , Config.PATH_INCLUDE_Ogre 
                    ]
        CCFLAGS =  '  -DBOOST_PYTHON_MAX_ARITY=19 '
        LINKFLAGS = ''
        externalFiles=['OgreMain.dll', 'OgreGuiRender.dll', Config.LIB_Boost+'.dll']
    elif os.sys.platform <> 'darwin':
        libs=[Config.LIB_Boost, 'OgreMain' ] #,  'OgreGUIRenderer', 'CEGUIBase']
        lib_dirs = [ Config.LOCAL_LIB
                    ]
        include_dirs = [ Config.PATH_Boost 
                    , Config.PATH_INCLUDE_Ogre 
                    ]
        CCFLAGS =  '' ## -DBOOST_PYTHON_MAX_ARITY=19'
        LINKFLAGS = ''
    else:
        libs=[Config.LIB_Boost]
        lib_dirs = [ Config.PATH_LIB_Boost ]
        include_dirs = [ Config.PATH_Boost 
                        , Config.PATH_INCLUDE_Ogre
                        , python_include_dirs
                        , '/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/CFMCarbon' 
                        ]
                                
        CCFLAGS = ' -DBOOST_PYTHON_MAX_ARITY=19 -D_POSIX_C_SOURCE -DCF_OPEN_SOURCE'
        LINKFLAGS = '-framework Python -framework Ogre'
        cflags += '--gccxml-cxxflags "-DCF_OPEN_SOURCE -D_POSIX_C_SOURCE -isysroot /Developer/SDKs/MacOSX10.4u.sdk"'
        
    CheckIncludes=['boost/python.hpp', 'Ogre.h']
    Source = [
    ["wget", "http://prdownloads.sourceforge.net/ogre/ogre-linux_osx-v1-4-5.tar.bz2",''],
    ["wget", "http://prdownloads.sourceforge.net/freeimage/FreeImage393.zip",''], 
 
    ]
     
class ois:
    active = True
    pythonModule = True
    version= "1.1"
    cflags=''
    parent = "ogre/io"
    if os.name=='nt':
        if _PreCompiled:
            pchstop = 'OIS.h'
            pchbuild = 'buildpch.cpp'
            pchincludes = ['boost/python.hpp', 'OIS.h']
        
        libs=['OIS_Static',Config.LIB_Boost]
    else:
        libs=['OIS',Config.LIB_Boost]
    if os.name=="nt":
        libs.append ( "User32" ) # needed for static linking
    include_dirs = [ Config.PATH_Boost 
            , Config.PATH_INCLUDE_OIS
            ]
    lib_dirs = [ Config.PATH_LIB_Boost 
            , Config.PATH_LIB_OIS
            ]
    ModuleName = 'OIS'
    CheckIncludes=['boost/python.hpp', 'OIS.h']
    #externalFiles = ['OIS.dll']
    if os.sys.platform == 'darwin':
        LINKFLAGS = '-framework Python -framework Carbon'
    
class ogrerefapp:
    active = True
    pythonModule = True
    version = "1.4"
    parent = "ogre/physics"
    if os.name=="nt":
        libs=[Config.LIB_Boost, 'OgreMain', 'ode', 'ReferenceAppLayer']
    else:
        libs=[Config.LIB_Boost, 'OgreMain', 'ode', 'ReferenceAppLayer']
    lib_dirs = [ Config.PATH_LIB_Boost
                , Config.PATH_LIB_Ogre_OgreMain
                , Config.PATH_LIB_ODE
                , Config.PATH_LIB_OgreRefApp
                ]
    include_dirs = [ Config.PATH_Boost 
                    ,Config.PATH_INCLUDE_Ogre
                    ,Config.PATH_INCLUDE_OgreRefApp
                    ,Config.PATH_INCLUDE_ODE
                    ]
    CCFLAGS =  ' -DBOOST_PYTHON_MAX_ARITY=19'
    ModuleName = 'OgreRefApp'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'OgreReferenceAppLayer.h', 'ode/ode.h']
    
class ogrenewt:
    active = True
    pythonModule = True
    version = "1.0"
    parent = "ogre/physics"
    if os.name =="nt":
        libs = ['Newton', Config.LIB_Boost, 'OgreNewt_Main', 'OgreMain']
    else:
        libs = ['Newton', Config.LIB_Boost, 'OgreNewt', 'OgreMain']
    include_dirs = [Config.PATH_Boost
                    , Config.PATH_Newton   # only one path for Newton
                    , Config.PATH_INCLUDE_Ogre 
                    , Config.PATH_INCLUDE_OgreNewt
                    , Config.PATH_INCLUDE_Ogre_Dependencies  #needed for OIS/OIS.h
                    ]
    lib_dirs = [ Config.PATH_LIB_Boost
                ,Config.PATH_LIB_Newton
                ,Config.PATH_LIB_OgreNewt
                , Config.PATH_LIB_Ogre_OgreMain
                
                ]
    CCFLAGS =  ' -DBOOST_PYTHON_MAX_ARITY=19'
    ModuleName = 'OgreNewt'
    CheckIncludes=['boost/python.hpp', 'Ogre.h', 'OgreNewt.h', 'Newton.h']
    
class cegui:
    active = True
    pythonModule = True
    version = "0.5.0b" 
    parent = "ogre/gui"
    if os.name=='nt':
        if _PreCompiled:
            pchstop = 'cegui.h'
            pchbuild = 'buildpch.cpp'
            pchincludes = ['boost/python.hpp', 'cegui.h']
        libs=[Config.LIB_Boost, 'CEGUIBase', 'OgreMain', 'OgreGUIRenderer' ]
    else:
        libs=[Config.LIB_Boost, 'CEGUIBase', 'OgreMain', 'CEGUIOgreRenderer' ]

    include_dirs = [Config.PATH_Boost
                    ,Config.PATH_INCLUDE_CEGUI
                    ,Config.PATH_CEGUI
                    , Config.PATH_INCLUDE_Ogre_CEGUIRenderer
                    , Config.PATH_INCLUDE_Ogre
                    , Config.PATH_INCLUDE_Ogre_Dependencies ## needed as OgreCEGUI uses CEGUI/.. in #includes
                    ]
                  
    lib_dirs = [ Config.PATH_LIB_Boost
                ##,  Config.PATH_LIB_Ogre_CEGUIRenderer
                , Config.PATH_LIB_Ogre_OgreMain
                , Config.PATH_LIB_CEGUI
                ,  Config.PATH_LIB_Ogre_Dependencies  
                ]
    CCFLAGS =  ' -DBOOST_PYTHON_MAX_ARITY=19'
    ModuleName = 'CEGUI'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'CEGUI.h', 'OgreCEGUIRenderer.h'] 

                    
#############  these are under construction and DO NOT WORK (Yet) #####################  
  
class ode:
    active = True
    pythonModule = True
    version= "0.9"
    parent = "ogre/physics"
    libs=[Config.LIB_Boost,  'ode']
    if os.name=="nt":
        libs.append ( "User32" ) # needed for MessageBox function
    lib_dirs = [ Config.PATH_LIB_Boost
                ,  Config.PATH_LIB_ODE
                ]
    include_dirs = [ Config.PATH_Boost 
                    ,  Config.PATH_INCLUDE_ODE
                    , os.path.join(Config.PATH_ODE, 'OPCODE')
                    ]

    ModuleName = 'ODE'
    CheckIncludes = ['boost/python.hpp',  'ode/ode.h'] 
    Source = [
        ["wget", "http://prdownloads.sourceforge.net/opende/ode-src-0.8.zip",'']
    ]
        
class opcode:
    active = True
    pythonModule = True
    version= "1.3"
    parent = "ogre/physics"
    libs=[Config.LIB_Boost]
    lib_dirs = [ Config.PATH_LIB_Boost
                ]
    include_dirs = [ Config.PATH_Boost 
                    ,  Config.PATH_INCLUDE_OPCODE
                    ,  os.path.join(Config.PATH_INCLUDE_OPCODE, 'Ice' )

                    ]
    CCFLAGS =  ' -DBAN_OPCODE_AUTOLINK -DICE_NO_DLL '
    ModuleName = 'Opcode'
    CheckIncludes = ['boost/python.hpp',  'Opcode.h'] 
       
class caelum:
    active = True
    pythonModule = True
    version="0.2.1"
    parent="ogre/addons"
    cflags = ""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_caelum
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain' ]
    ModuleName="caelum"  
     
class ogreode:
    active = True
    pythonModule = True
    version= "1.0"
    parent = "ogre/physics"

    lib_dirs = [ Config.PATH_LIB_Boost
                , Config.PATH_LIB_OgreOde
                , Config.PATH_LIB_OgreOdePrefab
                , Config.PATH_LIB_OgreOdeLoader
                , Config.PATH_LIB_Ogre_OgreMain
                ,  Config.PATH_LIB_ODE
                ]
    include_dirs = [ Config.PATH_Boost 
                , Config.PATH_INCLUDE_ODE
                , Config.PATH_INCLUDE_OgreOde
                , Config.PATH_INCLUDE_OgreOdePrefab
                , Config.PATH_INCLUDE_OgreOdeLoader
                , Config.PATH_INCLUDE_Ogre
                ]

    libs=[Config.LIB_Boost, 'OgreMain', 'ode', 'OgreOde_Core', 'OgreOde_Prefab', 'OgreOde_Loader' ]
    CCFLAGS =  ' -DBOOST_PYTHON_MAX_ARITY=19'
    CheckIncludes = ['boost/python.hpp', 'Ogre.h', 'ode/ode.h', 'ode/odecpp.h', 'OgreOde_Core.h', 'OgreOde_Loader.h', 
                    'Ogreode_Prefab.h'] 
                     
    ModuleName='OgreOde'
    
class quickgui:
    active = True
    pythonModule = True
    version="0.9.7"
    parent="ogre/gui"
    ## note the defined for _QuickGUIExport forces non dll usage
    if os.name=='nt': 
        ## note the defined for _QuickGUIExport forces non dll usage 
        CCFLAGS = ' -DWIN32 -DNDEBUG -DWINDOWS -D_QuickGUIExport="" ' 
#         if _PreCompiled:
#             pchstop = 'boost/python.hpp'
#             pchbuild = 'buildpch.cpp'
#             pchincludes = ['boost/python.hpp']
    else:
        CCFLAGS = ' -D_QuickGUIExport="" '
    cflags=""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_quickgui
                    #,Config.PATH_INCLUDE_freetype
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain
                ##,Config.PATH_LIB_quickgui
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain' ]
    ModuleName="QuickGUI"   

class navi:
    active = True
    pythonModule = True
    version="1.4"
    parent="ogre/gui"
    CCFLAGS = '-D"WIN32" -D"NDEBUG", -D"WINDOWS"' 
    cflags=""
    include_dirs = [ Config.PATH_Boost
                    ,Config.PATH_INCLUDE_Ogre
                    ,Config.PATH_INCLUDE_navi
                    ,os.path.join(Config.PATH_navi,'..','Dependencies','win32','llmozlib','include')
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain
                ,Config.PATH_LIB_navi
                ,os.path.join(Config.PATH_navi,'..','Dependencies','win32','llmozlib','lib')
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'Navi', 'OgreMain','llmozlib', 'user32',
            'kernel32.lib', 'gdi32.lib', 'winspool.lib', 'comdlg32.lib', 'advapi32.lib',
            'shell32.lib','ole32.lib','oleaut32.lib','uuid.lib' ]
    ModuleName="navi"   

class betagui:
    active = True
    pythonModule = True
    version="2.0"
    parent="ogre/gui"
    CCFLAGS = ' ' # -D"FT2_BUILD_LIBRARY"
    cflags=""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_betagui
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain,
                Config.PATH_LIB_betagui
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain' ]
    ModuleName="betagui"   

class ogreforests:
    active = True
    pythonModule = True
    version="0.1"
    parent="ogre/addons"
    CCFLAGS = ' ' # -D"FT2_BUILD_LIBRARY"
    cflags=""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_ogreforests
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain' ]
    ModuleName="ogreforests"   

    
class particleuniverse:
    active = True
    pythonModule = True
    version="0.4"
    parent="ogre/addons"
    CCFLAGS = ' ' 
    cflags=""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_particleuniverse
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain,
                Config.PATH_LIB_particleuniverse
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain', 'ParticleUniverse' ]
    ModuleName="particleuniverse"   
            
class nxogre:
    active = True
    pythonModule = True
    version="1.0a"
    parent="ogre/physics"
    cflags=""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_NxOgre,
                    ]
    for d in Config.PATH_INCLUDE_PhysX:
        include_dirs.append( d )
    if os.name == 'nt':
        CCFLAGS = ' -DWIN32 -DNxExport="" '
#         if _PreCompiled:
#             pchstop = 'nxogre.h'
#             pchbuild = 'buildpch.cpp'
#             pchincludes = ['boost/python.hpp', 'nxogre.h']

    else:
        CCFLAGS = ' -DNX_LINUX -DLINUX -DNX_DISABLE_FLUIDS '                
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain,
#                 Config.PATH_LIB_NxOgre,
                Config.PATH_LIB_PhysX
                ]
    CheckIncludes=[]
#     libs=[  Config.LIB_Boost, 'NxOgre','NxCharacter', 'NxCooking', 'NxExtensions', 'PhysXLoader','OgreMain' ]
    if os.name =='nt':
        libs=[  Config.LIB_Boost, 'NxCharacter', 'NxCooking', 'NxExtensions', 'PhysXLoader','OgreMain' ]
    else:
        libs=[  Config.LIB_Boost, 'NxCharacter', 'NxCooking', 'PhysXCore','PhysXLoader','OgreMain' ]
    ModuleName="NxOgre"   
    
    
class theora:
    active = True
    pythonModule = True
    version="0.5.0"
    parent="ogre/addons"
    cflags=""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_Theora,
                    ]
    for d in Config.PATH_INCLUDE_OggVorbisTheora:
        include_dirs.append( d )
        
    if os.name == 'nt':
        CCFLAGS = ' -D"WIN32" '
                    
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain,
                Config.PATH_LIB_Theora,
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'Plugin_TheoraVideoSystem','OgreMain' ]
    ModuleName="theora"   
 
class plib:
    active = True
    pythonModule = True
    version="1.8.4"
    parent="ogre/addons"
    cflags=""
    CCFLAGS = ' -D"_CRT_SECURE_NO_WARNINGS" ' 
    include_dirs= [ Config.PATH_INCLUDE_plib,
            Config.PATH_THIRDPARTY   # needed as net.h includes via plib/xxxx
            ]
    lib_dirs = [ Config.PATH_LIB_Boost,
            ]           
    ModuleName = "plib"
    CheckIncludes=[]
    if os.name == "nt":
        libs = [ Config.LIB_Boost, 'winmm', 'User32','wsock32', 'opengl32.lib','glu32.lib' ]
    else:
        libs = [ Config.LIB_Boost ] # Note sure what linux libraries are needed
    
class physx:
    active = True
    pythonModule = True
    version="2.7.3"
    parent="ogre/physics"
    cflags=""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    ]
    for d in Config.PATH_INCLUDE_PhysX:
        include_dirs.append( d )
    if os.name == 'nt':
        CCFLAGS = ' -D"WIN32" '
    else:
        CCFLAGS = ' -D"LINUX" '                
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain,
                Config.PATH_LIB_PhysX
                ]
    CheckIncludes=[]
    if os.name == 'nt':
        libs=[  Config.LIB_Boost, 'NxCharacter', 'NxCooking', 'NxExtensions', 'PhysXLoader' ]
    else:
        libs=[  Config.LIB_Boost, 'NxCharacter', 'NxCooking',  'PhysXCore', 'PhysXLoader' ]
    ModuleName="PhysX"   
           
class ogreal:
    active = True
    pythonModule = True
    version="0.3"
    parent = "ogre/sound"
    
    include_dirs = [ Config.PATH_Boost
                , Config.PATH_INCLUDE_Ogre
                , Config.PATH_INCLUDE_OgreAL
                , Config.PATH_INCLUDE_OGG
                , Config.PATH_INCLUDE_VORBIS
                , Config.PATH_INCLUDE_OPENAL
                ]  
    if os.name=='nt':                  
        lib_dirs = [ Config.PATH_LIB_Boost
                ,Config.PATH_LIB_Ogre_OgreMain    
                ,os.path.join(Config.PATH_OGG, 'win32', 'Static_Release')
                ,os.path.join(Config.PATH_VORBIS, 'win32','Vorbis_Static_Release')
                ,os.path.join(Config.PATH_VORBIS, 'win32','VorbisEnc_Static_Release')
                ,os.path.join(Config.PATH_VORBIS, 'win32','VorbisFile_Static_Release')
                ,os.path.join(Config.PATH_OPENAL, 'libs','Win32')
                  ] 
    else:
        lib_dirs = [ Config.PATH_LIB_Boost
                ,Config.PATH_LIB_Ogre_OgreMain    
                  ] 
                  
    if os.name =='nt':
        CCFLAGS = ' -DOgreAL_Export="" -DWIN32 -DNDEBUG -D_LIB -D_WIN32 -D_WINDOWS -DVORBIS_IEEE_FLOAT32 -D_USE_NON_INTEL_COMPILER '              
        libs=[Config.LIB_Boost, 'OgreMain', 
                    'ogg_static', 
                    'vorbis_static','vorbisfile_static','vorbisenc_static',
                    'OpenAL32', 'EFX-Util']  ##  'OgreAL' -- going to compile OgreAL ourselves
    else:                    
        libs=[Config.LIB_Boost, 'OgreMain', 
                    'ogg', 
                    'vorbis','vorbisfile','vorbisenc',
                    'openal']  ##  'OgreAL' -- going to compile OgreAL ourselves
    ModuleName = 'OgreAL'
    CheckIncludes = ['OgreAL.h']
    Source = [
        ["wget", "http://www.openal.org/openal_webstf/downloads/openal-0.0.8.tar.gz",''],
        ["wget", "http://downloads.xiph.org/releases/ogg/libogg-1.1.3.tar.gz",''],
        ["wget", "http://downloads.xiph.org/releases/vorbis/libvorbis-1.2.0.tar.gz",''],
        ]
        
class ogrevideoffmpeg:
    active = True
    pythonModule = True
    version="0.2.1"
    parent="ogre/addons"
    cflags = ""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_ogrevideoffmpeg,
                    Config.PATH_INCLUDE_ffmpeg
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain,
                Config.PATH_LIB_ogrevideoffmpeg,
                Config.PATH_LIB_ffmpeg
                ]
    CheckIncludes=[]
    if os.name == 'nt':
        libs=[  Config.LIB_Boost, 'OgreMain', 'avformat-51', 'avcodec-51', 'avutil-49' ]
    else:
        libs=[  Config.LIB_Boost, 'OgreMain', 'avformat', 'avcodec', 'avutil', 'z', 'GL', 'GLU', 'Xxf86vm', 'Xext', 'X11' ]
    ModuleName="ogrevideoffmpeg"   

class ogredshow:
    active = False
    pythonModule = True
    version="0.1"
    parent="ogre/addons"
    if os.name=='nt': 
        ## note the defined for _ETManagerExport forces non dll usage 
        CCFLAGS = ' -DWIN32 -DNDEBUG -DWINDOWS -D_ETManagerExport="" ' 
    else:
        CCFLAGS = ''
    cflags = ""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_INCLUDE_ogredshow
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain,
                
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain','Strmiids' , 'ole32']
    ModuleName="ogredshow"   
    
class et:  ## editable terrain
    active = True
    pythonModule = True
    version= "2.2"
    parent = "ogre/addons"
    libs=[ Config.LIB_Boost, 'OgreMain' ]
    lib_dirs = [ Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain
                ]
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_Ogre,
                    Config.PATH_et
                    ]
    ModuleName = 'et'
    CheckIncludes = ['boost/python.hpp'] 
    cflags = ""
    
class bullet:
    active = True
    pythonModule = True
    version= "2.64"
    parent = "ogre/physics"
    libs=[Config.LIB_Boost,  'LibBulletCollision', 'LibBulletDynamics']
    if os.name == 'nt':
        libs.append('libbulletmath')
    lib_dirs = [ Config.PATH_LIB_Boost
                ,  Config.PATH_LIB_Bullet
                ]
    include_dirs = [ Config.PATH_Boost 
                    ,  Config.PATH_INCLUDE_Bullet
                    ]

    ModuleName = 'bullet'
    CheckIncludes = ['boost/python.hpp'] 
    cflags = ""
    
class ogrebulletc:  # 
    active = True
    pythonModule = True
    version = "1.0"
    cflags = ""
    parent = "ogre/physics"
    libs = [Config.LIB_Boost,  'OgreMain', 
        'LibBulletCollision', 'LibBulletDynamics'
        ]
    if os.name == 'nt':
        libs.append('LibBulletMath')        
    include_dirs = [Config.PATH_Boost
                    , Config.PATH_INCLUDE_Bullet
                    , os.path.join(Config.PATH_OgreBullet, 'Collisions' )
                    , Config.PATH_INCLUDE_Ogre 
                    , Config.PATH_INCLUDE_Ogre_Dependencies  
                    ]
    lib_dirs = [ Config.PATH_LIB_Boost
                ,Config.PATH_LIB_Bullet
                , Config.PATH_LIB_Ogre_OgreMain
                ]
    if os.name=="nt":
        CCFLAGS =  ' -DWIN32 -DNDEBUG -D_WINDOWS -D_PRECOMP '
    else:
        CCFLAGS = ' -D_PRECOMP '
    ModuleName = 'OgreBulletC'
    CheckIncludes=['boost/python.hpp', 'Ogre.h']    

class ogrebulletd:  # 
    active = True
    pythonModule = True
    version = "1.0"
    cflags = ""
    parent = "ogre/physics"
    libs = [Config.LIB_Boost,  'OgreMain', 
        'LibBulletCollision', 'LibBulletDynamics'
        ]
    if os.name == 'nt':
        libs.append('LibBulletMath')        
    include_dirs = [Config.PATH_Boost
                    , Config.PATH_INCLUDE_Bullet
                    , os.path.join(Config.PATH_OgreBullet, 'Collisions' )
                    , os.path.join(Config.PATH_OgreBullet, 'Dynamics' )
                    , Config.PATH_INCLUDE_Ogre 
                    , Config.PATH_INCLUDE_Ogre_Dependencies  
                    ]
    lib_dirs = [ Config.PATH_LIB_Boost
                ,Config.PATH_LIB_Bullet
                , Config.PATH_LIB_Ogre_OgreMain
                ]
    if os.name=="nt":
        CCFLAGS =  ' -DWIN32 -DNDEBUG -D_WINDOWS -D_PRECOMP '
    else:
        CCFLAGS = ' -D_PRECOMP '
    ModuleName = 'OgreBulletD'
    CheckIncludes=['boost/python.hpp', 'Ogre.h']        
    
class noise:
    active = True
    pythonModule = True
    version="1.0"
    parent="ogre/addons"
    cflags = ""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_INCLUDE_noise,
                    os.path.join(Config.PATH_INCLUDE_noise, 'model'),
                    os.path.join(Config.PATH_INCLUDE_noise, 'module')
                    ]
    lib_dirs = [Config.PATH_LIB_Boost
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost]
    ModuleName="noise"   
    
class watermesh:
    active = True
    pythonModule = True
    version="1.0"
    parent="ogre/addons"
    cflags = ""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_watermesh
                    , Config.PATH_INCLUDE_Ogre
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain' ]
    ModuleName="watermesh"   
    
class ofusion:
    active = True
    pythonModule = True
    version="1.0"
    parent="ogre/addons"
    cflags = ""
    include_dirs = [ Config.PATH_Boost,
                    Config.PATH_ofusion
                    , Config.PATH_INCLUDE_Ogre
                    ]
    lib_dirs = [Config.PATH_LIB_Boost,
                Config.PATH_LIB_Ogre_OgreMain
                ]
    CheckIncludes=[]
    libs=[  Config.LIB_Boost, 'OgreMain' ]
    ModuleName="ofusion"   
    
############################################################################################

## Here is the master list....
## Keep eveything here in lowercase so the over rides work :)
    
projects = {
    'ois' : ois
    , 'ogre' : ogre
    , 'cegui' : cegui
    , 'ode' : ode
#     , 'newton' : newton
    , 'ogrerefapp' : ogrerefapp
    , 'ogrenewt' : ogrenewt
    , 'ogreode' : ogreode
    , 'ogreal' : ogreal
    , 'quickgui' : quickgui
    , 'opcode' : opcode
    , 'nxogre' : nxogre
    , 'bullet' : bullet
    , 'physx' : physx
    , 'betagui': betagui
    , 'theora' : theora
    , 'ogrevideoffmpeg' : ogrevideoffmpeg
    , 'ogredshow' : ogredshow
    , 'plib' : plib
    , 'navi': navi
    , 'ogrebulletc' : ogrebulletc
    , 'ogrebulletd' : ogrebulletd
    , 'ogreforests' : ogreforests
    , 'et' : et
    , 'caelum' : caelum
    , 'noise' : noise
    , 'watermesh' : watermesh
    , 'ofusion' : ofusion
    , 'particleuniverse' : particleuniverse
}        

#
# let's setup some defaults
#    
def CheckPaths ( cls , name):
    """ lets check we can find files listed in the CheckIncludes list somewhere in the include_dirs directories
    also look for libs in the lib_dirs.
    This way we can warn people early if they have a configuration problem"""
    if cls.active:  ## only check active classes
        for incfile in cls.CheckIncludes:
            found = False
            log ( "Checking for %s include file (%s class) in include_dirs" % (incfile, name) )
            for lookdir in cls.include_dirs:
                if os.path.isfile ( os.path.join ( lookdir, incfile ) ):
                    found = True
                    break
            if not found:
                print "WARNING: Unable to find %s include file (%s class) in include_dirs" % (incfile, name)
# #         if os.name =='nt': 
# #             for libfile in cls.libs :
# #                 libfile += '.lib'
# #                 found = False
# #                 log ( "Checking for %s library (%s class) in lib_dirs" % (libfile, name) )
# #                 for lookdir in cls.lib_dirs:
# #                     if os.path.isfile ( os.path.join ( lookdir, libfile ) ):
# #                         found = True
# #                         break
# #                 if not found:
# #                     print "WARNING: Unable to find %s library (%s class) in lib_dirs" % (libfile, name)
    
#
# a couple of specials that should be done differently
gccxml_bin = Config.gccxml_bin
pyplusplus_install_dir = Config.pyplusplus_install_dir      
         
rpath= Config.RPATH  
                          
for name, cls in projects.items():

    # little hack to allow overriding of settings from the config file
    if hasattr(Config, name):   # look to see if the class exists in the config file
        print "Using Override for class ", name
        _class = Config.__dict__[name]  # get the class
        for key, value in _class.__dict__.items():
            if not key.startswith('_'):
                cls.__dict__[key] = value   
                print "Set %s.%s to %s" % (name, key, value) 
                
    CheckPaths( cls, name )
    cls.root_dir = os.path.join( root_dir, 'code_generators', name )
    cls.dir_name = name + '_' + str(cls.version)
    cls.generated_dir = os.path.join( generated_dir, cls.dir_name )
    cls.cache_file = os.path.join( declarations_cache_dir, cls.dir_name + '_cache.xml' )
    if not hasattr (cls, 'ModuleName'):
        cls.ModuleName = name[0].upper() + name[1:]
    if not hasattr (cls, 'PydName'):
        cls.PydName = '_' + name.lower() + '_'
        if os.name == 'nt':
            cls.PydName = cls.PydName + '.pyd'
        elif os.name == 'posix':
            cls.PydName = cls.PydName + '.so'
        elif os.name =='mac':
            print "WARNING - check the last line of environment.py!!"
