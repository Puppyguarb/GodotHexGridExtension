/**
 * @file HexTile.h
 * @author Michael Palmer (michaelpalmern1@protonmail.com)
 * @brief A class designed to hold onto a set of coordinates for usage by the HexGrid class.
 * @version 1.0
 * @date 2023-12-27
 * 
 * 
 * 
 */
#ifndef GODOT_HEX_GRID_EXTENSION_HEX_TILE_H
#define GODOT_HEX_GRID_EXTENSION_HEX_TILE_H


#include "godot_cpp/classes/area3D.hpp"

/**
 * @brief Holds coordinate information for usage by the HexGrid.
 * 
 */
class HexTile : public godot::Area3D
{

    GDCLASS(HexTile, godot::Area3D)

public:
    HexTile();
    /**
     * @brief The slow way of getting the coordinates.
     * 
     * @return godot::Array An array containing q in index 0, and r in index 1.
     */
    godot::Array get_co_ords();
    void set_co_ords(int q, int r);
    /**
     * @brief Quick and more readable way for getting the q-coordinate without having to call get_co_ords()[0].
     * 
     * @return int The q-coordinate.
     */
    int get_q();
    /**
     * @brief Quick and more readable way for getting the r-coordinate without having to call get_co_ords()[1].
     * 
     * @return int The r-coordinate.
     */
    int get_r();
    /**
     * @brief The co_ords. The first is the q-coordinate, while the second is the r-coordinate.
     * 
     */
    std::pair<int, int> co_ords;
    /**
     * @brief Binds methods for usage in Godot.
     * 
     */
private:
    static void _bind_methods();
    
};

#endif //GODOT_HEX_GRID_EXTENSION_HEX_TILE_H
