/**
 * @file HexGrid.h
 * @author Michael Palmer (michaelpalmern1@protonmail.com)
 * @brief A class designed to manage and manipulate hexagonal grids.
 * @details
 * @version 1.0
 * 
 * @date 2023-12-27
 * 
 * 
 * 
 */
#ifndef GODOT_HEX_GRID_EXTENSION_HEX_GRID_H
#define GODOT_HEX_GRID_EXTENSION_HEX_GRID_H



#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "HexTile.h"
#include <unordered_map>
/// @brief Manages and Manipulates Hexagonal Grids.
class HexGrid : public godot::Node3D
{
    GDCLASS(HexGrid, godot::Node3D)
    /**
     * @brief The max grid size.
     * 
     */
    int max_size{};
    /**
     * @brief The tile size.
     * 
     */
    float tile_size{};
    /**
     * @brief A map that maps a hash value to a hex tile.
     * 
     */
    std::unordered_map<long, HexTile*> tile_map{};
    /**
     * @brief The default tile scene.
     * 
     */
    godot::Ref<godot::PackedScene> tile_scene;

public:
/**
 * @brief When implemented inside the gdscript, and with connect signals enabled in spawn hex, will be called whenver the mouse enters a tile.
 * 
 * @param q The q-coordinate of the tile entered.
 * @param r The r-coordinate of the tile entered.
 */
    virtual void on_mouse_enter_tile(int q, int r);
/**
 * @brief When implemented inside the gdscript, and with connect signals enabled in spawn hex, will be called whenver the mouse exits a tile.
 * 
 * @param q The q-coordinate of the tile exited.
 * @param r The r-coordinate of the tile exited.
 */
    virtual void on_mouse_exit_tile(int q, int r);

private:
    /**
     * @brief Binds methods for usage in Godot.
     * 
     */
    static void _bind_methods();
    /**
     * @brief Converts q and r into a hash value.
     * 
     * @param q The q-coordinate.
     * @param r  The r-coordinate.
     * @return long The resulting hash value from the two coordinates.
     */
    long hash(int q, int r);
    /**
     * @brief Unhashes a key to return a pair containing q and r coordinates.
     * 
     * @param key The key to be unhashed.
     * @return std::pair<int, int> A pair containing q as its first, and r as its second.
     */
    std::pair<int, int> unhash(long key);
    /**
     * @brief A vector containing pairs of hex neighbors based on coordinates.
     * 
     */
    std::vector<std::pair<int, int>>hex_neighbors{ {1,0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1} };
    /**
     * @brief A vector containing pairs of hex diagonals based on coordinates.
     * 
     */
    std::vector<std::pair<int, int>>hex_diagonals{ {2,-1}, {1, -2}, {-1, -1}, {-2, 1}, {-1, 2}, {1, 1} };
       

public: HexGrid();
    /**
     * @brief Enumerations for usage with the mirror hex methods.
     * 
     */
    enum MirrorType {
        /// @brief Creates a vector starting at the center, negates it, and then recreates the original point using the vector.
        LOCAL_NEGATE,
        /// @brief Mirrors the hex across the q axis.
        MIRROR_Q,
        /// @brief Mirrors the hex across the r axis.
        MIRROR_R,
        /// @brief Mirrors the hex across the s axis.
        MIRROR_S,
        /// @brief Creates a vector starting at the center, negates it, mirrors it over the q axis, and then recreates the original point using the vector.
        LOCAL_NEGATE_MIRROR_Q,
        /// @brief Creates a vector starting at the center, negates it, mirrors it over the r axis, and then recreates the original point using the vector.
        LOCAL_NEGATE_MIRROR_R,
        /// @brief Creates a vector starting at the center, negates it, mirrors it over the s axis, and then recreates the original point using the vector.
        LOCAL_NEGATE_MIRROR_S
    };

    /**
     * @brief Converts a given tuple into a string.
     * 
     * @param cube_hex The tuple containing cubic coordinates.
     * @return std::string A string that takes the form of "q,r,s"
     */
    std::string convert_cube_to_string(std::tuple<int, int, int> cube_hex);

public:
    /**
     * @brief Sets the tile size.
     * 
     * @param new_size Self-explanatory.
     */
      void set_tile_size(float new_size);
      /**
       * @brief Gets the tile size.
       * 
       * @return float The tile size.
       */
      float get_tile_size();
      /**
       * @brief Sets the max size of the grid.
       * 
       * @param new_size Self-explanatory.
       */
      void set_max_size(int new_size);
      /**
       * @brief Gets the max size of the grid.
       * 
       * @return int The size of the grid.
       */
      int get_max_size();
      /**
       * @brief Sets the default tile
       * 
       * @param new_scene The scene to use as the tile, must be a HexTile object.
       */
      void set_tile(const godot::Ref<godot::PackedScene> &new_scene);
      /**
       * @brief Gets the current default tile.
       * 
       * @return godot::Ref<godot::PackedScene> The default tile.
       */
      godot::Ref<godot::PackedScene> get_tile();

        /**
         * @brief Spawns a tile at the designated coordinates, provided there is space in the grid and the coordinates are not occupied. NOTE: If you do enable connect mouse signals, you must implement on_mouse_enter_tile and on_mouse_exit_tile in your script.
         * 
         * @param q The q-coordinate to spawn the tile at.
         * @param r The r-coordinate to spawn the tile at.
         * @param connect_mouse_signals Whether or not to connect mouse signals. By default, this is true.
         * @param tile_to_spawn An optional custom tile. GDExtension doesn't support default parameters, but it can be null.
         * @return HexTile* The spawned tile.
         */
      HexTile* spawn_hex(int q, int r, bool connect_mouse_signals, const godot::Ref<godot::PackedScene> &tile_to_spawn);
      /**
       * @brief Gets the hex at the given coordinates, if it exists.
       * 
       * @param q The q-coordinate.
       * @param r The r-coordinate.
       * @return HexTile* The requested hex if it exists, otherwise null.
       */
      HexTile* get_hex(int q, int r);
      
      /**
       * @brief Adds two axial coordinates together, represented by pairs of integers.
       * 
       * @param hex_one The first hex.
       * @param hex_two The second hex.
       * @return std::pair<int, int> The resulting hex from the pairing.
       */
      std::pair<int, int> axial_add(std::pair<int, int> hex_one, std::pair<int, int> hex_two);
      /**
       * @brief Subtracts the second hex from the first.
       * 
       * @param hex_one The first hex.
       * @param hex_two The second hex.
       * @return std::pair<int,int> The resulting hex from the subtraction.
       */
      std::pair<int,int> axial_subtract(std::pair<int,int> hex_one, std::pair<int,int> hex_two);
      /**
       * @brief Multiplies an axial coordinate by a factor.
       * 
       * @param hex The coordinates to multiply.
       * @param factor The amount to multiply by.
       * @return std::pair<int, int> The result of the multiplication.
       */
      std::pair<int, int> axial_scale(std::pair<int, int> hex, int factor);
      /**
       * @brief Adds two cubic coordinates together.
       * 
       * @param hex_one The first hex.
       * @param hex_two The second hex.
       * @return std::tuple<int, int, int> The sum of the two hexes.
       */
      std::tuple<int, int, int> cube_add(std::tuple<int, int, int> hex_one, std::tuple<int, int, int> hex_two);
      /**
       * @brief Subtracts the second hex from the first.
       * 
       * @param hex_one The first hex.
       * @param hex_two The second hex.
       * @return std::tuple<int, int, int> The resulting hex from the subtraction.
       */
      std::tuple<int, int, int> cube_subtract(std::tuple<int, int, int> hex_one, std::tuple<int, int, int> hex_two);
     
      /**
       * @brief Converts an axial coordinate into a cubic one.
       * 
       * @param axial_hex The axial coordinate.
       * @return std::tuple<int, int, int> The corresponding cubic coordinate.
       */
      std::tuple<int, int, int> axial_to_cube(std::pair<int, int> axial_hex);
      /**
       * @brief Converts a cubic coordinate into an axial one.
       * 
       * @param cube_hex The cubic coordinate
       * @return std::pair<int, int> The corresponding axial coordinate.
       */
      std::pair<int, int> cube_to_axial(std::tuple<int, int, int> cube_hex);
      /**
       * @brief Rotates a hex around a center 60'.
       * 
       * @param hex_to_rotate The hex to be rotated.
       * @param center The center to rotate around.
       * @param clockwise The direction to rotate in. True for clockwise, false for counter-clockwise.
       * @return std::tuple<int, int, int> The rotated hex.
       */
      std::tuple<int, int, int> cube_rotate(std::tuple<int, int, int> hex_to_rotate, std::tuple<int, int, int> center, bool clockwise);
      /**
       * @brief Rotates a hex around a center 60'.
       * 
       * @param hex_to_rotate The hex to be rotated.
       * @param center The center to rotate around.
       * @param clockwise The direction to rotate in. True for clockwise, false for counter-clockwise.
       * @return std::pair<int, int> The rotated hex.
       */
      std::pair<int, int> axial_rotate(std::pair<int, int> hex_to_rotate, std::pair<int, int> center, bool clockwise);
      /**
       * @brief Rotates a hex around a center in 60' increments.
       * 
       * @param hex_to_rotate The hex to be rotated.
       * @param center The center to rotate around.
       * @param num_and_direction The amount and direction to rotate in. Positive is clockwise, negative is counter clockwise.
       * @return HexTile* The rotated hex, if it exists. Null otherwise.
       */
      HexTile* rotate_hex(HexTile* hex_to_rotate, HexTile* center, int num_and_direction);
      /**
       * @brief Rotates a hex array around a center in 60' increments. Intended for usage directly from Godot.
       * 
       * @param hex_array The array to be rotated.
       * @param center The center to rotate around.
       * @param num_and_direction 
       * @return godot::Array The rotated hex array. Some or all of the hexes may be null if they do not exist, and will be excluded from the array.
       */
      godot::Array rotate_hex_array(godot::Array hex_array, HexTile* center, int num_and_direction);
      /**
       * @brief Rounds an axial coordinate defined by a pair of doubles into a pair of ints. Intended for usage directly from Godot.
       * 
       * @param hex The coordinate to be rounded.
       * @return std::pair<int, int> The integer pair returned.
       */
      std::pair<int, int> axial_round(std::pair<double, double> hex);
        /**
         * @brief Mirrors a hex along an origin.
         * @param hex The hex to be mirrored.
         * @param origin The origin to mirror around.
         * @param mirror_type The type of mirroring to be done. Typically a MirrorType enum.
        */
      std::pair<int, int> axial_mirror(std::pair<int, int> hex, std::pair<int, int> origin, int mirror_type);
      /**
         * @brief Mirrors a hex along an origin.
         * @param hex The hex to be mirrored.
         * @param origin The origin to mirror around.
         * @param mirror_type The type of mirroring to be done. Typically a MirrorType enum.
        */
      std::tuple<int, int, int> cube_mirror(std::tuple<int, int, int> hex, std::tuple<int, int, int> center, int mirror_type);
      /**
         * @brief Mirrors a hex along an origin. Intended for usage directly from Godot.
         * @param hex The hex to be mirrored.
         * @param origin The origin to mirror around.
         * @param mirror_type the type of mirroring to be done. See enums for the types.
        */
      HexTile* mirror_hex(HexTile* hex, HexTile* origin, int mirror_type);
     /**
        * @brief Mirrors hexes along an origin. Intended for usage directly from Godot.
        * @param hex_array The hexes to be mirrored.
        * @param origin The origin to mirror around.
        * @param mirror_type the type of mirroring to be done. See enums for the types.
        */
      godot::Array mirror_hex_array(godot::Array hex_array, HexTile *origin, int mirror_type);
      /**
       * @brief Negates a hex locally around a center.
       * 
       * @param hex The hex to be negated.
       * @param center The center to negate around.
       * @return std::tuple<int,int,int> The negated hex.
       */
      std::tuple<int,int,int> cube_negate_hex(std::tuple<int,int,int> hex, std::tuple<int,int,int> center);
      /**
       * @brief Negates a hex locally around a center.
       * 
       * @param hex The hex to be negated.
       * @param center The center to negate around.
       * @return HexTile* The negated hex.
       */
      HexTile* negate_hex_local(HexTile* hex, HexTile* center);
      /**
       * @brief Rounds a cubic coordinate from a tuple of doubles to a tuple of integers.
       * 
       * @param cube_hex The coordinates to round.
       * @return std::tuple<int, int, int> The rounded coordinates.
       */
      std::tuple<int, int, int> cube_round(std::tuple<double, double, double> cube_hex);
      /**
       * @brief Gets a line from one hex to another in axial coordinates.
       * 
       * @param hex_one The first hex.
       * @param hex_two The second hex.
       * @return std::vector<std::pair<int, int>> A vector containing all the coordinates from the line.
       */
      std::vector<std::pair<int, int>> get_axial_line(std::pair<int, int> hex_one, std::pair<int, int> hex_two);
      /**
       * @brief Gets a line from one hex to another in cubic coordinates.
       * 
       * @param hex_one The first hex.
       * @param hex_two The second hex.
       * @return std::vector<std::tuple<int, int, int>> 
       */
      std::vector<std::tuple<int, int, int>> get_cube_line(std::tuple<int, int, int> hex_one, std::tuple<int, int, int> hex_two);
      /**
       * @brief Gets a line from one hex to another in cubic coordinates. Mirrors around the y axis if the vector between the two hexes is negative.
       * 
       * @param hex_one The first hex. The order will matter for the resulting line.
       * @param hex_two The second hex. The order will matter for the resulting line.
       * @return std::vector<std::tuple<int, int, int>> The resulting hex array.
       */
      std::vector<std::tuple<int, int, int>> get_cube_symmetric_line(std::tuple<int, int, int> hex_one, std::tuple<int, int, int> hex_two);
        /**
        * @brief Gets a line between two hexes, returning both end points and the hexes between them.
        * @param hex_one The first hex to use as an starting point. If make_symmetric is true, the order will matter.
        * @param hex_two The second hex to use as an end point.
        * @param make_symmetric Forces the line to be symmetric when compared to another, but also makes which hex is the first affect the line.
        * @return An array containing a line of between the two hexes, with both hexes included.
        */
      godot::Array get_line(HexTile* hex_one, HexTile* hex_two, bool make_symmetric);

        /**
         * @brief A simple lerp function. Advances a to the value of b by the percentage of t.
         * 
         * @param a 
         * @param b 
         * @param t The percentage to lerp by.
         * @return double The result of the lerp.
         */
      double lerp(double a, double b, double t);

      /**
       * @brief Lerps the first hex towards the second hex by the percentage of t.
       * 
       * @param hex_one The first hex.
       * @param hex_two The second hex.
       * @param t The percentage to lerp by.
       * @return std::pair<double, double> The result of the lerp, it is in fractional coordinates.
       */
      std::pair<double, double> axial_lerp(std::pair<int, int> hex_one, std::pair<int, int> hex_two, double t);

        /**
         * @brief Calculates the distance between two axial coordinates.
         * 
         * @param hex_one The first hex.
         * @param hex_two The second hex.
         * @return float The resulting distance as a float.
         */
      float calculate_distance_axial(std::pair<int, int> hex_one, std::pair<int, int> hex_two);
        /**
        * @brief Calculates the distance between two hexes. Intended for usage directly from Godot.
        * 
        * @param hex_one The first hex.
        * @param hex_two The second hex.
        * @return float The resulting distance as a float.
        */
      float calculate_distance_hex(HexTile* tile1, HexTile* tile2);
        /**
         * @brief Calculates a ring of axial hexes.
         * 
         * @param q The origin q-coordinate.
         * @param r The origin r-coordinate.
         * @param radius The radius of the ring. Must be greater than zero if you want more than the original hex.
         * @return std::vector<std::pair<int, int>> A vector containing the hexes of the ring.
         */
      std::vector<std::pair<int, int>> get_ring(int q, int r, int radius);
      /**
       * @brief Calculates a ring of hexes. Intended for usage directly from Godot.
       * 
       * @param center The center of the ring.
       * @param radius The radius of the ring, must be greater than zero.
       * @return godot::Array An array containing a ring of hexes. Null hexes will be excluded.
       */
      godot::Array get_ring_hex(HexTile* center, int radius);
      /**
       * @brief Calculates several rings of hexes, in a sort of inward spiral. This can be an efficient way to get a count of hexes within a radius. Intended for usage directly from Godot.
       * 
         * @param q The origin q-coordinate.
         * @param r The origin r-coordinate.
         * @param radius The radius of the last ring. Must be greater than zero if you want more than the original hex.
       * @return std::vector<std::pair<int, int>> A vector containing the hexes of the ring.
       */
      std::vector<std::pair<int, int>> get_spiral_ring(int q, int r, int radius);
        /**
       * @brief Calculates a spiral ring of hexes by calculating multiple rings up to the radius.
       * 
       * @param center The center of the ring.
       * @param radius The radius of the ring, must be greater than zero.
       * @return godot::Array An array containing a ring of hexes. Null hexes will be excluded.
       */
      godot::Array get_spiral_ring_hex(HexTile* center, int radius);

    /**
     * @brief Searches outward in a breadth first search for every hex possible. Only stopped by those it has already visited.
     * 
     * @param origin The origin hex to start at.
     * @param visited The pre visited map. Hexes in this map will not be searched from, allowing a boundary to be defined.
     * @return std::unordered_map<long, HexTile*> A map containing the hexes from the search plus the pre visited hexes. Will only include hexes defined in the tile map.
     */
      std::unordered_map<long, HexTile*> breadth_first_search(std::pair<int, int> origin, std::unordered_map<long, HexTile*> visited);
    /**
     * @brief Searches outward in a breadth first search for every hex possible. Only stopped by those it has already visited. Intended for usage directly from Godot.
     * 
     * @param origin The origin hex to start at.
     * @param pre_visited The pre visited array. Hexes in this array will not be searched from, allowing a boundary to be defined.
     * @return godot::Array An array containing the hexes from the search, plus the pre visited hexes. Will not contain null hexes.
     */
      godot::Array breadth_first_search_hex(HexTile* origin, godot::Array pre_visited);
    /**
     * @brief Deletes a hex at the given coordinates, if it exists. This calls queue free on the hex. Intended for usage directly from Godot.
     * 
     * @param q The q-coordinate to delete at.
     * @param r The r-coordinate to delete at.
     */
      void delete_hex(int q, int r);
    /**
     * @brief Deletes the hex at the coordinates and spawns in a new hex with the given tile scene. Intended for usage directly from Godot.
     * 
     * @param q The q-coordinate to replace at.
     * @param r The r-coordinate to replace at.
     * @param connect_mouse_signals Whether or not to connect mouse signals. By default, this is true.
     * @param tile_to_replace_with The tile to replace this one with.
     */
      HexTile* replace_hex(int q, int r, bool connect_mouse_signals, const godot::Ref<godot::PackedScene> &tile_to_replace_with);
public:
    /**
     * @brief Gets the neighboring hexes.
     * 
     * @param center_hex the center hex to get neighbors from.
     * @return std::vector<std::pair<int, int>> The neighboring hexes.
     */
    std::vector<std::pair<int, int>> get_neighbors(std::pair<int, int> center_hex);
    /**
     * @brief Gets the neighboring hexes. Intended for usage directly from Godot.
     * 
     * @param hex the center hex to get neighbors from.
     * @return std::vector<std::pair<int, int>> The neighboring hexes.
     */
    godot::Array get_neighbors_hex(HexTile* hex);
    /**
     * @brief Gets the diagonal hexes.
     * 
     * @param center_hex the center hex to get diagonals from.
     * @return std::vector<std::pair<int, int>> The diagonal hexes.
     */
    std::vector<std::pair<int, int>> get_diagonals(std::pair<int, int> center_hex);
    /**
     * @brief Gets the diagonal hexes. Intended for usage directly from Godot.
     * 
     * @param hex the center hex to get diagonals from.
     * @return std::vector<std::pair<int, int>> The diagonal hexes.
     */
    godot::Array get_diagonals_hex(HexTile* hex);
};




#endif //GODOT_HEX_GRID_EXTENSION_HEX_GRID_H