/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <cstdint>

struct game_obj;
struct nvertex;

void ff8_widescreen_hook_init();
void ff8_widescreen_adjust_vertices(uint32_t mode, uint32_t vertextype, struct nvertex *vertices, uint32_t vertexcount, uint16_t *indices, uint32_t count);
void ff8_widescreen_swirl_capture(uint32_t &x, uint32_t &w);
void ff8_clear(uint32_t clear_color, uint32_t clear_depth, uint32_t unknown, struct game_obj *game_object);
