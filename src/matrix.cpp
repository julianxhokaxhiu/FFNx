/* 
 * FFNx - Complete OpenGL replacement of the Direct3D renderer used in 
 * the original ports of Final Fantasy VII and Final Fantasy VIII for the PC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * matrix.c - basic matrix and vector math
 */

#include <string.h>
#include <math.h>

#include "matrix.h"
#include "math.h"
#include "log.h"

void add_vector(struct point3d *a, struct point3d *b, struct point3d *dest)
{
	dest->x = a->x + b->x;
	dest->y = a->y + b->y;
	dest->z = a->z + b->z;
}

void subtract_vector(struct point3d *a, struct point3d *b, struct point3d *dest)
{
	dest->x = a->x - b->x;
	dest->y = a->y - b->y;
	dest->z = a->z - b->z;
}

void multiply_vector(struct point3d *vector, float scalar, struct point3d *dest)
{
	dest->x = vector->x * scalar;
	dest->y = vector->y * scalar;
	dest->z = vector->z * scalar;
}

void divide_vector(struct point3d *vector, float scalar, struct point3d *dest)
{
	dest->x = vector->x / scalar;
	dest->y = vector->y / scalar;
	dest->z = vector->z / scalar;
}

float vector_length(struct point3d *vector)
{
	return sqrtf(vector->x * vector->x + vector->y * vector->y + vector->z * vector->z);
}

void normalize_vector(struct point3d *vector)
{
	float length = vector_length(vector);

	divide_vector(vector, length, vector);
}

float dot_product(struct point3d *a, struct point3d *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

void cross_product(struct point3d *a, struct point3d *b, struct point3d *dest)
{
	dest->x = a->y * b->z - a->z * b->y;
	dest->y = a->z * b->x - a->x * b->z;
	dest->z = a->x * b->y - a->y * b->x;
}

void transform_point(struct matrix *matrix, struct point3d *point, struct point3d *dest)
{
	dest->x = matrix->_11 * point->x + matrix->_21 * point->y + matrix->_31 * point->z + matrix->_41;
	dest->y = matrix->_12 * point->x + matrix->_22 * point->y + matrix->_32 * point->z + matrix->_42;
	dest->z = matrix->_13 * point->x + matrix->_23 * point->y + matrix->_33 * point->z + matrix->_43;
}

void transform_point_w(struct matrix *matrix, struct point3d *point, struct point4d *dest)
{
	dest->x = matrix->_11 * point->x + matrix->_21 * point->y + matrix->_31 * point->z + matrix->_41;
	dest->y = matrix->_12 * point->x + matrix->_22 * point->y + matrix->_32 * point->z + matrix->_42;
	dest->z = matrix->_13 * point->x + matrix->_23 * point->y + matrix->_33 * point->z + matrix->_43;
	dest->w = matrix->_14 * point->x + matrix->_24 * point->y + matrix->_34 * point->z + matrix->_44;
}

void transform_point4d(struct matrix *matrix, struct point4d *point, struct point4d *dest)
{
	dest->x = matrix->_11 * point->x + matrix->_21 * point->y + matrix->_31 * point->z + matrix->_41 * point->w;
	dest->y = matrix->_12 * point->x + matrix->_22 * point->y + matrix->_32 * point->z + matrix->_42 * point->w;
	dest->z = matrix->_13 * point->x + matrix->_23 * point->y + matrix->_33 * point->z + matrix->_43 * point->w;
	dest->w = matrix->_14 * point->x + matrix->_24 * point->y + matrix->_34 * point->z + matrix->_44 * point->w;
}

void transpose_matrix(struct matrix *matrix, struct matrix *dest)
{
	dest->_11 = matrix->_11;
	dest->_12 = matrix->_21;
	dest->_13 = matrix->_31;
	dest->_14 = matrix->_41;
	dest->_21 = matrix->_12;
	dest->_22 = matrix->_22;
	dest->_23 = matrix->_32;
	dest->_24 = matrix->_42;
	dest->_31 = matrix->_13;
	dest->_32 = matrix->_23;
	dest->_33 = matrix->_33;
	dest->_34 = matrix->_43;
	dest->_41 = matrix->_14;
	dest->_42 = matrix->_24;
	dest->_43 = matrix->_34;
	dest->_44 = matrix->_44;
}

void multiply_matrix(struct matrix *a, struct matrix *b, struct matrix *dest)
{

#define MMUL(I, J, N) a->m[I - 1][N - 1] * b->m[N - 1][J - 1]
#define MMUL1(I, J) dest->m[I - 1][J - 1] = MMUL(I, J, 1) + MMUL(I, J, 2) + MMUL(I, J, 3) + MMUL(I, J, 4)
#define MMULROW(I) MMUL1(I, 1); MMUL1(I, 2); MMUL1(I, 3); MMUL1(I, 4)

	MMULROW(1);
	MMULROW(2);
	MMULROW(3);
	MMULROW(4);
}

void multiply_matrix_unary(struct matrix *a, struct matrix *b)
{
	struct matrix tmp;

	memcpy(&tmp, a, sizeof(tmp));
	multiply_matrix(&tmp, b, a);
}

void identity_matrix(struct matrix *matrix)
{
	matrix->_11 = 1.0f;
	matrix->_12 = 0.0f;
	matrix->_13 = 0.0f;
	matrix->_14 = 0.0f;
	matrix->_21 = 0.0f;
	matrix->_22 = 1.0f;
	matrix->_23 = 0.0f;
	matrix->_24 = 0.0f;
	matrix->_31 = 0.0f;
	matrix->_32 = 0.0f;
	matrix->_33 = 1.0f;
	matrix->_34 = 0.0f;
	matrix->_41 = 0.0f;
	matrix->_42 = 0.0f;
	matrix->_43 = 0.0f;
	matrix->_44 = 1.0f;
}

void uniform_scaling_matrix(float scale, struct matrix *matrix)
{
	identity_matrix(matrix);

	matrix->_11 = scale;
	matrix->_22 = scale;
	matrix->_33 = scale;
}

void scaling_matrix(struct point3d *scale, struct matrix *matrix)
{
	identity_matrix(matrix);

	matrix->_11 = scale->x;
	matrix->_22 = scale->y;
	matrix->_33 = scale->z;
}

void rotation_matrix_x(float angle, struct matrix *matrix)
{
	identity_matrix(matrix);

	matrix->_22 = cosf(angle);
	matrix->_23 = sinf(angle);
	matrix->_32 = -sinf(angle);
	matrix->_33 = cosf(angle);
}

void rotation_matrix_y(float angle, struct matrix *matrix)
{
	identity_matrix(matrix);

	matrix->_11 = cosf(angle);
	matrix->_13 = -sinf(angle);
	matrix->_31 = sinf(angle);
	matrix->_33 = cosf(angle);
}

void rotation_matrix_z(float angle, struct matrix *matrix)
{
	identity_matrix(matrix);

	matrix->_11 = cosf(angle);
	matrix->_12 = sinf(angle);
	matrix->_21 = -sinf(angle);
	matrix->_22 = cosf(angle);
}

void rotate_matrix_x(float angle, struct matrix *matrix)
{
	struct matrix tmp;

	rotation_matrix_x(angle, &tmp);
	multiply_matrix_unary(matrix, &tmp);
}

void rotate_matrix_y(float angle, struct matrix *matrix)
{
	struct matrix tmp;

	rotation_matrix_y(angle, &tmp);
	multiply_matrix_unary(matrix, &tmp);
}

void rotate_matrix_z(float angle, struct matrix *matrix)
{
	struct matrix tmp;

	rotation_matrix_z(angle, &tmp);
	multiply_matrix_unary(matrix, &tmp);
}

float determinant_3x3(struct matrix *m)
{
	return m->_11 * m->_22 * m->_33 + m->_12 * m->_23 * m->_31 + m->_13 * m->_21 * m->_32 - 
		m->_11 * m->_23 * m->_32 - m->_12 * m->_21 * m->_33 - m->_13 * m->_22 * m->_31;
}

void inverse_matrix(struct matrix *matrix, struct matrix *dest)
{
	float det = determinant_3x3(matrix);

	if((det >= 0.99 && det <= 1.01) || (det <= -0.99 && det >= -1.01))
	{
		struct point3d translation;

		transpose_matrix(matrix, dest);
		dest->_14 = matrix->_14;
		dest->_24 = matrix->_24;
		dest->_34 = matrix->_34;
		dest->_44 = matrix->_44;

		transform_point(dest, (struct point3d *)&matrix->_41, &translation);

		dest->_41 = -translation.x;
		dest->_42 = -translation.y;
		dest->_43 = -translation.z;
	}
	else glitch_once("Non-uniform scaling: %f\n", det);
}
