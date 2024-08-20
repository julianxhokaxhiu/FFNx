/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
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

#include <stdint.h>

#include "../ff7.h"
#include "../globals.h"

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

void __fastcall ff7_snowboard_parse_model_vertices(snowboard_this* _this, void* edx, const matrix *matrix, int current_obj, int obj_type, int unk)
{
  vector3<float> vertex;
  point4d *point;
  tmd_object *tmd_obj;

  if ( current_obj >= 0 && current_obj < _this->num_objects )
  {
    point = (point4d *)ff7_externals.battle_sub_661000(0);
    tmd_obj = (tmd_object *)(&_this->model_data->tmdobjectlist + 7 * current_obj);
    ff7_externals.sub_733479((void*)*((DWORD*)*ff7_externals.snowboard_global_object_off_926290 + 0x9D), matrix);

    for (int32_t i = 0; i < tmd_obj->nverts; ++i )
    {
      if (_this->model_data->tmdheader.version == 0xFF)
      {
        vertex.x = ((tmd_vertex_float*)tmd_obj->offsetverts)[i].vx;
        vertex.y = ((tmd_vertex_float*)tmd_obj->offsetverts)[i].vy;
        vertex.z = ((tmd_vertex_float*)tmd_obj->offsetverts)[i].vz;
      }
      else
      {
        vertex.x = (float)((tmd_vertex*)tmd_obj->offsetverts)[i].vx;
        vertex.y = (float)((tmd_vertex*)tmd_obj->offsetverts)[i].vy;
        vertex.z = (float)((tmd_vertex*)tmd_obj->offsetverts)[i].vz;
      }

      ff7_externals.sub_733564((void*)*((DWORD*)*ff7_externals.snowboard_global_object_off_926290 + 0x9D), &vertex, &point[i + 1]);
    }

    switch ( obj_type )
    {
      case 0:
        ff7_externals.sub_7322D6((tmd_primitive_packet*)tmd_obj->offsetprimitives, tmd_obj->nprimitives, unk);
        break;
      case 1:
        ff7_externals.sub_732429((tmd_primitive_packet*)tmd_obj->offsetprimitives, tmd_obj->nprimitives, unk);
        break;
      case 2:
        ff7_externals.sub_732BB9((tmd_primitive_packet*)tmd_obj->offsetprimitives, tmd_obj->nprimitives, unk);
        break;
      case 3:
        ff7_externals.sub_732546(_this, (tmd_primitive_packet*)tmd_obj->offsetprimitives, tmd_obj->nprimitives, unk);
        break;
      default:
        return;
    }
  }
}
