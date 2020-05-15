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

struct point3d
{
	float x;
	float y;
	float z;
};

struct point4d
{
	float x;
	float y;
	float z;
	float w;
};

void add_vector(struct point3d *a, struct point3d *b, struct point3d *dest);
void subtract_vector(struct point3d *a, struct point3d *b, struct point3d *dest);
void multiply_vector(struct point3d *vector, float scalar, struct point3d *dest);
void divide_vector(struct point3d *vector, float scalar, struct point3d *dest);
float vector_length(struct point3d *vector);
void normalize_vector(struct point3d *vector);
float dot_product(struct point3d *a, struct point3d *b);
void cross_product(struct point3d *a, struct point3d *b, struct point3d *dest);
void transform_point(struct matrix *matrix, struct point3d *point, struct point3d *dest);
void transform_point_w(struct matrix *matrix, struct point3d *point, struct point4d *dest);
void transform_point4d(struct matrix *matrix, struct point4d *point, struct point4d *dest);
void transpose_matrix(struct matrix *matrix, struct matrix *dest);
void multiply_matrix(struct matrix *a, struct matrix *b, struct matrix *dest);
void multiply_matrix_unary(struct matrix *a, struct matrix *b);
void identity_matrix(struct matrix *matrix);
void uniform_scaling_matrix(float scale, struct matrix *matrix);
void scaling_matrix(struct point3d *scale, struct matrix *matrix);
void rotation_matrix_x(float angle, struct matrix *matrix);
void rotation_matrix_y(float angle, struct matrix *matrix);
void rotation_matrix_z(float angle, struct matrix *matrix);
void rotate_matrix_x(float angle, struct matrix *matrix);
void rotate_matrix_y(float angle, struct matrix *matrix);
void rotate_matrix_z(float angle, struct matrix *matrix);
void inverse_matrix(struct matrix *matrix, struct matrix *dest);
