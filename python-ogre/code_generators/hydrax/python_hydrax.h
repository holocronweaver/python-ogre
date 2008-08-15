
#include "Hydrax.h"

// First we create a magic namespace to hold all our aliases
namespace pyplusplus { namespace aliases {
    
 #include "python_hydrax_aliases.h"
} } 

// then we exposed everything needed (and more) to ensure GCCXML makes them visible to Py++
//
namespace python_hydrax{ namespace details{
inline void instantiate(){
 using namespace Hydrax;
 #include "python_hydrax_sizeof.h"
 
} } }

