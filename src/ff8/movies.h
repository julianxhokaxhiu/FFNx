/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 myst6re                                            //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

void *ff8_bink_open(uint8_t disc, uint32_t movie);
int ff8_bink_read(void *opaque, uint8_t *buf, int buf_size);
int64_t ff8_bink_seek(void *opaque, int64_t offset, int whence);
void ff8_bink_close(void *opaque);

void *ff8_zzz_open(const char *fmv_name);
int ff8_zzz_read(void *opaque, uint8_t *buf, int buf_size);
int64_t ff8_zzz_seek(void *opaque, int64_t offset, int whence);
void ff8_zzz_close(void *opaque);

