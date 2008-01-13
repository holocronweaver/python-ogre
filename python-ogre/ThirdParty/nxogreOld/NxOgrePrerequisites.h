//
//	NxOgre a wrapper for the PhysX (formerly Novodex) physics library and the Ogre 3D rendering engine.
//	Copyright (C) 2005 - 2007 Robin Southern and NxOgre.org http://www.nxogre.org
//
//	This library is free software; you can redistribute it and/or
//	modify it under the terms of the GNU Lesser General Public
//	License as published by the Free Software Foundation; either
//	version 2.1 of the License, or (at your option) any later version.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//	Lesser General Public License for more details.
//
//	You should have received a copy of the GNU Lesser General Public
//	License along with this library; if not, write to the Free Software
//	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef __NXOGRE_PREREQUISITES_H__
#define __NXOGRE_PREREQUISITES_H__

#include "NxOgreStable.h"
#include "NxOgreExtendedTypes.h"

namespace NxOgre {

	///////////////////////////////////////////////////////


	class Actor;
	class ActorGroup;
	template<typename Class> class NxExport ActorGroupMethodPointer;
	class AccumulatorSceneController;
	class AccumulatorSceneRenderer;
	class BaseCharacterHitReport;
	class Body;
	class Compartment;
	class Capsule;
	class CapsuleShape;

#if (NX_USE_LEGACY_NXCONTROLLER == 1)

#if (NX_USE_CHARACTER_API == 1)
	class Character;
	class CharacterController;
	class CharacterHitReport;
	class CharacterMovementVectorController;
#endif

#else
	class Character;
	class CharacterSystem;
	class CharacterHitReport;
	class CharacterController;
	class CharacterModel;
	class CharacterMovementModel;
	class NxActorController;
#if (NX_USE_CHARACTER_API == 1)
	class NxCharacterController;
#endif
	class Performer;
	
#endif

#if (NX_USE_CLOTH_API == 1)
	class Cloth;
	class ClothRayCaster;
	struct ClothVertex;
#endif
	class CombustionEngine;
	class CompoundShape;
	class ContactStream;
	class Convex;
	class ConvexMeshIntermediary;
	class ConvexShape;
	template<typename TI, typename TT> class Container;
	class Cube;
	class CubeShape;
	class CulledIntersection;
	class CylindricalJoint;
#if (NX_USE_DEBUG_RENDERER_API == 1)
	class DebugRenderer;
#endif
#if (NX_SDK_VERSION_NUMBER >= 272) 
	class DominanceGroup;
#endif
	class Engine;
	class Error;
	struct ErrorReport;
	class ErrorReporter;
	class FileResourceStream;
	class FileResourceSystem;
	class FixedSceneController;
	class FixedJoint;
#if (NX_USE_FLUID_API == 1)
	class Fluid;
	class FluidDrain;
	class FluidEmitter;
	class FluidMesh;
#endif
#if (NX_USE_FORCEFIELD_API == 1)
	class ForceField;
	class ForceFieldExclusion;
	struct ForceFieldFormula;
	class ForceFieldShape;
	class ForceFieldCubeShape;
	class ForceFieldSphereShape;
	class ForceFieldCapsuleShape;
	class ForceFieldConvexShape;
#endif
	class Ground;
	class GroundShape;
	class GroupCallback;
	class InternalCombustionEngine;
	class Intersection;
	class Joint;
	class JointCallback;
	template<typename TT> class List;
	class Log;
	class Machine;
	class Material;
	class MaterialAlias;
	class MaterialAliasResource;
	class MeshResource;
	class Motor;
	class NodeRenderable;
	class NullSceneController;
	class NullSceneRenderer;
	class NxActorUserData;
	class OgreResourceStream;
	class OgreResourceSystem;
	class Prism;
	class PrismShape;
	class PrismaticJoint;
	class Params;
	class PhysXDriver;
	class Pose;
	class RayCaster;
	struct RayCastHit;
	class Renderable;
	class Renderables;
	class RenderableSource;
	class RemoteDebuggerConnection;
	class RevoluteJoint;
	class Resource;
	class ResourceManager;
	class ResourceStreamPtr;
	class ResourceStream;
	class ResourceSystem;
	class Scene;
	class SceneContactController;
	class SceneController;
	class SceneRenderer;
	class SceneTriggerController;
	class Shape;
	class ShapeBlueprint;
	class ShapeGroup;
	class SimpleBox;
	class SimpleCapsule;
	class SimpleConvex;
	class SimpleIntersection;
	class SimplePlane;
	class SimpleShape;
	class SimpleSphere;
	class Skeleton;
	class Sphere;
	class SphereShape;
	class SphericalJoint;
#if (NX_USE_SOFTBODY_API == 1)
	class SoftBody;
#endif
	class StringPairList;
	class Terrain;
	class TerrainShape;
	class TriangleMesh;
	class TriangleMeshIntermediary;
	class TriangleMeshShape;
	class Trigger;
	class TriggerCallback;
	class UserAllocator;
	class VariableSceneController;
	class Wheel;
	class WheelSet;
	class WheelShape;
	class World;
	
	////////////////////////////////////////////////////////

	class OgreSceneRenderer;
	class OgreNodeRenderable;

	////////////////////////////////////////////////////////

	class ActorParams;
	class CharacterParams;
#if (NX_USE_CLOTH_API == 1)
	class ClothParams;
#endif
#if (NX_USE_CLOTH_API == 1)
	class FluidParams;
#endif
#if (NX_USE_FORCEFIELD_API == 1)
	class ForceFieldParams;
#endif
	class JointParams;
	class SceneParams;
	class ShapeParams;
#if (NX_USE_SOFTBODY_API == 1)
	class SoftBodyParams;
#endif
	class PhysXDriverParams;
	class WheelParams;

	////////////////////////////////////////////////////////

	typedef Ogre::String	NxString;
	typedef NxU32			NxShapeIndex;
	typedef NxU32			NxErrorIndex;
	typedef NxU32			NxMeshID;
	typedef NxU32			NxSkeletonID;
	typedef NxU32			NxJointID;
	typedef NxReal			NxRadian;
	typedef unsigned long	NxHashIdentifier;
	typedef NxU16			NxShortHashIdentifier;
	typedef NxU32			NxRenderableSourceID;
	typedef NxU32			NxMachineID;
	
	////////////////////////////////////////////////////////

	namespace Serialiser {

		enum BlueprintsFormat {

#if (NX_USE_NXUSTREAM == 1)
			SF_Ageia_XML,
			SF_Ageia_COLLADA,
			SF_Ageia_Binary,
#endif

#if (NX_USE_JSON == 1)
			SF_JSON,
#endif

#if (NX_USE_TINYXML == 1)
			SF_XML,
#endif

			SF_Binary
		};

		void exportWorldToFile(NxOgre::Serialiser::BlueprintsFormat, NxString fileName);
		void importWorldFromFile(NxOgre::Serialiser::BlueprintsFormat, NxString fileName);
				
		NxString exportWorldToString(NxOgre::Serialiser::BlueprintsFormat);
		void importWorldFromString(NxOgre::Serialiser::BlueprintsFormat, NxString serialisedString);

		class SerialiserBase;

#if (NX_USE_NXUSTREAM == 1)
		namespace NxuStream {
			class Serialiser;
		};
#endif
		
		namespace NxSerialiser {

			class Serialiser;

#if (NX_USE_JSON == 1)
			class JSONCollection;
#endif

#if (NX_USE_TINYXML == 1)
			class XMLCollection;
#endif

			class WorldCollection;
		
		};

	};

	namespace Blueprints {
		class ActorBlueprint;
		class ActorFactory;
		class Blueprint;
		class EnumsBlueprint;
		class SceneBlueprint;
		class WorldBlueprint;
	};

	////////////////////////////////////////////////////////

	/** @brief Simple identifier to identify a resource, and a resource type.

		@example

			"file://myMesh.mesh" - Use the FileResourceSystem to read myMesh.mesh
			"file://media/resources/myMesh.mesh +write" - Use the FileResourceSystem to write to myMesh.mesh"
			"ogre://myMesh.mesh" - Use the OgreResourceSystem to load myMesh.mesh
			"myMesh.mesh" - Use the FileResourceSystem to load myMesh.mesh 
							Wihout a resourcetype header; file is assumed.

	*/
	typedef NxString ResourceIdentifier;

	/** @brief A list of resources, with an attached ResourceIdentifier
		First = Name of the resource (without extension);
		Second = Full ResourceIdentifier
	*/
	typedef std::map<NxString,NxString>	ResourceAccessList;

	/** @brief A list of actors, organised by name.
	*/
	typedef Container<NxString, Actor*>							Actors;
	
	/** @brief A list of actor groups, organised by name.
	*/
	typedef Container<NxString, ActorGroup*>					ActorGroups;
	
	/** @brief A list of actor groups, organised by NxActorGroup (unsigned short).
	*/
	typedef Container<NxActorGroup, ActorGroup*>				ActorGroupsByIndex;
		
	/** @brief A list of string based properties identified by key and value.
	*/
	typedef Container<NxString, NxString>						BlueprintUserData;

	/** @brief A list of Shapes for an Actor, identified by NxShapeIndex (unsigned int).
	*/
	typedef Container<NxShapeIndex, Shape*>						CollisionModel;

	/** @brief A list of Shape Blueprints for an Actor, identified by NxShapeIndex (unsigned int).
	*/
	typedef Container<NxShapeIndex, ShapeBlueprint*>			CollisionDescriptionModel;

#if (NX_USE_CLOTH_API == 1)
	
	/** @brief A list of cloths, organised by name.
	*/
	typedef Container<NxString, Cloth*>							Cloths;
#endif
	
#if (NX_SDK_VERSION_NUMBER >= 272) 
	
	/** @brief A list of dominance groups, organised by name.
	*/
	typedef Container<NxString, DominanceGroup*>				DominanceGroups;
	
	/** @brief A list of dominance groups, organised by NxDominanceGroup (unsigned short).
	*/
	typedef Container<NxDominanceGroup, DominanceGroup*>		DominanceGroupsByIndex;
#endif
	
	/** @brief A list of error reporters, organised by NxErrorIndex (unsigned int).
	*/
	typedef Container<NxErrorIndex, ErrorReporter*>				ErrorReporterList;

#if (NX_USE_FORCEFIELD_API == 1)
	
	/** @brief A list of forcefields, organised by name.
	*/
	typedef Container<NxString, ForceField*>					ForceFields;
#endif

//	typedef Container<NxString, Helper*>						Helpers;
	typedef Container<NxJointID, Joint*>						Joints;
	typedef Container<NxMachineID, Machine*>					Machines;
	typedef Container<NxString, Material*>						MaterialList;
	typedef Container<NxMaterialIndex, Material*>				MaterialListByIndex;
	typedef Container<NxString, NxMaterialIndex>				MaterialPair;
	typedef Container<NxString, RayCastHit>						RayCastReport;
	typedef Container<NxString, ResourceSystem*>				ResourceSystems;
	typedef Container<NxRenderableSourceID, RenderableSource*>	RenderableSources;
	typedef Container<NxString, Scene*>							Scenes;
	typedef Container<NxString, ShapeGroup*>					ShapeGroups;
	typedef Container<NxCollisionGroup, ShapeGroup*>			ShapeGroupsByIndex;
	typedef Container<NxString, Trigger*>						Triggers;
	
	////////////////////////////////////////////////////////

	typedef Container<unsigned int, Wheel*>						Wheels;
	typedef List<NxString>										StringList;
	
	////////////////////////////////////////////////////////

	typedef Container<NxString, Character*>						Characters;
	typedef List<CharacterHitReport*>							CharacterHitReports;

#if (NX_USE_LEGACY_NXCONTROLLER == 0)
	typedef Container<NxString, CharacterMovementModel*>		CharacterMovementModels;
#endif

	////////////////////////////////////////////////////////

#if (NX_USE_FLUID_API == 1)
	typedef Container<NxString, Fluid*>							Fluids;
	typedef Container<NxString, FluidDrain*>					FluidDrains;
	typedef Container<NxString, FluidEmitter*>					FluidEmitters;
#endif
	
	////////////////////////////////////////////////////////

#if (NX_USE_SOFTBODY_API == 1)
	typedef Container<NxString, SoftBody*>						SoftBodies;
#endif

	/** @brief Used to convert the material id's in a mesh to our own, aided with a MaterialAlias.
		@note  First is meshes material ID, second is the MaterialList's
	*/
	typedef std::map<NxMaterialIndex, NxMaterialIndex> MaterialConversionList;

	////////////////////////////////////////////////////////

	void NxExport NxThrow(NxString, int, NxString, unsigned int);

	#define NxDelete(x)						if(x){delete x;x=0;}
	#define NxThrow_Conflict(a)				NxThrow(a, 0, __FUNCTION__, __LINE__);
	#define NxThrow_Warning(a)				NxThrow(a, 1, __FUNCTION__, __LINE__);
	#define NxThrow_Error(a)				NxThrow(a, 2, __FUNCTION__, __LINE__);
	#define NxDebug(a)						NxThrow(a, 3, __FUNCTION__, __LINE__);
	#define NxLeakDump(a)					NxThrow(a, 4, __FUNCTION__, __LINE__);
	#define NxUnderConstruction				NxDebug("This function called contains no or little code.");
	#define	NxToConsole(x)					NxDebug(x);

	#define NxMemoryStreamIdentifier		"memory://"

	////////////////////////////////////////////////////////

	static const NxVec3 NxVec3Axis[6] ={
		NxVec3( 1, 0, 0),
		NxVec3( 0, 1, 0),
		NxVec3( 0, 0, 1),
		NxVec3(-1, 0, 0),
		NxVec3( 0,-1, 0),
		NxVec3( 0, 0,-1)
	};

	enum NxDirection {
		NX_DIRECTION_X = 0,
		NX_DIRECTION_Y = 1,
		NX_DIRECTION_Z = 2
	};

#if (NX_USE_LEGACY_TERMS == 1)
	typedef TriangleMeshShape	MeshShape
	typedef ShapeBlueprint		ShapeDescription
#endif

	////////////////////////////////////////////////////////

	enum NxStringOperator {
		SO_STARTS_WITH,
		SO_ENDS_WITH
	};

	////////////////////////////////////////////////////////

	enum NxComparisonOperator {
		CO_MORE_THAN,
		CO_LESS_THAN,
		CO_EXACTLY,
		CO_NOT
	};

	////////////////////////////////////////////////////////

	#define Nx_Version_Major    0
	#define Nx_Version_Minor    9
	#define Nx_Version_Commit   39
	#define Nx_Version_Codename "I Heart NxOgre"

	#ifndef NX_DEBUG
		static NxString Nx_Version_Full = "NxOgre 0.9-39";
	#else
		static NxString Nx_Version_Full = "NxOgre 0.9-39.Debug";
	#endif

	////////////////////////////////////////////////////////

	// NxOgre-Actor
	#define NxHashes_Actor	54846
	// NxOgre-Example-Actor
	#define NxHashes_Body	46412
	// NxOgre-Wheel
	#define NxHashes_Wheel	28186

};

#endif