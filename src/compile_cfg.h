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

/* 
 * RELEASE
 * 
 * Enables compile time checks to make sure no debugging options have been left
 * enabled and removes debug output.
 */
//#define RELEASE

/* 
 * PRERELEASE
 * 
 * Adds prerelease warning to applog and ingame. Only valid in conjunction with
 * RELEASE.
 */
//#define PRERELEASE

/* 
 * VERSION
 * 
 * Version string used in applog.
 */
//#define VERSION "0.8.1b"

/* 
 * SINGLE_STEP
 * 
 * Forces window mode in original resolution and places the renderer into
 * single step mode, in this mode double buffering is disabled and the result
 * of each render call is immediately flushed to the screen, allowing you
 * to set breakpoints in the rendering process and see exactly what has been
 * drawn up to that point.
 */
//#define SINGLE_STEP

/* 
 * HEAP_DEBUG
 * 
 * Trace and keep count of every allocation made by this program. Number of
 * allocations can be seen ingame through the show_stats option.
 */
//#define HEAP_DEBUG

/* 
 * NO_EXT_HEAP
 * 
 * Reroutes allocations from the game itself to our heap. Can be used with the
 * above option to also trace allocations from the game. The game WILL crash on
 * exit when this option is enabled. This is normal.
 */
//#define NO_EXT_HEAP

/* 
 * PROFILE
 * 
 * Enables profiling functions and profiling display through the show_stats
 * option. A running total of the time spent between PROFILE_START() and 
 * PROFILE_END() pairs will be displayed ingame.
 */
//#define PROFILE


// check for invalid combinations of options
#ifdef RELEASE
#ifdef SINGLE_STEP
#error
#endif
#ifdef HEAP_DEBUG
#error
#endif
#ifdef PROFILE
#error
#endif
#else
#ifdef PRERELEASE
#error
#endif
#endif

#ifdef PRERELEASE
#define PRERELEASE_WARNING " PRERELEASE DO NOT DISTRIBUTE"
#else
#define PRERELEASE_WARNING ""
#endif
