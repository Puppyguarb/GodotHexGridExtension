#include "HexTile.h"
#include "HexGrid.h"

void HexTile::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_co_ords"), &HexTile::get_co_ords);
	godot::ClassDB::bind_method(godot::D_METHOD("get_q"), &HexTile::get_q);
	godot::ClassDB::bind_method(godot::D_METHOD("get_r"), &HexTile::get_r);
}

HexTile::HexTile()  {
	
}



godot::Array HexTile::get_co_ords() {

	godot::Array co_ord_array = godot::Array();
	co_ord_array.append(co_ords.first);
	co_ord_array.append(co_ords.second);
	return co_ord_array;
}

void HexTile::set_co_ords(int q, int r) {
	co_ords = std::make_pair(q,r);
}

int HexTile::get_q() {
	return co_ords.first;
}

int HexTile::get_r() {
	return co_ords.second;
}