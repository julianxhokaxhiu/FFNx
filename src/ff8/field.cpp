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

// Field: models in sync with movies, models drawn without rounding

#include "field.h"

#include "field/movie.h"
#include "field/precise_models.h"

void ff8_field_init()
{
    // Field scripts and models in sync with movies above 15 FPS
    ff8_field_movie_init();
    // Field models projected without rounding their camera space position
    ff8_field_precise_models_init();
}
