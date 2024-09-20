/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

/**
 * Returns:
 *  -1 if the file was not found
 *   0 if the file is redirected
 *   1 if the file was not found but required
 */
int attempt_redirection(const char* in, char* out, size_t size, bool wantsSteamPath = false);
/**
 * Returns:
 *  -1 if the file was not found, in this case, `out` path is filled with `in` path content
 *  0  if the file is redirected
 *  1  if the file was not found but required
 */
int redirect_path_with_override(const char* in, char* out, size_t out_size);
