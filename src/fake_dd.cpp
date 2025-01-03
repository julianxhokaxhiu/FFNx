/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#include "renderer.h"

#include "fake_dd.h"
#include "gl.h"
#include "log.h"

uint32_t __stdcall fake_dd_blit_fast(struct ddsurface **me, uint32_t unknown1, uint32_t unknown2, struct ddsurface **target, LPRECT source, uint32_t unknown3);
uint32_t __stdcall fake_ddsurface_get_pixelformat(struct ddsurface **me, LPDDPIXELFORMAT pf);
uint32_t __stdcall fake_ddsurface_get_surface_desc(struct ddsurface **me, LPDDSURFACEDESC2 sd);
uint32_t __stdcall fake_ddsurface_get_dd_interface(struct ddsurface **me, struct dddevice ***dd);
uint32_t __stdcall fake_ddsurface_get_palette(struct ddsurface **me, void **palette);
uint32_t __stdcall fake_ddsurface_lock(struct ddsurface **me, LPRECT dest, LPDDSURFACEDESC sd, uint32_t flags, uint32_t unused);
uint32_t __stdcall fake_ddsurface_unlock(struct ddsurface **me, LPRECT dest);
uint32_t __stdcall fake_ddsurface_islost(struct ddsurface **me);
uint32_t __stdcall fake_ddsurface_restore(struct ddsurface **me);
uint32_t __stdcall fake_dd_query_interface(struct dddevice **me, uint32_t *iid, void **ppvobj);
uint32_t __stdcall fake_dd_addref(struct dddevice **me);
uint32_t __stdcall fake_dd_release(struct dddevice **me);
uint32_t __stdcall fake_dd_create_clipper(struct dddevice **me, DWORD flags, LPDIRECTDRAWCLIPPER *clipper);
uint32_t __stdcall fake_dd_create_palette(struct dddevice **me, LPPALETTEENTRY palette_entry, LPDIRECTDRAWPALETTE *palette, IUnknown *unused);
uint32_t __stdcall fake_dd_create_surface(struct dddevice **me, LPDDSURFACEDESC sd, LPDIRECTDRAWSURFACE *surface, IUnknown *unused);
uint32_t __stdcall fake_dd_get_caps(struct dddevice **me, LPDDCAPS halcaps, LPDDCAPS helcaps);
uint32_t __stdcall fake_dd_get_display_mode(struct dddevice **me, LPDDSURFACEDESC sd);
uint32_t __stdcall fake_dd_set_coop_level(struct dddevice **me, uint32_t coop_level);
uint32_t __stdcall fake_d3d_get_caps(struct d3d2device **me, void *a, void *b);

struct ddsurface fake_dd_surface = {
	fake_dd_query_interface,
	fake_dd_addref,
	fake_dd_release,
	0,
	0,
	0,
	0,
	fake_dd_blit_fast,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	fake_ddsurface_get_palette,
	fake_ddsurface_get_pixelformat,
	fake_ddsurface_get_surface_desc,
	0,
	fake_ddsurface_islost,
	fake_ddsurface_lock,
	0,
	fake_ddsurface_restore,
	0,
	0,
	0,
	0,
	fake_ddsurface_unlock,
	0,
	0,
	0,
	fake_ddsurface_get_dd_interface,
};
struct ddsurface *_fake_dd_back_surface = &fake_dd_surface;

struct ddsurface fake_dd_front_surface = {
	fake_dd_query_interface,
	fake_dd_addref,
	fake_dd_release,
	0,
	0,
	0,
	0,
	fake_dd_blit_fast,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	fake_ddsurface_get_palette,
	fake_ddsurface_get_pixelformat,
	fake_ddsurface_get_surface_desc,
	0,
	fake_ddsurface_islost,
	fake_ddsurface_lock,
	0,
	fake_ddsurface_restore,
	0,
	0,
	0,
	0,
	fake_ddsurface_unlock,
	0,
	0,
	0,
	fake_ddsurface_get_dd_interface,
};
struct ddsurface *_fake_dd_front_surface = &fake_dd_front_surface;

struct ddsurface fake_dd_temp_surface = {
	fake_dd_query_interface,
	fake_dd_addref,
	fake_dd_release,
	0,
	0,
	0,
	0,
	fake_dd_blit_fast,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	fake_ddsurface_get_palette,
	fake_ddsurface_get_pixelformat,
	fake_ddsurface_get_surface_desc,
	0,
	fake_ddsurface_islost,
	fake_ddsurface_lock,
	0,
	fake_ddsurface_restore,
	0,
	0,
	0,
	0,
	fake_ddsurface_unlock,
	0,
	0,
	0,
	fake_ddsurface_get_dd_interface,
};
struct ddsurface *_fake_dd_temp_surface = &fake_dd_temp_surface;

struct dddevice fake_dddevice = {
	fake_dd_query_interface,
	fake_dd_addref,
	fake_dd_release,
	0,
	fake_dd_create_clipper,
	fake_dd_create_palette,
	fake_dd_create_surface,
	0,
	0,
	0,
	0,
	fake_dd_get_caps,
	fake_dd_get_display_mode,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	fake_dd_set_coop_level,
	0,
	0,
	0,
};
struct dddevice *_fake_dddevice = &fake_dddevice;

struct d3d2device fake_d3d2device = {
	fake_dd_query_interface,
	fake_dd_addref,
	fake_dd_release,
	fake_d3d_get_caps,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
struct d3d2device *_fake_d3d2device = &fake_d3d2device;

uint8_t* fake_dd_surface_buffer = nullptr;

uint32_t movie_texture = 0;

uint32_t __stdcall fake_dd_blit_fast(struct ddsurface **me, uint32_t unknown1, uint32_t unknown2, struct ddsurface **source, LPRECT src_rect, uint32_t unknown3)
{
	if(trace_all || trace_fake_dx) ffnx_trace("blit_fast\n");

	return DD_OK;
}

uint32_t __stdcall fake_ddsurface_get_pixelformat(struct ddsurface **me, LPDDPIXELFORMAT pf)
{
	if(trace_all || trace_fake_dx) ffnx_trace("get_pixelformat\n");

	pf->dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	pf->dwRGBBitCount = 32;
	pf->dwBBitMask = 0xFF;
	pf->dwGBitMask = 0xFF00;
	pf->dwRBitMask = 0xFF0000;
	pf->dwRGBAlphaBitMask = 0xFF000000;

	return 0;
}

uint32_t __stdcall fake_ddsurface_get_surface_desc(struct ddsurface **me, LPDDSURFACEDESC2 sd)
{
	if(trace_all || trace_fake_dx) ffnx_trace("get_surface_desc\n");

	return 0;
}

uint32_t __stdcall fake_ddsurface_get_dd_interface(struct ddsurface **me, struct dddevice ***dd)
{
	if(trace_all || trace_fake_dx) ffnx_trace("get_dd_interface\n");

	*dd = &_fake_dddevice;
	return 0;
}

uint32_t __stdcall fake_ddsurface_get_palette(struct ddsurface **me, void **palette)
{
	if(trace_all || trace_fake_dx) ffnx_trace("get_palette\n");

	//*palette = 0;

	return DDERR_UNSUPPORTED;
}

uint32_t __stdcall fake_ddsurface_lock(struct ddsurface **me, LPRECT dest, LPDDSURFACEDESC sd, uint32_t flags, uint32_t unused)
{
	if(trace_all || trace_fake_dx) ffnx_trace("lock\n");

	if (fake_dd_surface_buffer == nullptr) fake_dd_surface_buffer = (uint8_t*)driver_calloc(game_width * game_height, 4);

	sd->lpSurface = fake_dd_surface_buffer;
	sd->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;

	sd->dwWidth = game_width;
	sd->dwHeight = game_height;
	sd->lPitch = game_width * 4;

	sd->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	sd->ddpfPixelFormat.dwRGBBitCount = 32;
	sd->ddpfPixelFormat.dwBBitMask = 0xFF;
	sd->ddpfPixelFormat.dwGBitMask = 0xFF00;
	sd->ddpfPixelFormat.dwRBitMask = 0xFF0000;
	sd->ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

	return DD_OK;
}

uint32_t __stdcall fake_ddsurface_unlock(struct ddsurface **me, LPRECT dest)
{
	if(trace_all || trace_fake_dx) ffnx_trace("unlock\n");

	if (movie_texture) newRenderer.deleteTexture(movie_texture);

	movie_texture = newRenderer.createTexture(
		fake_dd_surface_buffer,
		game_width,
		game_height,
		game_width * 4
	);

	newRenderer.useTexture(movie_texture);

	newRenderer.isMovie(true);

	gl_draw_movie_quad(game_width, game_height);

	newRenderer.isMovie(false);

	return DD_OK;
}

uint32_t __stdcall fake_ddsurface_islost(struct ddsurface **me)
{
	if(trace_all || trace_fake_dx) ffnx_trace("islost\n");

	return DD_OK;
}

uint32_t __stdcall fake_ddsurface_restore(struct ddsurface **me)
{
	if(trace_all || trace_fake_dx) ffnx_trace("restore\n");

	return DD_OK;
}

uint32_t __stdcall fake_dd_query_interface(struct dddevice **me, uint32_t *iid, void **ppvobj)
{
	if(trace_all || trace_fake_dx) ffnx_trace("query_interface: 0x%p 0x%p 0x%p 0x%p\n", iid[0], iid[1], iid[2], iid[3]);

	if(iid[0] == 0x6C14DB80)
	{
		*ppvobj = me;
		return S_OK;
	}

	if(iid[0] == 0x57805885)
	{
		*ppvobj = &_fake_dd_temp_surface;
		return S_OK;
	}

	return E_NOINTERFACE;
}

uint32_t __stdcall fake_dd_addref(struct dddevice **me)
{
	if(trace_all || trace_fake_dx) ffnx_trace("addref\n");

	return 1;
}

uint32_t __stdcall fake_dd_release(struct dddevice **me)
{
	if(trace_all || trace_fake_dx) ffnx_trace("release\n");

	return 0;
}

uint32_t __stdcall fake_dd_create_clipper(struct dddevice **me, DWORD flags, LPDIRECTDRAWCLIPPER *clipper)
{
	if(trace_all || trace_fake_dx) ffnx_trace("create_clipper\n");

	if(clipper == 0) return DDERR_INVALIDPARAMS;

	*clipper = 0;

	return DD_OK;
}

uint32_t __stdcall fake_dd_create_palette(struct dddevice **me, LPPALETTEENTRY palette_entry, LPDIRECTDRAWPALETTE *palette, IUnknown *unused)
{
	if(trace_all || trace_fake_dx) ffnx_trace("create_palette\n");

	if(palette == 0) return DDERR_INVALIDPARAMS;

	*palette = 0;

	return DD_OK;
}

uint32_t __stdcall fake_dd_create_surface(struct dddevice **me, LPDDSURFACEDESC sd, LPDIRECTDRAWSURFACE *surface, IUnknown *unused)
{
	if(trace_all || trace_fake_dx) ffnx_trace("create_surface %ix%i\n", sd->dwWidth, sd->dwHeight);

	*surface = (LPDIRECTDRAWSURFACE)&_fake_dd_temp_surface;

	return 0;
}

uint32_t __stdcall fake_dd_get_caps(struct dddevice **me, LPDDCAPS halcaps, LPDDCAPS helcaps)
{
	if(trace_all || trace_fake_dx) ffnx_trace("get_caps\n");

	halcaps->dwCaps = DDCAPS_BLTSTRETCH;
	return 0;
}

uint32_t __stdcall fake_dd_get_display_mode(struct dddevice **me, LPDDSURFACEDESC sd)
{
	if(trace_all || trace_fake_dx) ffnx_trace("get_display_mode\n");

	sd->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;

	sd->dwWidth = game_width;
	sd->dwHeight = game_height;
	sd->lPitch = game_width * 4;

	sd->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	sd->ddpfPixelFormat.dwRGBBitCount = 32;
	sd->ddpfPixelFormat.dwBBitMask = 0xFF;
	sd->ddpfPixelFormat.dwGBitMask = 0xFF00;
	sd->ddpfPixelFormat.dwRBitMask = 0xFF0000;
	sd->ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

	return 0;
}

uint32_t __stdcall fake_dd_set_coop_level(struct dddevice **me, uint32_t coop_level)
{
	if(trace_all || trace_fake_dx) ffnx_trace("set_coop_level\n");

	return 0;
}

uint32_t __stdcall fake_d3d_get_caps(struct d3d2device **me, void *a, void *b)
{
	if(trace_all || trace_fake_dx) ffnx_trace("d3d_get_caps\n");

	memset(a, -1, 0xFC);
	memset(b, -1, 0xFC);

	return DD_OK;
}
