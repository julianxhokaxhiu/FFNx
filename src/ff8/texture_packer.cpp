/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2023 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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
#include <set>

TexturePacker::TexturePacker() :
	_vram(nullptr), _vramTextureIds(VRAM_WIDTH * VRAM_HEIGHT, INVALID_TEXTURE), _disableDrawTexturesBackground(false)
{
}

void TexturePacker::cleanVramTextureIds(const TextureInfos &texture)
{
	for (int prevY = 0; prevY < texture.h(); ++prevY)
	{
		int clearKey = texture.x() + (texture.y() + prevY) * VRAM_WIDTH;
		std::fill_n(_vramTextureIds.begin() + clearKey, texture.w(), INVALID_TEXTURE);
	}
}

void TexturePacker::cleanTextures(ModdedTextureId previousTextureId, bool keepMods)
{
	if (_textures.contains(previousTextureId))
	{
		Texture &previousTexture = _textures.at(previousTextureId);

		if (trace_all || trace_vram) ffnx_info("TexturePacker::%s: clear texture %s (textureId = %d)\n", __func__, previousTexture.name().c_str(), previousTextureId);

		cleanVramTextureIds(previousTexture);

		_textures.erase(previousTextureId);
	}
	else if (!keepMods && _externalTextures.contains(previousTextureId))
	{
		Texture &previousTexture = _externalTextures.at(previousTextureId);

		if (trace_all || trace_vram) ffnx_info("TexturePacker::%s: clear texture %s (textureId = %d)\n", __func__, previousTexture.name().c_str(), previousTextureId);

		cleanVramTextureIds(previousTexture);

		previousTexture.destroyImage();
		_externalTextures.erase(previousTextureId);
	}
	else if (!keepMods && _backgroundTextures.contains(previousTextureId))
	{
		TextureBackground &previousTexture = _backgroundTextures.at(previousTextureId);

		if (trace_all || trace_vram) ffnx_info("TexturePacker::%s: clear texture background %s (textureId = %d)\n", __func__, previousTexture.name().c_str(), previousTextureId);

		cleanVramTextureIds(previousTexture);

		previousTexture.destroyImage();
		_backgroundTextures.erase(previousTextureId);
	}
	else if (_textureRedirections.contains(previousTextureId))
	{
		TextureRedirection &previousTexture = _textureRedirections.at(previousTextureId);

		if (trace_all || trace_vram) ffnx_info("TexturePacker::%s: clear texture redirection (textureId = %d)\n", __func__, previousTextureId);

		cleanVramTextureIds(previousTexture.oldTexture());

		previousTexture.destroyImage();
		_textureRedirections.erase(previousTextureId);
	}
}

void TexturePacker::setVramTextureId(ModdedTextureId textureId, int x, int y, int w, int h, bool keepMods)
{
	for (int i = 0; i < h; ++i)
	{
		int vramY = y + i;

		for (int j = 0; j < w; ++j)
		{
			int vramX = x + j;
			int key = vramX + vramY * VRAM_WIDTH;
			ModdedTextureId previousTextureId = _vramTextureIds.at(key);

			if (previousTextureId != INVALID_TEXTURE)
			{
				cleanTextures(previousTextureId, keepMods);
			}

			_vramTextureIds[key] = textureId;
		}
	}
}

void TexturePacker::uploadTexture(const uint8_t *source, int x, int y, int w, int h)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s x=%d y=%d w=%d h=%d\n", __func__, x, y, w, h);

	uint8_t *vram = vramSeek(x, y);
	const int vramLineWidth = VRAM_DEPTH * VRAM_WIDTH;
	const int lineWidth = VRAM_DEPTH * w;

	for (int i = 0; i < h; ++i)
	{
		memcpy(vram, source, lineWidth);

		source += lineWidth;
		vram += vramLineWidth;
	}
}

void TexturePacker::setTexture(const char *name, int x, int y, int w, int h, Tim::Bpp bpp, bool isPal)
{
	bool hasNamedTexture = name != nullptr && *name != '\0';

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s %s x=%d y=%d w=%d h=%d bpp=%d isPal=%d\n", __func__, hasNamedTexture ? name : "N/A", x, y, w, h, bpp, isPal);

	ModdedTextureId textureId = INVALID_TEXTURE;

	if (hasNamedTexture && !isPal)
	{
		Texture tex(name, x, y, w, h, bpp);
		textureId = makeTextureId(x, y);

		if (tex.createImage())
		{
			_externalTextures[textureId] = tex;
		}
		else
		{
			_textures[textureId] = tex;
		}
	}

	setVramTextureId(textureId, x, y, w, h);
}

void TexturePacker::setTextureBackground(const char *name, int x, int y, int w, int h, const std::vector<Tile> &mapTiles)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s %s x=%d y=%d w=%d h=%d tileCount=%d\n", __func__, name, x, y, w, h, mapTiles.size());

	TextureBackground tex(name, x, y, w, h, mapTiles);
	ModdedTextureId textureId = makeTextureId(x, y);

	if (tex.createImage())
	{
		_backgroundTextures[textureId] = tex;
	}
	else
	{
		_textures[textureId] = tex;
	}

	setVramTextureId(textureId, x, y, w, h);
}

bool TexturePacker::setTextureRedirection(const TextureInfos &oldTexture, const TextureInfos &newTexture, uint32_t *imageData)
{
	if (trace_all || trace_vram)  ffnx_info("TexturePacker::%s: old=(%d, %d, %d, %d) => new=(%d, %d, %d, %d)\n", __func__,
		oldTexture.x(), oldTexture.y(), oldTexture.w(), oldTexture.h(),
		newTexture.x(), newTexture.y(), newTexture.w(), newTexture.h());

	TextureRedirection redirection(oldTexture, newTexture);
	if (redirection.isValid() && redirection.createImage(imageData))
	{
		ModdedTextureId textureId = makeTextureId(oldTexture.x(), oldTexture.y());
		_textureRedirections[textureId] = redirection;

		setVramTextureId(textureId, oldTexture.x(), oldTexture.y(), oldTexture.w(), oldTexture.h(), true);

		return true;
	}

	return false;
}

uint8_t TexturePacker::getMaxScale(const uint8_t *texData) const
{
	if (_externalTextures.empty() && _backgroundTextures.empty() && _textureRedirections.empty())
	{
		return 1;
	}

	if (!_tiledTexs.contains(texData))
	{
		if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s Unknown tex data\n", __func__);

		return 0;
	}

	const TiledTex &tiledTex = _tiledTexs.at(texData);

	uint8_t maxScale = 1;

	for (int y = 0; y < 256; ++y)
	{
		int vramY = tiledTex.y + y;

		for (int x = 0; x < 64; ++x)
		{
			int vramX = tiledTex.x + x;
			ModdedTextureId textureId = _vramTextureIds.at(vramX + vramY * VRAM_WIDTH);

			if (textureId != INVALID_TEXTURE)
			{
				uint8_t scale = 0;

				if (_externalTextures.contains(textureId))
				{
					scale = _externalTextures.at(textureId).scale();
				}
				else if (_backgroundTextures.contains(textureId))
				{
					scale = _backgroundTextures.at(textureId).scale();
				}
				else if (_textureRedirections.contains(textureId))
				{
					scale = _textureRedirections.at(textureId).scale();
				}

				if (scale > maxScale)
				{
					maxScale = scale;
				}
			}
		}
	}

	return maxScale;
}

void TexturePacker::getTextureNames(const uint8_t *texData, std::list<std::string> &names) const
{
	if (_externalTextures.empty() && _backgroundTextures.empty() && _textures.empty())
	{
		return;
	}

	if (!_tiledTexs.contains(texData))
	{
		ffnx_warning("TexturePacker::%s Unknown tex data %p\n", __func__, texData);

		return;
	}

	const TiledTex &tiledTex = _tiledTexs.at(texData);
	std::set<ModdedTextureId> textureIds;

	for (int y = 0; y < 256; ++y)
	{
		int vramY = tiledTex.y + y;

		for (int x = 0; x < 64; ++x)
		{
			int vramX = tiledTex.x + x;
			ModdedTextureId textureId = _vramTextureIds.at(vramX + vramY * VRAM_WIDTH);

			if (textureId != INVALID_TEXTURE)
			{
				if (_externalTextures.contains(textureId))
				{
					textureIds.insert(textureId);
				}
				else if (_backgroundTextures.contains(textureId))
				{
					textureIds.insert(textureId);
				}
				else if (_textures.contains(textureId))
				{
					textureIds.insert(textureId);
				}
			}
		}
	}

	for (ModdedTextureId textureId: textureIds)
	{
		if (_externalTextures.contains(textureId))
		{
			names.push_back(_externalTextures.at(textureId).name());
		}
		else if (_backgroundTextures.contains(textureId))
		{
			names.push_back(_backgroundTextures.at(textureId).name());
		}
		else
		{
			names.push_back(_textures.at(textureId).name());
		}
	}
}

void TexturePacker::disableDrawTexturesBackground(bool disabled)
{
	_disableDrawTexturesBackground = disabled;
}

TexturePacker::TextureTypes TexturePacker::drawTextures(const uint8_t *texData, struct texture_format *tex_format, uint32_t *target, const uint32_t *originalImageData, int originalW, int originalH, uint8_t scale, uint32_t paletteIndex)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s pointer=0x%X bitsperpixel=%d\n", __func__, texData, (tex_format ? tex_format->bitsperpixel : -1));

	auto it = _tiledTexs.find(texData);

	if (it != _tiledTexs.end())
	{
		const TiledTex &tex = it->second;

		if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s tex=(%d, %d) bpp=%d paletteIndex=%d\n", __func__, tex.x, tex.y, tex.bpp, paletteIndex);

		return drawTextures(target, tex, originalW, originalH, scale, paletteIndex);
	}

	if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s Unknown tex data\n", __func__);

	return NoTexture;
}

TexturePacker::TextureTypes TexturePacker::drawTextures(uint32_t *target, const TiledTex &tiledTex, int targetW, int targetH, uint8_t scale, uint32_t paletteIndex)
{
	if (_externalTextures.empty() && _backgroundTextures.empty() && _textureRedirections.empty())
	{
		return NoTexture;
	}

	int w = targetW;

	if (tiledTex.bpp <= int(Tim::Bpp16))
	{
		w /= 4 >> tiledTex.bpp;
	}
	else
	{
		ffnx_warning("%s: Unknown bpp %d\n", tiledTex.bpp);

		return NoTexture;
	}

	if (_disableDrawTexturesBackground && (trace_all || trace_vram)) ffnx_info("TexturePacker::%s disabled\n", __func__);

	TextureTypes drawnTextureTypes = NoTexture;

	for (int y = 0; y < targetH; ++y)
	{
		int vramY = tiledTex.y + y;

		if (vramY >= VRAM_HEIGHT) {
			break;
		}

		for (int x = 0; x < w; ++x)
		{
			int vramX = tiledTex.x + x;

			if (vramX >= VRAM_WIDTH) {
				break;
			}

			ModdedTextureId textureId = _vramTextureIds.at(vramX + vramY * VRAM_WIDTH);

			if (textureId != INVALID_TEXTURE)
			{
				if (_externalTextures.contains(textureId))
				{
					const Texture &texture = _externalTextures.at(textureId);

					int textureX = vramX - texture.x(),
						textureY = vramY - texture.y();

					texture.copyRect(textureX, textureY, target, x, y, targetW, scale);

					drawnTextureTypes = TextureTypes(int(drawnTextureTypes) | int(ExternalTexture));
				}
				else if (_backgroundTextures.contains(textureId) && ! _disableDrawTexturesBackground)
				{
					const TextureBackground &texture = _backgroundTextures.at(textureId);

					int textureX = vramX - texture.x(),
						textureY = vramY - texture.y();

					texture.copyRect(textureX, textureY, tiledTex.bpp, target, x, y, targetW, scale);

					drawnTextureTypes = TextureTypes(int(drawnTextureTypes) | int(ExternalTexture));
				}
				else if (_textureRedirections.contains(textureId))
				{
					const TextureRedirection &redirection = _textureRedirections.at(textureId);

					int textureX = vramX - redirection.oldTexture().x(),
						textureY = vramY - redirection.oldTexture().y();

					redirection.copyRect(textureX, textureY, target, x, y, targetW, scale);

					drawnTextureTypes = TextureTypes(int(drawnTextureTypes) | int(InternalTexture));
				}
			}
		}
	}

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s x=%d y=%d bpp=%d w=%d targetW=%d targetH=%d scale=%d drawnTextureTypes=0x%X\n", __func__, tiledTex.x, tiledTex.y, tiledTex.bpp, w, targetW, targetH, scale, drawnTextureTypes);

	return drawnTextureTypes;
}

void TexturePacker::registerTiledTex(const uint8_t *texData, int x, int y, Tim::Bpp bpp, int palX, int palY)
{
	if (trace_all || trace_vram) ffnx_trace("%s pointer=0x%X x=%d y=%d bpp=%d palX=%d palY=%d psx_texture_pages=%X\n", __func__, texData, x, y, bpp, palX, palY, ff8_externals.psx_texture_pages);

	_tiledTexs[texData] = TiledTex(x, y, bpp, palX, palY);
}

bool TexturePacker::saveVram(const char *fileName, Tim::Bpp bpp) const
{
	uint16_t palette[256] = {};

	ff8_tim tim_infos = ff8_tim();

	tim_infos.img_data = _vram;
	tim_infos.img_w = VRAM_WIDTH;
	tim_infos.img_h = VRAM_HEIGHT;

	if (bpp < Tim::Bpp16)
	{
		tim_infos.pal_data = palette;
		tim_infos.pal_h = 1;
		tim_infos.pal_w = bpp == Tim::Bpp4 ? 16 : 256;

		// Greyscale palette
		for (int i = 0; i < tim_infos.pal_w; ++i)
		{
			uint8_t color = bpp == Tim::Bpp4 ? i * 16 : i;
			palette[i] = color | (color << 5) | (color << 10);
		}
	}

	return Tim(bpp, tim_infos).save(fileName, bpp);
}

TexturePacker::TextureInfos::TextureInfos() :
	_x(0), _y(0), _w(0), _h(0), _bpp(Tim::Bpp4)
{
}

TexturePacker::TextureInfos::TextureInfos(
	int x, int y, int w, int h,
	Tim::Bpp bpp
) : _x(x), _y(y), _w(w), _h(h), _bpp(bpp)
{
}

bimg::ImageContainer *TexturePacker::TextureInfos::createImageContainer(const char *name, uint8_t palette_index, bool hasPal)
{
	char filename[MAX_PATH] = {}, langPath[16] = {};

	if(trace_all || trace_loaders || trace_vram) ffnx_trace("texture file name (VRAM): %s\n", name);

	if(save_textures) return nullptr;

	ff8_fs_lang_string(langPath);
	strcat(langPath, "/");

	for (uint8_t lang = 0; lang < 2; lang++)
	{
		for (size_t idx = 0; idx < mod_ext.size(); idx++)
		{
			if (hasPal) {
				_snprintf(filename, sizeof(filename), "%s/%s/%s%s_%02i.%s", basedir, mod_path.c_str(), langPath, name, palette_index, mod_ext[idx].c_str());
			} else {
				_snprintf(filename, sizeof(filename), "%s/%s/%s%s.%s", basedir, mod_path.c_str(), langPath, name, mod_ext[idx].c_str());
			}
			bimg::ImageContainer *image = newRenderer.createImageContainer(filename, bimg::TextureFormat::BGRA8);

			if (image != nullptr)
			{
				if (trace_all || trace_loaders || trace_vram) ffnx_trace("Using texture: %s\n", filename);

				return image;
			}
			else if (trace_all || trace_loaders || trace_vram)
			{
				ffnx_warning("Texture does not exist, skipping: %s\n", filename);
			}
		}

		*langPath = '\0';
	}

	return nullptr;
}

uint8_t TexturePacker::TextureInfos::computeScale(int sourcePixelW, int sourceH, int targetPixelW, int targetH)
{
	if (targetPixelW < sourcePixelW
		|| targetH < sourceH
		|| targetPixelW % sourcePixelW != 0
		|| targetH % sourceH != 0)
	{
		ffnx_warning("Texture redirection size must be scaled to the original texture size\n");

		return 0;
	}

	int scaleW = targetPixelW / sourcePixelW, scaleH = targetH / sourceH;

	if (scaleW != scaleH)
	{
		ffnx_warning("Texture redirection size must have the same ratio as the original texture: (%d / %d)\n", sourcePixelW, sourceH);

		return 0;
	}

	if (scaleW > MAX_SCALE)
	{
		ffnx_warning("Texture redirection size cannot exceed original size * %d\n", MAX_SCALE);

		return MAX_SCALE;
	}

	return scaleW;
}

void TexturePacker::TextureInfos::copyRect(
	const uint32_t *sourceRGBA, int sourceXBpp2, int sourceY, int sourceW, uint8_t sourceScale, Tim::Bpp sourceDepth,
	uint32_t *targetRGBA, int targetX, int targetY, int targetW, uint8_t targetScale)
{
	if (targetScale < sourceScale)
	{
		return;
	}

	const uint8_t targetRectWidth = (4 >> int(sourceDepth)) * targetScale,
		targetRectHeight = targetScale,
		sourceRectWidth = (4 >> int(sourceDepth)) * sourceScale,
		sourceRectHeight = sourceScale;
	const uint8_t scaleRatio = targetScale / sourceScale;

	targetW *= targetScale;

	targetRGBA += targetX * targetRectWidth + targetY * targetRectHeight * targetW;
	sourceRGBA += sourceXBpp2 * sourceRectWidth + sourceY * sourceRectHeight * sourceW;

	for (int y = 0; y < targetRectHeight; ++y)
	{
		for (int x = 0; x < targetRectWidth; ++x)
		{
			*(targetRGBA + x + y * targetW) = *(sourceRGBA + x / scaleRatio + y / scaleRatio * sourceW);
		}
	}
}

TexturePacker::Texture::Texture() :
	TextureInfos(), _image(nullptr), _name(""), _scale(1)
{
}

TexturePacker::Texture::Texture(
	const char *name,
	int x, int y, int w, int h,
	Tim::Bpp bpp
) : TextureInfos(x, y, w, h, bpp), _image(nullptr), _name(name), _scale(1)
{
}

bool TexturePacker::Texture::createImage(uint8_t palette_index, bool has_pal)
{
	char filename[MAX_PATH], langPath[16] = {};

	_image = createImageContainer(_name.c_str(), palette_index, has_pal);

	if (_image == nullptr)
	{
		return false;
	}

	uint8_t scale = computeScale();

	if (scale == 0)
	{
		destroyImage();

		return false;
	}

	_scale = scale;

	return true;
}

void TexturePacker::Texture::destroyImage()
{
	if (_image != nullptr) {
		bimg::imageFree(_image);
		_image = nullptr;
	}
}

uint8_t TexturePacker::Texture::computeScale() const
{
	return TextureInfos::computeScale(pixelW(), h(), _image->m_width, _image->m_height);
}

void TexturePacker::Texture::copyRect(int sourceXBpp2, int sourceY, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const
{
	TextureInfos::copyRect(
		(const uint32_t *)_image->m_data, sourceXBpp2, sourceY, _image->m_width, _scale, bpp(),
		target, targetX, targetY, targetW, targetScale
	);
}

TexturePacker::TextureBackground::TextureBackground() :
	Texture()
{
}

TexturePacker::TextureBackground::TextureBackground(
	const char *name,
	int x, int y, int w, int h,
	const std::vector<Tile> &mapTiles
) : Texture(name, x, y, w, h, Tim::Bpp16), _mapTiles(mapTiles)
{
	// Build tileIdsByPosition for fast lookup
	_tileIdsByPosition.reserve(mapTiles.size());
	size_t tileId = 0;
	for (const Tile &tile: mapTiles) {
		const uint8_t textureId = tile.texID & 0xF;
		const uint16_t key = uint16_t(textureId) | (uint16_t(tile.srcX / TILE_SIZE) << 4) | (uint16_t(tile.srcY / TILE_SIZE) << 8) | (((uint16_t(tile.texID) >> 7) & 3) << 12);

		auto it = _tileIdsByPosition.find(key);
		// Remove some duplicates (but keep those which render differently)
		if (it == _tileIdsByPosition.end() || ! ff8_background_tiles_looks_alike(tile, mapTiles.at(it->second))) {
			_tileIdsByPosition.insert(std::pair<uint16_t, size_t>(key, tileId));
		}
		++tileId;
	}
	_colsCount = mapTiles.size() / (TEXTURE_HEIGHT / TILE_SIZE) + int(mapTiles.size() % (TEXTURE_HEIGHT / TILE_SIZE) != 0);
}

void TexturePacker::TextureBackground::copyRect(int sourceXBpp2, int sourceY, Tim::Bpp textureBpp, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const
{
	const uint8_t textureId = sourceXBpp2 / TEXTURE_WIDTH_BPP16;
	const uint16_t sourceX = (sourceXBpp2 % TEXTURE_WIDTH_BPP16) << (2 - int(textureBpp));
	const uint16_t key = uint16_t(textureId) | (uint16_t(sourceX / TILE_SIZE) << 4) | (uint16_t(sourceY / TILE_SIZE) << 8) | (uint16_t(textureBpp) << 12);

	auto [begin, end] = _tileIdsByPosition.equal_range(key);
	if (begin == end) {
		return;
	}

	bool multiMatch = _tileIdsByPosition.count(key) > 1, matched = false;

	// Iterate over matching tiles
	for (const std::pair<uint16_t, size_t> &pair: std::ranges::subrange{begin, end}) {
		const size_t tileId = pair.second;
		const Tile &tile = _mapTiles.at(tileId);

		if (multiMatch && tile.parameter != 255) {
			if (tile.parameter < *ff8_externals.field_state_background_count) {
				const ff8_field_state_background *field_state_backgrounds = *ff8_externals.field_state_backgrounds;

				if (tile.state != field_state_backgrounds[tile.parameter].bgstate >> 6) {
					continue;
				}
			} else {
				ffnx_warning("TextureBackground::%s: group script not found for background parameter %d\n", __func__, tile.parameter);

				continue;
			}
		}

		if (matched) {
			ffnx_warning("TextureBackground::%s: multiple tiles found for the same position (tile id %d)\n", __func__, tileId);

			break;
		}

		const Tim::Bpp bpp = Tim::Bpp((tile.texID >> 7) & 3);
		const int col = tileId % _colsCount, row = tileId / _colsCount;
		const int imageXBpp2 = col * TILE_SIZE / (4 >> int(bpp)) + sourceXBpp2 % (4 << int(bpp)), imageY = row * TILE_SIZE + sourceY % TILE_SIZE;

		TextureInfos::copyRect(
			(const uint32_t *)image()->m_data, imageXBpp2, imageY, image()->m_width, scale(), bpp,
			target, targetX, targetY, targetW, targetScale
		);

		matched = true;
	}

	if (matched == false) {
		ffnx_warning("TextureBackground::%s: tile not matched textureId=%d, sourceX=%d, sourceY=%d\n", __func__, textureId, sourceX, sourceY);
	}
}

uint8_t TexturePacker::TextureBackground::computeScale() const
{
	return TextureInfos::computeScale(_colsCount * TILE_SIZE, TEXTURE_HEIGHT, image()->m_width, image()->m_height);
}

TexturePacker::TiledTex::TiledTex()
 : x(0), y(0), palX(0), palY(0), bpp(Tim::Bpp4), renderedOnce(false)
{
}

TexturePacker::TiledTex::TiledTex(
	int x, int y, Tim::Bpp bpp, int palX, int palY
) : x(x), y(y), palX(palX), palY(palY), bpp(bpp), renderedOnce(false)
{
}

TexturePacker::TextureRedirection::TextureRedirection()
 : TextureInfos(), _image(nullptr), _scale(0)
{
}

TexturePacker::TextureRedirection::TextureRedirection(
	const TextureInfos &oldTexture,
	const TextureInfos &newTexture
) : TextureInfos(newTexture), _image(nullptr), _oldTexture(oldTexture),
	_scale(computeScale())
{
}

bool TexturePacker::TextureRedirection::createImage(uint32_t *imageData)
{
	_image = imageData;

	return _image != nullptr;
}

void TexturePacker::TextureRedirection::destroyImage()
{
	if (_image != nullptr) {
		driver_free(_image);
		_image = nullptr;
	}
}

uint8_t TexturePacker::TextureRedirection::computeScale() const
{
	return TextureInfos::computeScale(_oldTexture.pixelW(), _oldTexture.h(), pixelW(), h());
}

void TexturePacker::TextureRedirection::copyRect(int textureX, int textureY, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const
{
	TextureInfos::copyRect(
		_image, textureX, textureY, pixelW(), _scale, _oldTexture.bpp(),
		target, targetX, targetY, targetW, targetScale
	);
}
