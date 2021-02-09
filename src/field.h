/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2021 Julian Xhokaxhiu                                   //
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

void field_init();
void field_debug(bool *isOpen);

template<typename T>
T get_field_parameter(int id)
{
	return *(T*)(ff7_externals.field_array_1[*ff7_externals.current_entity_id] + *ff7_externals.field_ptr_1 + id + 1);
}

template<typename T>
void set_field_parameter(int id, T value)
{
	*(T*)(ff7_externals.field_array_1[*ff7_externals.current_entity_id] + *ff7_externals.field_ptr_1 + id + 1) = value;
}

byte get_field_bank_value(int16_t bank);
