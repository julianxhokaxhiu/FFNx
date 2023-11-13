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
#include "../saveload.h"
#include "../patch.h"
#include "mod.h"
#include <set>

uint32_t *image_data_scaled_cache = nullptr;
uint32_t image_data_scaled_size_cache = 0;

// Scale 32-bit BGRA image in place
void scale_up_image_data_in_place(uint32_t *sourceAndTarget, uint32_t w, uint32_t h, uint8_t scale)
{
	if (scale <= 1)
	{
		return;
	}

	uint32_t *source = sourceAndTarget + w * h,
		*target = sourceAndTarget + (w * scale) * (h * scale);

	for (int y = 0; y < h; ++y)
	{
		uint32_t *source_line_start = source;

		for (int i = 0; i < scale; ++i)
		{
			source = source_line_start;

			for (int x = 0; x < w; ++x)
			{
				source -= 1;
				target -= scale;

				for (int i = 0; i < scale; ++i)
				{
					target[i] = *source;
				}
			}
		}
	}
}

TexturePacker::TexturePacker() :
	_vramTextureIds(VRAM_WIDTH * VRAM_HEIGHT, INVALID_TEXTURE)
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

void TexturePacker::cleanTextures(ModdedTextureId previousTextureId)
{
	auto it = _textures.find(previousTextureId);

	if (it == _textures.end())
	{
		return;
	}

	if (trace_all || trace_vram) ffnx_info("TexturePacker::%s: clear texture %s textureId=0x%X\n", __func__, it->second.printableName(), previousTextureId);

	cleanVramTextureIds(it->second.texture());

	if (it->second.mod() != nullptr)
	{
		delete it->second.mod();
	}

	for (const std::pair<ModdedTextureId, const IdentifiedTexture &> &pair: it->second.redirections())
	{
		if (pair.second.mod() != nullptr)
		{
			delete pair.second.mod();
		}
	}

	_textures.erase(it);
}

void TexturePacker::setVramTextureId(ModdedTextureId textureId, int xBpp2, int y, int wBpp2, int h, bool clearOldTexture)
{
	if (trace_all || trace_vram) ffnx_trace("%s: textureId=0x%X xBpp2=%d y=%d wBpp2=%d h=%d clearOldTexture=%d\n", __func__, textureId, xBpp2, y, wBpp2, h, clearOldTexture);

	for (int i = 0; i < h; ++i)
	{
		int vramY = y + i;

		for (int j = 0; j < wBpp2; ++j)
		{
			int vramX = xBpp2 + j;
			int key = vramX + vramY * VRAM_WIDTH;

			if (clearOldTexture)
			{
				ModdedTextureId previousTextureId = _vramTextureIds.at(key);

				if (previousTextureId != INVALID_TEXTURE)
				{
					cleanTextures(previousTextureId);
				}
			}

			_vramTextureIds[key] = textureId;
		}
	}
}

bool TexturePacker::setTexture(const char *name, const TextureInfos &texture, const TextureInfos &palette, int textureCount, bool clearOldTexture)
{
	bool hasNamedTexture = name != nullptr && *name != '\0';

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s %s xBpp2=%d y=%d wBpp2=%d h=%d bpp=%d xPal=%d yPal=%d wPal=%d hPal=%d textureCount=%d clearOldTexture=%d\n", __func__,
		hasNamedTexture ? name : "N/A",
		texture.x(), texture.y(), texture.w(), texture.h(), texture.bpp(),
		palette.x(), palette.y(), palette.w(), palette.h(), textureCount, clearOldTexture);

	ModdedTextureId textureId = makeTextureId(texture.x(), texture.y());
	setVramTextureId(textureId, texture.x(), texture.y(), texture.w(), texture.h(), clearOldTexture);

	if (palette.isValid())
	{
		setVramTextureId(makeTextureId(texture.x(), texture.y(), true), palette.x(), palette.y(), palette.w(), palette.h(), clearOldTexture);
	}

	IdentifiedTexture tex(name, texture, palette);

	if (hasNamedTexture)
	{
		TextureModStandard *mod = new TextureModStandard(tex);

		if (mod->createImages(textureCount))
		{
			tex.setMod(mod);
		}
		else
		{
			delete mod;
		}
	}

	_textures[textureId] = tex;

	return tex.mod() != nullptr;
}

bool TexturePacker::setTextureBackground(const char *name, int x, int y, int w, int h, const std::vector<Tile> &mapTiles, int bgTexId, const char *extension, char *found_extension)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s %s x=%d y=%d w=%d h=%d tileCount=%d bgTexId=%d\n", __func__, name, x, y, w, h, mapTiles.size(), bgTexId);

	ModdedTextureId textureId = makeTextureId(x, y);
	setVramTextureId(textureId, x, y, w, h);

	IdentifiedTexture tex(name, TextureInfos(x, y, w, h, Tim::Bpp16, true));

	TextureBackground *mod = new TextureBackground(tex, mapTiles, bgTexId);
	if (mod->createImages(extension, found_extension))
	{
		tex.setMod(mod);
	}
	else
	{
		delete mod;
	}

	_textures[textureId] = tex;

	return tex.mod() != nullptr;
}

bool TexturePacker::setTextureRedirection(const char *name, const TextureInfos &oldTexture, const TextureInfos &newTexture, uint32_t *imageData)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s: %s old=(%d, %d, %d, %d) <= new=(%d, %d, %d, %d)\n", __func__, name,
		oldTexture.x(), oldTexture.y(), oldTexture.w(), oldTexture.h(),
		newTexture.x(), newTexture.y(), newTexture.w(), newTexture.h());

	ModdedTextureId textureId = _vramTextureIds.at(oldTexture.x() + oldTexture.y() * VRAM_WIDTH);

	if (textureId == INVALID_TEXTURE)
	{
		ffnx_warning("TexturePacker::%s cannot find original texture of redirection\n", __func__);

		return false;
	}

	auto it = _textures.find(textureId);

	if (it == _textures.end())
	{
		ffnx_warning("TexturePacker::%s cannot find original texture of redirection 2\n", __func__);

		return false;
	}

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s: found %s\n", __func__, it->second.printableName());

	ModdedTextureId redirectionTextureId = makeTextureId(newTexture.x(), newTexture.y());
	auto it2 = it->second.redirections().find(redirectionTextureId);
	if (it2 != it->second.redirections().end() && it2->second.mod() != nullptr)
	{
		delete it2->second.mod();
	}
	IdentifiedTexture red(name, oldTexture);
	TextureModStandard *mod = new TextureModStandard(red);
	const int scaleLod = newTexture.h() / oldTexture.h(); // Force original texture size to be the one from new texture

	if (mod->createImages(1, scaleLod))
	{
		// Use external texture
		red.setMod(mod);
	}
	else
	{
		if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s: use internal texture\n", __func__);

		delete mod;
		// Use internal texture
		red.setMod(new TextureRawImage(red, imageData, newTexture.pixelW(), newTexture.h()));
	}

	_textures[textureId].setRedirection(redirectionTextureId, red);

	return true;
}

void TexturePacker::animateTextureByCopy(int sourceXBpp2, int sourceY, int sourceWBpp2, int sourceH, int targetXBpp2, int targetY)
{
	if (_textures.empty())
	{
		return;
	}

	ModdedTextureId textureId = _vramTextureIds.at(sourceXBpp2 + sourceY * VRAM_WIDTH),
		textureIdTarget = _vramTextureIds.at(targetXBpp2 + targetY * VRAM_WIDTH);
	if (textureId == INVALID_TEXTURE)
	{
		if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s pos=(%d, %d) not found\n", __func__, sourceXBpp2, sourceY);

		return;
	}

	bool inPalette = textureIdIspalette(textureId) && textureIdIspalette(textureIdTarget);

	if (inPalette)
	{
		textureId = getTextureIdWithoutFlags(textureId);
		textureIdTarget = getTextureIdWithoutFlags(textureIdTarget);
	}

	auto it = _textures.find(textureId);

	if (it == _textures.end())
	{
		return;
	}

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s matches %s inPalette=%d\n", __func__, it->second.printableName(), inPalette);

	if (textureId != textureIdTarget)
	{
		if (textureIdTarget == INVALID_TEXTURE)
		{
			return;
		}

		auto itTarget = _textures.find(textureIdTarget);

		if (itTarget == _textures.end())
		{
			return;
		}

		if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s also matches %s for target\n", __func__, itTarget->second.printableName());

		if (itTarget->second.mod() != nullptr)
		{
			if (inPalette)
			{
				dynamic_cast<TextureModStandard *>(itTarget->second.mod())->forceCurrentPalette(sourceY - it->second.palette().y());
			}
			else if (it->second.mod() != nullptr && it->second.mod()->canCopyRect())
			{
				dynamic_cast<TextureModStandard *>(it->second.mod())->copyRect(
					sourceXBpp2, sourceY, sourceWBpp2, sourceH, targetXBpp2, targetY,
					*dynamic_cast<TextureModStandard *>(itTarget->second.mod())
				);
			}
		}
	}
	else if (it->second.mod() != nullptr && it->second.mod()->canCopyRect())
	{
		if (inPalette)
		{
			dynamic_cast<TextureModStandard *>(it->second.mod())->forceCurrentPalette(sourceY - it->second.palette().y());
		}
		else
		{
			dynamic_cast<TextureModStandard *>(it->second.mod())->copyRect(
				sourceXBpp2, sourceY, sourceWBpp2, sourceH, targetXBpp2, targetY
			);
		}
	}
}

void TexturePacker::forceCurrentPalette(int xBpp2, int y, int8_t paletteId)
{
	ModdedTextureId textureId = _vramTextureIds.at(xBpp2 + y * VRAM_WIDTH);
	if (textureId == INVALID_TEXTURE)
	{
		return;
	}

	auto it = _textures.find(textureId);

	if (it == _textures.end())
	{
		return;
	}

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s matches %s\n", __func__, it->second.name());

	if (it->second.mod() != nullptr && it->second.mod()->canCopyRect())
	{
		dynamic_cast<TextureModStandard *>(it->second.mod())->forceCurrentPalette(paletteId);
	}
}

void TexturePacker::clearTiledTexs()
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s\n", __func__);

	_tiledTexs.clear();
}

void TexturePacker::clearTextures()
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s\n", __func__);

	std::fill_n(_vramTextureIds.begin(), _vramTextureIds.size(), INVALID_TEXTURE);

	for (const std::pair<ModdedTextureId, const IdentifiedTexture &> &texture: _textures) {
		if (texture.second.mod() != nullptr) {
			delete texture.second.mod();
		}
	}
	_textures.clear();
}

std::list<TexturePacker::IdentifiedTexture> TexturePacker::matchTextures(const TiledTex &tiledTex, bool withModsOnly) const
{
	std::list<IdentifiedTexture> ret;

	if (_textures.empty())
	{
		return ret;
	}

	if (!tiledTex.isValid()) {
		if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s Unknown tex data\n", __func__);

		return ret;
	}

	std::set<ModdedTextureId> textureIds;

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s looking for textures at (%d, %d, %d, %d) in VRAM\n", __func__, tiledTex.x(), tiledTex.y(), tiledTex.w(), tiledTex.h());

	for (int y = 0; y < tiledTex.h(); ++y)
	{
		int vramY = tiledTex.y() + y;

		if (vramY >= VRAM_HEIGHT)
		{
			break;
		}

		for (int x = 0; x < tiledTex.w(); ++x)
		{
			int vramX = tiledTex.x() + x;

			if (vramX >= VRAM_WIDTH)
			{
				break;
			}

			ModdedTextureId textureId = _vramTextureIds.at(vramX + vramY * VRAM_WIDTH);

			if (textureId != INVALID_TEXTURE && !textureIdIspalette(textureId))
			{
				textureIds.insert(textureId);
			}
		}
	}

	for (const ModdedTextureId &textureId: textureIds)
	{
		auto it = _textures.find(textureId);

		if (it == _textures.end())
		{
			if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s texture with textureId=0x%X not found!\n", __func__, textureId);

			continue;
		}

		const IdentifiedTexture &texture = it->second;

		if (withModsOnly)
		{
			const ModdedTexture *mod = texture.mod();
			bool hasModToDraw = mod != nullptr && mod->isBpp(tiledTex.bpp());

			if (!hasModToDraw)
			{
				for (const std::pair<ModdedTextureId, const IdentifiedTexture &> &pair: texture.redirections())
				{
					const ModdedTexture *redirectionMod = pair.second.mod();
					if (redirectionMod != nullptr && redirectionMod->isBpp(tiledTex.bpp()))
					{
						hasModToDraw = true;
						break;
					}
				}

				if (!hasModToDraw)
				{
					if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s ignore matches %s because not modded rect=(%d, %d, %d, %d) bpp=%d (tiledTexBpp=%d)\n", __func__, texture.printableName(), texture.texture().x(), texture.texture().y(), texture.texture().w(), texture.texture().h(), texture.texture().bpp(), tiledTex.bpp());

					continue;
				}
			}
		}
		else if (!texture.texture().hasMultiBpp() && texture.texture().bpp() != tiledTex.bpp())
		{
			if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s ignore matches %s because not right bpp rect=(%d, %d, %d, %d) bpp=%d (tiledTexBpp=%d)\n", __func__, texture.printableName(), texture.texture().x(), texture.texture().y(), texture.texture().w(), texture.texture().h(), texture.texture().bpp(), tiledTex.bpp());

			continue;
		}

		if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s matches %s rect=(%d, %d, %d, %d)\n", __func__, texture.printableName(), texture.texture().x(), texture.texture().y(), texture.texture().w(), texture.texture().h());

		ret.push_back(texture);
	}

	return ret;
}

int textureId = 0;

TexturePacker::TextureTypes TexturePacker::drawTextures(
	const uint8_t *texData, uint32_t *rgbaImageData, uint32_t dataSize, int originalW, int originalH,
	int palIndex, uint8_t *outScale, uint32_t **outTarget) const
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s texData=0x%X originalSize=(%d, %d) palIndex=%d\n", __func__, texData, originalW, originalH, palIndex);

	textureId++;

	*outScale = 1;
	*outTarget = rgbaImageData;

	if (_textures.empty())
	{
		if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s No textures to draw\n", __func__);

		return NoTexture;
	}

	auto it = _tiledTexs.find(texData);

	if (it == _tiledTexs.end())
	{
		if (trace_all || trace_vram) ffnx_error("TexturePacker::%s Unknown tiledTex data\n", __func__);

		return NoTexture;
	}

	const TiledTex &tiledTex = it->second;

	if (!tiledTex.isValid())
	{
		if (trace_all || trace_vram) ffnx_error("TexturePacker::%s Invalid tiledTex data\n", __func__);

		return NoTexture;
	}

	if (!tiledTex.isPaletteValid(palIndex))
	{
		if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s Palette is not set yet, we should not consider this texture\n", __func__);

		return NoTexture;
	}

	std::list<IdentifiedTexture> textures = matchTextures(tiledTex, true);

	if (textures.empty())
	{
		if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s No matched textures at (%d, %d)\n", __func__, tiledTex.x(), tiledTex.y());

		return NoTexture;
	}

	TextureInfos palette = tiledTex.palette(palIndex);
	uint8_t scale = 0;

	for (const IdentifiedTexture &tex: textures)
	{
		for (const std::pair<ModdedTextureId, const IdentifiedTexture &> &pair: tex.redirections())
		{
			if (pair.second.mod() != nullptr)
			{
				scale = std::max(scale, pair.second.mod()->scale(palette.x(), palette.y()));
			}
		}

		if (tex.mod() != nullptr)
		{
			scale = std::max(scale, tex.mod()->scale(palette.x(), palette.y()));
		}
	}

	uint32_t *target = rgbaImageData;

	if (scale > 1)
	{
		uint32_t image_data_size = originalW * scale * originalH * scale * 4;
		// Allocate with cache
		if (image_data_scaled_size_cache == 0 || image_data_size > image_data_scaled_size_cache) {
			if (image_data_scaled_cache != nullptr) {
				driver_free(image_data_scaled_cache);
			}
			image_data_scaled_cache = (uint32_t *)driver_malloc(image_data_size);
			image_data_scaled_size_cache = image_data_size;
		}

		target = image_data_scaled_cache;

		// convert source data
		if (target != nullptr)
		{
			memcpy(target, rgbaImageData, dataSize);
			scale_up_image_data_in_place(target, originalW, originalH, scale);
		}
	}
	else if (scale == 0)
	{
		return TextureTypes::NoTexture;
	}

	*outScale = scale;
	*outTarget = target;

	TextureTypes ret = drawTextures(textures, tiledTex, palette, target, originalW, originalH, scale);

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s tex=(%d, %d) bpp=%d paletteVram=(%d, %d) drawnTextureTypes=0x%X\n", __func__, tiledTex.x(), tiledTex.y(), tiledTex.bpp(), palette.x(), palette.y(), ret);

	return ret;
}

TexturePacker::TextureTypes TexturePacker::drawTextures(const std::list<IdentifiedTexture> &textures, const TiledTex &tiledTex, const TextureInfos &palette, uint32_t *target, int targetW, int targetH, uint8_t scale) const
{
	TextureTypes drawnTextureTypes = NoTexture;
	int x = tiledTex.pixelX(), y = tiledTex.y();

	for (const IdentifiedTexture &texture: textures)
	{
		for (const std::pair<ModdedTextureId, const IdentifiedTexture &> &pair: texture.redirections())
		{
			if (pair.second.mod() != nullptr && pair.second.mod()->isInternal())
			{
				TextureTypes textureType = pair.second.mod()->drawToImage(
					pair.second.texture().pixelX() - x, pair.second.texture().y() - y,
					target, targetW, targetH, scale, tiledTex.bpp(),
					palette.x(), palette.y()
				);

				drawnTextureTypes = TextureTypes(int(drawnTextureTypes) | int(textureType));
			}
		}

		if (texture.mod() != nullptr)
		{
			TextureTypes textureType = texture.mod()->drawToImage(
				texture.texture().pixelX() - x, texture.texture().y() - y,
				target, targetW, targetH, scale, tiledTex.bpp(),
				palette.x(), palette.y()
			);

			drawnTextureTypes = TextureTypes(int(drawnTextureTypes) | int(textureType));
		}

		for (const std::pair<ModdedTextureId, const IdentifiedTexture &> &pair: texture.redirections())
		{
			if (pair.second.mod() != nullptr && !pair.second.mod()->isInternal())
			{
				TextureTypes textureType = pair.second.mod()->drawToImage(
					pair.second.texture().pixelX() - x, pair.second.texture().y() - y,
					target, targetW, targetH, scale, tiledTex.bpp(),
					palette.x(), palette.y()
				);

				drawnTextureTypes = TextureTypes(int(drawnTextureTypes) | int(textureType));
			}
		}
	}

	return drawnTextureTypes;
}

void TexturePacker::registerTiledTex(const uint8_t *texData, int x, int y, Tim::Bpp bpp, int palX, int palY)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s pointer=0x%X x=%d y=%d bpp=%d palX=%d palY=%d\n", __func__, texData, x, y, bpp, palX, palY);

	// If this entry already exist, override
	_tiledTexs[texData] = TiledTex(x, y, bpp, palX, palY);
}

void TexturePacker::registerPaletteWrite(const uint8_t *texData, int palIndex, int palX, int palY)
{
	if (trace_all || trace_vram) ffnx_trace("%s pointer=0x%X palIndex=%d palX=%d palY=%d\n", __func__, texData, palIndex, palX, palY);

	if (_tiledTexs.contains(texData))
	{
		TiledTex &tex = _tiledTexs[texData];
		tex.palettes[palIndex] = TextureInfos(palX, palY, 256, 1, Tim::Bpp16);
	}
	else
	{
		if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s pointer=0x%X register palette before image\n", __func__, texData);

		_tiledTexs[texData] = TiledTex(-1, -1, Tim::Bpp4, palX, palY);
	}
}

TexturePacker::TiledTex TexturePacker::getTiledTex(const uint8_t *texData) const
{
	auto it = _tiledTexs.find(texData);

	if (it != _tiledTexs.end())
	{
		return it->second;
	}

	return TiledTex();
}

void TexturePacker::debugSaveTexture(int textureId, const uint32_t *source, int w, int h, bool removeAlpha, bool after, TextureTypes textureType)
{
	uint32_t *target = new uint32_t[w * h];

	for (int i = 0; i < h * w; ++i)
	{
		target[i] = removeAlpha ? source[i] | 0xff000000 : source[i]; // Remove alpha
	}

	char filename[MAX_PATH];
	snprintf(filename, sizeof(filename), "texture-%d-%s-type-%s", textureId, after ? "z-after" : "a-before", textureType == TextureTypes::ExternalTexture ? "external" : (textureType == TextureTypes::InternalTexture ? "internal" : "nomatch"));

	if (trace_all || trace_vram) ffnx_trace("%s %s\n", __func__, filename);

	save_texture(target, w * h * 4, w, h, -1, filename, false);

	delete[] target;
}

TexturePacker::TextureInfos::TextureInfos() :
	_x(-1), _y(0), _w(0), _h(0), _bpp(Tim::Bpp4)
{
}

TexturePacker::TextureInfos::TextureInfos(
	int xBpp2, int y, int wBpp2, int h,
	Tim::Bpp bpp, bool multiBpp
) : _x(xBpp2), _y(y), _w(wBpp2), _h(h), _bpp(bpp), _multiBpp(multiBpp)
{
}

TexturePacker::TiledTex::TiledTex()
 : TextureInfos()
{
}

TexturePacker::TiledTex::TiledTex(
	int x, int y, Tim::Bpp bpp, int palVramX, int palVramY
) : TextureInfos(x, y, 256 / (4 >> int(bpp)), 256, bpp)
{
	if (palVramX >= 0) {
		palettes[0] = TextureInfos(palVramX, palVramY, 256, 1, Tim::Bpp16);
	}
}

TexturePacker::IdentifiedTexture::IdentifiedTexture() :
	_texture(TextureInfos()), _palette(TextureInfos()), _name(""), _mod(nullptr)
{
}

TexturePacker::IdentifiedTexture::IdentifiedTexture(
	const char *name,
	const TextureInfos &texture,
	const TextureInfos &palette
) : _texture(texture), _palette(palette), _name(name == nullptr ? "" : name), _mod(nullptr)
{
}

void TexturePacker::IdentifiedTexture::setMod(ModdedTexture *mod)
{
	if (_mod != nullptr)
	{
		delete _mod;
	}

	_mod = mod;
}

void TexturePacker::IdentifiedTexture::setRedirection(ModdedTextureId textureId, const IdentifiedTexture &redirection)
{
	_redirections[textureId] = redirection;
}
