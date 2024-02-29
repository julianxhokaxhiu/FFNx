/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
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

#include <map>
#include <vector>
#include <unordered_map>
#include <bimg/bimg.h>
#include <bx/allocator.h>

#include "texture_packer.h"
#include "field/background.h"

class TextureImage {
public:
	TextureImage();
	bool createImage(const char *filename, int originalTexturePixelWidth, int originalTextureHeight, int internalLodScale = 1);
	void destroyImage();
	inline uint8_t scale() const {
		return _scale;
	}
	inline void setScale(uint8_t scale) {
		_scale = scale;
	}
	inline bool hasImage() const {
		return _image != nullptr;
	}
	inline const bimg::ImageMip &mip() const {
		return _mip;
	}
	static int computeMaxScale();
private:
	void setLod(uint8_t lod);
	uint8_t computeLod(int originalTexturePixelWidth, int imageWidth, int numMips, int internalScale = 1, const char *filename = "") const;
	uint8_t computeScale(int originalTexturePixelWidth, int originalTextureHeight, const char *filename = "") const;
	bimg::ImageContainer *_image;
	bimg::ImageMip _mip;
	uint8_t _scale;

	static bx::DefaultAllocator defaultAllocator;
};

class ModdedTexture {
public:
	ModdedTexture(const TexturePacker::IdentifiedTexture &originalTexture, bool isInternal = false);
	ModdedTexture(const ModdedTexture &other) = delete;
	virtual ~ModdedTexture() {}
	virtual bool isBpp(Tim::Bpp bpp) const {
		return bpp == _originalTexture.texture().bpp();
	}
	virtual uint8_t scale(int vramPalXBpp2, int vramPalY) const=0;
	inline virtual bool canCopyRect() const {
		return false;
	}
	inline bool isInternal() const {
		return _isInternal;
	}
	virtual TexturePacker::TextureTypes drawToImage(
		int offsetX, int offsetY,
		uint32_t *targetRgba, int targetW, int targetH, uint8_t targetScale, Tim::Bpp targetBpp,
		int16_t paletteVramX, int16_t paletteVramY
	) const=0;
protected:
	static bool findExternalTexture(const char *name, char *outFilename, uint8_t palette_index, bool hasPal, const char *extension = nullptr, char *foundExtension = nullptr);
	static void drawImage(
		const uint32_t *sourceRgba, int sourceRgbaW, uint8_t sourceScale,
		uint32_t *targetRgba, int targetRgbaW, uint8_t targetScale,
		int sourceX, int sourceY, int sourceW, int sourceH,
		int targetX, int targetY
	);
	static void copyRect(
		const uint32_t *sourceRgba, int sourceRgbaW, uint32_t *targetRgba, int targetRgbaW, uint8_t scale, Tim::Bpp depth,
		int sourceXBpp2, int sourceY, int sourceWBpp2, int sourceH,
		int targetXBpp2, int targetY
	);
	inline const TexturePacker::IdentifiedTexture &originalTexture() const {
		return _originalTexture;
	}
private:
	TexturePacker::IdentifiedTexture _originalTexture;
	bool _isInternal;
};

class TextureModStandard : public ModdedTexture {
public:
	TextureModStandard(const TexturePacker::IdentifiedTexture &originalTexture) : ModdedTexture(originalTexture), _currentPalette(-1) {}
	TextureModStandard(const TextureModStandard &other) = delete;
	~TextureModStandard();
	inline bool canCopyRect() const override {
		return true;
	}
	bool createImages(int paletteCount, int internalLodScale = 1);
	uint8_t scale(int vramPalXBpp2, int vramPalY) const override;
	TexturePacker::TextureTypes drawToImage(
		int offsetX, int offsetY,
		uint32_t *targetRgba, int targetW, int targetH, uint8_t targetScale, Tim::Bpp targetBpp,
		int16_t vramPalXBpp2, int16_t vramPalY
	) const override;
	// Copy rect inside the textures
	void copyRect(int sourceXBpp2, int sourceY, int sourceWBpp2, int sourceH, int targetXBpp2, int targetY);
	// Copy rect to another textures
	void copyRect(int sourceXBpp2, int sourceY, int sourceWBpp2, int sourceH, int targetXBpp2, int targetY, const TextureModStandard &targetTexture);
	// Set to -1 to unforce
	void forceCurrentPalette(int8_t currentPalette);
private:
	bool createImage(const char *name, int paletteId = -1, const char *extension = nullptr, char *foundExtension = nullptr);
	uint8_t computePaletteId(int vramPalXBpp2, int vramPalY) const;
	const TextureImage &textureImage(uint8_t paletteId) const;
	std::map<uint8_t, TextureImage> _textures; // Index: paletteId or current texture
	int8_t _currentPalette;
};

class TextureBackground : public ModdedTexture {
public:
	TextureBackground(
		const TexturePacker::IdentifiedTexture &originalTexture,
		const std::vector<Tile> &mapTiles,
		int vramPageId
	);
	TextureBackground(const TextureBackground &other) = delete;
	~TextureBackground();
	bool isBpp(Tim::Bpp bpp) const override;
	uint8_t scale(int vramPalXBpp2, int vramPalY) const override {
		return _texture.scale();
	}
	bool createImages(const char *extension = nullptr, char *foundExtension = nullptr);
	TexturePacker::TextureTypes drawToImage(
		int offsetX, int offsetY,
		uint32_t *targetRgba, int targetW, int targetH, uint8_t targetScale, Tim::Bpp targetBpp,
		int16_t vramPalXBpp2, int16_t vramPalY
	) const override;
private:
	std::vector<Tile> _mapTiles;
	std::unordered_multimap<uint16_t, size_t> _tileIdsByTextureId;
	TextureImage _texture;
	int _vramPageId;
	uint8_t _colsCount, _usedBpps;
};

struct TextureRawImage : public ModdedTexture {
	TextureRawImage(
		const TexturePacker::IdentifiedTexture &originalTexture,
		uint32_t *imageData, int imageWidth, int imageHeight
	);
	TextureRawImage(const TextureRawImage &other) = delete;
	~TextureRawImage();
	inline bool isValid() const {
		return _scale != 0;
	}
	uint8_t scale(int vramPalXBpp2, int vramPalY) const override {
		return _scale;
	}
	TexturePacker::TextureTypes drawToImage(
		int offsetX, int offsetY,
		uint32_t *targetRgba, int targetW, int targetH, uint8_t targetScale, Tim::Bpp targetBpp,
		int16_t vramPalXBpp2, int16_t vramPalY
	) const override;
private:
	uint8_t computeScale() const;
	uint32_t *_image;
	int _width, _height;
	uint8_t _scale;
};
