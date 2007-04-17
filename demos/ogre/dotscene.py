#!/usr/bin/python
"""
this file parses .scene node (dotscene) files and
creates them in OGRE with user data

Doesn't do any fancy stuff (skydome, XODE, lightmapping, etc) but you can use this as a base for adding those features.)

cpp:
http://www.ogre3d.org/wiki/index.php/DotScene_Loader_with_User_Data_Class
"""
from xml.dom import minidom, Node
import string
from pyogre import ogre

"""
self.dotscene = DotScene(self.fileName, self.sceneManager)
"""		

class DotScene:
	def __init__ (self, fileName, sceneManager):
		self.fileName = fileName
		self.sceneManager = sceneManager
		nodes = self.findNodes(minidom.parse(self.fileName).documentElement,'nodes')
		self.root = nodes[0].childNodes
		self.parseDotScene()
	
	# allows self['nodeName'] to reference xml node in '<nodes>'
	def __getitem__ (self,name):
		return self.findNodes(self.root,name)

	def parseDotScene (self):
		# TODO: check DTD to make sure you get all nodes/attributes
		# TODO: Use the userData for sound/physics
		for node in self.root:
			if node.nodeType == Node.ELEMENT_NODE and node.nodeName == 'node':			
				# create new scene node
				newNode = self.sceneManager.rootSceneNode.createChildSceneNode()

				#position it
				pos = self.findNodes(node, 'position')[0].attributes
				newNode.position = (float(pos['x'].nodeValue), float(pos['y'].nodeValue), float(pos['z'].nodeValue))
				
				# rotate it
				rot = self.findNodes(node, 'rotation')[0].attributes
				newNode.orientation = ogre.Quaternion(float(rot['qw'].nodeValue), float(rot['qx'].nodeValue), float(rot['qy'].nodeValue), float(rot['qz'].nodeValue))
				print float(rot['qw'].nodeValue), float(rot['qx'].nodeValue), float(rot['qy'].nodeValue), float(rot['qz'].nodeValue)

				# scale it
				scale = self.findNodes(node, 'scale')[0].attributes
				newNode.scale = (float(scale['x'].nodeValue), float(scale['y'].nodeValue), float(scale['z'].nodeValue))
				
				# is it a light?
				try:
					thingy = self.findNodes(node, 'light')[0].attributes
					name = str(thingy['name'].nodeValue)
					attachMe = self.sceneManager.createLight(name)
					ltypes={'point':ogre.Light.LT_POINT,'directional':ogre.Light.LT_DIRECTIONAL,'spot':ogre.Light.LT_SPOTLIGHT,'radPoint':ogre.Light.LT_POINT}
					try:
						attachMe.type = ltypes[thingy['type'].nodeValue]
					except IndexError:
						pass
					
					lightNode = self.findNodes(node, 'light')[0]

					try:
						tempnode = self.findNodes(lightNode, 'colourSpecular')[0]
						attachMe.specularColour = (float(tempnode.attributes['r'].nodeValue), float(tempnode.attributes['g'].nodeValue), float(tempnode.attributes['b'].nodeValue), 1.0)
					except IndexError:
						pass
					
					try:
						tempnode = self.findNodes(lightNode, 'colourDiffuse')[0]
						attachMe.diffuseColour = (float(tempnode.attributes['r'].nodeValue), float(tempnode.attributes['g'].nodeValue), float(tempnode.attributes['b'].nodeValue), 1.0)
					except IndexError:
						pass
					
					try:
						tempnode = self.findNodes(lightNode, 'colourDiffuse')[0]
						attachMe.diffuseColour = (float(tempnode.attributes['r'].nodeValue), float(tempnode.attributes['g'].nodeValue), float(tempnode.attributes['b'].nodeValue), 1.0)
					except IndexError:
						pass
					
					print 'added light: "%s"' % name
				except IndexError:
					pass
				
				# is it an entity?
				try:
					thingy = self.findNodes(node, 'entity')[0].attributes
					name = str(thingy['name'].nodeValue)
					mesh = str(thingy['meshFile'].nodeValue)
					attachMe = self.sceneManager.createEntity(name,mesh)
					print 'added entity: "%s" %s' % (name, mesh)
				except IndexError:
					pass

				# is it a camera?
				# TODO: there are other attributes I need in here
				try:
					thingy = self.findNodes(node, 'camera')[0].attributes
					name = str(thingy['name'].nodeValue)
					fov = float(thingy['fov'].nodeValue)
					projectionType= str(thingy['projectionType'].nodeValue)
					attachMe = self.sceneManager.createCamera(name)
					try:
						tempnode = self.findNodes(node, 'clipping')[0]
						attachMe.nearClipDistance = float(tempnode.attributes['near'].nodeValue)
						attachMe.farClipDistance = float(tempnode.attributes['far'].nodeValue)
					except IndexError:
						pass
					attachMe.FOVy = fov
					print 'added camera: "%s" fov: %f type: %s clipping: %f,%f' % (name, fov, projectionType,attachMe.nearClipDistance,attachMe.farClipDistance)
				except IndexError:
					pass
				
				# attach it to the scene
				try:
					newNode.attachObject(attachMe)
				except:
					print "could not attach:",node.nodeName
				

		
	def findNodes (self,root, name):
		out=minidom.NodeList()
		if root.hasChildNodes:
			nodes = root.childNodes
			for node in nodes:
				if node.nodeType == Node.ELEMENT_NODE and node.nodeName == name:
					out.append(node)
		return out

if __name__ == "__main__":
	import sys
	sys.exit(main(sys.argv))