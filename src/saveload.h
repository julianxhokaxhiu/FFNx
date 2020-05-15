#pragma once

#include "types.h"

uint save_texture(void *data, uint width, uint height, uint palette_index, char *name);
uint load_texture(char *name, uint palette_index, uint *width, uint *height, uint use_compression);
