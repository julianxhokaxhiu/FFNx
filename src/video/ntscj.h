#pragma once

#include "../renderer.h"

struct AVCodecContext;

extern bool fullrange_input;
extern ColorMatrixType colormatrix;
extern ColorGamutType colorgamut;
extern InverseGammaFunctionType gammatype;
extern ChromaLocationType chromaloc;
extern bool okcolorspace;
extern bool yuvjfixneeded;

bool ntscj_detect_logo_movie(const char* name, const AVCodecContext* codec_ctx);
void ntscj_prepare_movie_colors(const AVCodecContext* codec_ctx, bool islogomovie);
ColorGamutType ntscj_get_overall_color_gamut();