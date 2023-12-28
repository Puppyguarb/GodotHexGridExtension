#include "godot_cpp/core/class_db.hpp"

#include "GodotHexGridExtension.h"


godot::String GodotHexGridExtension::version()
{
    return "GodotHexGridExtension-v1.0";
    
    
    
    
}

/// Bind our methods so GDScript can access them.
void GodotHexGridExtension::_bind_methods()
{
    godot::ClassDB::bind_static_method( "GodotHexGridExtension", godot::D_METHOD( "version" ),
                                        &GodotHexGridExtension::version );
}
