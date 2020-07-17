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

#include <ddraw.h>
#include <stdint.h>

void movie_init();
uint32_t ff7_prepare_movie(char *, uint32_t, struct dddevice **, uint32_t);
void ff7_release_movie_objects();
uint32_t ff7_start_movie();
uint32_t ff7_update_movie_sample(LPDIRECTDRAWSURFACE);
uint32_t ff7_stop_movie();
uint32_t ff7_get_movie_frame();
void draw_current_frame();
void ff8_prepare_movie(uint32_t disc, uint32_t movie);
void ff8_release_movie_objects();
void ff8_start_movie();
void ff8_update_movie_sample();
void ff8_stop_movie();
uint32_t ff8_get_movie_frame();
