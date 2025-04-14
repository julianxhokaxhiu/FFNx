/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2025 Julian Xhokaxhiu                                   //
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

#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <map>

#include "../ff8.h"
#include "../image/tim.h"
#include "field/background.h"

typedef uint32_t ModdedTextureId;

constexpr int VRAM_WIDTH = 1024;
constexpr int VRAM_HEIGHT = 512;
constexpr int VRAM_DEPTH = 2;
constexpr ModdedTextureId INVALID_TEXTURE = ModdedTextureId(0xFFFFFFFF);
constexpr int MAX_SCALE = 128;
constexpr int FF8_BASE_RESOLUTION_X = 320;

class ModdedTexture;

class TexturePacker {
public:
	class TextureInfos {
	public:
		TextureInfos();
		TextureInfos(
			int x, int y, int w, int h,
			Tim::Bpp bpp, bool multiBpp = false
		);
		inline bool isValid() const {
			return _x >= 0;
		}
		inline int x() const {
			return _x;
		}
		inline int pixelX() const {
			return _x * (4 >> int(_bpp));
		}
		inline int y() const {
			return _y;
		}
		inline int w() const {
			return _w;
		}
		inline int pixelW() const {
			return _w * (4 >> int(_bpp));
		}
		inline int h() const {
			return _h;
		}
		inline Tim::Bpp bpp() const {
			return _bpp;
		}
		inline bool hasMultiBpp() const {
			return _multiBpp;
		}
		int vramId() const;
	private:
		int _x, _y;
		int _w, _h;
		Tim::Bpp _bpp;
		bool _multiBpp;
	};

	struct TiledTex : public TextureInfos {
		TiledTex();
		TiledTex(int x, int y, int w, int h, Tim::Bpp bpp, int palVramX = -1, int palVramY = -1);
		inline bool isPaletteValid(int palIndex) const {
			return bpp() == Tim::Bpp16 || (palettes.contains(palIndex) && palettes.at(palIndex).isValid());
		}
		inline TextureInfos palette(int palIndex) const {
			return bpp() == Tim::Bpp16 || !palettes.contains(palIndex) ? TextureInfos() : palettes.at(palIndex);
		}
		std::map<uint8_t, TextureInfos> palettes;
	};

	class IdentifiedTexture {
	public:
		IdentifiedTexture();
		IdentifiedTexture(
			const char *name,
			const TextureInfos &texture,
			const TextureInfos &palette = TextureInfos()
		);
		void setMod(ModdedTexture *mod);
		inline ModdedTexture *mod() const {
			return _mod;
		}
		void setRedirection(ModdedTextureId textureId, const IdentifiedTexture &redirection);
		const std::unordered_map<ModdedTextureId, IdentifiedTexture> &redirections() const {
			return _redirections;
		}
		inline const TextureInfos &texture() const {
			return _texture;
		}
		inline const TextureInfos &palette() const {
			return _palette;
		}
		inline const std::string &name() const {
			return _name;
		}
		inline const char *printableName() const {
			return _name.empty() ? "N/A" : _name.c_str();
		}
		inline bool isValid() const {
			return !_name.empty();
		}
		inline bool isAnimated() const {
			return _isAnimated;
		}
		void setCurrentAnimationFrame(int frameId);
		void setCurrentAnimationFrame(int xBpp2, int y, int wBpp2, int h);
		inline int currentAnimationFrame() const {
			return _frameId;
		}
	private:
		TextureInfos _texture, _palette;
		std::string _name;
		ModdedTexture *_mod;
		std::unordered_map<ModdedTextureId, IdentifiedTexture> _redirections;
		int _frameId;
		std::vector<uint64_t> _frames;
		bool _isAnimated;
	};

	enum TextureTypes {
		NoTexture = 0,
		ExternalTexture = 1,
		InternalTexture = 2
	};

	explicit TexturePacker();
	bool setTexture(const char *name, const TextureInfos &texture, const TextureInfos &palette = TextureInfos(), int textureCount = -1, bool clearOldTexture = true);
	bool setTextureBackground(const char *name, int x, int y, int w, int h, const std::vector<Tile> &mapTiles, const char *extension = nullptr, char *found_extension = nullptr);
	// Override a part of the VRAM from another part of the VRAM, typically with biggest textures (Worldmap)
	bool setTextureRedirection(const char *name, const TextureInfos &oldTexture, const TextureInfos &newTexture, const Tim &tim);
	void animateTextureByCopy(int sourceXBpp2, int y, int sourceWBpp2, int sourceH, int targetXBpp2, int targetY);
	void setCurrentAnimationFrame(int xBpp2, int y, int8_t frameId);
	void clearTiledTexs();
	void clearTextures();
	// Returns the textures matching the tiledTex
	std::list<IdentifiedTexture> matchTextures(const TiledTex &tiledTex, bool withModsOnly = false, bool withAnimatedOnly = false) const;
	const TiledTex &registerTiledTex(const uint8_t *texData, int xBpp2, int y, int pixelW, int h, Tim::Bpp sourceBpp, int palX = -1, int palY = -1);
	void registerPaletteWrite(const uint8_t *texData, int palIndex, int palX, int palY);
	TiledTex getTiledTex(const uint8_t *texData) const;

	uint32_t composeTextures(
		const uint8_t *texData, uint32_t *rgbaImageData, int originalW, int originalH,
		int palIndex, uint32_t* width, uint32_t* height, struct gl_texture_set* gl_set, bool *isExternal
	) const;

	static void debugSaveTexture(int textureId, const uint32_t *source, int w, int h, bool removeAlpha = true, bool after = false, TextureTypes textureType = NoTexture);
private:
	inline static ModdedTextureId makeTextureId(int xBpp2, int y, bool isPal = false) {
		return (xBpp2 + y * VRAM_WIDTH) | (isPal << 31);
	}
	inline static bool textureIdIspalette(ModdedTextureId textureId) {
		return (textureId & 0x80000000) != 0;
	}
	inline static ModdedTextureId getTextureIdWithoutFlags(ModdedTextureId textureId) {
		return textureId & 0x7FFFFFFF;
	}
	inline static int getWidthFromTextureId(ModdedTextureId textureId) {
		return getTextureIdWithoutFlags(textureId) % VRAM_WIDTH;
	}
	inline static int getHeightFromTextureId(ModdedTextureId textureId) {
		return getTextureIdWithoutFlags(textureId) / VRAM_WIDTH;
	}

	void setVramTextureId(ModdedTextureId textureId, int x, int y, int w, int h, bool clearOldTexture = true);
	uint8_t getMaxScale(const TiledTex &tiledTex) const;
	TextureTypes drawTextures(const std::list<IdentifiedTexture> &textures, const TiledTex &tiledTex, const TextureInfos &palette, uint32_t *target, int w, int h, uint8_t scale) const;
	void cleanVramTextureIds(const TextureInfos &texture);
	void cleanTextures(ModdedTextureId textureId, int xBpp2, int y, int wBpp2, int h);

	// Link between texture data pointer sent to the graphic driver and VRAM coordinates
	std::unordered_map<const uint8_t *, TiledTex> _tiledTexs;
	// Keep track of where textures are uploaded to the VRAM
	std::vector<ModdedTextureId> _vramTextureIds; // ModdedTextureId[VRAM_WIDTH * VRAM_HEIGHT]
	// List of uploaded textures to the VRAM
	std::unordered_map<ModdedTextureId, IdentifiedTexture> _textures;
};
