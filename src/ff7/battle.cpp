#include "../ff7.h"
#include "../log.h"

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
