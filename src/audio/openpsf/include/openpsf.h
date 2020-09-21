/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#if defined(__cplusplus)
extern "C" {
#endif
	typedef void Psf;

	struct OPENPSF {
		bool (*is_our_path)(const char* p_full_path, const char* p_extension);
		Psf* (*create)();
		Psf* (*create_with_params)(bool reverb, bool do_filter, bool suppressEndSilence, bool suppressOpeningSilence,
			int endSilenceSeconds);
		void (*destroy)(Psf* self);
		bool (*open)(Psf* self, const char* p_path, bool infinite);
		void (*close)(Psf* self);
		size_t(*decode)(Psf* self, float* data, unsigned int sample_count);
		bool (*seek)(Psf* self, unsigned int ms);
		int (*get_length)(Psf* self);
		int (*get_sample_rate)(Psf* self);
		int (*get_channel_count)(Psf* self);
		int (*get_bits_per_seconds)(Psf* self);
		const char* (*get_last_error)(Psf* self);
		const char* (*get_last_status)(Psf* self);
	};

	typedef OPENPSF (* get_openpsf)();
#if defined(__cplusplus)
}
#endif
