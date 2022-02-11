/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 myst6re                                            //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
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
#include "texture_packer.h"
#include "../renderer.h"
#include "../saveload.h"
#include "../patch.h"
#include "file.h"
#include "../image/tim.h"

TexturePacker::TexturePacker() :
	_vram(nullptr)
{
	memset(_vramTextureIds, INVALID_TEXTURE, VRAM_WIDTH * VRAM_HEIGHT);
}

void TexturePacker::setTexture(const char *name, const uint8_t *source, int x, int y, int w, int h, uint8_t bpp, bool isPal)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s x=%d y=%d w=%d h=%d bpp=%d isPal=%d\n", __func__, x, y, w, h, bpp, isPal);

	bool hasNamedTexture = name != nullptr && *name != '\0';
	uint8_t *vram = vramSeek(x, y);
	const int vramLineWidth = VRAM_DEPTH * VRAM_WIDTH;
	const int lineWidth = VRAM_DEPTH * w;
	Texture tex;
	ModdedTextureId textureId = INVALID_TEXTURE;

	if (hasNamedTexture && !isPal)
	{
		tex = Texture(name, x, y, w, h, bpp);

		if (tex.createImage())
		{
			textureId = x + y * VRAM_WIDTH;
			_moddedTextures[textureId] = tex;
		}
	}

	for (int i = 0; i < h; ++i)
	{
		memcpy(vram, source, lineWidth);

		int vramY = y + i;

		for (int j = 0; j < w; ++j)
		{
			int vramX = x + j;
			int key = vramX + vramY * VRAM_WIDTH;
			ModdedTextureId previousTextureId = _vramTextureIds[key];

			if (previousTextureId != INVALID_TEXTURE)
			{
				if (_moddedTextures.contains(previousTextureId))
				{
					Texture &previousTexture = _moddedTextures[previousTextureId];

					if (trace_all || trace_vram) ffnx_info("TexturePacker::%s: clear texture %s (textureId = %d)\n", __func__, previousTexture.name().c_str(), previousTextureId);

					for (int prevY = 0; prevY < previousTexture.h(); ++prevY)
					{
						int clearKey = previousTexture.x() + (previousTexture.y() + prevY) * VRAM_WIDTH;
						std::fill_n(_vramTextureIds + clearKey, previousTexture.w(), INVALID_TEXTURE);
					}

					previousTexture.destroyImage();
					_moddedTextures.erase(previousTextureId);
				}
				else if (_textureRedirections.contains(previousTextureId))
				{
					if (trace_all || trace_vram) ffnx_info("TexturePacker::%s: clear texture redirection (textureId = %d)\n", __func__, previousTextureId);

					_textureRedirections.erase(previousTextureId);
				}
			}

			_vramTextureIds[key] = textureId;
		}

		source += lineWidth;
		vram += vramLineWidth;
	}

	updateMaxScale();
}

bool TexturePacker::setTextureRedirection(
	const TextureInfos &oldTexture,
	const TextureInfos &newTexture,
	const TextureInfos &oldPal,
	const TextureInfos &newPal
)
{
	TextureRedirection redirection(oldTexture, newTexture, oldPal, newPal);
	if (redirection.isValid())
	{
		ModdedTextureId textureId = oldTexture.x() + oldTexture.y() * VRAM_WIDTH;
		_textureRedirections[textureId] = redirection;

		updateMaxScale();

		return true;
	}

	return false;
}

void TexturePacker::clearTextureRedirections()
{
	if (! _textureRedirections.empty())
	{
		_textureRedirections.clear();
		updateMaxScale();
	}
}

void TexturePacker::updateMaxScale()
{
	_maxScaleCached = 1;

	for (const std::pair<ModdedTextureId, Texture> &pair: _moddedTextures)
	{
		const Texture &tex = pair.second;
		const uint8_t texScale = tex.scale();

		if (texScale > _maxScaleCached) {
			_maxScaleCached = texScale;
		}
	}

	for (const std::pair<ModdedTextureId, TextureRedirection> &pair: _textureRedirections)
	{
		const TextureRedirection &redirection = pair.second;
		const uint8_t texScale = redirection.scale();

		if (texScale > _maxScaleCached) {
			_maxScaleCached = texScale;
		}
	}

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s scale=%d\n", __func__, _maxScaleCached);
}

bool TexturePacker::drawModdedTextures(const uint8_t *texData, uint32_t *target, const uint32_t *originalImageData, int originalW, int originalH, uint8_t scale)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s pointer=0x%X\n", __func__, texData);

	if (_tiledTexs.contains(texData))
	{
		const TiledTex &tex = _tiledTexs[texData];

		return drawModdedTextures(target, tex, originalW, originalH, scale);
	}

	if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s Unknown tex data\n", __func__);

	return false;
}

bool TexturePacker::drawModdedTextures(uint32_t *target, const TiledTex &tiledTex, int targetW, int targetH, uint8_t scale)
{
	if (_moddedTextures.empty() && _textureRedirections.empty())
	{
		return false;
	}

	int w = targetW;

	if (tiledTex.bpp <= 2)
	{
		w /= 4 >> tiledTex.bpp;
	}
	else
	{
		ffnx_warning("%s: Unknown bpp %d\n", tiledTex.bpp);

		return false;
	}

	bool hasModdedTexture = false;

	int scaledW = w * scale,
		scaledH = targetH * scale;

	for (int y = 0; y < targetH; ++y)
	{
		int vramY = tiledTex.y + y;

		for (int x = 0; x < w; ++x)
		{
			int vramX = tiledTex.x + x;
			ModdedTextureId textureId = _vramTextureIds[vramX + vramY * VRAM_WIDTH];

			if (textureId != INVALID_TEXTURE)
			{
				if (_moddedTextures.contains(textureId))
				{
					const Texture &texture = _moddedTextures[textureId];

					int textureX = vramX - texture.x(),
						textureY = vramY - texture.y();

					texture.copyRect(textureX, textureY, target, x, y, targetW, scale);

					hasModdedTexture = true;
				}
				else if (_textureRedirections.contains(textureId))
				{
					const TextureRedirection &redirection = _textureRedirections[textureId];

					int textureX = vramX - redirection.oldTexture().x(),
						textureY = vramY - redirection.oldTexture().y();

					uint32_t image_data_size = redirection.newTexture().pixelW() * redirection.newTexture().h() * 4;
					uint32_t *image_data = (uint32_t*)driver_malloc(image_data_size);

					if (image_data != nullptr)
					{
						if (toRGBA32(image_data, redirection.newTexture(), redirection.newPalette(), false))
						{
							redirection.copyRect(image_data, textureX, textureY, target, x, y, targetW, scale);

							hasModdedTexture = true;
						}

						driver_free(image_data);
					}
				}
			}
		}
	}

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s x=%d y=%d bpp=%d w=%d targetW=%d targetH=%d scale=%d hasModdedTexture=%d\n", __func__, tiledTex.x, tiledTex.y, tiledTex.bpp, w, targetW, targetH, scale, hasModdedTexture);

	return hasModdedTexture;
}

void TexturePacker::getVramRect(uint8_t *target, const TextureInfos &texture) const
{
	uint8_t *vram = vramSeek(texture.x(), texture.y());
	const int vramLineWidth = VRAM_DEPTH * VRAM_WIDTH;
	const int lineWidth = VRAM_DEPTH * texture.w();

	for (int i = 0; i < texture.h(); ++i)
	{
		memcpy(target, vram, lineWidth);

		target += lineWidth;
		vram += vramLineWidth;
	}
}

void TexturePacker::registerTiledTex(uint8_t *target, int x, int y, uint8_t bpp, int palX, int palY)
{
	if (trace_all || trace_vram) ffnx_trace("%s pointer=0x%X x=%d y=%d bpp=%d palX=%d palY=%d\n", __func__, target, x, y, bpp, palX, palY);

	_tiledTexs[target] = TiledTex(x, y, bpp, palX, palY);
}

void TexturePacker::saveVram(const char *fileName, uint8_t bpp) const
{
	uint16_t palette[256] = {};

	ff8_tim tim_infos = ff8_tim();

	tim_infos.img_data = _vram;
	tim_infos.img_w = VRAM_WIDTH;
	tim_infos.img_h = VRAM_HEIGHT;

	if (bpp < 2)
	{
		tim_infos.pal_data = palette;
		tim_infos.pal_h = 1;
		tim_infos.pal_w = bpp == 0 ? 16 : 256;

		// Greyscale palette
		for (int i = 0; i < tim_infos.pal_w; ++i)
		{
			uint8_t color = bpp == 0 ? i * 16 : i;
			palette[i] = color | (color << 5) | (color << 10);
		}
	}

	Tim(bpp, tim_infos).save(fileName, bpp);
}

bool TexturePacker::toRGBA32(uint32_t *target, const TextureInfos &texture, const TextureInfos &palette, bool withAlpha) const
{
	if (texture.bpp() == 0)
	{
		uint16_t *pal_data = (uint16_t *)vramSeek(palette.x(), palette.y());

		for (int y = 0; y < texture.h(); ++y)
		{
			uint8_t *img_data = vramSeek(texture.x(), texture.y() + y);

			for (int x = 0; x < texture.w() * 2; ++x)
			{
				*target = fromR5G5B5Color(pal_data[*img_data & 0xF], withAlpha);

				++target;

				*target = fromR5G5B5Color(pal_data[*img_data >> 4], withAlpha);

				++target;
				++img_data;
			}
		}
	}
	else if (texture.bpp() == 1)
	{
		uint16_t *pal_data = (uint16_t *)vramSeek(palette.x(), palette.y());

		for (int y = 0; y < texture.h(); ++y)
		{
			uint8_t *img_data = vramSeek(texture.x(), texture.y() + y);

			for (int x = 0; x < texture.w() * 2; ++x)
			{
				*target = fromR5G5B5Color(pal_data[*img_data], withAlpha);

				++target;
				++img_data;
			}
		}
	}
	else if (texture.bpp() == 2)
	{
		for (int y = 0; y < texture.h(); ++y)
		{
			uint16_t *img_data = (uint16_t *)vramSeek(texture.x(), texture.y() + y);

			for (int x = 0; x < texture.w(); ++x)
			{
				*target = fromR5G5B5Color(*img_data, withAlpha);

				++target;
				++img_data;
			}
		}
	}
	else
	{
		ffnx_error("%s unknown bpp %d\n", __func__, texture.bpp());

		return false;
	}

	return true;
}

TexturePacker::TextureInfos::TextureInfos() :
	_x(0), _y(0), _w(0), _h(0), _bpp(255)
{
}

TexturePacker::TextureInfos::TextureInfos(
	int x, int y, int w, int h,
	uint8_t bpp
) : _x(x), _y(y), _w(w), _h(h), _bpp(bpp)
{
}

TexturePacker::Texture::Texture() :
	TextureInfos(), _image(nullptr), _name(""), _scale(1)
{
}

TexturePacker::Texture::Texture(
	const char *name,
	int x, int y, int w, int h,
	uint8_t bpp
) : TextureInfos(x, y, w, h, bpp), _image(nullptr), _name(name), _scale(1)
{
}

bool TexturePacker::Texture::createImage(uint8_t palette_index)
{
	char filename[MAX_PATH], langPath[16] = {};

	if(trace_all || trace_loaders || trace_vram) ffnx_trace("texture file name (VRAM): %s\n", _name.c_str());

	ff8_fs_lang_string(langPath);
	strcat(langPath, "/");

	for (int lang = 0; lang < 2; lang++)
	{
		for (int idx = 0; idx < mod_ext.size(); idx++)
		{
			_snprintf(filename, sizeof(filename), "%s/%s/%s%s_%02i.%s", basedir, mod_path.c_str(), langPath, _name.c_str(), palette_index, mod_ext[idx].c_str());
			_image = newRenderer.createImageContainer(filename, bimg::TextureFormat::BGRA8);

			if (_image != nullptr)
			{
				if (trace_all || trace_loaders || trace_vram) ffnx_trace("Using texture: %s\n", filename);

				uint8_t scale = computeScale();

				if (scale == 0)
				{
					destroyImage();

					return false;
				}

				_scale = scale;

				return true;
			}
			else if (trace_all || trace_loaders || trace_vram)
			{
				ffnx_warning("Texture does not exist, skipping: %s\n", filename);
			}
		}

		*langPath = '\0';
	}

	return false;
}

void TexturePacker::Texture::destroyImage()
{
	if (_image != nullptr) {
		bimg::imageFree(_image);
	}
}

uint8_t TexturePacker::Texture::computeScale() const
{
	if (_image == nullptr) {
		return 0;
	}

	int w = pixelW();

	if (_image->m_width < w || _image->m_height < _h || _image->m_width % w != 0 || _image->m_height % _h != 0)
	{
		ffnx_warning("Texture size must be scaled to the original texture size: %s\n", _name.c_str());

		return 0;
	}

	int scaleW = _image->m_width / w, scaleH = _image->m_height / _h;

	if (scaleW != scaleH)
	{
		ffnx_warning("Texture size must have the same ratio as the original texture: %s (%d / %d)\n", _name.c_str(), w, _h);

		return 0;
	}

	if (scaleW > MAX_SCALE)
	{
		ffnx_warning("Texture size cannot exceed original size * %d: %s\n", MAX_SCALE, _name.c_str());

		return MAX_SCALE;
	}

	return scaleW;
}

void TexturePacker::Texture::copyRect(int textureX, int textureY, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const
{
	if (targetScale < _scale)
	{
		return;
	}

	const uint32_t *textureData = (const uint32_t *)_image->m_data, textureW = _image->m_width;
	uint8_t targetRectWidth = (4 >> _bpp) * targetScale,
		targetRectHeight = targetScale,
		textureRectWidth = (4 >> _bpp) * _scale,
		textureRectHeight = _scale;
	uint8_t scaleRatio = targetScale / _scale;

	targetX *= targetRectWidth;
	targetY *= targetRectHeight;
	targetW *= targetScale;

	textureX *= textureRectWidth;
	textureY *= textureRectHeight;

	for (int y = 0; y < targetRectHeight; ++y)
	{
		for (int x = 0; x < targetRectWidth; ++x)
		{
			*(target + targetX + x + (targetY + y) * targetW) = *(textureData + textureX + x / scaleRatio + (textureY + y / scaleRatio) * textureW);
		}
	}
}

TexturePacker::TiledTex::TiledTex()
 : x(0), y(0), palette(nullptr), bpp(0)
{
}

TexturePacker::TiledTex::TiledTex(
	int x, int y, uint8_t bpp, uint16_t *palette
) : x(x), y(y), palette(palette), bpp(bpp)
{
}

TexturePacker::TextureRedirection::TextureRedirection() : _scale(0)
{
}

TexturePacker::TextureRedirection::TextureRedirection(
	const TextureInfos &oldTexture,
	const TextureInfos &newTexture,
	const TextureInfos &oldPal,
	const TextureInfos &newPal
) : _oldTexture(oldTexture), _newTexture(newTexture),
	_oldPal(oldPal), _newPal(newPal), _scale(computeScale())
{
}

uint8_t TexturePacker::TextureRedirection::computeScale() const
{
	int oldW = _oldTexture.w(), newW = _newTexture.w();

	if (_newTexture.w() < _oldTexture.w()
		|| _newTexture.h() < _oldTexture.h()
		|| _newTexture.w() % _oldTexture.w() != 0
		|| _newTexture.h() % _oldTexture.h() != 0)
	{
		ffnx_warning("Texture redirection size must be scaled to the original texture size\n");

		return 0;
	}

	int scaleW = _newTexture.w() / _oldTexture.w(), scaleH = _newTexture.h() / _oldTexture.h();

	if (scaleW != scaleH)
	{
		ffnx_warning("Texture redirection size must have the same ratio as the original texture: (%d / %d)\n", _oldTexture.w(), _oldTexture.h());

		return 0;
	}

	if (scaleW > MAX_SCALE)
	{
		ffnx_warning("Texture redirection size cannot exceed original size * %d\n", MAX_SCALE);

		return MAX_SCALE;
	}

	return scaleW;
}

void TexturePacker::TextureRedirection::copyRect(const uint32_t *textureData, int textureX, int textureY, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const
{
	if (targetScale < _scale)
	{
		return;
	}

	uint32_t textureW = _newTexture.pixelW();
	uint8_t targetRectWidth = (4 >> _newTexture.bpp()) * targetScale,
		targetRectHeight = targetScale,
		textureRectWidth = (4 >> _oldTexture.bpp()) * _scale,
		textureRectHeight = _scale;
	uint8_t scaleRatio = targetScale / _scale;

	targetX *= targetRectWidth;
	targetY *= targetRectHeight;
	targetW *= targetScale;

	textureX *= textureRectWidth;
	textureY *= textureRectHeight;

	for (int y = 0; y < targetRectHeight; ++y)
	{
		for (int x = 0; x < targetRectWidth; ++x)
		{
			*(target + targetX + x + (targetY + y) * targetW) = *(textureData + textureX + x / scaleRatio + (textureY + y / scaleRatio) * textureW);
		}
	}
}
