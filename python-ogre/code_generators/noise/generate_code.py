#!/usr/bin/env python
# -----------------------------------------------------------------------------
# This source file is part of Python-Ogre and is covered by the LGPL
# For the latest info, see http://python-ogre.org/
#
# -----------------------------------------------------------------------------

## STARTER TEMPLATE..
## replace PROJECT with lowercase project name
## set MAIN_NAMESPACE
## rename and configure .h files


import os, sys, time, shutil

#add environment to the path
sys.path.append( os.path.join( '..', '..' ) )
#add common utils to the pass
sys.path.append( '..' )
sys.path.append( '.' )

import environment
import common_utils
import customization_data
import hand_made_wrappers

from pygccxml import parser
from pygccxml import declarations
from pyplusplus import messages
from pyplusplus import module_builder
from pyplusplus import decl_wrappers

from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies
from pyplusplus.module_creator import sort_algorithms

import common_utils.extract_documentation as exdoc
import common_utils.var_checker as varchecker
import common_utils.ogre_properties as ogre_properties
from common_utils import docit

MAIN_NAMESPACE = 'noise'

############################################################
##
##  Here is where we manually exclude stuff
##
############################################################

def ManualExclude ( mb ):
    global_ns = mb.global_ns
    if MAIN_NAMESPACE:
        main_ns = global_ns.namespace( MAIN_NAMESPACE )
    else:
        main_ns = global_ns   

    ## Manual Excludes 
    global_ns.mem_fun('::noise::module::Terrace::GetControlPointArray').exclude()


############################################################
##
##  And there are things that manually need to be INCLUDED 
##
############################################################
    
def ManualInclude ( mb ):
    global_ns = mb.global_ns
    if MAIN_NAMESPACE:
        main_ns = global_ns.namespace( MAIN_NAMESPACE )
    else:
        main_ns = global_ns    


    
############################################################
##
##  And things that need manual fixes, but not necessarly hand wrapped
##
############################################################
def ManualFixes ( mb ):    
    global_ns = mb.global_ns
    if MAIN_NAMESPACE:
        main_ns = global_ns.namespace( MAIN_NAMESPACE )
    else:
        main_ns = global_ns

              
############################################################
##
##  And things that need to have their argument and call values fixed.
##  ie functions that pass pointers in the argument list and of course we need
##  to read the updated values - so instead we pass them back 
##  as new values in a tuple (ETC ETC)
##
############################################################
        
def ManualTransformations ( mb ):
    global_ns = mb.global_ns
    if MAIN_NAMESPACE:
        main_ns = global_ns.namespace( MAIN_NAMESPACE )
    else:
        main_ns = global_ns
                
    def create_output( size ):
        return [ ft.output( i ) for i in range( size ) ]
        
    
###############################################################################
##
##  Now for the AUTOMATIC stuff that should just work
##
###############################################################################
    
def AutoFixes ( mb, MAIN_NAMESPACE ): 
    """ now we fix a range of things automatically - typically by going through 
    the entire name space trying to guess stuff and fix it:)
    """    
    global_ns = mb.global_ns
    if MAIN_NAMESPACE:
        main_ns = global_ns.namespace( MAIN_NAMESPACE )
    else:
        main_ns = global_ns
        
    # Functions that have void pointers in their argument list need to change to unsigned int's  
    pointee_types=[]
    ignore_names=[]
    common_utils.Fix_Void_Ptr_Args  ( main_ns ) # , pointee_types, ignore_names )

    # and change functions that return a variety of pointers to instead return unsigned int's
    pointee_types=[]
    ignore_names=[]  # these are function names we know it's cool to exclude
    common_utils.Fix_Pointer_Returns ( main_ns ) # , pointee_types, ignore_names )   

    # functions that need to have implicit conversions turned off
    ImplicitClasses=[] 
    common_utils.Fix_Implicit_Conversions ( main_ns, ImplicitClasses )
    
    if os.name =='nt':
        Fix_NT( mb )
    elif os.name =='posix':
        Fix_Posix( mb )
        
    common_utils.Auto_Document( mb, MAIN_NAMESPACE )
               


###############################################################################
##
##  Now for the AUTOMATIC stuff that should just work
##
###############################################################################
     
def AutoFixes ( mb, MAIN_NAMESPACE ): 
    """ now we fix a range of things automatically - typically by going through 
    the entire name space trying to guess stuff and fix it:)
    """    
    global_ns = mb.global_ns
    if MAIN_NAMESPACE:
        main_ns = global_ns.namespace( MAIN_NAMESPACE )
    else:
        main_ns = global_ns
        
    # Functions that have void pointers in their argument list need to change to unsigned int's  
    pointee_types=[]
    ignore_names=[]
    common_utils.Fix_Void_Ptr_Args  ( main_ns ) # , pointee_types, ignore_names )

    # and change functions that return a variety of pointers to instead return unsigned int's
    pointee_types=[]
    ignore_names=[]  # these are function names we know it's cool to exclude
    common_utils.Fix_Pointer_Returns ( main_ns ) # , pointee_types, ignore_names )   

    # functions that need to have implicit conversions turned off
    ImplicitClasses=[] 
    common_utils.Fix_Implicit_Conversions ( main_ns, ImplicitClasses )
    
    if os.name =='nt':
        Fix_NT( mb )
    elif os.name =='posix':
        Fix_Posix( mb )
        
    common_utils.Auto_Document( mb, MAIN_NAMESPACE )
        
 
###############################################################################
##
## here are the helper functions that do much of the work
##
###############################################################################     
def Fix_Posix ( mb ):
    """ fixup for posix specific stuff -- note only expect to be called on a posix machine
    """
    ## we could do more here if need be...
    if sys.platform == 'darwin':
        pass
    elif sys.platform.startswith ('linux'):
        pass


def Fix_NT ( mb ):
    """ fixup for NT systems
    """


###############################################################################
##
## Generate code
##
###############################################################################     
def generate_code():  
    messages.disable( 
#           Warnings 1020 - 1031 are all about why Py++ generates wrapper for class X
          messages.W1020
        , messages.W1021
        , messages.W1022
        , messages.W1023
        , messages.W1024
        , messages.W1025
        , messages.W1026
        , messages.W1027
        , messages.W1028
        , messages.W1029
        , messages.W1030
        , messages.W1031
# # #         , messages.W1035
# # #         , messages.W1040 
# # #         , messages.W1038        
# # #         , messages.W1041
# # #         , messages.W1036 # pointer to Python immutable member
# # #         , messages.W1033 # unnamed variables
# # #         , messages.W1018 # expose unnamed classes
# # #         , messages.W1049 # returns reference to local variable
# # #         , messages.W1014 # unsupported '=' operator
         )
    #
    # Use GCCXML to create the controlling XML file.
    # If the cache file (../cache/*.xml) doesn't exist it gets created, otherwise it just gets loaded
    # NOTE: If you update the source library code you need to manually delete the cache .XML file   
    #
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.noise.root_dir, "python_noise.h" )
                        , environment.noise.cache_file )


    defined_symbols = [ 'OGRE_NONCLIENT_BUILD' ]

    defined_symbols.append( 'VERSION_' + environment.noise.version )  
    
    #
    # build the core Py++ system from the GCCXML created source
    #    
    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.noise.include_dirs
                                          , define_symbols=defined_symbols
                                          , indexing_suite_version=2
                                          , cflags=environment.noise.cflags
                                           )
    # NOTE THE CHANGE HERE                                           
    mb.constructors().allow_implicit_conversion = False
    
    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True


    global_ns = mb.global_ns
    global_ns.exclude()

    for n in mb.global_ns.namespaces():
        print n._name

    namespaces = ['noise', 'utils', 'model', 'module']
    for n in namespaces:
        global_ns.namespace(n).include()
        common_utils.AutoExclude ( mb, n )

    ## Manual Excludes 
    global_ns.mem_fun('::noise::module::Terrace::GetControlPointArray').exclude()

    ## These generate "<<protected declaration>>\n" and compile fails
    global_ns.mem_fun('::noise::module::Curve::FindInsertionPos').exclude()
    global_ns.mem_fun('::noise::module::Curve::InsertAtPos').exclude()
    global_ns.mem_fun('::noise::module::RidgedMulti::CalcSpectralWeights').exclude()
    global_ns.mem_fun('::noise::module::Terrace::FindInsertionPos').exclude()
    global_ns.mem_fun('::noise::module::Terrace::InsertAtPos').exclude()
    global_ns.mem_fun('::noise::utils::WriterBMP::CalcWidthByteCount').exclude()
    global_ns.mem_fun('::noise::utils::WriterTER::CalcWidthByteCount').exclude()

    for n in namespaces:
        common_utils.AutoInclude ( mb, n)

    ManualInclude ( mb )
    # here we fixup functions that expect to modifiy their 'passed' variables    
    ManualTransformations ( mb )
    
    for n in namespaces:
        AutoFixes ( mb, n )
    ManualFixes ( mb )
    
    ## namespaces 
    namespaces = ['noise', 'model' , 'module', 'utils']

    #
    # We need to tell boost how to handle calling (and returning from) certain functions
    #
    for ns in namespaces:
        common_utils.Set_DefaultCall_Policies (mb.global_ns.namespace(ns))
    
    
    #
    # the manual stuff all done here !!!
    #
    hand_made_wrappers.apply( mb )

    ## add properties to the 4 namespaces, in a conservative fashion
    for ns in namespaces:
        for cls in mb.global_ns.namespace(ns).classes():
            cls.add_properties( recognizer=ogre_properties.ogre_property_recognizer_t() )

    common_utils.add_constants( mb, { 'noise_version' :  '"%s"' % environment.noise.version.replace("\n", "\\\n") 
                                      , 'python_version' : '"%s"' % sys.version.replace("\n", "\\\n" ) } )
                                      
    ## need to create a welcome doc string for this...                                  
    common_utils.add_constants( mb, { '__doc__' :  '"noise DESCRIPTION"' } ) 
    
    
    ##########################################################################################
    #
    # Creating the code. After this step you should not modify/customize declarations.
    #
    ##########################################################################################
    #extractor = exdoc.doc_extractor("::Ogre") # I'm excluding the UTFstring docs as lots about nothing 
    mb.build_code_creator (module_name='_noise_' ) #, doc_extractor= extractor )
    
    for inc in environment.noise.include_dirs:
        mb.code_creator.user_defined_directories.append(inc )
    mb.code_creator.user_defined_directories.append( environment.noise.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files( environment.noise.version ) )

    huge_classes = map( mb.class_, customization_data.huge_classes( environment.noise.version ) )

    mb.split_module(environment.noise.generated_dir, huge_classes, use_files_sum_repository=False)

    ## now we need to ensure a series of headers and additional source files are
    ## copied to the generated directory..
    common_utils.copyTree ( sourcePath = environment.Config.PATH_noise, 
                            destPath = environment.noise.generated_dir, 
                            recursive=False )
        
if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )