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
 * macro.h - useful macros
 */

#pragma once

#include "globals.h"
#include "cfg.h"

#define LIST_FOR_EACH(X, Y) for((X) = (Y)->head; (X); (X) = (X)->next)

/*
 * Versioned structure macros, used to access FF7 and FF8 data in a portable
 * way. Allows massive, relatively straightforward code reuse between the two
 * games. Watch out for block statements and using expressions with side
 * effects! These macros can have unintended effects if you're not careful.
 */

/*
 * VOBJ - Define a versioned object, structure must have FF7 and FF8 variants
 * named ff7_<type name> and ff8_<type name> respectively.
 * X - type name
 * Y - object name
 * Z - initial value
 */
#define VOBJ(X,Y,Z) struct ff7_ ## X *ff7_ ## Y = ff8 ? 0 : (struct ff7_ ## X *)(Z); struct ff8_ ## X *ff8_ ## Y = ff8 ? (struct ff8_ ## X *)(Z) : 0

/*
 * VPTR - Access the raw pointer contained in an object.
 * X - object
 */
#define VPTR(X) (ff8 ? (void *)ff8_ ## X : (void *)ff7_ ## X)

 /*
  * VPTRCAST - Access the raw pointer contained in an object, casting to a defined type
  * T - object type
  * X - object
  */
#define VPTRCAST(T, X) (ff8 ? (struct T *)ff8_ ## X : (struct T *)ff7_ ## X)

/*
 * VASS - Assign a new pointer to an object.
 * T - object type
 * X - object
 * Y - new value
 */
#define VASS(T,X,Y) { if(ff8) ff8_ ## X = (struct ff8_ ## T *)(Y); else ff7_ ## X = (struct ff7_ ## T *)(Y); }

/*
 * VREF - Retrieve the value of a member of an object.
 * X - object
 * Y - member name (member must be identical in both versions of the structure!)
 */
#define VREF(X,Y) (ff8 ? ff8_ ## X->Y : ff7_ ## X->Y)

/*
 * VREFP - Create a pointer to a member.
 * X - object
 * Y - member name (member must be identical in both versions of the structure!)
 */
#define VREFP(X,Y) (ff8 ? &(ff8_ ## X->Y) : &(ff7_ ## X->Y))

/*
 * VRASS - Assign a value to a member.
 * X - object
 * Y - member name (member must be identical in both versions of the structure!)
 */
#define VRASS(X,Y,Z) { if(ff8) ff8_ ## X->Y = (Z); else ff7_ ## X->Y = (Z); }

/*
 * UNSAFE_VREF - Retrieve the value of a member of an object WITHOUT type safety.
 * T - object type
 * X - object
 * Y - member name (types can be different in the different versions)
 */
#define UNSAFE_VREF(T,X,Y) (ff8 ? (struct T *)ff8_ ## X->Y : (struct T *)ff7_ ## X->Y)

#define BGRA2RGBA(n) (((n >> 24) & 0xFF) << 24 | (n & 0xFF) << 16 | ((n >> 8) & 0xFF) << 8 | ((n >> 16) & 0xFF))

#define BGRA_R(x) (x >> 16 & 0xFF)
#define BGRA_G(x) (x >> 8 & 0xFF)
#define BGRA_B(x) (x & 0xFF)
#define BGRA_A(x) (x >> 24 & 0xFF)

#define BIT(x) (1 << x)
