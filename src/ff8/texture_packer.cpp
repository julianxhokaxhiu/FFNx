
#include "texture_packer.h"
#include "../renderer.h"
#include "../saveload.h"
#include "../patch.h"
#include "file.h"

#include <png.h>

TexturePacker::TexturePacker() :
	_vram(nullptr)
{
	memset(_vramTextureIds, INVALID_TEXTURE, VRAM_WIDTH * VRAM_HEIGHT);
}

void TexturePacker::setTexture(const char *name, const uint8_t *source, int x, int y, int w, int h)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s x=%d y=%d w=%d h=%d\n", __func__, x, y, w, h);

	bool hasNamedTexture = name != nullptr && *name != '\0';
	uint8_t *vram = vramSeek(x, y);
	const int vramLineWidth = VRAM_DEPTH * VRAM_WIDTH;
	const int lineWidth = VRAM_DEPTH * w;
	Texture tex;
	ModdedTextureId textureId = INVALID_TEXTURE;

	if (hasNamedTexture)
	{
		tex = Texture(name, x, y, w, h);

		if (tex.createImage())
		{
			textureId = (vram - _vram) / VRAM_DEPTH;
			_moddedTextures[textureId] = tex;
		}
	}

	for (int i = 0; i < h; ++i)
	{
		memcpy(vram, source, lineWidth);

		const int vramPosition = (vram - _vram) / VRAM_DEPTH;

		for (int j = 0; j < w; ++j)
		{
			ModdedTextureId previousTextureId = _vramTextureIds[vramPosition + j];

			if (previousTextureId != INVALID_TEXTURE && _moddedTextures.contains(previousTextureId))
			{
				_moddedTextures[previousTextureId].destroyImage();
				_moddedTextures.erase(previousTextureId);
			}

			_vramTextureIds[vramPosition + j] = textureId;
		}

		source += lineWidth;
		vram += vramLineWidth;
	}

	updateMaxScale();
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

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s scale=%d\n", __func__, _maxScaleCached);
}

bool TexturePacker::drawModdedTextures(const uint8_t *texData, uint32_t paletteIndex, uint32_t *target, uint8_t scale)
{
	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s pointer=0x%X paletteIndex=%d\n", __func__, texData, paletteIndex);

	if (_tiledTexs.contains(texData))
	{
		const TiledTex &tex = _tiledTexs[texData];
		bool ret = drawModdedTextures(target, tex.x, tex.y, tex.w, tex.h, scale);
		_tiledTexs.erase(texData);

		return ret;
	}

	if (trace_all || trace_vram) ffnx_warning("TexturePacker::%s Unknown tex data\n", __func__);

	return false;
}

bool TexturePacker::drawModdedTextures(uint32_t *target, int x, int y, int w, int h, uint8_t scale)
{
	bool hasModdedTexture = false;
	int scaledW = w * scale,
		scaledH = h * scale;

	for (int i = 0; i < scaledH; ++i)
	{
		int vramY = y + i / scale;

		for (int j = 0; j < scaledW; ++j)
		{
			int vramX = x + j / scale;
			ModdedTextureId textureId = _vramTextureIds[vramX + vramY * VRAM_WIDTH];

			if (textureId != INVALID_TEXTURE && _moddedTextures.contains(textureId)) {
				const Texture &texture = _moddedTextures[textureId];

				int realX = j - (texture.x() - x) * scale,
					realY = i - (texture.y() - y) * scale;
				*target = texture.getColor(realX, realY);
				hasModdedTexture = true;
			}

			target += 1;
		}
	}

	if (trace_all || trace_vram) ffnx_trace("TexturePacker::%s x=%d y=%d w=%d h=%d scale=%d hasModdedTexture=%d\n", __func__, x, y, w, h, scale, hasModdedTexture);

	return hasModdedTexture;
}

void TexturePacker::registerTiledTex(uint8_t *target, int x, int y, int w, int h)
{
	if (trace_all || trace_vram) ffnx_trace("%s pointer=0x%X x=%d y=%d w=%d h=%d\n", __func__, target, x, y, w, h);

	_tiledTexs[target] = TiledTex(x, y, w, h);
}

bool TexturePacker::saveVram(const char *fileName) const
{
	uint32_t *vram = new uint32_t[VRAM_WIDTH * VRAM_HEIGHT];
	vramToR8G8B8(vram);

	bool ret = newRenderer.saveTexture(
		fileName,
		VRAM_WIDTH,
		VRAM_HEIGHT,
		vram
	);

	delete[] vram;

	return ret;
}

void TexturePacker::vramToR8G8B8(uint32_t *output) const
{
	uint16_t *vram = (uint16_t *)_vram;

	for (int y = 0; y < VRAM_HEIGHT; ++y)
	{
		for (int x = 0; x < VRAM_WIDTH; ++x)
		{
			*output = fromR5G5B5Color(*vram);

			++output;
			++vram;
		}
	}
}

TexturePacker::Texture::Texture() :
	_image(nullptr), _name(""), _x(0), _y(0), _w(0), _h(0)
{
}

TexturePacker::Texture::Texture(
	const char *name,
	int x, int y, int w, int h
) : _image(nullptr), _name(name), _x(x), _y(y), _w(w), _h(h)
{
}

bool TexturePacker::Texture::createImage(uint8_t palette_index)
{
	char filename[MAX_PATH], langPath[16];

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

uint8_t TexturePacker::Texture::scale() const
{
	if (_image == nullptr) {
		return 1;
	}

	if (_image->m_width < _w || _image->m_height < _h || _image->m_width % _w != 0 || _image->m_height % _h != 0)
	{
		ffnx_warning("Texture size must be scaled to the original texture size: %s\n", _name.c_str());

		return 1;
	}

	int scaleW = _image->m_width / _w, scaleH = _image->m_height / _h;

	if (scaleW != scaleH)
	{
		ffnx_warning("Texture size must have the same ratio as the original texture: %s\n", _name.c_str());

		return 1;
	}

	if (scaleW > MAX_SCALE)
	{
		ffnx_warning("Texture size cannot exceed original size * %d: %s\n", MAX_SCALE, _name.c_str());

		return MAX_SCALE;
	}

	return scaleW;
}

uint32_t TexturePacker::Texture::getColor(int scaledX, int scaledY) const
{
	return ((uint32_t *)_image->m_data)[scaledX + scaledY * _image->m_width];
}

TexturePacker::TiledTex::TiledTex()
 : x(0), y(0), w(0), h(0)
{
}

TexturePacker::TiledTex::TiledTex(
	int x, int y, int w, int h
) : x(x), y(y), w(w), h(h)
{
}
