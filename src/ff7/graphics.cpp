/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Cosmos                                             //
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

#define _USE_MATH_DEFINES
#include <math.h>
#include "../renderer.h"

#include "../ff7.h"
#include "../macro.h"
#include "../log.h"
#include "../gl.h"

/*
 * Most of these functions are lifted from the game with only minor changes to
 * replace Direct3D calls. Modifying or even attempting to understand most of
 * this code is not recommended.
 */

void destroy_d3d2_indexed_primitive(struct indexed_primitive *ip)
{
	if(!ip) return;

	if(ip->vertices) external_free(ip->vertices);
	if(ip->indices) external_free(ip->indices);

	external_free(ip);
}

uint32_t ff7gl_load_group(uint32_t group_num, struct matrix_set *matrix_set, struct p_hundred *_hundred_data, struct p_group *_group_data, struct polygon_data *polygon_data, struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object)
{
	struct indexed_primitive *ip;
	struct p_hundred *hundred_data;
	struct p_group *group_data;
	uint32_t numvert;
	uint32_t numpoly;
	uint32_t offvert;
	uint32_t offpoly;
	uint32_t offtex;
	uint32_t i;

	if(!polygon_data) return false;
	if(!polygon_set->indexed_primitives) return false;
	if(group_num >= polygon_data->numgroups) return false;

	ip = (indexed_primitive*)external_calloc(sizeof(*ip), 1);

	ip->primitivetype = RendererPrimitiveType::PT_TRIANGLES;
	ip->vertex_size = sizeof(struct nvertex);

	group_data = &polygon_data->groupdata[group_num];
	hundred_data = &polygon_data->hundredsdata[group_num];
	numvert = group_data->numvert;
	numpoly = group_data->numpoly;
	offvert = group_data->offvert;
	offpoly = group_data->offpoly;
	offtex = group_data->offtex;

	ip->vertexcount = numvert;
	ip->indexcount = numpoly * 3;
	ip->vertices = (nvertex*)external_calloc(sizeof(*ip->vertices), ip->vertexcount);
	ip->indices = (WORD*)external_calloc(sizeof(*ip->indices), ip->indexcount);

	if(polygon_data->vertextype == 0) ip->vertextype = VERTEX;
	else if(polygon_data->vertextype == 1) ip->vertextype = LVERTEX;
	else if(polygon_data->vertextype == 2) ip->vertextype = TLVERTEX;

	for(i = 0; i < numvert; i++)
	{
		if(polygon_data->vertdata) memcpy(&ip->vertices[i]._, &polygon_data->vertdata[offvert + i], sizeof(ip->vertices[i]._));
		if(polygon_data->vertexcolordata) memcpy(&ip->vertices[i].color.color, &polygon_data->vertexcolordata[offvert + i], sizeof(ip->vertices[i].color.color));
		if(group_data->textured && polygon_data->texcoorddata) memcpy(&ip->vertices[i].u, &polygon_data->texcoorddata[offtex + i], sizeof(polygon_data->texcoorddata[i]));

		if(hundred_data && (hundred_data->field_4 & BIT(V_ALPHABLEND))) ip->vertices[i].color.a = hundred_data->vertex_alpha;
	}

	for(i = 0; i < numpoly; i++)
	{
		if(polygon_data->polydata)
		{
			struct p_polygon *poly_data = &polygon_data->polydata[offpoly + i];

			ip->indices[i * 3] = poly_data->vertex1;
			ip->indices[i * 3 + 1] = poly_data->vertex2;
			ip->indices[i * 3 + 2] = poly_data->vertex3;
		}
	}

	polygon_set->indexed_primitives[group_num] = ip;

	return true;
}

void ff7gl_field_78(struct ff7_polygon_set *polygon_set, struct ff7_game_obj *game_object)
{
	struct matrix_set *matrix_set;
	struct struc_49 *struc_49;
	struct p_hundred *hundred_data = NULL;
	struct p_group *group_data = NULL;
	struct matrix *matrix = NULL;
	struct struc_77 *struc_77;
	struct indexed_primitive *ip = NULL;
	struct nvertex *vertices = NULL;
	struct struc_84 *struc_84;
	struct struc_186 *struc_186;
	uint32_t instance_type = -1;
	uint32_t group_counter = 0;
	uint32_t instanced = false;
	uint32_t correct_frame = false;
	uint32_t instance_transform_mode;
	struct matrix tmp_matrix;
	struct matrix *model_matrix = 0;

	if(trace_all) ffnx_trace("dll_gfx: field_78 0x%x\n", polygon_set);

	if(!game_object->in_scene) return;

	if(!polygon_set) return;

	if(!polygon_set->field_0) return;

	if(trace_all) ffnx_trace("field_78: %s (%i groups) (0x%x)\n", polygon_set->polygon_data ? polygon_set->polygon_data->pc_name : "unknown", polygon_set->numgroups, polygon_set);

	matrix_set = polygon_set->matrix_set;

	if(matrix_set) model_matrix = matrix_set->matrix_view;

	struc_49 = &polygon_set->field_14;

	if(struc_49->field_0)
	{
		instanced = true;

		correct_frame = (struc_49->graphics_instance->frame_counter == struc_49->frame_counter);

		instance_transform_mode = struc_49->field_8;
		struc_84 = struc_49->struc_84;

		if(struc_84) instance_type = struc_84->field_4;

		if(trace_all) ffnx_trace("instanced, %s, type %i, transform %i\n", correct_frame ? "correct frame" : "wrong frame", instance_type, instance_transform_mode);
	}

	if(polygon_set->field_2C) hundred_data = polygon_set->hundred_data;
	if(polygon_set->polygon_data) group_data = polygon_set->polygon_data->groupdata;

	while(group_counter < polygon_set->numgroups)
	{
		uint32_t defer = false;
		uint32_t zsort = false;

		struc_84 = struc_49->struc_84;

		if(polygon_set->indexed_primitives) ip = polygon_set->indexed_primitives[group_counter];

		if(ip) vertices = ip->vertices;

		if(polygon_set->per_group_hundreds) hundred_data = polygon_set->hundred_data_group_array[group_counter];

		if(hundred_data)
		{
			if(game_object->field_91C && hundred_data->zsort) zsort = true;
			else if(!game_object->field_928) defer = (hundred_data->options & (BIT(V_ALPHABLEND) | BIT(V_TMAPBLEND)));
		}

		if(trace_all) ffnx_trace("group %i: %s, %s\n", group_counter, zsort ? "zsort" : "no zsort", defer ? "deferred" : "not deferred");

		if(!defer) common_setrenderstate(hundred_data, (struct game_obj *)game_object);

		if(matrix_set && matrix_set->matrix_projection) gl_set_d3dprojection_matrix(matrix_set->matrix_projection);

		if(instanced)
		{
			if(correct_frame)
			{
				while(struc_84)
				{
					if(trace_all) ffnx_trace("drawing instance 0x%x\n", struc_84);

					if(instance_type == 2)
					{
						if(instance_transform_mode == 1) matrix = &struc_84->matrix;
						else if(instance_transform_mode == 2)
						{
							multiply_matrix(&struc_84->matrix, game_object->camera_matrix, &tmp_matrix);
							matrix = &tmp_matrix;
						}

						if(matrix && matrix_set && !zsort) gl_set_worldview_matrix(matrix);

						struc_186 = struc_84->struc_186;

						if(struc_186->polytype == 0x11) vertices = struc_186->nvertex_pointer;
						else ff7_externals.sub_671742(zsort, hundred_data, struc_186);

						if(zsort) ff7_externals.sub_665D9A(matrix, vertices, ip, hundred_data, struc_186, game_object);
						else gl_draw_indexed_primitive(ip->primitivetype, ip->vertextype, vertices, 0, ip->vertexcount, ip->indices, ip->indexcount, 0, 0, 0, polygon_set->field_4, true);
					}
					else if(defer)
					{
						struc_77 = (struct struc_77*)ff7_externals.sub_6A2865(game_object->_3dobject_pool);

						if(struc_77)
						{
							struc_77->current_group = group_counter;
							struc_77->polygon_set = (struct polygon_set *)polygon_set;
							struc_77->hundred_data = hundred_data;

							if(polygon_set->has_struc_173) memcpy(&struc_77->struc_173, polygon_set->struc_173, sizeof(*polygon_set->struc_173));

							struc_77->use_matrix = 0;
							struc_77->use_matrix_pointer = 0;

							if(instance_transform_mode == 1)
							{
								struc_77->use_matrix_pointer = 1;
								struc_77->matrix_pointer = &struc_84->matrix;
							}

							else if(instance_transform_mode == 2)
							{
								struc_77->use_matrix = 1;
								multiply_matrix(&struc_84->matrix, game_object->camera_matrix, &struc_77->matrix);
								ff7_externals.matrix3x4(&struc_77->matrix);
							}
						}
					}
					else if(instance_type == 0)
					{
						if(instance_transform_mode == 1) matrix = &struc_84->matrix;
						else if(instance_transform_mode == 2)
						{
							multiply_matrix(&struc_84->matrix, game_object->camera_matrix, &tmp_matrix);
							matrix = &tmp_matrix;
						}

						if(ip)
						{
							if(zsort) ff7_externals.sub_665793(matrix, 0, ip, polygon_set, hundred_data, group_data, game_object);
							else
							{
								if(matrix && matrix_set) gl_set_worldview_matrix(matrix);
								gl_draw_without_lighting(ip, polygon_set->polygon_data, nullptr, polygon_set->field_4);
							}
						}
					}
					else if(instance_type == 1)
					{
						if(instance_transform_mode == 1) matrix = &struc_84->matrix;
						else if(instance_transform_mode == 2)
						{
							multiply_matrix(&struc_84->matrix, game_object->camera_matrix, &tmp_matrix);
							matrix = &tmp_matrix;
						}

						if(ip)
						{
							if(struc_84->struc_173.add_offsets || struc_84->struc_173.field_7)
							{
								ff7_externals.sub_68D2B8(group_counter, polygon_set, &struc_84->struc_173);
								ff7_externals.sub_6B27A9(matrix, ip, polygon_set, hundred_data, group_data, &struc_84->struc_173, game_object);
							}
							else
							{
								if(zsort) ff7_externals.sub_665793(matrix, 0, ip, polygon_set, hundred_data, group_data, game_object);
								else
								{
									ff7_externals.sub_68D2B8(group_counter, polygon_set, &struc_84->struc_173);

									if(matrix && matrix_set) gl_set_worldview_matrix(matrix);
									gl_draw_without_lighting(ip, polygon_set->polygon_data, nullptr, polygon_set->field_4);
								}
							}
						}
					}

					struc_84 = struc_84->next;
				}
			}
		}
		else
		{
			if(trace_all) ffnx_trace("drawing single\n");

			if(defer)
			{
				struc_77 = (struct struc_77*)ff7_externals.sub_6A2865(game_object->_3dobject_pool);

				struc_77->current_group = group_counter;
				struc_77->polygon_set = (struct polygon_set *)polygon_set;
				struc_77->hundred_data = hundred_data;

				if(polygon_set->has_struc_173) memcpy(&struc_77->struc_173, polygon_set->struc_173, sizeof(*polygon_set->struc_173));

				struc_77->use_matrix = 0;
				struc_77->use_matrix_pointer = 0;

				if(matrix_set && matrix_set->matrix_world)
				{
					struc_77->use_matrix = 1;
					memcpy(&struc_77->matrix, matrix_set->matrix_world, sizeof(*matrix_set->matrix_world));
				}
			}
			else
			{
				if(ip)
				{
					if(zsort) ff7_externals.sub_665793(matrix_set->matrix_world, 0, ip, polygon_set, hundred_data, group_data, game_object);
					else
					{
						if (matrix_set)	gl_set_worldview_matrix(matrix_set->matrix_world);
						if (enable_lighting)
						{					
							update_view_matrix(game_object);		
							if(polygon_set->light != nullptr && game_lighting != GAME_LIGHTING_ORIGINAL)
							{
								struct light_data lightData;
								fill_light_data(&lightData, polygon_set);
								gl_draw_with_lighting(ip, polygon_set->polygon_data, &lightData, polygon_set->field_4);
							} else 	gl_draw_with_lighting(ip, polygon_set->polygon_data, nullptr, polygon_set->field_4);						
						} else 
						{
							if(polygon_set->light != nullptr && game_lighting != GAME_LIGHTING_ORIGINAL)
							{
								struct light_data lightData;
								fill_light_data(&lightData, polygon_set);
								update_view_matrix(game_object);
								gl_draw_without_lighting(ip, polygon_set->polygon_data, &lightData, polygon_set->field_4);
							} else gl_draw_without_lighting(ip, polygon_set->polygon_data, nullptr, polygon_set->field_4);
						}
					}
				}
			}
		}

		if(hundred_data) hundred_data = &hundred_data[1];
		if(group_data) group_data = &group_data[1];

		group_counter++;
	}
}

struct tex_header *sub_673F5C(struct struc_91 *struc91)
{
	if(trace_all) ffnx_trace("sub_673F5C: 0x%x\n", struc91);

	if(struc91->field_0 == 2) ffnx_glitch_once("unsupported framebuffer operation\n");

	return make_framebuffer_tex(struc91->width, struc91->height, struc91->x_offset, struc91->y_offset, struc91->width * struc91->xscale, struc91->height * struc91->yscale, struc91->color_key);
}

void draw_single_triangle(struct nvertex *vertices)
{
	WORD indices[] = {0, 1, 2};

	newRenderer.bindVertexBuffer(vertices, 0, 3);
	newRenderer.bindIndexBuffer(indices, 3);

	newRenderer.isTLVertex(true);

	newRenderer.draw();
}

void sub_6B2720(struct indexed_primitive *ip)
{
	gl_draw_indexed_primitive(ip->primitivetype, TLVERTEX, ip->vertices, 0, ip->vertexcount, ip->indices, ip->indexcount, 0, 0, 0, true, true);
}

void draw_3d_model(uint32_t current_frame, struct anim_header *anim_header, struct struc_110 *struc_110, struct hrc_data *hrc_data, struct ff7_game_obj *game_object)
{
	struct anim_frame *anim_frame;
	struct stack *matrix_stack;
	struct matrix *root_matrix;
	void (*root_animation_sub)(struct matrix *, struct anim_frame *, struct anim_header *, struct hrc_data *);
	void (*frame_animation_sub)(uint32_t, struct matrix *, vector3<float> *, struct anim_frame *, struct anim_header *, struct hrc_bone *, struct hrc_data *);

	if(!anim_header) return;
	if(!hrc_data) return;
	if(current_frame >= anim_header->num_frames) return;

	anim_frame = &anim_header->anim_frames[current_frame];

	if(anim_header->use_matrix_array)
	{
		anim_header->current_matrix_array = &anim_header->matrix_array[(anim_header->num_bones + 1) * current_frame];
	}

	matrix_stack = game_object->matrix_stack1;

	ff7_externals.stack_push(matrix_stack);

	root_matrix = (matrix*)ff7_externals.stack_top(matrix_stack);

	if(hrc_data->field_A4 && *hrc_data->field_A4)
	{
		root_animation_sub = ff7_externals._root_animation;
		frame_animation_sub = ff7_externals._frame_animation;
	}
	else
	{
		root_animation_sub = ff7_externals.root_animation;
		frame_animation_sub = ff7_externals.frame_animation;
	}

	root_animation_sub(root_matrix, anim_frame, anim_header, hrc_data);

	if(hrc_data->flags & 0x400) memcpy(&hrc_data->field_24, root_matrix, sizeof(*root_matrix));

	if(struc_110)
	{
		struct matrix scale_matrix;

		if(struc_110->scale_factor != 1.0f)
		{
			float scale_factor = struc_110->scale_factor;

			root_matrix->_41 *= scale_factor;
			root_matrix->_42 *= scale_factor;
			root_matrix->_43 *= scale_factor;

			uniform_scaling_matrix(scale_factor, &scale_matrix);
			multiply_matrix_unary(root_matrix, &scale_matrix);
		}

		if(struc_110->scale.x != 1.0f || struc_110->scale.y != 1.0f || struc_110->scale.z != 1.0f)
		{
			scaling_matrix(&struc_110->scale, &scale_matrix);
			multiply_matrix_unary(root_matrix, &scale_matrix);

			root_matrix->_41 *= struc_110->scale.x;
			root_matrix->_42 *= struc_110->scale.y;
			root_matrix->_43 *= struc_110->scale.z;
		}

		if(*ff7_externals.model_mode & MDL_USE_STRUC110_MATRIX) multiply_matrix_unary(root_matrix, &struc_110->matrix);

		if(struc_110->rotation.y != 0.0) rotate_matrix_y(DEG2RAD(struc_110->rotation.y), root_matrix);
		if(struc_110->rotation.x != 0.0) rotate_matrix_x(DEG2RAD(struc_110->rotation.x), root_matrix);
		if(struc_110->rotation.z != 0.0) rotate_matrix_z(DEG2RAD(struc_110->rotation.z), root_matrix);

		root_matrix->_41 += struc_110->position.x;
		root_matrix->_42 += struc_110->position.y;
		root_matrix->_43 += struc_110->position.z;
	}

	if(hrc_data->flags & 0x800) memcpy(&hrc_data->field_64, root_matrix, sizeof(*root_matrix));

	if(hrc_data->flags & 0x2000 && struc_110->bone_positions)
	{
		struc_110->bone_positions[0].x = root_matrix->_41;
		struc_110->bone_positions[0].y = root_matrix->_42;
		struc_110->bone_positions[0].z = root_matrix->_43;
	}

	if(hrc_data->bone_list)
	{
		struct list_node *bone_list_node;

		LIST_FOR_EACH(bone_list_node, hrc_data->bone_list)
		{
			struct bone_list_member *bone_list_member = (struct bone_list_member *)&bone_list_node->object;

			if(bone_list_member->bone_type == 1)
			{
				uint32_t bone_index = bone_list_member->bone_index;
				struct hrc_bone *bone = &hrc_data->bones[bone_index];
				struct matrix *parent_matrix;
				struct matrix *bone_matrix;
				vector3<float> *frame_rotation;
				struct matrix local_matrix;
				struct matrix eye_matrix;
				struct matrix *matrix;
				vector3<float> dummy_point = {0.0f, 0.0f, 0.0f};

				parent_matrix = (struct matrix*)ff7_externals.stack_top(matrix_stack);
				ff7_externals.stack_push(matrix_stack);
				bone_matrix = (struct matrix*)ff7_externals.stack_top(matrix_stack);

				if(anim_header->num_bones <= bone_index) frame_rotation = &dummy_point;
				else frame_rotation = &anim_frame->data[bone_index];

				frame_animation_sub(bone_index, &local_matrix, frame_rotation, anim_frame, anim_header, bone, hrc_data);

				multiply_matrix(&local_matrix, parent_matrix, bone_matrix);

				if(*ff7_externals.model_mode & MDL_USE_CAMERA_MATRIX)
				{
					if(hrc_data->flags & 0x4000 && struc_110->bone_matrices) matrix = &struc_110->bone_matrices[bone_index + 1];
					else matrix = &eye_matrix;

					multiply_matrix(bone_matrix, game_object->camera_matrix, matrix);

					matrix->_14 = 0.0f;
					matrix->_24 = 0.0f;
					matrix->_34 = 0.0f;
					matrix->_44 = 1.0f;

					if(hrc_data->flags & 0x2000 && struc_110->bone_positions)
					{
						struc_110->bone_positions[bone_index + 1].x = bone_matrix->_41;
						struc_110->bone_positions[bone_index + 1].y = bone_matrix->_42;
						struc_110->bone_positions[bone_index + 1].z = bone_matrix->_43;
					}
				}
				else matrix = bone_matrix;

				if(bone->rsd_array)
				{
					uint32_t i;
					struct rsd_array_member *rsd_array_member;

					for(i = 0, rsd_array_member = bone->rsd_array; i < bone->num_rsd; i++, rsd_array_member++)
					{
						struct ff7_polygon_set *polygon_set;

						if(!rsd_array_member->rsd_data) continue;

						polygon_set = rsd_array_member->rsd_data->polygon_set;

						if(!polygon_set) continue;

						common_setmatrix(0, matrix, polygon_set->matrix_set, (struct game_obj *)game_object);
						if(polygon_set->matrix_set) polygon_set->matrix_set->matrix_view = (struct matrix*)external_calloc(sizeof(struct matrix), 1);
						common_setmatrix(1, bone_matrix, polygon_set->matrix_set, (struct game_obj *)game_object);

						if(hrc_data->flags & 0x2000000)
						{
							struct ff7_light *light = polygon_set->light;

							if(light)
							{
								if(polygon_set->matrix_set) light->matrix_pointer = polygon_set->matrix_set->matrix_world;
								else light->matrix_pointer = 0;

								if(light->field_138)
								{
									struct matrix tmp;

									multiply_matrix(bone_matrix, &light->normal_matrix, &tmp);

									ff7_externals.sub_69C69F(&tmp, light);
								}
								else ff7_externals.sub_69C69F(bone_matrix, light);

								common_externals.generic_light_polygon_set((struct polygon_set *)polygon_set, (struct light *)light);
							}
						}

						if(hrc_data->field_4 && hrc_data->flags & 0x100000) ff7gl_field_78(polygon_set, game_object);
					}
				}
			}
			if(bone_list_member->bone_type == 2) ff7_externals.stack_pop(matrix_stack);
		}
	}

	ff7_externals.stack_pop(matrix_stack);
}

void fill_light_data(struct light_data* lightData, struct ff7_polygon_set *polygon_set)
{
	lightData->global_light_color = polygon_set->light->global_light_color_abgr_norm;
	lightData->light_dir_1 = polygon_set->light->color_1->point;
	lightData->light_color_1 = polygon_set->light->color_1->d3dcol;
	lightData->light_dir_2 = polygon_set->light->color_2->point;
	lightData->light_color_2 = polygon_set->light->color_2->d3dcol;
	lightData->light_dir_3 = polygon_set->light->color_3->point;
	lightData->light_color_3 = polygon_set->light->color_3->d3dcol;

	lightData->scripted_light_color = {1.0, 1.0, 1.0, 1.0};

	if ((polygon_set->light->flags & 4) != 0)
	{
		lightData->scripted_light_color.r = polygon_set->light->color.r / 255.0f;
		lightData->scripted_light_color.g = polygon_set->light->color.g / 255.0f;
		lightData->scripted_light_color.b = polygon_set->light->color.b / 255.0f;
	}
}

void ff7_get_field_view_matrix(struct matrix *outViewMatrix)
{
	struct matrix viewMatrix;
	identity_matrix(&viewMatrix);

	byte *level_data = *ff7_externals.field_level_data_pointer;
	if (!level_data)
	{
		return;
	}

	ff7_camdata *field_camera_data = *ff7_externals.field_camera_data;
	if (!field_camera_data)
	{
		return;
	}

	vector3<float> vx = {(float)(field_camera_data->eye.x), (float)(field_camera_data->eye.y), (float)(field_camera_data->eye.z)};
	vector3<float> vy = {(float)(field_camera_data->target.x), (float)(field_camera_data->target.y), (float)(field_camera_data->target.z)};
	vector3<float> vz = {(float)(field_camera_data->up.x), (float)(field_camera_data->up.y), (float)(field_camera_data->up.z)};

	divide_vector(&vx, 4096.0f, &vx);
	divide_vector(&vy, 4096.0f, &vy);
	divide_vector(&vz, 4096.0f, &vz);

	float ox = static_cast<float>(field_camera_data->position.x);
	float oy = static_cast<float>(field_camera_data->position.y);
	float oz = static_cast<float>(field_camera_data->position.z);

	float tx = ox;
	float ty = oy;
	float tz = oz;

	viewMatrix._11 = vx.x;
	viewMatrix._21 = vx.y;
	viewMatrix._31 = vx.z;
	viewMatrix._12 = vy.x;
	viewMatrix._22 = vy.y;
	viewMatrix._32 = vy.z;
	viewMatrix._13 = vz.x;
	viewMatrix._23 = vz.y;
	viewMatrix._33 = vz.z;
	viewMatrix._41 = tx;
	viewMatrix._42 = ty;
	viewMatrix._43 = tz;
	viewMatrix._44 = 1.0;

	memcpy(outViewMatrix, &viewMatrix, sizeof(matrix));
}

void update_view_matrix(struct ff7_game_obj *game_object)
{
	if (newRenderer.isViewMatrixSet()) return;

	struct game_mode *mode = getmode_cached();

	struct matrix viewMatrix;
	struct matrix *pViewMatrix = &viewMatrix;

	switch(mode->driver_mode)
	{
		case MODE_FIELD:
			// Get Field view matrix
			ff7_get_field_view_matrix(&viewMatrix);
			newRenderer.setViewMatrix(&viewMatrix);
			break;
		case MODE_WORLDMAP:
		{
			if (enable_worldmap_external_mesh)
			{
				int world_pos_x = ff7_externals.world_player_pos_E04918->x;
				int world_pos_y = ff7_externals.world_player_pos_E04918->y;
				int world_pos_z = ff7_externals.world_player_pos_E04918->z;

				auto rot_matrix = ff7_externals.world_camera_direction_matrix_DFC448;
				auto tr_matrix = ff7_externals.world_camera_position_matrix_DE6A20;

				float cameraRotationMatrixFloat[16];

				cameraRotationMatrixFloat[0] = rot_matrix->r3_sub_matrix[0][0] / 4096.0f;
				cameraRotationMatrixFloat[1] = rot_matrix->r3_sub_matrix[0][1] / 4096.0f;
				cameraRotationMatrixFloat[2] = rot_matrix->r3_sub_matrix[0][2] / 4096.0f;
				cameraRotationMatrixFloat[3] = 0.0f;

				cameraRotationMatrixFloat[4] = rot_matrix->r3_sub_matrix[1][0] / 4096.0f;
				cameraRotationMatrixFloat[5] = rot_matrix->r3_sub_matrix[1][1] / 4096.0f;
				cameraRotationMatrixFloat[6] = rot_matrix->r3_sub_matrix[1][2] / 4096.0f;
				cameraRotationMatrixFloat[7] = 0.0f;

				cameraRotationMatrixFloat[8] = rot_matrix->r3_sub_matrix[2][0] / 4096.0f;
				cameraRotationMatrixFloat[9] = rot_matrix->r3_sub_matrix[2][1] / 4096.0f;
				cameraRotationMatrixFloat[10] = rot_matrix->r3_sub_matrix[2][2] / 4096.0f;
				cameraRotationMatrixFloat[11] = 0.0f;

				cameraRotationMatrixFloat[12] = 0.0f;
				cameraRotationMatrixFloat[13] = 0.0f;
				cameraRotationMatrixFloat[14] = 0.0f;
				cameraRotationMatrixFloat[15] = 1.0f;

				float cameraTranslationMatrixFloat[16];
				bx::mtxTranslate(cameraTranslationMatrixFloat, 0, 0, -tr_matrix->position[2]);

				float cameraTranslationMatrixFloat2[16];
				bx::mtxTranslate(cameraTranslationMatrixFloat2, world_pos_x, world_pos_y, world_pos_z);

				float tmp[16];
				bx::mtxMul(tmp, cameraTranslationMatrixFloat, cameraRotationMatrixFloat);

				float cameraMatrixFloat[16];
				bx::mtxMul(cameraMatrixFloat, tmp, cameraTranslationMatrixFloat2);

				float viewMatrixFloat[16];
				bx::mtxInverse(viewMatrixFloat, cameraMatrixFloat);

				struct matrix viewMatrix;
				::memcpy(&viewMatrix.m[0][0], viewMatrixFloat, sizeof(viewMatrix.m));

				newRenderer.setViewMatrix(&viewMatrix);
			}
			else
			{
				pViewMatrix = game_object->camera_matrix;
				if (pViewMatrix)
				{
					newRenderer.setViewMatrix(pViewMatrix);
				}
			}
		}
			break;
		default:
			pViewMatrix = game_object->camera_matrix;
			if (pViewMatrix)
			{
				newRenderer.setViewMatrix(pViewMatrix);
			}
			break;
	}
};
