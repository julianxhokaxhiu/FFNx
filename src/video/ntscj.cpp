#include "../gl.h"
#include "../cfg.h"
#include "../globals.h"
#include "../log.h"
#include "ntscj.h"

#include <cctype>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}

bool fullrange_input = false;
ColorMatrixType colormatrix = COLORMATRIX_BT601;
ColorGamutType colorgamut = COLORGAMUT_SRGB;
InverseGammaFunctionType gammatype = GAMMAFUNCTION_SRGB;
ChromaLocationType chromaloc = CHROMALOC_CENTER;
bool okcolorspace = false;
bool yuvjfixneeded = false;

ColorGamutType ntscj_get_overall_color_gamut()
{
	return enable_ntscj_gamut_mode ? COLORGAMUT_NTSCJ : COLORGAMUT_SRGB;
}

bool ntscj_detect_logo_movie(const char* name, const AVCodecContext* codec_ctx)
{
	bool islogomovie = false;
	int lastbackslashindex = -1;
	int bytessincebackslash = 0;
	int scanoffset = 0;

	// figure out if this is the eidos logo or square logo; they need special treatment
	// scan till we hit 0 terminator
	if ((codec_ctx->color_trc == AVCOL_TRC_UNSPECIFIED) && (codec_ctx->color_primaries == AVCOL_PRI_UNSPECIFIED)){
		while (true){
			bytessincebackslash++;
			// note the index of the last backslash, and how far the string continues after that
			if (name[scanoffset] == 92){
				lastbackslashindex = scanoffset;
				bytessincebackslash = 0;
			}
			else if (name[scanoffset] == 0){
				break;
			}
			scanoffset++;
		}
		char upperbuffer[128];
		memset(upperbuffer, 0, 128);
		if ((lastbackslashindex > -1) && (bytessincebackslash > 1) && (bytessincebackslash <= 128)){
			memcpy(&upperbuffer, &name[lastbackslashindex+1], bytessincebackslash);
			// convert to uppercase
			for (int i=0; i<bytessincebackslash; i++){
				upperbuffer[i] = toupper(upperbuffer[i]);
			}
			// strip the file extension
			for (int i=bytessincebackslash-1; i>=0; i--){
				char backchar = upperbuffer[i];
				upperbuffer[i] = 0;
				if (backchar == 46){
					break;
				}
			}
			if (	(strcmp(upperbuffer, "EIDOSLOGO") == 0) ||
				(strcmp(upperbuffer, "SQLOGO") == 0)
			){
				islogomovie = true;
				if (trace_movies  || trace_all) ffnx_trace("prepare_movie: %s detected as logo movie.\n", name);
			}
		}
	}

	return islogomovie;
}

void ntscj_prepare_movie_colors(const AVCodecContext* codec_ctx, bool islogomovie)
{
	int chromawidthshift = 0;
	int chromaheightshift = 0;
	const AVPixFmtDescriptor* pix_desc = nullptr;
	AVPixelFormat nativepixformat = codec_ctx->pix_fmt;

	switch (codec_ctx->color_range){
		case AVCOL_RANGE_JPEG:
			fullrange_input = true;
			break;
		case AVCOL_RANGE_UNSPECIFIED:
		case AVCOL_RANGE_MPEG:
		default:
			fullrange_input = false;
			break;
	}

	// some pixel formats are inherently full-range
	// so we should treat them as such, even if the color range metadata is missing
	// some of these formats also trigger a bogus automatic color range conversion that we must suppress
	switch (nativepixformat){
		case AV_PIX_FMT_YUVJ420P:
		case AV_PIX_FMT_YUVJ411P:
		case AV_PIX_FMT_YUVJ422P:
		case AV_PIX_FMT_YUVJ444P:
		case AV_PIX_FMT_YUVJ440P:
		case AV_PIX_FMT_GRAY8:
		case AV_PIX_FMT_YA8:
		case AV_PIX_FMT_GRAY16LE:
		case AV_PIX_FMT_GRAY16BE:
		case AV_PIX_FMT_YA16BE:
		case AV_PIX_FMT_YA16LE:
		case AV_PIX_FMT_BGR24:
			fullrange_input = true;
			yuvjfixneeded = true;
			break;
		default:
			yuvjfixneeded = false;
	}

	if (trace_movies  || trace_all) ffnx_trace("prepare_movie: color range detected as %i (0=tv, 1=pc).\n", fullrange_input);

	// will we need to convert the YUV colorspace?
	switch(codec_ctx->colorspace){
		// these are all the same (bt601)
		case AVCOL_SPC_UNSPECIFIED: // ffmpeg guesses and treats this as bt601
		case AVCOL_SPC_RESERVED: // ffmpeg guesses and treats this as bt601
			if (nativepixformat == AV_PIX_FMT_BGR24){
				if (trace_movies  || trace_all) ffnx_trace("prepare_movie: BGR24 detected.\n");
				okcolorspace = false;
				break;
			}
			else if (ff8 && (codec_ctx->color_range != AVCOL_RANGE_JPEG) && !yuvjfixneeded){
				if (trace_movies  || trace_all) ffnx_trace("prepare_movie: Assuming FF8 bink or bink-derived movie. (e.g., PC2000 or Steam) Using bink color matrix and forcing color range to TV (limited).\n");
				colormatrix = COLORMATRIX_BINK;
				fullrange_input = false;
				okcolorspace = true;
				break;
			}
			// fall through to next case if we didn't already break
		case AVCOL_SPC_BT470BG:
		case AVCOL_SPC_SMPTE170M:
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: bt601 color matrix detected or defaulted to.\n");
			colormatrix = COLORMATRIX_BT601;
			okcolorspace = true;
			break;
		case AVCOL_SPC_BT709:
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: bt709 color matrix detected.\n");
			colormatrix = COLORMATRIX_BT709;
			okcolorspace = true;
			break;
		case AVCOL_SPC_RGB:
			okcolorspace = false;
			break;
		default:
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: unhandled color matrix detected; will use swscale to convert. Expect incorrect gamut.\n");
			okcolorspace = false;
	}

	// what gamma should we use?
	switch(codec_ctx->color_trc){
		case AVCOL_TRC_UNSPECIFIED:
		case AVCOL_TRC_RESERVED:
		case AVCOL_TRC_RESERVED0:
			if (islogomovie){
				gammatype = GAMMAFUNCTION_BT1886_APPX1;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: missing gamma metadata, using BT1886 Appendix 1 (CRT television/monitor) gamma curve because this is a logo movie.\n");
			}
			else {
				gammatype = GAMMAFUNCTION_BT1886_APPX1;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: missing gamma metadata, assuming Playstation-derived video, using BT1886 Appendix 1 (CRT television) gamma curve.\n");
			}
			break;
		case AVCOL_TRC_IEC61966_2_1: //srgb
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: srgb gamma transfer function detected\n");
			gammatype = GAMMAFUNCTION_SRGB;
			break;
		case AVCOL_TRC_GAMMA22: //2.2
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: 2.2 gamma transfer function detected, using sRGB instead.\n");
			gammatype = GAMMAFUNCTION_SRGB;
			break;
		case AVCOL_TRC_SMPTE170M:
		case AVCOL_TRC_BT709: // same as SMPTE170M
		case AVCOL_TRC_BT2020_10: // same as SMPTE170M
		case AVCOL_TRC_BT2020_12: // same as SMPTE170M
		case AVCOL_TRC_IEC61966_2_4: // same as SMPTE170M, but is defined for negative numbers too (which we ignore)
		case AVCOL_TRC_BT1361_ECG: // same as SMPTE170M, but is defined for negative numbers too (which we ignore)
			gammatype = GAMMAFUNCTION_SMPTE170M;
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: SMPTE170M transfer function detected\n");
			break;
		case AVCOL_TRC_GAMMA28: //fall through
		default:
			ffnx_error("prepare_movie: Unsupported transfer (inverse gamma) function. Using default instead.\n");
			gammatype = GAMMAFUNCTION_BT1886_APPX1;
      break;
	}

	// Figure out which color gamut to use.
	switch(codec_ctx->color_primaries){
		case AVCOL_PRI_BT709:
			colorgamut = COLORGAMUT_SRGB;
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: srgb/bt709 color gamut detected.\n");
			break;
		case AVCOL_PRI_BT470M:
			// Assume user meant SMPTE-C.
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: NTSC1953 color gamut detected. Assuming user error and using SMPTE-C instead.\n");
			// fall through to next case
		case AVCOL_PRI_SMPTE170M:
		case AVCOL_PRI_SMPTE240M:

			colorgamut = COLORGAMUT_SMPTEC;
			if (trace_movies || trace_all) ffnx_trace("prepare_movie: SMPTE-C color gamut detected.\n");
			break;
		case AVCOL_PRI_UNSPECIFIED:
		case AVCOL_PRI_RESERVED0:
		case AVCOL_PRI_RESERVED:
			if (islogomovie){
				colorgamut = COLORGAMUT_RAWP22;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: missing color gamut metadata; assuming uncorrected P22 (CRT computer monitor) because this is a logo movie.\n");
			}
			else {
				colorgamut = COLORGAMUT_NTSCJ;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: missing color gamut metadata; assuming NTSC-J.\n");
			}
			break;
		case AVCOL_PRI_BT470BG: // fall through
		default:
			if (islogomovie){
				colorgamut = COLORGAMUT_RAWP22;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: Unsupported color primaries. Instead using uncorrected P22 (CRT computer monitor) because this is a logo movie. (This will probably look wrong.)\n");
			}
			else {
				colorgamut = COLORGAMUT_NTSCJ;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: Unsupported color primaries. Instead using NTSC-J. (This will probably look wrong.)\n");
			}
	}

	// what is the chroma sample location?
	// first, we may need to know about the pixel format
	pix_desc = av_pix_fmt_desc_get(nativepixformat);
	if (pix_desc){
		chromawidthshift = pix_desc->log2_chroma_w;
		chromaheightshift = pix_desc->log2_chroma_h;
	}
	switch (codec_ctx->chroma_sample_location){
		case AVCHROMA_LOC_LEFT:
			chromaloc = CHROMALOC_LEFT;
			break;
		case AVCHROMA_LOC_CENTER:
			chromaloc = CHROMALOC_CENTER;
			break;
		case AVCHROMA_LOC_TOPLEFT:
			chromaloc = CHROMALOC_TOPLEFT;
			break;
		case AVCHROMA_LOC_TOP:
			chromaloc = CHROMALOC_TOP;
			break;
		case AVCHROMA_LOC_BOTTOMLEFT:
			chromaloc = CHROMALOC_BOTTOMLEFT;
			break;
		case AVCHROMA_LOC_BOTTOM:
			chromaloc = CHROMALOC_BOTTOM;
			break;
		case AVCHROMA_LOC_UNSPECIFIED: //fall through
		default:
			// chroma sample location isn't specified so we must guess

			// if the pixel format is 444, it must be center
			if (pix_desc && (chromawidthshift == 0) && (chromaheightshift == 0)){
				chromaloc = CHROMALOC_CENTER;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: Guessing chroma sample location must be center because 4:4:4 pixel format.\n");
			}
			// if the pixel format is 420, guess from the codec
			else if (pix_desc && (chromawidthshift == 1) && (chromaheightshift == 1)){
				// defaults for some common codecs
				switch (codec_ctx->codec_id){
					case AV_CODEC_ID_MPEG2VIDEO:
					case AV_CODEC_ID_MPEG4:
					case AV_CODEC_ID_H264:
					case AV_CODEC_ID_HEVC:
						chromaloc = CHROMALOC_LEFT;
						if (trace_movies || trace_all) ffnx_trace("prepare_movie: Guessing chroma sample location is left for 4:2:0 pixel format based on codec.\n");
						break;
					case AV_CODEC_ID_MJPEG:
					case AV_CODEC_ID_H261:
					case AV_CODEC_ID_MPEG1VIDEO:
					case AV_CODEC_ID_H263:
						chromaloc = CHROMALOC_CENTER;
						if (trace_movies || trace_all) ffnx_trace("prepare_movie: Guessing chroma sample location is center for 4:2:0 pixel format based on codec.\n");
						break;
					default:
						// per jpsxdec documentation, PSX video has chroma position center https://github.com/m35/jpsxdec/blob/readme/jpsxdec/PlayStation1_STR_format.txt
						// no idea what bink chroma position is.
						chromaloc = CHROMALOC_CENTER;
						if (trace_movies || trace_all) ffnx_trace("prepare_movie: Unable to guess chroma sample location for 4:2:0 pixel format based on codec. Assuming center.\n");
						break;
				}
			}
			else if (pix_desc && (chromawidthshift == 1) && (chromaheightshift == 0)){
				if (codec_ctx->codec_id == AV_CODEC_ID_MPEG2VIDEO){
					// ffmpeg docs say topleft, but that's impossible for 422
					// (Think ffmpeg docs are always relative to 420's 2x2 box even when it's not 420.)
					chromaloc = CHROMALOC_LEFT;
					if (trace_movies || trace_all) ffnx_trace("prepare_movie: Guessing chroma sample location is left for 4:2:2 pixel format based on codec.\n");
				}
				else {
					chromaloc = CHROMALOC_CENTER;
					if (trace_movies || trace_all) ffnx_trace("prepare_movie: Unable to guess chroma sample location for 4:2:2 pixel format based on codec. Assuming center.\n");
				}
			}
			else {
				chromaloc = CHROMALOC_CENTER;
				if (trace_movies || trace_all) ffnx_trace("prepare_movie: Unable to guess chroma sample location based on pixel format and codec. Assuming center.\n");
			}
			break;
	}
	// Metadata may have lied. Force center for 4:4:4.
	if (pix_desc && (chromawidthshift == 0) && (chromaheightshift == 0) && (chromaloc != CHROMALOC_CENTER)){
		chromaloc = CHROMALOC_CENTER;
		if (trace_movies || trace_all) ffnx_trace("prepare_movie: Guessing chroma sample location must be center because 4:4:4 pixel format. (Overriding erroneous metadata.)\n");
	}
	// We also haven't addressed the possibility that a modder did an improper conversion without adjusting the chroma sample location.
	// But that's the modder's fault and we can't fix it here.
	if (trace_movies || trace_all) ffnx_trace("prepare_movie: Chroma sample location detected or guessed as %d (0=topleft, 1=top, 2=left, 3=center, 4=bottomleft, 5=bottom).\n", chromaloc);
}

void Renderer::setPostprocessingBackend()
{
		if (internalState.bIsOverallColorGamut == COLORGAMUT_NTSCJ){
			backendProgram = RendererProgram::POSTPROCESSING_NTSCJ;
		}
		else {
			backendProgram = RendererProgram::POSTPROCESSING;
		}
}

void Renderer::setYUVMovieBackend(){
		if (internalState.bIsOverallColorGamut == COLORGAMUT_NTSCJ){
				backendProgram = RendererProgram::YUVMOVIE_TRUECOLOR;
				if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: YUVMOVIE_TRUECOLOR\n", __func__);
		}
		else {
				backendProgram = RendererProgram::YUVMOVIE;
				if (trace_all || trace_renderer) ffnx_trace("Renderer::%s: YUVMOVIE\n", __func__);
		}
}

void Renderer::AssignGamutLUT()
{
	// If we aren't in NTSC-J mode, we won't be using a LUT at all
	if (internalState.bIsOverallColorGamut != COLORGAMUT_NTSCJ){
		return;
	}

	// NTSC-J mode post-processing uses the NTSCJ to sRGB LUT in SDR mode (but uses a matrix in HDR mode)
	if ((backendProgram == RendererProgram::POSTPROCESSING_NTSCJ) && !internalState.bIsHDR){
		LoadGamutLUT(INDEX_LUT_NTSCJ_TO_SRGB); // load LUT if not already loaded
		useTexture(GLUTHandleNTSCJtoSRGB.idx, RendererTextureSlot::TEX_G_LUT);
	}
	// Movies may use LUTs in NTSC-J mode
	else if (backendProgram == RendererProgram::YUVMOVIE_TRUECOLOR){
		// This is slightly wrong for HDR, since the LUTs invert gamut compression that wouldn't happen in HDR.
		// The consequence is that some colors near places where the P22 gamut exceeds the sRGB/SMPTE-C gamuts
		// will appear slightly brighter and/or more saturated in HDR than they would if the round-trip were perfect.
		// I don't think anyone will mind...
		if (internalState.bIsMovieColorGamut == COLORGAMUT_SRGB){
			LoadGamutLUT(INDEX_LUT_INVERSE_NTSCJ_TO_SRGB); // load LUT if not already loaded
			useTexture(GLUTHandleInverseNTSCJtoSRGB.idx, RendererTextureSlot::TEX_G_LUT);
		}
		else if (internalState.bIsMovieColorGamut == COLORGAMUT_SMPTEC){
			LoadGamutLUT(INDEX_LUT_INVERSE_NTSCJ_TO_SMPTEC); // load LUT if not already loaded
			useTexture(GLUTHandleInverseNTSCJtoSMPTEC.idx, RendererTextureSlot::TEX_G_LUT);
		}
	}
	return;
}

void Renderer::prepareGamutLUTs()
{

	// flush everything (they should have all initialized to BGFX_INVALID_HANDLE, but let's be careful)
	if (bgfx::isValid(GLUTHandleNTSCJtoSRGB))
		bgfx::destroy(GLUTHandleNTSCJtoSRGB);
	if (bgfx::isValid(GLUTHandleInverseNTSCJtoSRGB))
		bgfx::destroy(GLUTHandleInverseNTSCJtoSRGB);
	if (bgfx::isValid(GLUTHandleInverseNTSCJtoSMPTEC))
		bgfx::destroy(GLUTHandleInverseNTSCJtoSMPTEC);

	// load the NTSC-J to sRGB LUT if we're likely to need it.
	// (consult the global setting so we don't get tripped up by renderer state changing to accomodate the FFNx logo)
	if (enable_ntscj_gamut_mode && !internalState.bIsHDR){
		LoadGamutLUT(INDEX_LUT_NTSCJ_TO_SRGB);
	}
	// Any other LUTs we end up needing will be lazy loaded by AssignGamutLUT()

	return;
}

void Renderer::LoadGamutLUT(GamutLUTIndexType whichLUT)
{

	static char fullpath[MAX_PATH];
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t mipCount = 0;

	// Note: It's important that the final parameter to createTextureHandle() -- isSrgb -- is false.
	// Otherwise the sRGB gamma function will be applied to convert sRGB to linear RGB.
	// But we don't want that because these LUTs are already in linear RGB.

	switch (whichLUT){
		case INDEX_LUT_NTSCJ_TO_SRGB:
			if (!bgfx::isValid(GLUTHandleNTSCJtoSRGB)){
				sprintf(fullpath, "%s/shaders/glut_ntscj_to_srgb.png", basedir);
				GLUTHandleNTSCJtoSRGB = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleNTSCJtoSRGB.idx) GLUTHandleNTSCJtoSRGB = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_INVERSE_NTSCJ_TO_SRGB:
			if (!bgfx::isValid(GLUTHandleInverseNTSCJtoSRGB)){
				sprintf(fullpath, "%s/shaders/glut_inverse_ntscj_to_srgb.png", basedir);
				GLUTHandleInverseNTSCJtoSRGB = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleInverseNTSCJtoSRGB.idx) GLUTHandleInverseNTSCJtoSRGB = BGFX_INVALID_HANDLE;
			}
			break;
		case INDEX_LUT_INVERSE_NTSCJ_TO_SMPTEC:
			if (!bgfx::isValid(GLUTHandleInverseNTSCJtoSMPTEC)){
				sprintf(fullpath, "%s/shaders/glut_inverse_ntscj_to_smptec.png", basedir);
				GLUTHandleInverseNTSCJtoSMPTEC = createTextureHandle(fullpath, &width, &height, &mipCount, false);
				if (!GLUTHandleInverseNTSCJtoSMPTEC.idx) GLUTHandleInverseNTSCJtoSMPTEC = BGFX_INVALID_HANDLE;
			}
			break;
		default:
			ffnx_error("LoadGamutLUT: called with invalid index: %i\n", whichLUT);
			break;
	}
	return;
}