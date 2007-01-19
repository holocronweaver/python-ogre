#!/usr/bin/env python

#
import os, sys, time

#add environment to the path
sys.path.append( os.path.join( '..', '..' ) )
#add common utils to the pass
sys.path.append( '..' )
sys.path.append( '.' )
import shutil

import environment
import common_utils
import customization_data
import hand_made_wrappers

from pyplusplus import module_builder
from pyplusplus import module_creator
from pyplusplus import function_transformers as ft
from pyplusplus.module_builder import call_policies

from pygccxml import parser
from pygccxml import declarations

def filter_declarations( mb ):
    global_ns = mb.global_ns
    global_ns.exclude()
    
    FMOD_ns = global_ns.namespace( 'FMOD' )
    FMOD_ns.include()
        
    FMOD_ns = global_ns  ##  need wrappers
    
    for cls in FMOD_ns.classes():       # these are really all the structures etc
        if  cls.decl_string[2:6]=='FMOD' :
            print "Including Class:", cls.name
            cls.include()
  
    ## and we'll need the free functions as well
     
#     for funcs in FMOD_ns.free_functions ():
#         #print "FREE Func:", funcs.name
#         if funcs.name[0:4]=='FMOD':
#             print "Including Free Function", funcs.name
#             funcs.include() 
#      
#     print dir (FMOD_ns)
#     for var in FMOD_ns.variables():
#         print "VAR:", var.name       
#         var.include()
    for var in FMOD_ns.enums():
        if var.name[0:4] == 'FMOD':
            var.include()
            print "ENUM INCLUDED", var.name       
    #sys.exit()               
#    FMOD_ns.member_functions( "FMOD_System_Create").exclude()
    
    ## now we need to exclude a few things - I thing the return char * * is a problem
#     FMOD_ns.class_( "FMOD_ADVANCEDSETTINGS" ).member_functions( "get_ASIOChannelList").exclude()
#     FMOD_ns.class_( "FMOD_DSP_STATE" ).member_functions( "get_instance").exclude()
            
#    global_ns.include()
#     FMOD_ns = global_ns.namespace( 'FMOD' )
#     FMOD_ns.class_('System').include()
    #FMOD_ns.include()
    
    FMOD_ns.class_('FMOD_DSP_STATE').exclude()
    FMOD_ns.class_('FMOD_ADVANCEDSETTINGS').exclude() # calldef('ASIOChannelList').exclude()
       
    
def set_call_policies( mb ):
    FMOD_ns = mb

    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the FMOD Default.
    mem_funs = FMOD_ns.calldefs ()
    mem_funs.create_with_signature = True #Generated code will not compile on
#     #MSVC 7.1 if function has throw modifier.
#     for mem_fun in mem_funs:
#         if mem_fun.call_policies:
#             continue
#         if declarations.is_pointer (mem_fun.return_type) or declarations.is_reference (mem_fun.return_type):
#             mem_fun.call_policies = call_policies.return_value_policy(
#                 call_policies.reference_existing_object )

#                 
   #MSVC 7.1 if function has throw modifier.
    resolver = module_creator.built_in_resolver_t()
    for mem_fun in mem_funs:
        if mem_fun.call_policies:
            continue
        decl_call_policies = resolver( mem_fun )
        if decl_call_policies:
            mem_fun.call_policies = decl_call_policies
            continue
        rtype = declarations.remove_alias( mem_fun.return_type )
        if declarations.is_pointer(rtype) or declarations.is_reference(rtype):
#             mem_fun.call_policies \
#                 = call_policies.return_value_policy( call_policies.reference_existing_object )
            mem_fun.call_policies = call_policies.return_value_policy( '::boost::python::return_pointee_value' )
                
                
                 
def configure_exception(mb):
    pass
    #We don't exclude  Exception, because it contains functionality, that could
    #be useful to user. But, we will provide automatic exception translator
#     Exception = mb.namespace( 'FMOD' ).class_( 'Exception' )
#     Exception.translate_exception_to_string( 'PyExc_RuntimeError',  'exc.getMessage().c_str()' )

## this is to fix specific challenges where a class (CaratIndex for example) is defined in multiple namespaces
##   

def change_cls_alias( ns ):
   for cls in ns.classes():
       if 1 < len( ns.classes( cls.name ) ):
           alias = cls.decl_string[ len('::FMOD::'): ]
           print "Adjust:",cls.decl_string
           cls.alias = alias.replace( '::', '' )
           cls.wrapper_alias = cls.alias + 'Wrapper' # or 'Wrapper' ??
           ##cls.exclude()
           
           
def add_transformations ( mb ):
    ns = mb.global_ns.namespace ('FMOD')
    
    def create_output( size ):
        return [ ft.output( i ) for i in range( size ) ]
   
    ns.mem_fun('::FMOD::System::getVersion') \
        .add_transformation(ft.output('version'))
    ns.mem_fun('::FMOD::System::createSound') \
        .add_transformation(ft.output('sound'))
        
##        FMOD_RESULT F_API getVersion             (unsigned int *version);
        
## typedef ::FMOD_RESULT ( ::FMOD::System::*createSound_function_type )( char const *,::FMOD_MODE,::FMOD_CREATESOUNDEXINFO *,::FMOD::Sound * * ) ;
##        FMOD_RESULT F_API createSound            (const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, Sound **sound);
        
##    result = system->createSound("../media/drumloop.wav", FMOD_HARDWARE, 0, &sound1);
        
 
    
def set_smart_pointers( mb ):
    for v in mb.variables():
       if not declarations.is_class( v.type ):
           continue
       cls = declarations.class_traits.get_declaration( v.type )
       print "Could smartpr:", cls.name
#        if cls.name.startswith( 'SharedPtr<' ):
#            v.apply_smart_ptr_wa = True    
#            print "Applying Smart Pointer: ",  v.name, " of class: ",  cls.name
#        elif cls.name.endswith( 'SharedPtr' ):
#            v.apply_smart_ptr_wa = True    
#            print "Applying Smart Pointer: ",  v.name, " of class: ",  cls.name


def generate_code():
    xml_cached_fc = parser.create_cached_source_fc(
                        os.path.join( environment.FMOD.root_dir, "python_FMOD.h" )
                        , environment.FMOD.cache_file )

    mb = module_builder.module_builder_t( [ xml_cached_fc ]
                                          , gccxml_path=environment.gccxml_bin
                                          , working_directory=environment.root_dir
                                          , include_paths=environment.FMOD.include_dirs
                                          , define_symbols=['FMOD_NONCLIENT_BUILD', '_CONSOLE', 'NDEBUG', 'WIN32']
                                          , indexing_suite_version=2 )
    filter_declarations (mb)
   
##    common_utils.set_declaration_aliases( mb.global_ns, customization_data.aliases(environment.FMOD.version) )

    mb.BOOST_PYTHON_MAX_ARITY = 25
    mb.classes().always_expose_using_scope = True

    
#     common_utils.fix_unnamed_classes( mb.namespace( 'FMOD' ).classes( name='' ), 'FMOD' )

#     common_utils.configure_shared_ptr(mb)
    configure_exception( mb )

    
    hand_made_wrappers.apply( mb )
    
    set_call_policies (mb.global_ns.namespace ('FMOD'))
    
    # now we fix up the smart pointers ...
#     set_smart_pointers ( mb.global_ns.namespace ('FMOD') )  
   # set_smart_pointers ( mb.global_ns )  
   # sys.exit()
    # here we fixup functions that expect to modifiy their 'passed' variables    
    add_transformations ( mb )
    
    
    
#     common_utils.add_properties(  mb.global_ns.namespace ('FMOD').classes() )

    common_utils.add_constants( mb, { 'FMOD_version' :  '"%s"' % environment.FMOD.version
                                       , 'python_version' : '"%s"' % sys.version } )
                                      
    #Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator (module_name='_fmod_')
    for incs in environment.FMOD.include_dirs:
        mb.code_creator.user_defined_directories.append( incs )
    mb.code_creator.user_defined_directories.append( environment.FMOD.generated_dir )
    mb.code_creator.replace_included_headers( customization_data.header_files(environment.FMOD.version) )
    huge_classes = map( mb.class_, customization_data.huge_classes(environment.FMOD.version) )
    mb.split_module(environment.FMOD.generated_dir, huge_classes)
    
    return_pointee_value_source_path = os.path.join( environment.pyplusplus_install_dir
                    , 'pyplusplus_dev'
                    , 'pyplusplus'
                    , 'code_repository'
                    , 'return_pointee_value.hpp' )

    return_pointee_value_target_path \
        = os.path.join( environment.FMOD.generated_dir, 'return_pointee_value.hpp' )

    if not os.path.exists( return_pointee_value_target_path ):
        shutil.copy( return_pointee_value_source_path, environment.FMOD.generated_dir )

    
    
    

if __name__ == '__main__':
    start_time = time.clock()
    generate_code()
    print 'Python-FMOD source code was updated( %f minutes ).' % (  ( time.clock() - start_time )/60 )
