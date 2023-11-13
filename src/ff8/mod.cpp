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
#include "mod.h"

#include <set>

#include "../image/image.h"
#include "../log.h"
#include "../renderer.h"
#include "file.h"


bx::DefaultAllocator TextureImage::defaultAllocator;

TextureImage::TextureImage() :
	_image(nullptr), _scale(1)
{
}

bool TextureImage::createImage(const char *filename, int originalTexturePixelWidth, int originalTextureHeight, int internalLodScale)
{
	if (_image != nullptr)
	{
		destroyImage();
	}

	_image = loadImageContainer(&defaultAllocator, filename, bimg::TextureFormat::BGRA8);

	if (_image == nullptr)
	{
		return false;
	}

	setLod(computeLod(originalTexturePixelWidth, internalLodScale));

	uint8_t scale = computeScale(originalTexturePixelWidth, originalTextureHeight);

	if (trace_all || trace_vram)
	{
		ffnx_info("TextureImage::%s: size=%dx%d scaled to %dx%d (x%d) format=%d depth=%d numMips=%d hasAlpha=%d\n", __func__,
			_image->m_width, _image->m_height, _mip.m_width, _mip.m_height, scale, _mip.m_format, _mip.m_depth, _image->m_numMips, _mip.m_hasAlpha);
	}

	if (scale == 0)
	{
		destroyImage();

		return false;
	}

	_scale = scale;

	return true;
}

void TextureImage::destroyImage()
{
	if (_image != nullptr)
	{
		bimg::imageFree(_image);
		_image = nullptr;
	}
}

void TextureImage::setLod(uint8_t lod)
{
	if (_image != nullptr)
	{
		bimg::imageGetRawData(*_image, 0, lod, _image->m_data, _image->m_size, _mip);
	}
}

int TextureImage::computeMaxScale()
{
	int resWidth = window_size_x * newRenderer.getScalingFactor(),
		baseResW = FF8_BASE_RESOLUTION_X;

	return std::max(resWidth, FF8_BASE_RESOLUTION_X) / baseResW;
}

uint8_t TextureImage::computeLod(int originalTexturePixelWidth, int internalScale) const
{
	if (_image == nullptr || _image->m_numMips <= 1)
	{
		return 0;
	}

	int maxScale = computeMaxScale(),
		maxWidth = originalTexturePixelWidth * maxScale * internalScale,
		imageWidth = _image->m_width;

	if (trace_all || trace_vram)
	{
		ffnx_info("ModdedTexture::%s: maxScale=%d maxWidth=%d imageWidth=%d\n", __func__, maxScale, maxWidth, imageWidth);
	}

	for (uint8_t lod = 0; lod < _image->m_numMips; ++lod)
	{
		if (imageWidth == maxWidth)
		{
			return lod;
		}

		imageWidth /= 2;
	}

	ffnx_warning("ModdedTexture::%s: Cannot detect the LOD of the texture\n", __func__);

	return 0;
}

uint8_t TextureImage::computeScale(int sourcePixelW, int sourceH) const
{
	int targetPixelW = _mip.m_width, targetH = _mip.m_height;

	if (targetPixelW < sourcePixelW
		|| targetH < sourceH
		|| targetPixelW % sourcePixelW != 0
		|| targetH % sourceH != 0)
	{
		ffnx_warning("Texture redirection size must be scaled to the original texture size with the same ratio (modded texture size: %dx%d, original texture size: %dx%d)\n", targetPixelW, targetH, sourcePixelW, sourceH);

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
		ffnx_warning("External texture size cannot exceed \"original size * %d\" (scale=%d)\n", MAX_SCALE, scaleW);

		return 0;
	}

	return scaleW;
}

ModdedTexture::ModdedTexture(const TexturePacker::IdentifiedTexture &originalTexture, bool isInternal):
	_originalTexture(originalTexture), _isInternal(isInternal)
{
}

bool ModdedTexture::findExternalTexture(const char *name, char *filename, uint8_t palette_index, bool hasPal, const char *extension, char *found_extension)
{
	char langPath[16] = "/";

	if(trace_all || trace_loaders) ffnx_trace("texture file name (VRAM): %s palette_index=%d hasPal=%d\n", name, palette_index, hasPal);

	if(save_textures) return false;

	ff8_fs_lang_string(langPath + 1);

	for (size_t idx = 0; idx < mod_ext.size(); idx++)
	{
		// Force extension
		if (extension && stricmp(extension, mod_ext[idx].c_str()) != 0)
		{
			continue;
		}

		for (uint8_t lang = 0; lang < 2; lang++)
		{
			if (hasPal)
			{
				_snprintf(filename, MAX_PATH, "%s/%s%s/%s_%02i.%s", basedir, mod_path.c_str(), langPath, name, palette_index, mod_ext[idx].c_str());
			}
			else
			{
				_snprintf(filename, MAX_PATH, "%s/%s%s/%s.%s", basedir, mod_path.c_str(), langPath, name, mod_ext[idx].c_str());
			}

			if (fileExists(filename))
			{
				if (trace_all || trace_loaders) ffnx_trace("Using texture: %s\n", filename);

				if (found_extension != nullptr) {
					strncpy(found_extension, mod_ext[idx].c_str(), mod_ext[idx].size());
				}

				return true;
			}
			else if (trace_all || trace_loaders)
			{
				ffnx_warning("Texture does not exist, skipping: %s\n", filename);
			}

			*langPath = '\0';
		}

		*langPath = '/';
	}

	return false;
}

void ModdedTexture::drawImage(
	const uint32_t *sourceRgba, int sourceRgbaW, uint8_t sourceScale,
	uint32_t *targetRgba, int targetRgbaW, uint8_t targetScale,
	int sourceX, int sourceY, int sourceW, int sourceH,
	int targetX, int targetY)
{
	if (targetScale < sourceScale)
	{
		return;
	}

	sourceRgbaW *= sourceScale;
	targetRgbaW *= targetScale;
	const uint8_t scaleRatio = targetScale / sourceScale;

	for (int y = 0; y < sourceH; ++y)
	{
		for (int x = 0; x < sourceW; ++x)
		{
			const uint32_t *sourceRgbaCur = sourceRgba + (sourceX + x) * sourceScale + (sourceY + y) * sourceScale * sourceRgbaW;
			uint32_t *targetRgbaCur = targetRgba + (targetX + x) * targetScale + (targetY + y) * targetScale * targetRgbaW;

			for (int yPix = 0; yPix < targetScale; ++yPix)
			{
				for (int xPix = 0; xPix < targetScale; ++xPix)
				{
					uint32_t color = *(sourceRgbaCur + xPix / scaleRatio + yPix / scaleRatio * sourceRgbaW),
						alpha = color & 0xFF000000;
					*(targetRgbaCur + xPix + yPix * targetRgbaW) = (color & 0xFFFFFF) | (alpha >= 0x7F000000 ? 0x7F000000 : 0);
				}
			}
		}
	}
}

void ModdedTexture::copyRect(
	const uint32_t *sourceRgba, int sourceRgbaW, uint32_t *targetRgba, int targetRgbaW, uint8_t scale, Tim::Bpp depth,
	int sourceXBpp2, int sourceY, int sourceWBpp2, int sourceH, int targetXBpp2, int targetY)
{
	int sourceX = sourceXBpp2 * (4 >> int(depth)),
		sourceW = sourceWBpp2 * (4 >> int(depth)),
		targetX = targetXBpp2 * (4 >> int(depth));

	const uint32_t *source = sourceRgba + (sourceX + sourceY * sourceRgbaW) * scale;
	uint32_t *target = targetRgba + (targetX + targetY * targetRgbaW) * scale;

	for (int y = 0; y < sourceH; ++y)
	{
		memcpy(target, source, sizeof(uint32_t) * sourceW);

		source += sourceRgbaW;
		target += targetRgbaW;
	}
}

TextureModStandard::~TextureModStandard()
{
	for (std::pair<uint8_t, TextureImage> image: _textures)
	{
		image.second.destroyImage();
	}
}

bool TextureModStandard::createImages(int paletteCount, int internalLodScale)
{
	paletteCount = paletteCount >= 0 ? paletteCount : originalTexture().palette().h(); // Works most of the time

	if (trace_all || trace_vram) ffnx_trace("TextureModStandard::%s: paletteCount=%d internalLodScale=%d\n", __func__, paletteCount, internalLodScale);

	int modCount = std::max(paletteCount, 1);
	char filename[MAX_PATH] = {}, *extension = nullptr, found_extension[16] = {};

	for (int paletteId = 0; paletteId < modCount; ++paletteId) {
		if (!findExternalTexture(originalTexture().name().c_str(), filename, paletteId, true, extension, found_extension))
		{
			continue;
		}

		// Force the same extension for every palettes to reduce file existence checks
		extension = found_extension;

		TextureImage externalTexture;
		if (!externalTexture.createImage(filename, originalTexture().texture().pixelW(), originalTexture().texture().h(), internalLodScale))
		{
			continue;
		}

		_textures[paletteId] = externalTexture;
	}

	return !_textures.empty();
}

uint8_t TextureModStandard::computePaletteId(int vramPalXBpp2, int vramPalY) const
{
	if (_currentPalette >= 0)
	{
		return uint8_t(_currentPalette);
	}

	if (vramPalXBpp2 == originalTexture().palette().x()
		&& vramPalY >= originalTexture().palette().y() && vramPalY < originalTexture().palette().y() + originalTexture().palette().h())
	{
		return vramPalY - originalTexture().palette().y();
	}

	return 0;
}

const TextureImage &TextureModStandard::textureImage(uint8_t paletteId) const
{
	if (trace_all || trace_vram) ffnx_trace("TextureModStandard::%s paletteId=%d\n", __func__, paletteId);

	auto it = _textures.find(paletteId);

	if (it == _textures.end())
	{
		if (trace_all || trace_vram) ffnx_warning("TextureModStandard::%s cannot find image for paletteId=%d, fallback to the first one\n", __func__, paletteId);

		return _textures.begin()->second; // Use the first image
	}

	return it->second;
}

uint8_t TextureModStandard::scale(int vramPalXBpp2, int vramPalY) const
{
	if (trace_all || trace_vram) ffnx_trace("TextureModStandard::%s %s vramPal=(%d, %d) original=(%d, %d)\n", __func__, originalTexture().name().c_str(), vramPalXBpp2, vramPalY, originalTexture().palette().x(), originalTexture().palette().y());

	return textureImage(computePaletteId(vramPalXBpp2, vramPalY)).scale();
}

TexturePacker::TextureTypes TextureModStandard::drawToImage(
	int offsetX, int offsetY,
	uint32_t *targetRgba, int targetW, int targetH, uint8_t targetScale, Tim::Bpp targetBpp,
	int16_t vramPalXBpp2, int16_t vramPalY) const
{
	const TexturePacker::TextureInfos &origTexture = originalTexture().texture();

	if (origTexture.bpp() != targetBpp)
	{
		return TexturePacker::NoTexture;
	}

	int sourceX = offsetX < 0 ? -offsetX : 0,
		sourceY = offsetY < 0 ? -offsetY : 0,
		targetX = offsetX > 0 ? offsetX : 0,
		targetY = offsetY > 0 ? offsetY : 0,
		width = std::min(origTexture.pixelW() - sourceX, targetW - targetX),
		height = std::min(origTexture.h() - sourceY, targetH - targetY);

	if (width <= 0 || height <= 0)
	{
		return TexturePacker::NoTexture;
	}

	uint8_t paletteId = computePaletteId(vramPalXBpp2, vramPalY);
	const TextureImage &image = textureImage(paletteId);
	const bimg::ImageMip &mip = image.mip();

	drawImage(
		(const uint32_t *)mip.m_data, mip.m_width / image.scale(), image.scale(),
		targetRgba, targetW, targetScale,
		sourceX, sourceY, width, height,
		targetX, targetY
	);

	return TexturePacker::ExternalTexture;
}

void TextureModStandard::copyRect(int sourceXBpp2, int sourceY, int sourceWBpp2, int sourceH, int targetXBpp2, int targetY)
{
	const TexturePacker::TextureInfos &texture = originalTexture().texture();

	for (const std::pair<uint8_t, TextureImage> &image: _textures)
	{
		const bimg::ImageMip &mip = image.second.mip();
		ModdedTexture::copyRect(
			reinterpret_cast<const uint32_t *>(mip.m_data), mip.m_width,
			const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(mip.m_data)), mip.m_width,
			image.second.scale(), texture.bpp(),
			sourceXBpp2 - texture.x(), sourceY - texture.y(), sourceWBpp2, sourceH,
			targetXBpp2 - texture.x(), targetY - texture.y()
		);
	}
}

void TextureModStandard::copyRect(
	int sourceXBpp2, int sourceY, int sourceWBpp2, int sourceH, int targetXBpp2, int targetY,
	const TextureModStandard &targetTexture)
{
	const TexturePacker::TextureInfos &tex = originalTexture().texture(),
		&targetTex = targetTexture.originalTexture().texture();

	if (tex.bpp() != targetTex.bpp()) {
		return;
	}

	for (const std::pair<uint8_t, TextureImage> &image: _textures)
	{
		for (const std::pair<uint8_t, TextureImage> &targetImage: targetTexture._textures)
		{
			const bimg::ImageMip &mip = image.second.mip();
			const bimg::ImageMip &targetMip = targetImage.second.mip();

			if (image.second.scale() != targetImage.second.scale()) {
				continue;
			}

			ModdedTexture::copyRect(
				reinterpret_cast<const uint32_t *>(mip.m_data), mip.m_width,
				const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(targetMip.m_data)), targetMip.m_width,
				image.second.scale(), tex.bpp(),
				sourceXBpp2 - tex.x(), sourceY - tex.y(), sourceWBpp2, sourceH,
				targetXBpp2 - targetTex.x(), targetY - targetTex.y()
			);
		}
	}
}

void TextureModStandard::forceCurrentPalette(int8_t currentPalette)
{
	if (_textures.contains(currentPalette)) {
		_currentPalette = currentPalette;
	}
}

TextureBackground::TextureBackground(
	const TexturePacker::IdentifiedTexture &originalTexture,
	const std::vector<Tile> &mapTiles,
	int vramPageId
) : ModdedTexture(originalTexture), _mapTiles(mapTiles), _vramPageId(vramPageId)
{
}

TextureBackground::~TextureBackground()
{
	_texture.destroyImage();
}

bool TextureBackground::createImages(const char *extension, char *foundExtension)
{
	size_t size = _mapTiles.size();

	_colsCount = size / (TEXTURE_HEIGHT / TILE_SIZE) + int(size % (TEXTURE_HEIGHT / TILE_SIZE) != 0);

	char filename[MAX_PATH] = {};

	if (!findExternalTexture(originalTexture().name().c_str(), filename, 0, false, extension, foundExtension))
	{
		return false;
	}

	if (!_texture.createImage(filename, _vramPageId >= 0 ? TEXTURE_HEIGHT : _colsCount * TILE_SIZE, TEXTURE_HEIGHT))
	{
		return false;
	}

	// Build tileIdsByPosition for fast lookup
	_tileIdsByTextureId.reserve(size);

	_usedBpps = 0; // 1: 4-bit, 2: 8-bit, 4: 16-bit
	size_t tileId = 0;
	for (const Tile &tile: _mapTiles) {
		const uint8_t texId = tile.texID & 0xF;
		if (_vramPageId < 0 || _vramPageId == texId) {
			const uint8_t bpp = (tile.texID >> 7) & 3;
			const uint16_t key = uint16_t(texId) | (uint16_t(tile.srcX / TILE_SIZE) << 4) | (uint16_t(tile.srcY / TILE_SIZE) << 8) | (uint16_t(bpp) << 12);
			_usedBpps |= 1 << bpp;

			auto it = _tileIdsByTextureId.find(key);
			// Remove some duplicates (but keep those which render differently)
			if (it == _tileIdsByTextureId.end() || ! ff8_background_tiles_looks_alike(tile, _mapTiles.at(it->second))) {
				_tileIdsByTextureId.insert(std::pair<uint16_t, size_t>(key, tileId));
			}
		}
		++tileId;
	}

	return true;
}

bool TextureBackground::isBpp(Tim::Bpp bpp) const
{
	return true;
}

TexturePacker::TextureTypes TextureBackground::drawToImage(
	int offsetX, int offsetY,
	uint32_t *targetRgba, int targetW, int targetH, uint8_t targetScale, Tim::Bpp targetBpp,
	int16_t vramPalXBpp2, int16_t vramPalY) const
{
	const bimg::ImageMip &mip = _texture.mip();
	const uint32_t *imgData = (const uint32_t *)mip.m_data;
	const uint8_t imgScale = _texture.scale();
	const uint32_t imgWidth = mip.m_width / imgScale, imgHeight = mip.m_height / imgScale;

	// Tomberry way
	if (_vramPageId >= 0) {
		drawImage(
			imgData, imgWidth, imgScale,
			targetRgba, targetW, targetScale,
			0, 0, imgWidth, imgHeight,
			0, 0
		);

		return TexturePacker::ExternalTexture;
	}

	const uint8_t cols = targetW / TILE_SIZE, rows = targetH / TILE_SIZE;
	const uint8_t colsBpp = TILE_SIZE / (1 << uint16_t(targetBpp));
	const uint8_t paletteId = vramPalY - 232 - 8; // The palette always is at (0, 232) and the first 8 palettes are unused
	const uint8_t initialTextureId = -offsetX / (4 >> uint16_t(targetBpp)) / TEXTURE_WIDTH_BPP16;

	for (uint8_t y = 0; y < 16; ++y) {
		for (uint8_t x = 0; x < 16; ++x) {
			const uint8_t textureId = initialTextureId + x / colsBpp;
			const uint8_t textureX = x % colsBpp;
			const uint16_t key = uint16_t(textureId) | (uint16_t(textureX) << 4) | (uint16_t(y) << 8) | (uint16_t(targetBpp) << 12);
			auto [begin, end] = _tileIdsByTextureId.equal_range(key);
			if (begin == end) {
				if (trace_all || trace_vram) ffnx_warning("TextureBackground::%s: No tile found for textureId=%d x=%d y=%d targetBpp=%d paletteId=%d\n", __func__, textureId, textureX, y, targetBpp, paletteId);

				continue;
			}

			const size_t tileIdDefault = begin->second;
			std::vector<size_t> matched;

			// When matching multiple tiles for the same texture part, try to choose the good tile
			for (const std::pair<uint16_t, size_t> &pair: std::ranges::subrange{begin, end}) {
				const Tile &tile = _mapTiles.at(pair.second);
				uint8_t palId = (tile.palID >> 6) & 0xF;

				if (targetBpp != Tim::Bpp16 && paletteId != palId) {
					continue;
				}

				/* if (tile.parameter != 255) {
					if (tile.parameter < *ff8_externals.field_state_background_count) {
						const ff8_field_state_background *field_state_backgrounds = *ff8_externals.field_state_backgrounds;

						if (tile.state != field_state_backgrounds[tile.parameter].bgstate >> 6) {
							continue;
						}
					} else {
						if (trace_all || trace_vram) ffnx_warning("TextureBackground::%s: group script not found for background parameter %d\n", __func__, tile.parameter);

						continue;
					}
				} */

				matched.push_back(pair.second);
			}

			size_t matchedCount = matched.size(),
				tileId = matchedCount > 0 ? matched.at(0) : tileIdDefault;

			if (matchedCount > 1) {
				if (trace_all || trace_vram) ffnx_warning("TextureBackground::%s: tile %d conflict\n", __func__, tileIdDefault);
			}

			const int col = tileId % _colsCount, row = tileId / _colsCount;

			drawImage(
				imgData, imgWidth / imgScale, imgScale,
				targetRgba, targetW, targetScale,
				col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE,
				x * TILE_SIZE, y * TILE_SIZE
			);
		}
	}

	return TexturePacker::ExternalTexture;
}

TextureRawImage::TextureRawImage(
	const TexturePacker::IdentifiedTexture &originalTexture,
	uint32_t *imageData, int imageWidth, int imageHeight
) : ModdedTexture(originalTexture, true), _image(imageData), _width(imageWidth), _height(imageHeight),
	_scale(computeScale())
{
}

TextureRawImage::~TextureRawImage()
{
	if (_image != nullptr) {
		driver_free(_image);
	}
}

uint8_t TextureRawImage::computeScale() const
{
	return _height / originalTexture().texture().h();
}

TexturePacker::TextureTypes TextureRawImage::drawToImage(
	int offsetX, int offsetY,
	uint32_t *targetRgba, int targetW, int targetH, uint8_t targetScale, Tim::Bpp targetBpp,
	int16_t vramPalXBpp2, int16_t vramPalY) const
{
	const TexturePacker::TextureInfos &origTexture = originalTexture().texture();

	if (origTexture.bpp() != targetBpp)
	{
		return TexturePacker::NoTexture;
	}

	int sourceX = offsetX < 0 ? -offsetX : 0,
		sourceY = offsetY < 0 ? -offsetY : 0,
		targetX = offsetX > 0 ? offsetX : 0,
		targetY = offsetY > 0 ? offsetY : 0,
		width = std::min(origTexture.pixelW() - sourceX, targetW - targetX),
		height = std::min(origTexture.h() - sourceY, targetH - targetY);

	if (width <= 0 || height <= 0)
	{
		return TexturePacker::NoTexture;
	}

	drawImage(
		_image, _width / _scale, _scale,
		targetRgba, targetW, targetScale,
		sourceX, sourceY, width, height,
		targetX, targetY
	);

	return TexturePacker::InternalTexture;
}
