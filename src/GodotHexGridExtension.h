/**
 * @file GodotHexGridExtension.h
 * @author Michael Palmer (michaelpalmern1@protonmail.com)
 * @brief The extension class, contains the version string but does little else.
 * @version 1.0
 * @date 2023-12-27
 * 
 * 
 * 
 */
#ifndef GODOT_HEX_GRID_EXTENSION_H
#define GODOT_HEX_GRID_EXTENSION_H

#include "godot_cpp/classes/object.hpp"

namespace godot
{
    class ClassDB;
};
/// @brief Contains the version string. That's pretty much it.
class GodotHexGridExtension : public godot::Object
{
    GDCLASS(GodotHexGridExtension, godot::Object )


public:
/**
 * @brief Gets the version string.
 * 
 * @return godot::String The version string.
 */
    static godot::String version();
/**
 * @brief Binds methods for usage in Godot.
 * 
 */
private:
    static void _bind_methods();
};

#endif //GODOT_HEX_GRID_EXTENSION_H