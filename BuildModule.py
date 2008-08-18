#
#  BuildModule will build a Python-Ogre module.
#

## Curent

from optparse import OptionParser
import subprocess
import environment
import logging
import sys
import types
import os
import time

logger = None
FULL_LOGGING = False    # Set to true to log everything, even if successful
ENV_SET = False         # global to ensure we don't set the environment too many times and break the shell.

def setupLogging (logfilename):
    # set up logging to file
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
                        datefmt='%m-%d %H:%M',
                        filename=logfilename,
                        filemode='w')
    # define a Handler which writes INFO messages or higher to the sys.stderr
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    # set a format which is simpler for console use
    formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
    # tell the handler to use this format
    console.setFormatter(formatter)
    # add the handler to the root logger
    logging.getLogger('').addHandler(console)
  
    
def getClassList ():
    """ create a dictionary of classes from the environment modules
    """
    dict = {}
    for c in dir(environment):
        var = environment.__dict__[c]
        if isinstance ( var, types.ClassType ) :    ## OK so we know it's a class      
#             logger.debug ( "getClassList: Checking %s" % c )
            if hasattr(var, 'active') and hasattr(var, 'pythonModule'):   # and it looks like one we care about
#                 logger.debug ( "getClassList: it's one of ours")
                dict[c] = var
    return dict
                
def exit( ExitMessage ):
     logger.error( ExitMessage )
     sys.exit( -1 )

  
## This stuff has to be in my spawing sub process        
# # export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$PREFIX/lib/pkgconfig
# # export LD_LIBRARY_PATH=$PREFIX/lib
# # export CFLAGS="-I$PREFIX/include -L$PREFIX/lib"
# # export CXXFLAGS=$CFLAGS
# # export LDFLAGS="-Wl,-rpath='\$\$ORIGIN/../../lib' -Wl,-rpath='\$\$ORIGIN' -Wl,-z,origin"
# # export PATH=$PREFIX/bin:$PATH
# # export PYTHONPATH=$PREFIX/lib/python$PYTHONVERSION/site-packages     
     
     
def spawnTask ( task, cwdin = '', getoutput=None ):
    """Execute a command line task and manage the return code etc
    """
    global ENV_SET
    PREFIX = environment.PREFIX
    PATH = os.environ["PATH"]
    env = os.environ
    env["USESYSTEM"] = str(environment.UseSystem)
    if not ENV_SET: # this actually changes the environment so we shouldn't do it more than once
        env["PKG_CONFIG_PATH"]=os.path.join(PREFIX,"lib/pkgconfig")
        env["LD_LIBRARY_PATH"]=os.path.join(PREFIX,"lib")
        if environment.isMac():
            env["CFLAGS"]="-I"+PREFIX+"/include -L"+PREFIX+"/lib"  
            env["CXXFLAGS"]="-I"+PREFIX+"/include -L"+PREFIX+"/lib"
            ##env["LDFLAGS"]="-Wl,-rpath='\$\$ORIGIN/../../lib' -Wl,-rpath='\$\$ORIGIN' -Wl,-z,origin"  ### Mac GCC 4.0.1 doesn't support rpath
            env["PYTHONPATH"]=PREFIX+"/lib/python"+environment.PythonVersionString+"/site-packages"
        else:
            env["CFLAGS"]="-I"+os.path.join(PREFIX,"include")+ " -L"+os.path.join(PREFIX,"lib")  
            env["CXXFLAGS"]=env["CFLAGS"]
            env["LDFLAGS"]="-Wl,-rpath='$$ORIGIN/../../lib' -Wl,-rpath='$$ORIGIN' -Wl,-z,origin"
            env["PYTHONPATH"]=PREFIX+"/lib/python"+environment.PythonVersionString+"/site-packages"
            env["ZZIPLIB_LIBS"]="-lzzip"
    
        env["PATH"]=PREFIX+"/bin:" + PATH
        ENV_SET=True        
     
    logger.debug ( "Spawning '%s' in '%s'" % (task,cwdin) )
    process = subprocess.Popen (task, shell=True,stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd = cwdin, env=env)
    try:
        out,err = process.communicate()
        returncode = process.returncode
    except:
        returncode = -1
    
    if getoutput is not None:
        if returncode != -1:
            getoutput.write(out)
            getoutput.write(err)

    if returncode != 0:
        logger.warning ( "Task Failed" )
        logger.debug ( out )
        logger.debug ( err )
    elif FULL_LOGGING:
        logger.warning ( "Full Logging ON" )
        logger.debug ( out )
        logger.debug ( err )
            
    if returncode != 0 and FAILHARD:
        exit(" The following command failed %s" % task)
    return returncode     
     
def retrieveSource ( module ):
    """ Here's we we retrieve the necessary source files
    """    
    for cmd,args,cwd in module.source:
        logger.info ("Retrieving %s" % args )
        ret = spawnTask ( cmd+ " " + args, cwd  )

    if not hasattr(module, "extractCmds"):
        return

    for type_,cmd,path in module.extractCmds:
        logger.info ("Extracting and patching %s" % args )
        if path =='':
            path = os.getcwd()
        ret = spawnTask ( cmd, path  )

        
def buildModule ( module ):
    """ Execute the build commands for the module
    """
    for type_,cmd,path  in module.buildCmds:
        if path =='':
            path = os.getcwd()
        if type_ == 0 : # it's a shell command
            logger.info ("Build Command %s " % cmd)
            ret = spawnTask ( cmd, path )
        elif type_ == 1 :
            logger.debug ("Compiling %s " % cmd )
            code = compile ( cmd, '<string>', 'exec' )
            logger.debug ("executing codeblock "  )
            exec code 
            logger.debug ( "Exec done")

from cStringIO import StringIO
def buildDeb ( module, install = False ):
    """ Create a debian package for the module
    """
    logger.info("Trying to build a debain package for module %s" % module.base)
    srcdir = os.path.join(os.getcwd(), module.base)
    if not os.path.exists(srcdir):
        exit("You need to get the src first, use -r")

    # Check if the module already has debian rules to create a package
    debiandir = os.path.join(srcdir, "debian")
    debianour = os.path.join("python-ogre", "debs", "%s-debian" % module.base)

    if not os.path.exists(debiandir):
        logger.info("Package does not come with a debian dir, seeing if we have our own..")

        # Do we have a debian rules we can copy into the package?
        if not os.path.exists(debianour):
            logger.error("We do not have a debian dir!?")
            exit("Can not build debian packages for this module.")
        else:
            # Do the copy
            ret = spawnTask("cp -rvf %s %s" % (debianour, debiandir), os.getcwd())
            if ret != 0:
                exit("Was not able to copy the debian directory over")
   
    # Rewrite any place holders
    ret = spawnTask( "sed --in-place "+debiandir+"/changelog -e \"s|%%SHORTDATE%%|`date +%Y%m%d`|\" -e \"s|%%LONGDATE%%|`date +'%a, %d %b %Y %H:%m:%S %z'`|\"", srcdir)
    if ret != 0:
        exit("Was not able to update the debian change log.")
    ret = spawnTask( "sed --in-place "+debiandir+"/control -e \"s|%%SHORTDATE%%|`date +%Y%m%d`|\" -e \"s|%%LONGDATE%%|`date +'%a, %d %b %Y %H:%m:%S %z'`|\"", srcdir)
    if ret != 0:
        exit("Was not able to update the debian control file.")
 
    # Check that all the dependencies for this package are avaliable
    c = StringIO()
    ret = spawnTask("dpkg-checkbuilddeps", srcdir, c)
    if ret != 0:
        c = c.getvalue().strip()
        c = c.split(':')[-1].strip().split()
        logger.info ("Please install the following dependencies %r " % c)
        exit("The package's dependencies have not been installed.")

    os.chmod(os.path.join(debiandir, "rules"), 0755)

    ret = spawnTask("fakeroot ./debian/rules binary", srcdir, c)
    if ret != 0:
        logger.warning("Was not able to build package")
    logger.info("Package successfully built!")

def buildInstall ( module ):
    """ Create a debian package for the module
    """
    logger.info("Trying to install the debian package (make sure you run sudo before this command)")
    srcdir = os.path.join(os.getcwd(), module.base)
    ret = spawnTask("sudo dpkg --install %s*.deb" % module.base, os.path.split(srcdir)[0])
    if ret != 0:
        logger.warning("Was not able to the install package!")
    logger.info("Package installed.")
 
def generateCode ( module ):
    """ Generate the C++ wrapper code
    """
    logger.info ( "Building Source code for " + module.name )
    ### AJM -- note the assumption that environment.py is sitting in the 'python-ogre' directory...
    ret = spawnTask ( 'python generate_code.py'+('', ' --usesystem')[environment.UseSystem], os.path.join(environment.root_dir, 'code_generators', module.name) ) 

def compileCode ( module ):
    """ Compile the wrapper code and make the modules
    """
    logger.info ( "Compiling Source code for " + module.name )
    ### AJM -- note the assumption that environment.py is sitting in the 'python-ogre' directory...
    ret = spawnTask ( 'scons PROJECTS='+module.name, os.path.join(environment.root_dir) )     
    if ret != 0 :
        time.sleep(5)  ## not sure why scons doesn't work after first failure       

FAILHARD=False
def parseInput():
    """Handle command line input """
    usage = "usage: %prog [options] moduleName"
    parser = OptionParser(usage=usage, version="%prog 1.0")
    parser.add_option("-r", "--retrieve", action="store_true", default=False,dest="retrieve", help="Retrieve the appropiate source before building")
    parser.add_option("-b", "--build", action="store_true", default=False ,dest="build", help="Build the appropiate module")
    parser.add_option("-g", "--gen", action="store_true", default=False ,dest="gencode", help="Generate Source Code for the module")
    parser.add_option("-d", "--build-deb", action="store_true", default=False ,dest="builddeb", help="Build a debian package for the module")
    parser.add_option("",   "--install-deb", action="store_true", default=False ,dest="installdeb", help="Install the debian packages after building")
    parser.add_option("-c", "--compile", action="store_true", default=False ,dest="compilecode", help="Compile Source Code for the module")
    parser.add_option("-l", "--logfilename",  default="log.out" ,dest="logfilename", help="Override the default log file name")
    parser.add_option("-G", "--genall", action="store_true", default=False ,dest="gencodeall", help="Generate Source Code for all possible modules")
    parser.add_option("-C", "--compileall", action="store_true", default=False ,dest="compilecodeall", help="Compile Source Code for all posssible modules")
    parser.add_option("-e", "--failhard", action="store_true", default=False, dest="failhard", help="Exit with failure code on first error.")  
 
    (options, args) = parser.parse_args()
    return (options,args)
    
if __name__ == '__main__':
    classList = getClassList ()
    
    (options, args) = parseInput()
    if len(args) == 0 and not (options.compilecodeall or options.gencodeall):
        exit("The module to build wasn't specified.  Use -h for help")

    setupLogging(options.logfilename)
    logger = logging.getLogger('PythonOgre.BuildModule')
        
    if options.retrieve==False and options.build==False and options.gencode==False and options.compilecode==False\
            and options.compilecodeall==False and options.gencodeall==False and options.builddeb==False\
            and options.installdeb==False:
        exit ( "You need to specific at least one option. Use -h for help")
    if options.builddeb and options.build:
        exit ( "You can only specify build or builddeb, not both!" )

    FAILHARD=options.failhard

    if not os.path.exists( environment.downloadPath ):
        os.mkdir ( environment.downloadPath )    

    if not os.path.exists( environment.Config.ROOT_DIR ):
        os.mkdir ( environment.Config.ROOT_DIR )    
    if not os.path.exists( os.path.join(environment.Config.ROOT_DIR, 'usr' ) ):
        os.mkdir ( os.path.join(environment.Config.ROOT_DIR, 'usr' )  )    
    if options.gencodeall or options.compilecodeall:
        for name,cls in environment.projects.items():
            if cls.active and cls.pythonModule:
                if options.gencodeall:
                    generateCode( cls )
                if options.compilecodeall:
                    compileCode( cls )                    

    else:
        for moduleName  in args:        
            if not classList.has_key( moduleName ):
                exit("Module specificed was not found (%s is not in environment.py) " % moduleName )
            if options.retrieve:    
                retrieveSource ( classList[ moduleName ] )
            if options.builddeb:
                buildDeb( classList[ moduleName ] )
            if options.installdeb:
                buildInstall( classList[ moduleName ] )

            if options.build :       
                buildModule ( classList[ moduleName ] )
            if options.gencode :
                if classList[ moduleName ].pythonModule == True:
                    generateCode ( classList[ moduleName ] )
                else:
                    print ( "Module specificed does not generate source code (%s is a supporting module)" % moduleName )
            if options.compilecode :
                if classList[ moduleName ].pythonModule == True:
                    compileCode ( classList[ moduleName ] )
                else:
                    print ( "Module specificed does not need compiling (%s is a supporting module)" % moduleName )
            
                