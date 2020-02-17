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
 * ff7/battle.c - replacement routines for FF7's battle system
 */

#include "../types.h"
#include "../common.h"
#include "../ff7.h"
#include "../log.h"
#include "../globals.h"

void magic_thread_start(void (*func)())
{
	ff7_externals.destroy_magic_effects();

	/*
	 * Original function creates a separate thread but the code is not thread
	 * safe in any way! Luckily modern PCs are fast enough to load magic
	 * effects synchronously.
	 */
	func();
}
