/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

uint32_t replace_function(uint32_t offset, void *func);
void unreplace_function(uint32_t func);
void unreplace_functions();

void replace_call(uint32_t offset, void *func);
uint32_t replace_call_function(uint32_t offset, void* func);

uint32_t get_relative_call(uint32_t base, uint32_t offset);
uint32_t get_absolute_value(uint32_t base, uint32_t offset);
void patch_code_byte(uint32_t offset, unsigned char r);
void patch_code_word(uint32_t offset, WORD r);
void patch_code_dword(uint32_t offset, DWORD r);
void patch_code_int(uint32_t offset, int r);
void patch_code_uint(uint32_t offset, uint32_t r);
void patch_code_float(uint32_t offset, float r);
void patch_code_double(uint32_t offset, double r);
void memcpy_code(uint32_t offset, void *data, uint32_t size);
void memset_code(uint32_t offset, uint32_t val, uint32_t size);
