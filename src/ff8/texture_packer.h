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

#pragma once

#include <string>
#include <map>
#include <bimg/bimg.h>

typedef uint32_t ModdedTextureId;

#define VRAM_WIDTH 1024
#define VRAM_HEIGHT 512
#define VRAM_DEPTH 2
#define INVALID_TEXTURE 0xFFFFFFFF
#define MAX_SCALE 10

class TexturePacker {
public:
    explicit TexturePacker();
    inline void setVram(uint8_t *vram) {
        _vram = vram;
    }
    void setTexture(const char *name, const uint8_t *texture, int x, int y, int w, int h);
    inline uint8_t getMaxScale() const {
        return _maxScaleCached;
    }
    void registerTiledTex(uint8_t *target, int x, int y, int w, int h);
    bool drawModdedTextures(const uint8_t *texData, uint32_t paletteIndex, uint32_t *target, uint8_t scale);

    bool saveVram(const char *fileName) const;
private:
    inline uint8_t *vramSeek(int x, int y) const {
        return _vram + VRAM_DEPTH * (x + y * VRAM_WIDTH);
    }
    void updateMaxScale();
    bool drawModdedTextures(uint32_t *target, int x, int y, int w, int h, uint8_t scale);

    void vramToR8G8B8(uint32_t *output) const;
    static inline uint32_t fromR5G5B5Color(uint16_t color) {
        uint8_t r = color & 31,
                g = (color >> 5) & 31,
                b = (color >> 10) & 31;

        return (0xffu << 24) |
            ((((r << 3) + (r >> 2)) & 0xffu) << 16) |
            ((((g << 3) + (g >> 2)) & 0xffu) << 8) |
            (((b << 3) + (b >> 2)) & 0xffu);
    }
    class Texture {
    public:
        Texture();
        Texture(
            const char *name,
            int x, int y, int w, int h
        );
        inline const std::string &name() const {
            return _name;
        }
        inline int x() const {
            return _x;
        }
        inline int y() const {
            return _y;
        }
        inline int w() const {
            return _w;
        }
        inline int h() const {
            return _h;
        }
        uint8_t scale() const;
        bool createImage();
        void destroyImage();
        uint32_t getColor(int scaledX, int scaledY) const;
    private:
        bimg::ImageContainer *_image;
        std::string _name;
        int _x, _y;
        int _w, _h;
    };
    struct TiledTex {
        TiledTex();
        TiledTex(int x, int y, int w, int h);
        int x, y;
        int w, h;
    };

    uint8_t *_vram; // uint16_t[VRAM_WIDTH * VRAM_HEIGHT] aka uint8_t[VRAM_WIDTH * VRAM_HEIGHT * VRAM_DEPTH]
    std::map<const uint8_t *, TiledTex> _tiledTexs;
    ModdedTextureId _vramTextureIds[VRAM_WIDTH * VRAM_HEIGHT];
    std::map<ModdedTextureId, Texture> _moddedTextures;
    uint8_t _maxScaleCached;
};
