// typedef name nicename;
//typedef DataStructures::WeightedGraph<ConnectionGraph::SystemAddressAndGroupId, unsigned short, false> DDMapConnectionGraphSystemAddressId;
typedef DataStructures::Map<SystemAddress, DataStructures::OrderedList<RakNet::AutoRPC::RPCIdentifier, RakNet::AutoRPC::RemoteRPCFunction, RakNet::AutoRPC::RemoteRPCFunctionComp> *> RakNetremoteFunctions;
// typedef DataStructures::List<DataStructures::Map<SystemAddress, DataStructures::OrderedList<RakNet::AutoRPC::RPCIdentifier, RakNet::AutoRPC::RemoteRPCFunction, &(RakNet::AutoRPC::RemoteRPCFunctionComp(RakNet::AutoRPC::RPCIdentifier const&, RakNet::AutoRPC::RemoteRPCFunction const&))>*, &(int DataStructures::defaultMapKeyComparison<SystemAddress>(SystemAddress const&, SystemAddress const&))>::MapNode> RN_DataStreamWildName;
