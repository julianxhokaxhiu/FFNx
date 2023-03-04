/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
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

#include <stdint.h>

#include "../ff7.h"
#include "../log.h"

void ff7_condor_fix_unit_texture_load(uint32_t unk, struc_3 *struc_3)
{
  ff7_externals.make_struc3(unk, struc_3);

  // tell the game engine the files live inside the lgp file
  if ( *ff7_externals.condor_uses_lgp )
  {
    struc_3->file_context.use_lgp = 1;
    struc_3->file_context.lgp_num = 6;
    struc_3->file_context.name_mangler = 0;
  }
}