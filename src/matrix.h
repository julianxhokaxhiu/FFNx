/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#define DEG2RAD(X) ((X) * (M_PI/180.0f))

struct matrix
{
	union
	{
		struct
		{
			float _11;
			float _12;
			float _13;
			float _14;
			float _21;
			float _22;
			float _23;
			float _24;
			float _31;
			float _32;
			float _33;
			float _34;
			float _41;
			float _42;
			float _43;
			float _44;
		};

		float m[4][4];
	};
};

template <typename T>
struct vector2
{
	T x;
	T y;
};

template <typename T>
struct vector3
{
	T x;
	T y;
	T z;
};

struct point4d
{
	float x;
	float y;
	float z;
	float w;
};

void add_vector(vector3<float> *a, vector3<float> *b, vector3<float> *dest);
void subtract_vector(vector3<float> *a, vector3<float> *b, vector3<float> *dest);
void multiply_vector(vector3<float> *vector, float scalar, vector3<float> *dest);
void divide_vector(vector3<float> *vector, float scalar, vector3<float> *dest);
float vector_length(vector3<float> *vector);
void normalize_vector(vector3<float> *vector);
float dot_product(vector3<float> *a, vector3<float> *b);
void cross_product(vector3<float> *a, vector3<float> *b, vector3<float> *dest);
void transform_point(struct matrix *matrix, vector3<float> *point, vector3<float> *dest);
void transform_point_w(struct matrix *matrix, vector3<float> *point, struct point4d *dest);
void transform_point4d(struct matrix *matrix, struct point4d *point, struct point4d *dest);
void transpose_matrix(struct matrix *matrix, struct matrix *dest);
void multiply_matrix(struct matrix *a, struct matrix *b, struct matrix *dest);
void multiply_matrix_unary(struct matrix *a, struct matrix *b);
void identity_matrix(struct matrix *matrix);
void uniform_scaling_matrix(float scale, struct matrix *matrix);
void scaling_matrix(vector3<float> *scale, struct matrix *matrix);
void rotation_matrix_x(float angle, struct matrix *matrix);
void rotation_matrix_y(float angle, struct matrix *matrix);
void rotation_matrix_z(float angle, struct matrix *matrix);
void rotate_matrix_x(float angle, struct matrix *matrix);
void rotate_matrix_y(float angle, struct matrix *matrix);
void rotate_matrix_z(float angle, struct matrix *matrix);
void inverse_matrix(struct matrix *matrix, struct matrix *dest);
