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

#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <bimg/bimg.h>

#include "../ff8.h"
#include "../image/tim.h"
#include "field/background.h"

typedef uint32_t ModdedTextureId;

#define VRAM_WIDTH 1024
#define VRAM_HEIGHT 512
#define VRAM_DEPTH 2
#define INVALID_TEXTURE ModdedTextureId(0xFFFFFFFF)
#define MAX_SCALE 10

class TexturePacker {
public:
	class TextureInfos {
	public:
		TextureInfos();
		TextureInfos(
			int x, int y, int w, int h,
			Tim::Bpp bpp
		);
		inline int x() const {
			return _x;
		}
		inline int y() const {
			return _y;
		}
		inline int w() const {
			return _w;
		}
		inline int pixelW() const {
			return _w * (4 >> _bpp);
		}
		inline int h() const {
			return _h;
		}
		inline Tim::Bpp bpp() const {
			return _bpp;
		}
	protected:
		static bimg::ImageContainer *createImageContainer(const char *name, uint8_t palette_index, bool hasPal);
		static uint8_t computeScale(int sourcePixelW, int sourceH, int targetPixelW, int targetH);
		static void copyRect(
			const uint32_t *sourceRGBA, int sourceXBpp2, int sourceYBpp2, int sourceW, uint8_t sourceScale, Tim::Bpp sourceDepth,
			uint32_t *targetRGBA, int targetX, int targetY, int targetW, uint8_t targetScale
		);
	private:
		int _x, _y;
		int _w, _h;
		Tim::Bpp _bpp;
	};

	enum TextureTypes {
		NoTexture = 0,
		ExternalTexture = 1,
		InternalTexture = 2
	};

	explicit TexturePacker();
	inline void setVram(uint8_t *vram) {
		_vram = vram;
	}
	void uploadTexture(const uint8_t *texture, int x, int y, int w, int h);
	void setTexture(const char *name, int x, int y, int w, int h, Tim::Bpp bpp, bool isPal);
	void setTextureBackground(const char *name, int x, int y, int w, int h, const std::vector<Tile> &mapTiles);
	// Override a part of the VRAM from another part of the VRAM, typically with biggest textures (Worldmap)
	bool setTextureRedirection(const TextureInfos &oldTexture, const TextureInfos &newTexture, uint32_t *imageData);
	uint8_t getMaxScale(const uint8_t *texData) const;
	void getTextureNames(const uint8_t *texData, std::list<std::string> &names) const;
	void registerTiledTex(const uint8_t *texData, int x, int y, Tim::Bpp bpp, int palX = 0, int palY = 0);

	void disableDrawTexturesBackground(bool disabled);
	TextureTypes drawTextures(const uint8_t *texData, struct texture_format *tex_format, uint32_t *target, const uint32_t *originalImageData, int originalW, int originalH, uint8_t scale, uint32_t paletteIndex);

	bool saveVram(const char *fileName, Tim::Bpp bpp) const;
private:
	inline uint8_t *vramSeek(int x, int y) const {
		return _vram + VRAM_DEPTH * (x + y * VRAM_WIDTH);
	}
	inline static ModdedTextureId makeTextureId(int x, int y) {
		return x + y * VRAM_WIDTH;
	}
	class Texture : public TextureInfos {
	public:
		Texture();
		Texture(
			const char *name,
			int x, int y, int w, int h,
			Tim::Bpp bpp
		);
		inline const std::string &name() const {
			return _name;
		}
		inline uint8_t scale() const {
			return _scale;
		}
		bool createImage(uint8_t palette_index = 0, bool has_pal = true);
		void destroyImage();
		inline bool hasImage() const {
			return _image != nullptr;
		}
		inline bool isValid() const {
			return _scale != 0;
		}
		void copyRect(int sourceXBpp2, int sourceYBpp2, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const;
	protected:
		const bimg::ImageContainer *image() const {
			return _image;
		}
		virtual uint8_t computeScale() const;
	private:
		bimg::ImageContainer *_image;
		std::string _name;
		uint8_t _scale;
	};
	class TextureBackground : public Texture {
	public:
		TextureBackground();
		TextureBackground(
			const char *name,
			int x, int y, int w, int h,
			const std::vector<Tile> &mapTiles
		);
		bool createImage() {
			return Texture::createImage(0, false);
		}
		void copyRect(int sourceXBpp2, int sourceYBpp2, Tim::Bpp textureBpp, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const;
	private:
		virtual uint8_t computeScale() const override;
		std::vector<Tile> _mapTiles;
		std::unordered_multimap<uint16_t, size_t> _tileIdsByPosition;
		uint8_t _colsCount;
	};
	struct TiledTex {
		TiledTex();
		TiledTex(int x, int y, Tim::Bpp bpp, int palX, int palY);
		int x, y;
		int palX, palY;
		Tim::Bpp bpp;
		bool renderedOnce;
	};
	struct TextureRedirection : public TextureInfos {
		TextureRedirection();
		TextureRedirection(
			const TextureInfos &oldTexture,
			const TextureInfos &newTexture
		);
		inline bool isValid() const {
			return _scale != 0;
		}
		inline uint8_t scale() const {
			return _scale;
		}
		bool createImage(uint32_t *imageData);
		void destroyImage();
		inline uint32_t *imageData() {
			return _image;
		}
		inline const TextureInfos &oldTexture() const {
			return _oldTexture;
		}
		void copyRect(int textureX, int textureY, uint32_t *target, int targetX, int targetY, int targetW, uint8_t targetScale) const;
	private:
		uint8_t computeScale() const;
		uint32_t *_image;
		TextureInfos _oldTexture;
		uint8_t _scale;
	};
	void setVramTextureId(ModdedTextureId textureId, int x, int y, int w, int h, bool keepMods = false);
	TextureTypes drawTextures(uint32_t *target, const TiledTex &tiledTex, int w, int h, uint8_t scale, uint32_t paletteIndex);
	void cleanVramTextureIds(const TextureInfos &texture);
	void cleanTextures(ModdedTextureId textureId, bool keepMods = false);

	uint8_t *_vram; // uint16_t[VRAM_WIDTH * VRAM_HEIGHT] aka uint8_t[VRAM_WIDTH * VRAM_HEIGHT * VRAM_DEPTH]
	std::unordered_map<const uint8_t *, TiledTex> _tiledTexs;
	std::vector<ModdedTextureId> _vramTextureIds; // ModdedTextureId[VRAM_WIDTH * VRAM_HEIGHT]
	std::unordered_map<ModdedTextureId, Texture> _textures;
	std::unordered_map<ModdedTextureId, Texture> _externalTextures;
	std::unordered_map<ModdedTextureId, TextureRedirection> _textureRedirections;
	std::unordered_map<ModdedTextureId, TextureBackground> _backgroundTextures;

	bool _disableDrawTexturesBackground;
};
