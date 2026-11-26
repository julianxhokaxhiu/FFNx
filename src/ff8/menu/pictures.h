/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
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

// AddMorePictures: lets the magazines and tutorial books show new pictures. A page whose picture
// is mngrp.bin slot 185-255 shows menu/magNNN.TEX (NNN = the slot) when a mod provides that file
// in the direct folder; the slot itself must hold a TIM in mngrp.bin, as the game's pictures do.
// Inert without such a file. Call once from ff8_init_hooks().
void ff8_menu_pictures_init();
