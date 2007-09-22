// typedef name nicename;
typedef btAlignedObjectArray<btOptimizedBvhNode>	NodeArray;
typedef btAlignedObjectArray<btQuantizedBvhNode>	QuantizedNodeArray;
typedef btAlignedObjectArray<btBvhSubtreeInfo>		BvhSubtreeInfoArray;
typedef btAlignedObjectArray<btWheelInfo>		BvhWheelInfoArray;
typedef btAlignedObjectArray<btBvhSubtreeInfo>		BvhSubtreeInfoArray;
typedef btAlignedAllocator<btWheelInfo, 16> btWheelInfo16;
typedef btAlignedObjectArray<int> IntArray;
typedef btAlignedObjectArray<btVector3> btVector3Array;
typedef btAlignedObjectArray<btRaycastVehicle*> btRaycastVehiclePtrArray;
typedef btAlignedObjectArray<btTypedConstraint*> btTypesConstraintPtrArray;
typedef btAlignedObjectArray<btTransform> btTransformArray;
typedef btAlignedObjectArray<btCollisionShape*> btCollisionShapePtrArray;
typedef btAlignedObjectArray<btBroadphasePair> btBroadphasePairArray;
typedef btAlignedObjectArray<btPersistentManifold*> btPersistentManifoldPtrArray;
typedef btAlignedAllocator<int, 16> Int16;
typedef btAlignedAllocator<btIndexedMesh, 16> btIndexedMesh16;
typedef btAlignedAllocator<btVector3, 16> btVector316;
typedef btAlignedAllocator<btRaycastVehicle*, 16> raycastVehiclePtr16;
typedef btAlignedAllocator<btTypedConstraint*, 16> TypedConstraintPtr16;
typedef btAlignedAllocator<btBvhSubtreeInfo, 16> BvhSubtreeInfo16;
typedef btAlignedAllocator<btQuantizedBvhNode, 16> QuantizedBvhNode16;
typedef btAlignedAllocator<btOptimizedBvhNode, 16> OptimizedBvhNode16;

typedef btAlignedAllocator<btCollisionShape*, 16> CollisionShapePtr16;
typedef btAlignedAllocator<btTransform, 16> Transform16;
typedef btAlignedAllocator<btCollisionObject*, 16> CollisionObjectPtr16;
typedef btAlignedAllocator<btBroadphasePair, 16> BroadphasePair16;
typedef btAlignedAllocator<btPersistentManifold*, 16> PersistentManifoldPtr16;

typedef btAlignedObjectArray<btElement> btElementArray;
typedef btAlignedAllocator<btElement, 16> btElement16;

typedef btAxisSweep3Internal<unsigned> btAxisSweep3InternalUnsigned;
typedef btAxisSweep3Internal<unsigned short> btAxisSweep3InternalUnsignedShort;
typedef btAlignedObjectArray<btCompoundShapeChild> CompoundShapeChildObjectArray;
typedef btAlignedAllocator<btCompoundShapeChild, 16> CompoundShapeChild16;

