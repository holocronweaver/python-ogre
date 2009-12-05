# This code is in the Public Domain
# -----------------------------------------------------------------------------
# This source file is part of Python-Ogre
# For the latest info, see http://python-ogre.org/
#
# It is likely based on original code from OGRE and/or PyOgre
# For the latest info, see http://www.ogre3d.org/
#
# You may use this sample code for anything you like, it is not covered by the
# LGPL.
# -----------------------------------------------------------------------------
import sys
sys.path.insert(0,'..')
import PythonOgreConfig

import ogre.renderer.OGRE as ogre
import ogre.physics.bullet as bullet
import ogre.physics.OgreBulletC as bulletC
import ogre.physics.OgreBulletD as bulletD

import SampleFramework as sf
pThrusters = None








import ogre.io.OIS as OIS
KC_N = OIS.KC_N
KC_M = OIS.KC_M
KC_H = OIS.KC_H
KC_J = OIS.KC_J

class SampleApplication(sf.Application):
    def _createScene(self):
        global pThrusters
        sceneManager = self.sceneManager
        sceneManager.ambientLight = ogre.ColourValue(0.5, 0.5, 0.5)
        sceneManager.setSkyBox(True, "Examples/SpaceSkyBox")
        
        # Need a light 
   
        light = sceneManager.createLight('MainLight')
        light.setPosition (20, 80, 50)
        # Also add a nice starship in
        ent = sceneManager.createEntity( "razor", "razor.mesh" )

        node = sceneManager.getRootSceneNode().createChildSceneNode()
        node.attachObject( ent )
        
        # Now for the thrusters
        pThrusters = sceneManager.createParticleSystem( "ParticleSys1", 200 )
        pThrusters.setMaterialName( "Examples/Flare" )
        pThrusters.setDefaultDimensions( 25, 25 )
        pEmit1 = pThrusters.addEmitter( "Point" )
        pEmit2 = pThrusters.addEmitter( "Point" )

        # Thruster 1
        pEmit1.setAngle( ogre.Degree(d=3) )
        pEmit1.setTimeToLive( 0.2 )
        pEmit1.setEmissionRate( 70 )
        pEmit1.setParticleVelocity( 50 )
        pEmit1.setDirection(- ogre.Vector3().UNIT_Z)
        pEmit1.setColour( ogre.ColourValue().White, ogre.ColourValue().Red)        

        # Thruster 2
        pEmit2.setAngle( ogre.Degree(d=3) )
        pEmit2.setTimeToLive( 0.2 )
        pEmit2.setEmissionRate( 70 )
        pEmit2.setParticleVelocity( 50 )
        pEmit2.setDirection( -ogre.Vector3().UNIT_Z )
        pEmit2.setColour( ogre.ColourValue().White, ogre.ColourValue().Red )

        # Set the position of the thrusters and attach to the craft
        pEmit1.setPosition( ogre.Vector3( 5.7, 6.5, -70 ) )
        pEmit2.setPosition( ogre.Vector3( -18.0, 6.5, -70 ) )
        node.attachObject(pThrusters)
        node.yaw(ogre.Degree(d=60.0), ogre.Node.TS_PARENT)
        node.pitch(ogre.Degree(d=30.0), ogre.Node.TS_PARENT)
        node.roll(ogre.Degree(d=20.0), ogre.Node.TS_PARENT)
        

    def _createFrameListener(self):
        self.frameListener = SkyBoxListener(self.renderWindow, self.camera, self.sceneManager)
        self.root.addFrameListener(self.frameListener)

class SkyBoxListener(sf.FrameListener):
    def __init__(self, renderWindow, camera, sceneManager):
        sf.FrameListener.__init__(self, renderWindow, camera)
        self.sceneManager = sceneManager
           
    def frameStarted(self, frameEvent):
            
        if self._isToggleKeyDown(KC_N, 0.1):
            pThrusters.setDefaultDimensions( self.fDefDim + 0.25, self.fDefDim + 0.25 )
            self.fDefDim += 0.25
        if self._isToggleKeyDown(KC_M, 0.1):
            pThrusters.setDefaultDimensions( self.fDefDim - 0.25, self.fDefDim - 0.25 )
            self.fDefDim -= 0.25
        if self._isToggleKeyDown(KC_H, 0.1):
            pThrusters.getEmitter( 0 ).setParticleVelocity( self.fDefVel + 1 )
            pThrusters.getEmitter( 1 ).setParticleVelocity( self.fDefVel + 1 )
            self.fDefVel += 1            
        if self._isToggleKeyDown(KC_J, 0.1) and not (self.fDefVel < 0.0):
            pThrusters.getEmitter( 0 ).setParticleVelocity( self.fDefVel - 1 )
            pThrusters.getEmitter( 1 ).setParticleVelocity( self.fDefVel - 1 )
            self.fDefVel -= 1            
        return sf.FrameListener.frameStarted(self, frameEvent)

if __name__ == '__main__':
    try:
        application = SampleApplication()
        application.go()
    except ogre.OgreException, e:
        print e