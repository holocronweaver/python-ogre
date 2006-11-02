import os
import environment
from pygccxml import declarations

OGRE_SP_HELD_TYPE_TMPL = \
"""
namespace boost{ namespace python{

%(class_name)s* get_pointer( %(class_ptr_name)s const& p ){
    return p.get();
}

template <>
struct pointee< %(class_ptr_name)s >{
    typedef %(class_name)s type;
};

}}// namespace boost::python
"""

REGISTER_SP_TO_PYTHON = \
"""
boost::python::register_ptr_to_python< %(sp_inst_class_name)s >();
"""

REGISTER_SPTR_CONVERSION =\
"""
boost::python::implicitly_convertible< %(derived)s, %(base)s >();
"""


class exposer_t:
    def __init__( self, mb ):
        self.ogre_ns = mb.namespace ('Ogre')
        self.visited_classes = set()
        #For this classes wrapper does not exist
        self.special_cases = ['PatchMesh', 'Controller<float>', 'Compositor'
                               , 'vector<std::string, std::allocator<std::string> >' ]

    def get_pointee( self, sp_instantiation ):
        #sp_instantiation - reference to SharedPtr<XXX>
        #returns reference to XXX type/declaration
        no_ptr = declarations.remove_pointer( sp_instantiation.variable ('pRep').type )
        no_alias = declarations.remove_alias( no_ptr )
        return declarations.remove_declarated( no_alias )

    def expose_single( self, sp_instantiation ):
        sp_instantiation.exclude() # we don't want to export SharedPtr< X >

        pointee = self.get_pointee( sp_instantiation )
        if sp_instantiation.derived:
            #We have struct XPtr : public SharedPtr<X>
            assert 1 == len( sp_instantiation.derived )
            sp_derived = sp_instantiation.derived[0].related_class
            sp_derived.exclude()

            pointee.add_declaration_code(
                OGRE_SP_HELD_TYPE_TMPL % { 'class_name': pointee.decl_string
                                           , 'class_ptr_name': sp_derived.decl_string } )

            pointee.add_registration_code(
                REGISTER_SPTR_CONVERSION % { 'derived' : sp_derived.decl_string
                                             , 'base' : sp_instantiation.decl_string }
                , works_on_instance=False )

            pointee.add_registration_code(
                REGISTER_SP_TO_PYTHON % { 'sp_inst_class_name' : sp_derived.decl_string }
                , works_on_instance=False )

        if pointee.name not in self.special_cases:
            pointee.held_type = '::Ogre::SharedPtr< %s >' % pointee.wrapper_alias
            pointee.add_registration_code(
                REGISTER_SPTR_CONVERSION % { 'derived' : pointee.held_type
                                             , 'base' : sp_instantiation.decl_string }
                , works_on_instance=False )

            pointee.add_registration_code(
                REGISTER_SP_TO_PYTHON % { 'sp_inst_class_name' : sp_instantiation.decl_string }
                , works_on_instance=False )

        else:
            pointee.held_type = sp_instantiation.decl_string

        base_classes = filter( lambda hi: hi.access_type == 'public', pointee.bases )
        for base in base_classes:
            pointee.add_registration_code(
                REGISTER_SPTR_CONVERSION % { 'derived' : sp_instantiation.decl_string
                                             , 'base' : '::Ogre::SharedPtr< %s >' % base.related_class.decl_string }
                , works_on_instance=False)

    def expose(self):
        sp_instantiations = self.ogre_ns.classes( lambda decl: decl.name.startswith( 'SharedPtr' ) )
        map( lambda sp: self.expose_single( sp ), sp_instantiations )


