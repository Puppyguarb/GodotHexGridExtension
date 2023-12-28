#include "HexGrid.h"
#include "godot_cpp/core/class_db.hpp"

HexGrid::HexGrid() {

}

std::string HexGrid::convert_cube_to_string(std::tuple<int,int,int> cube_hex) {
	int q, r, s;
	std::tie(q,r,s) = cube_hex;
	return std::to_string(q)+","+std::to_string(r)+","+std::to_string(s);
}

long HexGrid::hash(int q, int r) {
	return (long(q) + long(r) * max_size);
}

std::pair<int, int> HexGrid::unhash(long key)
{
	return std::make_pair(int(key % max_size), int(key / max_size));
}

int HexGrid::get_max_size() {
	return max_size;
}

void HexGrid::set_max_size(int new_size) {
	max_size = new_size;
}

void HexGrid::set_tile_size(float new_size) {
	tile_size = new_size;
}

float HexGrid::get_tile_size() {
	return tile_size;
}

void HexGrid::set_tile(const godot::Ref<godot::PackedScene> &new_scene) {
	tile_scene = new_scene;
}

godot::Ref<godot::PackedScene> HexGrid::get_tile() {
	return tile_scene;
}

void HexGrid::on_mouse_enter_tile(int q, int r) {

}

void HexGrid::on_mouse_exit_tile(int q, int r) {

}


HexTile* HexGrid::spawn_hex(int q, int r, bool connect_mouse_signals, const godot::Ref<godot::PackedScene> &tile_to_spawn) {
	ERR_FAIL_COND_V_MSG(tile_map.size() >= max_size,nullptr, "Grid is full. Increase the grid's max size to add more hexes.");
	ERR_FAIL_COND_V_MSG((tile_scene == NULL),nullptr, "No default hex assigned, check that the grid has one.");
	ERR_FAIL_COND_V_MSG(tile_map[hash(q, r)] != NULL,nullptr, "Hex was already spawned.");
	godot::Node* tile_node;
	if (tile_to_spawn != NULL) {
		tile_node = tile_to_spawn->instantiate();
	} else {
		tile_node = tile_scene->instantiate();
	}

	this->add_child(tile_node);
	HexTile* tile_hex = godot::Object::cast_to<HexTile>(tile_node);
	ERR_FAIL_COND_V_MSG(tile_hex == nullptr,nullptr,"Spawned Tile was not a HexTile.");


	tile_hex->set_co_ords(q, r);
	if (connect_mouse_signals) {
		tile_hex->connect("mouse_entered", godot::Callable(this, "on_mouse_enter_tile").bind(q,r));
		tile_hex->connect("mouse_exited", godot::Callable(this, "on_mouse_exit_tile").bind(q,r));
	}

	float x = (float(q) + (float(r) * .5f)) * tile_size;
	float z = float(r) * tile_size * sqrtf(3) / 2.0f;

	tile_hex->set_position(godot::Vector3(x, 0, z));

	//NOTE: Insert function w/ a pair causes issues that end up creating a null ptr in the pair, use this way instead.
	tile_map[hash(q, r)] = tile_hex;

	return tile_hex;
}

float HexGrid::calculate_distance_axial(std::pair<int,int> hex_one, std::pair<int,int> hex_two) {
	int q1,r1;
	std::tie(q1,r1) = hex_one;
	int q2, r2;
	std::tie(q2,r2) = hex_two;

	return (abs(q1 - q2) + abs(q1 - q2 + r1 - r2) + abs(r1 - r2)) / 2.0f;
}

float HexGrid::calculate_distance_hex(HexTile* tile1, HexTile* tile2) {
	ERR_FAIL_NULL_V_MSG(tile1, -1, "First tile was null.");
	ERR_FAIL_NULL_V_MSG(tile2, -1, "Second tile was null.");
	return calculate_distance_axial(tile1->co_ords, tile2->co_ords);
}

std::pair<int, int> HexGrid::axial_round(std::pair<double, double> axial_hex) {
	// Can't use axial to cube here since it uses ints instead of doubles.
	return cube_to_axial(cube_round(std::tuple<double,double,double>(axial_hex.first,axial_hex.second,-axial_hex.first-axial_hex.second)));
}


std::pair<int,int> HexGrid::axial_mirror(std::pair<int,int> hex, std::pair<int,int>origin, int mirror_type) {
	return cube_to_axial(cube_mirror(axial_to_cube(hex),axial_to_cube(origin),mirror_type));
}


std::tuple<int,int,int> HexGrid::cube_mirror(std::tuple<int,int,int> hex, std::tuple<int,int,int> origin, int mirror_type) {
	int vector_q,vector_r,vector_s;
	std::tie(vector_q,vector_r,vector_s) = cube_subtract(hex,origin);
	

	std::tuple<int,int,int>mirrored_hex{};
	switch(mirror_type) {
		// No mirror, but negate the vector.
		case 0:
			mirrored_hex = {-vector_q, -vector_r, -vector_s};
			break;
		// Mirror over q
		case 1:
			mirrored_hex = {vector_q, vector_s, vector_r};
			break;
		// Mirror over r
		case 2:
			mirrored_hex = {vector_s, vector_r, vector_q};
			break;
		// Mirror over s
		case 3:
			mirrored_hex = {vector_r, vector_q, vector_s};
			break;
		// Mirror over q and negate
		case 4:
			mirrored_hex = {-vector_q, -vector_s, -vector_r};
			break;
		// Mirror over r and negate
		case 5:
			mirrored_hex = {-vector_s, -vector_r, -vector_q};
			break;
		// Mirror over s and negate
		case 6:
			mirrored_hex = {-vector_r, -vector_q, -vector_s};
			break;
	}

	return cube_add(mirrored_hex, origin);
}


HexTile* HexGrid::mirror_hex(HexTile* hex, HexTile* origin, int mirror_type) {
	ERR_FAIL_NULL_V_MSG(hex, nullptr, "Hex to be mirrored cannot be null.");
	ERR_FAIL_NULL_V_MSG(origin, nullptr, "Origin hex cannot be null.");
	ERR_FAIL_COND_V_MSG((mirror_type < 0) || (mirror_type > 6), nullptr, "Invalid mirror type.");
	std::pair<int,int> pair = axial_mirror(hex->co_ords, origin->co_ords, mirror_type);

	return get_hex(pair.first,pair.second);
}


godot::Array HexGrid::mirror_hex_array(godot::Array hex_array, HexTile* origin, int mirror_type) {
	ERR_FAIL_NULL_V_MSG(origin, godot::Array(), "Origin hex cannot be null.");
	ERR_FAIL_COND_V_MSG((mirror_type < 0) || (mirror_type > 6), godot::Array(), "Invalid mirror type.");

	godot::Array return_array = godot::Array();

	for (int i = 0; i < hex_array.size(); i++) {
		HexTile* hex = godot::Object::cast_to<HexTile>(hex_array[i]);
		ERR_FAIL_NULL_V_MSG(hex, godot::Array(), "Can only mirror arrays of hexes.");

		std::pair<int,int> pair = axial_mirror(hex->co_ords, origin->co_ords, mirror_type);
		HexTile* mirrored_hex = get_hex(pair.first,pair.second);
		if (mirrored_hex) {
			return_array.append(mirrored_hex);
		}
	}

	return return_array;
}

// Locally negates a hex around a center.
std::tuple<int,int,int> HexGrid::cube_negate_hex(std::tuple<int,int,int> hex, std::tuple<int,int,int> center) {
	std::tuple<int,int,int> local_hex = cube_subtract(hex,center);
	int q, r, s;
	std::tie(q,r,s) = local_hex;
	std::tuple<int,int,int> negated_hex = std::tuple<int,int,int>{-q,-r,-s};
	return cube_add(negated_hex,center);
}

// Locally negates a hex around a center.
HexTile* HexGrid::negate_hex_local(HexTile* hex, HexTile* center) {
	ERR_FAIL_NULL_V_MSG(hex,nullptr,"Cannot negate non-existing hex");
	ERR_FAIL_NULL_V_MSG(center,nullptr,"Cannot center around non-existing hex");

	std::tuple<int,int,int> negated_hex = cube_negate_hex(axial_to_cube(std::pair(hex->co_ords.first,hex->co_ords.second)),axial_to_cube(std::pair(center->co_ords.first,center->co_ords.second)));
	int q, r, _s;
	std::tie(q,r,_s) = negated_hex;

	return get_hex(q,r);
}

std::tuple<int,int,int> HexGrid::cube_round(std::tuple<double,double,double> cube_hex) {
	double q = round(std::get<0>(cube_hex));
	double r = round(std::get<1>(cube_hex));
	double s = round(std::get<2>(cube_hex));

	double q_diff = abs(q - std::get<0>(cube_hex));
	double r_diff = abs(r - std::get<1>(cube_hex));
	double s_diff = abs(s - std::get<2>(cube_hex));

	if (q_diff > r_diff && q_diff > s_diff) {
		q = -r - s;
	}
	else if (r_diff > s_diff) {
		r = -q - s;
	}
	else {
		s = -q - r;
	}
	return std::tuple<int,int,int>(int(q),int(r),int(s));
}

std::vector<std::pair<int, int>> HexGrid::get_axial_line(std::pair<int, int> hex_one, std::pair<int, int> hex_two) {
	std::vector<std::pair<int, int>> results{};

	int number_of_points = calculate_distance_axial(hex_one, hex_two);

	for (int i = 0; i <= number_of_points; i++) {
		std::pair<double, double> point = axial_lerp(hex_one, hex_two, 1.0 / number_of_points * i);
		results.push_back(axial_round(point));
	}


	return results;
}

std::vector<std::tuple<int,int,int>> HexGrid::get_cube_line(std::tuple<int,int,int> hex_one, std::tuple<int,int,int> hex_two) {
	//Gets the difference between two values as an absolute value, and also gets the sign.
	auto Difference = [](int start, int end) {
		return end >= start ? std::make_pair(end - start, 1) : std::make_pair(start - end, -1);
	};

	std::vector<std::tuple<int,int,int>> results{};

	auto Plot = [](int x, int y, std::vector<std::tuple<int,int,int>> &result_list) {
		result_list.push_back(std::make_tuple(x,y,-x-y));
	};

	// Assigns variables based on variables from the tuples.
	int x1, y1, z1;
	std::tie(x1,y1,z1) = hex_one;
	int x2, y2, z2;
	std::tie(x2,y2,z2) = hex_two;

	// Assigns variables based on the tuple generated from the Difference function.
	int diff_x, diff_y, diff_z;
	int sign_x, sign_y, sign_z;
	std::tie(diff_x,sign_x) = Difference(x1,x2);
	std::tie(diff_y,sign_y) = Difference(y1,y2);
	std::tie(diff_z,sign_z) = Difference(z1,z2);

	int x = x1;
	int y = y1;
	int z = z1;

	Plot(x, y, results);

	// A variation of the Bresenham's line algorithm called the TranThong algorithm.
	// I slapped together this crude implementation from denismr's Symmetric Precomputed Visibility Trie project
	// Check out that at https://github.com/denismr/SymmetricPCVT/

	int test1 = ((sign_y == -1) ? -1 : 0);// + (sign_x == -1) ? -1 : 0;
	int test2 = ((sign_z == -1) ? -1 : 0);// + (sign_x == -1) ? -1 : 0;

	if (diff_x >= diff_y && diff_x >= diff_z) {

		test1 = (diff_x + test1) >> 1;
		test2 = (diff_x + test2) >> 1;

		for (int i = 0; i < diff_x; i++) {
			test1 -= diff_y;
			test2 -= diff_z;
			x += sign_x;
			if (test1 < 0) {
				y += sign_y;
				test1 += diff_x;
			}
			if (test2 < 0) {
				z += sign_z;
				test2 += diff_x;
			}
			Plot(x, y, results);
		}

	} else if (diff_y >= diff_x && diff_y >= diff_z) {

		test1 = (diff_y + test1) >> 1;
		test2 = (diff_y + test2) >> 1;

		for (int i = 0; i < diff_y; i++) {
			test1 -= diff_x;
			test2 -= diff_z;
			y += sign_y;
			if (test1 < 0) {
				x += sign_x;
				test1 += diff_y;
			}
			if (test2 < 0) {
				z += sign_z;
				test2 += diff_y;
			}
			Plot(x, y, results);
		}

	} else {

		test1 = (diff_z + test1) >> 1;
		test2 = (diff_z + test2) >> 1;


		for (int i = 0; i < diff_z; i++) {
			test1 -= diff_x;
			test2 -= diff_y;
			z += sign_z;
			if (test1 < 0) {
				x += sign_x;
				test1 += diff_z;
			}
			if (test2 < 0) {
				y += sign_y;
				test2 += diff_z;
			}
			Plot(x, y, results);
		}
	}

	return results;
}

std::vector<std::tuple<int,int,int>> HexGrid::get_cube_symmetric_line(std::tuple<int,int,int> hex_one, std::tuple<int,int,int> hex_two) {
		auto Sign = [](int val) {
    	return (int(0) < val) - (val < int(0));
	};
	
	std::vector<std::tuple<int,int,int>> return_list{};
	
	auto vector = cube_subtract(hex_two,hex_one);

	// We need to mirror if q is less than s. That is when the algorithm has trouble.
	if ((std::get<0>(vector)) < (std::get<2>(vector))) {
		//Mirror
		std::tuple<int,int,int> mirrored_hex_two = cube_mirror(hex_two,hex_one,1);

		//
		auto line = get_cube_line(hex_one,mirrored_hex_two);

		//Unmirror
		for (int i = 0; i < line.size(); i++) {
			auto unmirrored_hex = line[i];
			auto mirrored_hex = cube_mirror(unmirrored_hex,hex_one,1);
			return_list.push_back(mirrored_hex);

		}

	} else {
	// Otherwise, just draw as normal.
		 auto line = get_cube_line(hex_one, hex_two);
		 for (std::tuple<int,int,int> hex : line) {
			return_list.push_back(hex);
		}
	}

	return return_list;
}



godot::Array HexGrid::get_line(HexTile* hex_one, HexTile* hex_two, bool make_symmetric) {
	godot::Array return_array = godot::Array();
	ERR_FAIL_NULL_V_MSG(hex_one, return_array, "First hex on line was null.");
	ERR_FAIL_NULL_V_MSG(hex_two, return_array, "Second hex on line was null.");

	std::vector<std::tuple<int,int,int>> unfiltered_hexes;
	if (!make_symmetric) {
		unfiltered_hexes = get_cube_line(axial_to_cube(hex_one->co_ords), axial_to_cube(hex_two->co_ords));
	} else {
		unfiltered_hexes = get_cube_symmetric_line(axial_to_cube(hex_one->co_ords), axial_to_cube(hex_two->co_ords));
	}

	for (std::tuple<int, int,int> tuple : unfiltered_hexes) {
		HexTile* hex = get_hex(std::get<0>(tuple),std::get<1>(tuple));
		if (hex) {
			return_array.append(hex);
		}
	}

	return return_array;
}



double HexGrid::lerp(double a, double b, double t) {
	return a + (b - a) * t;
}


std::pair<double, double> HexGrid::axial_lerp(std::pair<int, int> hex_one, std::pair<int, int> hex_two, double t) {

	return std::pair<double, double>(lerp(hex_one.first, hex_two.first, t), lerp(hex_one.second, hex_two.second, t));
}



std::vector<std::pair<int,int>> HexGrid::get_neighbors(std::pair<int,int> center_hex) {
	std::vector<std::pair<int, int>> return_array{};

	for (std::pair<int, int> direction : hex_neighbors) {
		std::pair<int, int> neighbor = std::pair<int, int>(center_hex.first + direction.first, center_hex.second + direction.second);

		return_array.push_back(neighbor);

	}
	return return_array;
}


godot::Array HexGrid::get_neighbors_hex(HexTile* hex) {
	godot::Array return_array = godot::Array();
	ERR_FAIL_NULL_V_MSG(hex, return_array, "Cannot get neighbors on non-existing hex.");

	for (std::pair<int, int> pair : get_neighbors(hex->co_ords)) {
		HexTile* neighbor = get_hex(pair.first, pair.second);
		if (neighbor) {
			return_array.append(neighbor);
		}

	}
	return return_array;
}


std::unordered_map<long, HexTile*> HexGrid::breadth_first_search(std::pair<int, int> origin, std::unordered_map<long, HexTile*> visited = std::unordered_map<long, HexTile*>()) {

	std::vector<std::pair<int, int>> queue{};
	queue.push_back(origin);

	//While we have tiles left in the queue

	while (!queue.empty()) {
		// Remove the last tile from the queue
		std::pair<int, int> tile = queue.back();
		queue.pop_back();

		long hash_value = hash(tile.first, tile.second);
		//Check if it's already in our visited map
		if (!visited[hash_value]) {
			//If it's not, add it.
			visited[hash_value] = tile_map[hash_value];

			//Since it wasn't in our visited dict, add its neighbors to our queue.
			for (std::pair<int, int> neighbor : get_neighbors(tile)) {
				// Only add neighbors that are actually in the tile_map. Otherwise, we could iterate infinitely.
				if (tile_map[hash(neighbor.first, neighbor.second)]) {
					queue.push_back(neighbor);
				}
			}
		}
	}

	//After doing the search, return the results.
	return visited;
}

godot::Array HexGrid::breadth_first_search_hex(HexTile* origin, godot::Array pre_visited = godot::Array()) {
	ERR_FAIL_NULL_V_MSG(origin, godot::Array(), "Origin hex cannot be null.");

	godot::Array results = godot::Array();

	std::unordered_map<long, HexTile*> input_hex_map{};

	for (int i = 0; i < pre_visited.size(); i++) {
		HexTile* hex = godot::Object::cast_to<HexTile>(pre_visited[i]);
		ERR_FAIL_NULL_V_MSG(hex, godot::Array(), "Can only use pre-visited arrays of hexes.");
		ERR_FAIL_COND_V_MSG((origin == hex), godot::Array(), "Origin cannot be in pre-visited array, search will simply return nothing.");
		input_hex_map[hash(hex->co_ords.first, hex->co_ords.second)] = hex;
	}

	std::unordered_map<long, HexTile*> search_map = breadth_first_search(origin->co_ords, input_hex_map);

	for (std::pair<long, HexTile*> pair : search_map) {
		results.append(pair.second);
	}

	return results;
}


std::vector<std::pair<int, int>> HexGrid::get_diagonals(std::pair<int, int> center_hex) {
	std::vector<std::pair<int, int>> return_array{};

	for (std::pair<int, int> direction : hex_diagonals) {
		std::pair<int, int> diagonal = std::pair<int, int>(center_hex.first + direction.first, center_hex.second + direction.second);

		return_array.push_back(diagonal);

	}
	return return_array;
}

godot::Array HexGrid::get_diagonals_hex(HexTile* hex) {
	godot::Array return_array = godot::Array();
	ERR_FAIL_NULL_V_MSG(hex, return_array, "Cannot get diagonals on non-existing hex.");

	for (std::pair<int, int> pair : get_diagonals(hex->co_ords)) {
		HexTile* diagonal = get_hex(pair.first, pair.second);
		if (diagonal) {
			return_array.append(diagonal);
		}

	}
	return return_array;
}


std::pair<int, int> HexGrid::axial_add(std::pair<int, int> hex_one, std::pair<int, int> hex_two) {
	return std::pair<int, int>{hex_one.first + hex_two.first, hex_one.second + hex_two.second};
}

std::pair<int,int> HexGrid::axial_subtract(std::pair<int,int> hex_one, std::pair<int,int> hex_two) {
	return std::pair<int, int>{hex_one.first - hex_two.first, hex_one.second - hex_two.second}; 
}

std::pair<int, int> HexGrid::axial_scale(std::pair<int, int> hex, int factor) {
	return std::pair<int, int>{hex.first * factor, hex.second * factor};
}


std::tuple<int, int, int> HexGrid::cube_add(std::tuple<int, int, int> hex_one, std::tuple<int, int, int> hex_two) {
	int q1 = std::get<0>(hex_one);
	int r1 = std::get<1>(hex_one);
	int s1 = std::get<2>(hex_one);

	int q2 = std::get<0>(hex_two);
	int r2 = std::get<1>(hex_two);
	int s2 = std::get<2>(hex_two);

	return std::tuple<int, int, int>(q1 + q2, r1 + r2, s1 + s2);
}

std::tuple<int, int, int> HexGrid::cube_subtract(std::tuple<int, int, int> hex_one, std::tuple<int, int, int> hex_two) {
	int q1 = std::get<0>(hex_one);
	int r1 = std::get<1>(hex_one);
	int s1 = std::get<2>(hex_one);

	int q2 = std::get<0>(hex_two);
	int r2 = std::get<1>(hex_two);
	int s2 = std::get<2>(hex_two);

	return std::tuple<int, int, int>(q1-q2,r1-r2,s1-s2);
}

std::tuple<int, int, int> HexGrid::axial_to_cube(std::pair<int, int> axial_hex) {
	return std::tuple<int, int, int>(axial_hex.first, axial_hex.second, -axial_hex.first - axial_hex.second);
}

std::pair<int, int> HexGrid::cube_to_axial(std::tuple<int, int, int> cube_hex) {
	return std::pair<int, int>(std::get<0>(cube_hex), std::get<1>(cube_hex));
}


std::tuple<int, int, int> HexGrid::cube_rotate(std::tuple<int, int, int> hex_to_rotate, std::tuple<int, int, int> center, bool clockwise) {
	std::tuple<int, int, int> cube_vector = cube_subtract(hex_to_rotate, center);

	int q = std::get<0>(cube_vector);
	int r = std::get<1>(cube_vector);
	int s = std::get<2>(cube_vector);

	std::tuple<int, int, int> rotated_vector = clockwise ? std::tuple<int, int, int>(-r, -s, -q) : std::tuple<int, int, int>(-s, -q, -r);

	return cube_add(rotated_vector, center);
}


std::pair<int, int> HexGrid::axial_rotate(std::pair<int, int> hex_to_rotate, std::pair<int, int> center, bool clockwise) {
	return cube_to_axial(cube_rotate(axial_to_cube(hex_to_rotate), axial_to_cube(center), clockwise));
}

HexTile* HexGrid::rotate_hex(HexTile* hex_to_rotate, HexTile* center, int num_and_direction) {
	ERR_FAIL_NULL_V_MSG(hex_to_rotate, nullptr, "Cannot rotate non-existing hex.");
	ERR_FAIL_NULL_V_MSG(center, nullptr, "Cannot rotate around non-existing center.");
	bool clockwise = num_and_direction >= 0;
	int rotation_count = abs(num_and_direction) % 6;

	std::pair<int, int> rotated_hex = hex_to_rotate->co_ords;

	while (rotation_count > 0) {
		rotation_count--;
		rotated_hex = axial_rotate(rotated_hex, center->co_ords, clockwise);
	}

	return get_hex(rotated_hex.first, rotated_hex.second);
}

godot::Array HexGrid::rotate_hex_array(godot::Array hex_array, HexTile* center, int num_and_direction) {
	ERR_FAIL_NULL_V_MSG(center, godot::Array(), "Cannot rotate around non-existing center.");
	godot::Array return_array = godot::Array();

	for (int i = 0; i < hex_array.size(); i++) {
		HexTile* hex = godot::Object::cast_to<HexTile>(hex_array[i]);
		ERR_FAIL_NULL_V_MSG(hex, godot::Array(), "Can only rotate arrays of hexes.");

		HexTile* rotated_hex = rotate_hex(hex, center, num_and_direction);
		if (rotated_hex) {
			return_array.append(rotated_hex);
		}
	}

	return return_array;
}

std::vector<std::pair<int, int>> HexGrid::get_ring(int q, int r, int radius) {
	std::vector<std::pair<int, int>> results{};
	std::pair<int, int> hex = axial_add(std::pair<int, int>(q, r), axial_scale(hex_neighbors[4], radius));

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < radius; j++) {
			results.push_back(hex);
			
			hex = axial_add(hex, hex_neighbors[i]);
		}
	}

	return results;
}

godot::Array HexGrid::get_ring_hex(HexTile* center, int radius) {
	godot::Array return_array = godot::Array();
	ERR_FAIL_COND_V_MSG((radius <= 0), return_array, "Radius cannot be less than or equal to zero.");
	ERR_FAIL_NULL_V_MSG(center, return_array, "Cannot center on non-existing hex.");

	std::vector<std::pair<int, int>> hex_array = get_ring(center->co_ords.first, center->co_ords.second, radius);


	for (std::pair<int, int> pair : hex_array) {
		HexTile* hex = get_hex(pair.first, pair.second);
		if (hex) {
			return_array.append(hex);
		}
	}

	return return_array;
}

std::vector<std::pair<int, int>> HexGrid::get_spiral_ring(int q, int r, int radius) {
	std::vector<std::pair<int, int>> results{};

	//Add the center so we don't have a spiral with everything but the origin.
	results.push_back(std::pair<int, int>{q, r});

	for (int i = 1; i < radius; i++) {
		std::vector<std::pair<int, int>> ring_pairs = get_ring(q, r, i);
		for (std::pair<int, int> pair : ring_pairs) {
			results.push_back(pair);
		}
	}

	return results;
}

godot::Array HexGrid::get_spiral_ring_hex(HexTile* center, int radius) {
	godot::Array return_array = godot::Array();
	ERR_FAIL_COND_V_MSG((radius <= 0), return_array, "Radius cannot be less than or equal to zero.");
	ERR_FAIL_NULL_V_MSG(center, return_array, "Cannot center on non-existing hex.");

	std::vector<std::pair<int, int>> hex_array = get_spiral_ring(center->co_ords.first, center->co_ords.second, radius);


	for (std::pair<int, int> pair : hex_array) {
		HexTile* hex = get_hex(pair.first, pair.second);
		if (hex) {
			return_array.append(hex);
		}
	}

	return return_array;
}


HexTile* HexGrid::get_hex(int q, int r) {
	return tile_map[hash(q, r)];
}

void HexGrid::delete_hex(int q, int r) {
	ERR_FAIL_COND_MSG(!(tile_map[hash(q, r)]), "Cannot delete non-existing tile.");

	auto hex = tile_map.extract(hash(q, r));
	hex.mapped()->queue_free();
}

HexTile* HexGrid::replace_hex(int q, int r, bool connect_mouse_signals, const godot::Ref<godot::PackedScene> &tile_to_replace_with) {
	ERR_FAIL_COND_V_MSG(!(tile_map[hash(q, r)]),nullptr, "Cannot replace non-existing tile.");
	ERR_FAIL_COND_V_MSG((tile_scene == NULL),nullptr, "No default hex assigned, check that the grid has one.");
	
	delete_hex(q,r);
	spawn_hex(q,r,connect_mouse_signals,tile_to_replace_with);

}

void HexGrid::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("spawn_hex", "q", "r", "connect_mouse_signals", "custom_tile"), &HexGrid::spawn_hex, DEFVAL(true), DEFVAL(nullptr));
	godot::ClassDB::bind_method(godot::D_METHOD("replace_hex", "q", "r", "connect_mouse_signals", "custom_tile"), &HexGrid::replace_hex, DEFVAL(true), DEFVAL(nullptr));
	godot::ClassDB::bind_method(godot::D_METHOD("delete_hex", "q", "r"), &HexGrid::delete_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("get_hex", "q", "r"), &HexGrid::get_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("get_neighbors","hex"), &HexGrid::get_neighbors_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("get_diagonals", "hex"), &HexGrid::get_diagonals_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("get_ring", "center", "radius"), &HexGrid::get_ring_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("get_spiral_ring", "center", "radius"), &HexGrid::get_spiral_ring_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("calculate_distance", "hex_one", "hex_two"), &HexGrid::calculate_distance_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("breadth_first_search", "origin_hex", "pre_visited_array"), &HexGrid::breadth_first_search_hex);

	godot::ClassDB::bind_method(godot::D_METHOD("get_line", "hex_one", "hex_two", "make_symmetric"), &HexGrid::get_line, DEFVAL(false));
	godot::ClassDB::bind_method(godot::D_METHOD("rotate_hex", "hex", "center", "rotation_count_and_direction"), &HexGrid::rotate_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("rotate_hex_array", "hex_array", "center", "rotation_count_and_direction"), &HexGrid::rotate_hex_array);

	godot::ClassDB::bind_method(godot::D_METHOD("mirror_hex", "hex", "origin", "mirror_type"), &HexGrid::mirror_hex);
	godot::ClassDB::bind_method(godot::D_METHOD("mirror_hex_array", "hex_array", "origin", "mirror_type"), &HexGrid::mirror_hex_array);

	godot::ClassDB::bind_method(godot::D_METHOD("negate_hex_local", "hex", "center"), &HexGrid::negate_hex_local);

	BIND_VIRTUAL_METHOD(HexGrid, on_mouse_enter_tile);
	BIND_VIRTUAL_METHOD(HexGrid, on_mouse_exit_tile);

	godot::ClassDB::bind_method(godot::D_METHOD("set_max_size","size"), &HexGrid::set_max_size);
	godot::ClassDB::bind_method(godot::D_METHOD("get_max_size"), &HexGrid::get_max_size);
	godot::ClassDB::bind_method(godot::D_METHOD("set_tile_size","size"), &HexGrid::set_tile_size);
	godot::ClassDB::bind_method(godot::D_METHOD("get_tile_size"), &HexGrid::get_tile_size);
	godot::ClassDB::bind_method(godot::D_METHOD("get_tile"), &HexGrid::get_tile);
	godot::ClassDB::bind_method(godot::D_METHOD("set_tile","tile"), &HexGrid::set_tile);
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "max_size"), "set_max_size", "get_max_size");
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "tile_size"), "set_tile_size", "get_tile_size");
	ADD_PROPERTY(godot::PropertyInfo(godot::Variant::OBJECT, "tile_scene", godot::PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_tile", "get_tile");


	godot::ClassDB::bind_integer_constant(get_class_static(), "MirrorType", "LOCAL_NEGATE", LOCAL_NEGATE);
	godot::ClassDB::bind_integer_constant(get_class_static(), "MirrorType", "MIRROR_Q", MIRROR_Q);
	godot::ClassDB::bind_integer_constant(get_class_static(), "MirrorType", "MIRROR_R", MIRROR_R);
	godot::ClassDB::bind_integer_constant(get_class_static(), "MirrorType", "MIRROR_S", MIRROR_S);
	godot::ClassDB::bind_integer_constant(get_class_static(), "MirrorType", "LOCAL_NEGATE_MIRROR_Q", LOCAL_NEGATE_MIRROR_Q);
	godot::ClassDB::bind_integer_constant(get_class_static(), "MirrorType", "LOCAL_NEGATE_MIRROR_R", LOCAL_NEGATE_MIRROR_R);
	godot::ClassDB::bind_integer_constant(get_class_static(), "MirrorType", "LOCAL_NEGATE_MIRROR_S", LOCAL_NEGATE_MIRROR_S);
}

