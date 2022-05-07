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

#include "tim.h"

#include "../common.h"
#include "../log.h"
#include "../saveload.h"

Tim::Tim(uint8_t bpp, const ff8_tim &tim) :
	_bpp(bpp), _tim(tim)
{
	_tim.img_w *= 4 >> bpp;
}

uint32_t PaletteDetectionStrategyFixed::palOffset(uint16_t, uint16_t) const
{
	return _palX + _palY * _tim->_tim.pal_w;
}

uint32_t PaletteDetectionStrategyFixed::palIndex() const
{
	return palOffset(0, 0);
}

bool Tim::save(const char *fileName, uint8_t palX, uint8_t palY, bool withAlpha) const
{
	PaletteDetectionStrategyFixed fixed(this, palX, palY);
	return fixed.isValid() && save(fileName, &fixed, withAlpha);
}

bool Tim::toRGBA32(uint32_t *target, uint8_t palX, uint8_t palY, bool withAlpha) const
{
	PaletteDetectionStrategyFixed fixed(this, palX, palY);
	return fixed.isValid() && toRGBA32(target, &fixed, withAlpha);
}

PaletteDetectionStrategyGrid::PaletteDetectionStrategyGrid(const Tim *const tim, uint8_t cellCols, uint8_t cellRows, uint16_t colorsPerPal, uint8_t palColsPerRow) :
	PaletteDetectionStrategy(tim), _cellCols(cellCols), _cellRows(cellRows), _colorsPerPal(colorsPerPal), _palColsPerRow(palColsPerRow)
{
	if (_colorsPerPal == 0)
	{
		_colorsPerPal = tim->bpp() == 0 ? 16 : 256;
	}
	_palCols = tim->_tim.pal_w / _colorsPerPal;
	_cellWidth = tim->_tim.img_w / _cellCols;
	_cellHeight = tim->_tim.img_h / _cellRows;
}

bool PaletteDetectionStrategyGrid::isValid() const
{
	if (_tim->_bpp >= 2)
	{
		ffnx_error("PaletteDetectionStrategyGrid::%s bpp should not be 2\n", __func__);
		return false;
	}

	if (_tim->_tim.img_w % _cellCols != 0)
	{
		ffnx_error("PaletteDetectionStrategyGrid::%s img_w=%d mod cellCols=%d != 0\n", __func__, _tim->_tim.img_w, _cellCols);
		return false;
	}

	if (_tim->_tim.pal_h * _palCols != _cellCols * _cellRows)
	{
		ffnx_error("PaletteDetectionStrategyGrid::%s not enough palette for this image %d (%d * %d)\n", __func__, _palCols, _tim->_tim.pal_w, _tim->_tim.pal_h);
		return false;
	}

	return true;
}

uint32_t PaletteDetectionStrategyGrid::palOffset(uint16_t imgX, uint16_t imgY) const
{
	// Direction: top to bottom then left to right
	uint16_t cellX = imgX / _cellWidth, cellY = imgY / _cellHeight;
	int palId = (cellX % _palColsPerRow) + cellY * _palColsPerRow + (cellX / _palColsPerRow) * (_cellRows * _palColsPerRow);
	uint16_t palX = (palId % _palCols) * _colorsPerPal, palY = palId / _palCols;

	return palX + palY * _tim->_tim.pal_w;
}

uint32_t PaletteDetectionStrategyGrid::palIndex() const
{
	return 0;
}

bool Tim::saveMultiPaletteGrid(const char *fileName, uint8_t cellCols, uint8_t cellRows, uint8_t colorsPerPal, uint8_t palColsPerRow, bool withAlpha) const
{
	PaletteDetectionStrategyGrid grid(this, cellCols, cellRows, colorsPerPal, palColsPerRow);
	return grid.isValid() && save(fileName, &grid, withAlpha);
}

bool Tim::toRGBA32MultiPaletteGrid(uint32_t *target, uint8_t cellCols, uint8_t cellRows, uint8_t colorsPerPal, uint8_t palColsPerRow, bool withAlpha) const
{
	PaletteDetectionStrategyGrid grid(this, cellCols, cellRows, colorsPerPal, palColsPerRow);
	return grid.isValid() && toRGBA32(target, &grid, withAlpha);
}

bool Tim::toRGBA32(uint32_t *target, PaletteDetectionStrategy *paletteDetectionStrategy, bool withAlpha) const
{
	if (_tim.img_data == nullptr)
	{
		ffnx_error("%s img_data is null\n", __func__);

		return false;
	}

	if (_bpp == 0)
	{
		if (_tim.pal_data == nullptr || paletteDetectionStrategy == nullptr)
		{
			ffnx_error("%s bpp 0 without palette\n", __func__);

			return false;
		}

		uint8_t *img_data = _tim.img_data;

		for (int y = 0; y < _tim.img_h; ++y)
		{
			for (int x = 0; x < _tim.img_w / 2; ++x)
			{
				*target = fromR5G5B5Color((_tim.pal_data + paletteDetectionStrategy->palOffset(x * 2, y))[*img_data & 0xF], withAlpha);

				++target;

				*target = fromR5G5B5Color((_tim.pal_data + paletteDetectionStrategy->palOffset(x * 2 + 1, y))[*img_data >> 4], withAlpha);

				++target;
				++img_data;
			}
		}
	}
	else if (_bpp == 1)
	{
		if (_tim.pal_data == nullptr || paletteDetectionStrategy == nullptr)
		{
			ffnx_error("%s bpp 1 without palette\n", __func__);

			return false;
		}

		uint8_t *img_data = _tim.img_data;

		for (int y = 0; y < _tim.img_h; ++y)
		{
			for (int x = 0; x < _tim.img_w; ++x)
			{
				*target = fromR5G5B5Color((_tim.pal_data + paletteDetectionStrategy->palOffset(x, y))[*img_data], withAlpha);

				++target;
				++img_data;
			}
		}
	}
	else if (_bpp == 2)
	{
		uint16_t *img_data16 = (uint16_t *)_tim.img_data;

		for (int y = 0; y < _tim.img_h; ++y)
		{
			for (int x = 0; x < _tim.img_w; ++x)
			{
				*target = fromR5G5B5Color(*img_data16, withAlpha);

				++target;
				++img_data16;
			}
		}
	}
	else
	{
		ffnx_error("%s unknown bpp %d\n", __func__, _bpp);

		return false;
	}

	return true;
}

bool Tim::save(const char *fileName, PaletteDetectionStrategy *paletteDetectionStrategy, bool withAlpha) const
{
	// allocate PBO
	uint32_t image_data_size = _tim.img_w * _tim.img_h * 4;
	uint32_t *image_data = (uint32_t*)driver_malloc(image_data_size);

	// convert source data
	if (image_data != nullptr)
	{
		if (toRGBA32(image_data, paletteDetectionStrategy, withAlpha))
		{
			// TODO: is animated
			save_texture(image_data, image_data_size, _tim.img_w, _tim.img_h, paletteDetectionStrategy->palIndex(), fileName, false);
		}

		driver_free(image_data);
	}

	return true;
}

Tim Tim::fromLzsData(uint8_t *uncompressed_data)
{
	uint16_t *header = (uint16_t *)uncompressed_data;
	ff8_tim tim_infos = ff8_tim();
	tim_infos.img_w = header[2];
	tim_infos.img_h = header[3];
	tim_infos.img_data = uncompressed_data + 8;

	return Tim(2, tim_infos);
}

struct TimDataHeader {
	uint32_t size;
	uint16_t x, y;
	uint16_t w, h;
};

Tim Tim::fromTimData(uint8_t *data)
{
	uint8_t bpp = data[4] & 3;
	bool hasPal = (data[4] & 8) != 0;
	TimDataHeader palHeader = TimDataHeader();
	ff8_tim tim_infos = ff8_tim();

	if (hasPal)
	{
		memcpy(&palHeader, data + 8, sizeof(palHeader));

		tim_infos.pal_data = (uint16_t *)(data + 8 + sizeof(palHeader));
		tim_infos.pal_x = palHeader.x;
		tim_infos.pal_y = palHeader.y;
		tim_infos.pal_w = palHeader.w;
		tim_infos.pal_h = palHeader.h;
	}

	TimDataHeader imgHeader = TimDataHeader();
	memcpy(&imgHeader, data + 8 + palHeader.size, sizeof(imgHeader));

	tim_infos.img_data = data + 8 + palHeader.size + sizeof(imgHeader);
	tim_infos.img_x = imgHeader.x;
	tim_infos.img_y = imgHeader.y;
	tim_infos.img_w = imgHeader.w;
	tim_infos.img_h = imgHeader.h;

	return Tim(bpp, tim_infos);
}

uint16_t Tim::colorsPerPal() const
{
	if (_bpp == 1)
	{
		return 256;
	}
	else if (_bpp == 0)
	{
		return 16;
	}

	return 0;
}
