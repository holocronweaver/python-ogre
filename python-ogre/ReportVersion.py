#
# module to assit with versioning on python-ogre
#
import os, sys, time, shutil
import logging
import BuildModule as bm

#add environment to the path
sys.path.append(  '.' )
#add common utils to the pass
if sys.platform == 'win32': 
    newpath = os.path.join ( os.path.abspath(os.path.dirname(__file__)), 'plugins')
    os.environ['PATH'] =  newpath +';' + os.environ['PATH']
    
      
if __name__ == '__main__':

    moduleList =      ( 'ogre.renderer.OGRE',
                        'ogre.io.OIS', 
                        'ogre.gui.CEGUI','ogre.gui.QuickGUI', 'ogre.gui.betagui',
                        'ogre.sound.OgreAL', 
                        'ogre.physics.ODE', 'ogre.physics.OgreRefApp',
                        'ogre.physics.OgreOde', 'ogre.physics.OgreNewt', 'ogre.physics.Opcode',
                        'ogre.physics.bullet','ogre.physics.PhysX','ogre.physics.NxOgre',
                        'ogre.physics.OgreBulletC', 'ogre.physics.OgreBulletD',
                        'ogre.addons.theora', 'ogre.addons.plib',
                        'ogre.addons.ogreforests', 'ogre.addons.et', 'ogre.addons.caelum',
                        'ogre.addons.noise', 'ogre.addons.watermesh', 
                        'ogre.addons.particleuniverse', 'ogre.addons.cadunetree',
                        'ogre.renderer.ogrepcz', 'ogre.addons.hydrax' )
#                         'ogre.gui.hikari' )
                        
    bm.setupLogging("version.info") # options.logfilename)
    logger = logging.getLogger('PythonOgre.ReportVersionInfo')
    for name in moduleList:
#         logger.info ("Attemping to import " + name )
        try:
            basename = name.split('.')[-1]
            mod = __import__(name)
            components = name.split('.')
            for comp in components[1:]:
                mod = getattr(mod, comp)
            print "Module %s ( %s )" % (basename, name)                
            print "   PythonOgre Ver: %s" % (getattr(mod, "PythonOgreVersion__") )
            print "   Module Version: %s" % (getattr(mod, "Version__") )
            print "   Python Version: %s" % (getattr(mod, 'PythonVersion__') )
            print "   Code Generated: %s" % (getattr(mod, 'PythonOgreDetail__').split('_')[-1])
            print "   Compiled      : %s %s" % (getattr(mod, 'CompileTime__'), getattr(mod, 'CompileDate__') )
            print "   Raw Detail    : %s" % (getattr(mod, 'PythonOgreDetail__') )
#             logger.info ( "Import Successful: " + basename )
        except ImportError:
            logger.error ( "Import failed:" + basename )