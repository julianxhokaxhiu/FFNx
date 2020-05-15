/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
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

#include "log.h"

uint replace_function(uint offset, void *func);
void unreplace_function(uint func);
void unreplace_functions();

void replace_call(uint offset, void *func);

uint get_relative_call(uint base, uint offset);
uint get_absolute_value(uint base, uint offset);
void patch_code_byte(uint offset, unsigned char r);
void patch_code_word(uint offset, word r);
void patch_code_dword(uint offset, DWORD r);
void patch_code_int(uint offset, int r);
void patch_code_uint(uint offset, uint r);
void patch_code_float(uint offset, float r);
void patch_code_double(uint offset, double r);
void memcpy_code(uint offset, void *data, uint size);
void memset_code(uint offset, uint val, uint size);
