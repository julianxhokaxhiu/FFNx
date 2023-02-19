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

#include <stdint.h>
#include "../ff8.h"

inline uint32_t fromR5G5B5Color(uint16_t color, bool withAlpha = false)
{
	uint8_t r = color & 0x1F,
		g = (color >> 5) & 0x1F,
		b = (color >> 10) & 0x1F;

	return ((color == 0 && withAlpha ? 0x00 : 0xffu) << 24) |
		((((r << 3) + (r >> 2)) & 0xffu) << 16) |
		((((g << 3) + (g >> 2)) & 0xffu) << 8) |
		(((b << 3) + (b >> 2)) & 0xffu);
}

class Tim;

class PaletteDetectionStrategy {
public:
	PaletteDetectionStrategy(const Tim *const tim) : _tim(tim) {};
	virtual bool isValid() const {
		return true;
	}
	virtual uint32_t palOffset(uint16_t x, uint16_t y) const = 0;
	virtual uint32_t palIndex() const = 0;
protected:
	const Tim *const _tim;
};

// One palette at (x, y)
class PaletteDetectionStrategyFixed : public PaletteDetectionStrategy {
public:
	PaletteDetectionStrategyFixed(const Tim *const tim, uint16_t palX, uint16_t palY) :
		PaletteDetectionStrategy(tim), _palX(palX), _palY(palY) {}
	virtual uint32_t palOffset(uint16_t imgX, uint16_t imgY) const override;
	virtual uint32_t palIndex() const override;
private:
	uint16_t _palX, _palY;
};

// A grid of fixed size cells, with one palette per cell
class PaletteDetectionStrategyGrid : public PaletteDetectionStrategy {
public:
	enum Direction {
		TopBottomLeftRight,
		LeftRightTopBottom
	};
	PaletteDetectionStrategyGrid(const Tim *const tim, uint8_t cellCols, uint8_t cellRows, uint16_t colorsPerPal, uint8_t palColsPerRow);
	virtual bool isValid() const override;
	virtual uint32_t palOffset(uint16_t imgX, uint16_t imgY) const override;
	virtual uint32_t palIndex() const override;
private:
	uint8_t _cellCols, _cellRows;
	uint16_t _cellWidth, _cellHeight;
	uint16_t _colorsPerPal;
	uint8_t _palColsPerRow;
	uint8_t _palCols;
};

class Tim {
	friend class PaletteDetectionStrategyFixed;
	friend class PaletteDetectionStrategyGrid;
public:
	enum Bpp {
		Bpp4 = 0,
		Bpp8 = 1,
		Bpp16 = 2
	};

	Tim(Bpp bpp, const ff8_tim &tim);
	uint16_t colorsPerPal() const;
	inline Bpp bpp() const {
		return _bpp;
	}
	inline uint16_t imageX() const {
		return _tim.img_x;
	}
	inline uint16_t imageY() const {
		return _tim.img_y;
	}
	inline uint16_t imageWidth() const {
		return _tim.img_w;
	}
	inline uint16_t imageHeight() const {
		return _tim.img_h;
	}
	inline uint16_t paletteX() const {
		return _tim.pal_x;
	}
	inline uint16_t paletteY() const {
		return _tim.pal_y;
	}
	inline uint16_t paletteWidth() const {
		return _tim.pal_w;
	}
	inline uint16_t paletteHeight() const {
		return _tim.pal_h;
	}
	bool save(const char *fileName, uint8_t palX = 0, uint8_t palY = 0, bool withAlpha = false) const;
	bool saveMultiPaletteGrid(
		const char *fileName, uint8_t cellCols, uint8_t cellRows, uint8_t colorsPerPal = 0, uint8_t palColsPerRow = 1,
		bool withAlpha = false
	) const;
	bool toRGBA32(uint32_t *target, uint8_t palX = 0, uint8_t palY = 0, bool withAlpha = false) const;
	bool toRGBA32MultiPaletteGrid(
		uint32_t *target, uint8_t cellCols, uint8_t cellRows, uint8_t colorsPerPal = 0, uint8_t palColsPerRow = 1,
		bool withAlpha = false
	) const;
	static Tim fromLzsData(const uint8_t *uncompressed_data);
	static Tim fromTimData(const uint8_t *data);
private:
	bool save(const char *fileName, PaletteDetectionStrategy *paletteDetectionStrategy, bool withAlpha) const;
	bool toRGBA32(uint32_t *target, PaletteDetectionStrategy *paletteDetectionStrategy, bool withAlpha) const;
	ff8_tim _tim;
	Bpp _bpp;
};
