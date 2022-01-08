/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
//    Copyright (C) 2021 Cosmos                                             //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#pragma once

#include <unordered_set>

void field_init();
void field_debug(bool *isOpen);

std::unordered_set<void*> fieldPatchedAddress{};

template<typename T>
T* get_field_parameter_address(int id)
{
	byte* scriptPtr = *ff7_externals.field_script_ptr;
	return (T*)(&scriptPtr[ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] + id + 1]);
}

template<typename T>
T get_field_parameter(int id)
{
	return *get_field_parameter_address<T>(id);
}

template<typename T>
void set_field_parameter(int id, T value)
{
	byte* scriptPtr = *ff7_externals.field_script_ptr;
	*(T*)(&scriptPtr[ff7_externals.field_curr_script_position[*ff7_externals.current_entity_id] + id + 1]) = value;
}

byte get_field_bank_value(int16_t bank);

byte* get_level_data_pointer();

template<typename T>
void patch_field_parameter(int id, T value)
{
	T* field_parameter_address = get_field_parameter_address<T>(id);

	if(!fieldPatchedAddress.contains(field_parameter_address))
		set_field_parameter<T>(id, value);

	fieldPatchedAddress.insert(field_parameter_address);
}

template<typename T>
void patch_generic_field_parameter(int id, byte frame_multiplier, bool is_multiplication)
{
	T field_parameter = get_field_parameter<T>(id);
	if(is_multiplication)
		patch_field_parameter<T>(id, field_parameter * frame_multiplier);
	else
		patch_field_parameter<T>(id, field_parameter / frame_multiplier);
}