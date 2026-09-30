#include "multilanguage.h"
#include "multilanguage_data.h"
#include "../cfg.h"
#include "../common.h"
#include "../ff7.h"
#include "../globals.h"
#include "../log.h"
#include "../patch.h"

#include <algorithm>
#include <cstring>
#include <unordered_map>

namespace ff7::multilanguage
{
namespace
{
std::unordered_map<uint32_t, const char *> translations;
std::unordered_map<uint32_t, const char *> ascii_translations;

template<size_t Count>
void apply_code_patches(const data::CodePatch (&patches)[Count])
{
	for (const auto &entry : patches)
	{
		switch (entry.size)
		{
		case 1: patch_code_byte(entry.address, entry.value); break;
		case 2: patch_code_word(entry.address, entry.value); break;
		case 4: patch_code_dword(entry.address, entry.value); break;
		}
	}
}

int text_width(uint8_t *text)
{
	return reinterpret_cast<int (*)(uint8_t *)>(ff7_externals.sub_6F54A2)(translate(text));
}

int draw_text(int x, int y, uint8_t *text, uint8_t color, float depth)
{
	return ff7_externals.draw_string_from_buffer_sub_6F5B03(x, y, translate(text), color, depth);
}

int16_t draw_text_fixed(int x, int y, uint8_t *text, uint8_t color, float depth)
{
	return reinterpret_cast<int16_t (*)(int, int, uint8_t *, uint8_t, float)>(data::text_fixed_original)(x, y, translate(text), color, depth);
}

void __stdcall draw_ascii(const char *text, int x, int y, int color, ff7_graphics_object *graphics_object)
{
	auto entry = ascii_translations.find(reinterpret_cast<uint32_t>(text));
	if (entry != ascii_translations.end())
	{
		memcpy_code(reinterpret_cast<uint32_t>(data::text_buffer), const_cast<char *>(entry->second), std::strlen(entry->second) + 1);
		text = reinterpret_cast<const char *>(data::text_buffer);
	}
	reinterpret_cast<void (__stdcall *)(const char *, int, int, int, ff7_graphics_object *)>(data::ascii_original)(
		text, x, y, color, graphics_object);
}

template<size_t TextCount, size_t FormatCount, size_t PatchCount, size_t ByteCount>
void apply(const data::Text (&text)[TextCount], const data::Text (&formats)[FormatCount],
	const data::Patch (&patches)[PatchCount], const data::BytePatch (&bytes)[ByteCount],
	const char *credits_title, const char *credits_subtitle)
{
	for (const auto &entry : text)
		translations[entry.address] = entry.text;
	for (const auto &entry : formats)
		ascii_translations[entry.address] = entry.text;
	for (const auto &entry : patches)
	{
		bool archive_copied = false;
		for (const auto &copy : data::archive_copies)
		{
			if (copy.source != entry.address)
				continue;
			memcpy_code(copy.address, const_cast<uint8_t *>(data::archive_copy_code), sizeof(data::archive_copy_code));
			patch_code_dword(copy.address + data::archive_copy_source_offset, reinterpret_cast<uint32_t>(entry.bytes));
			patch_code_dword(copy.address + data::archive_copy_size_offset, std::strlen(entry.bytes) + 1);
			memset_code(copy.address + sizeof(data::archive_copy_code), 0x90, copy.size - sizeof(data::archive_copy_code));
			archive_copied = true;
			break;
		}
		if (archive_copied)
			continue;
		memcpy_code(entry.address, const_cast<char *>(entry.bytes), std::strlen(entry.bytes) + !entry.unterminated);
	}
	for (const auto &entry : bytes)
		patch_code_byte(entry.address, entry.value);

	for (const auto &entry : { data::Text{ data::credits_addresses[0], credits_title }, data::Text{ data::credits_addresses[1], credits_subtitle } })
	{
		size_t length = 0;
		while (static_cast<uint8_t>(entry.text[length]) != 0xFF)
			++length;
		memcpy_code(entry.address, const_cast<char *>(entry.text), length + 1);
	}
}
}

void init()
{
	const char *help_texture;
	switch (game_language)
	{
	case GAME_LANGUAGE_FR:
		apply(data::fr_text, data::fr_format, data::fr_patches, data::fr_byte_patches,
			data::fr_credits_title, data::fr_credits_subtitle);
		apply_code_patches(data::fr_code_patches);
		help_texture = data::help_textures[0];
		break;
	case GAME_LANGUAGE_DE:
		apply(data::de_text, data::de_format, data::de_patches, data::de_byte_patches,
			data::de_credits_title, data::de_credits_subtitle);
		apply_code_patches(data::de_code_patches);
		help_texture = data::help_textures[1];
		break;
	case GAME_LANGUAGE_SP:
		apply(data::es_text, data::es_format, data::es_patches, data::es_byte_patches,
			data::es_credits_title, data::es_credits_subtitle);
		apply_code_patches(data::es_code_patches);
		help_texture = data::help_textures[2];
		break;
	default:
		return;
	}

	size_t language = game_language == GAME_LANGUAGE_FR ? 0 : game_language == GAME_LANGUAGE_DE ? 1 : 2;
	if (game_language == GAME_LANGUAGE_DE || game_language == GAME_LANGUAGE_SP)
		apply_code_patches(data::save_window_patches);
	if (game_language == GAME_LANGUAGE_FR || game_language == GAME_LANGUAGE_DE)
	{
		apply_code_patches(data::fr_de_code_patches);
		for (const auto &entry : data::save_grid_patches)
			patch_code_byte(entry.address, entry.value);
		for (const auto &entry : data::save_grid_code)
			patch_code_byte(entry.address, entry.value);
		apply_code_patches(data::save_grid_tail_patches);
	}
	apply_code_patches(data::common_code_patches);
	apply_code_patches(data::credits_code_patches[language]);
	const char *normal_help_texture = game_language == GAME_LANGUAGE_FR ? data::normal_help_textures[0] :
		game_language == GAME_LANGUAGE_DE ? data::normal_help_textures[1] : data::normal_help_textures[2];
	patch_code_dword(data::help_texture_addresses[0], reinterpret_cast<uint32_t>(help_texture));
	patch_code_dword(data::help_texture_addresses[1], reinterpret_cast<uint32_t>(normal_help_texture));
	const uint8_t (*hud2_records)[36] = game_language == GAME_LANGUAGE_FR ? data::fr_hud_2 :
		game_language == GAME_LANGUAGE_DE ? data::de_hud_2 : data::es_hud_2;
	size_t hud2_size = game_language == GAME_LANGUAGE_FR ? sizeof(data::fr_hud_2) :
		game_language == GAME_LANGUAGE_DE ? sizeof(data::de_hud_2) : sizeof(data::es_hud_2);
	memcpy_code(reinterpret_cast<uint32_t>(data::hud2), const_cast<uint8_t (*)[36]>(hud2_records), hud2_size);
	const uint8_t (*hud64_records)[36] = game_language == GAME_LANGUAGE_FR ? data::fr_hud_64 :
		game_language == GAME_LANGUAGE_DE ? data::de_hud_64 : data::es_hud_64;
	size_t hud64_size = game_language == GAME_LANGUAGE_FR ? sizeof(data::fr_hud_64) :
		game_language == GAME_LANGUAGE_DE ? sizeof(data::de_hud_64) : sizeof(data::es_hud_64);
	memcpy_code(reinterpret_cast<uint32_t>(data::hud64), const_cast<uint8_t (*)[36]>(hud64_records), hud64_size);
	const uint8_t (*hud128_records)[36] = game_language == GAME_LANGUAGE_FR ? data::fr_hud_128 :
		game_language == GAME_LANGUAGE_DE ? data::de_hud_128 : data::es_hud_128;
	size_t hud128_size = game_language == GAME_LANGUAGE_FR ? sizeof(data::fr_hud_128) :
		game_language == GAME_LANGUAGE_DE ? sizeof(data::de_hud_128) : sizeof(data::es_hud_128);
	memcpy_code(reinterpret_cast<uint32_t>(data::hud128), const_cast<uint8_t (*)[36]>(hud128_records), hud128_size);
	const struct {
		uint32_t address;
		const uint8_t (*records)[36];
		size_t size;
		uint8_t instruction_offset;
	} hud_tables[] = {
		{ data::hud_table_addresses[0], data::hud2, hud2_size, data::hud_instruction_offsets[0] },
		{ data::hud_table_addresses[1], game_language == GAME_LANGUAGE_FR ? data::fr_hud_8 :
			game_language == GAME_LANGUAGE_DE ? data::de_hud_8 : data::es_hud_8,
			game_language == GAME_LANGUAGE_FR ? sizeof(data::fr_hud_8) :
			game_language == GAME_LANGUAGE_DE ? sizeof(data::de_hud_8) : sizeof(data::es_hud_8), 0 },
		{ data::hud_table_addresses[2], game_language == GAME_LANGUAGE_FR ? data::fr_hud_16 :
			game_language == GAME_LANGUAGE_DE ? data::de_hud_16 : data::es_hud_16,
			game_language == GAME_LANGUAGE_FR ? sizeof(data::fr_hud_16) :
			game_language == GAME_LANGUAGE_DE ? sizeof(data::de_hud_16) : sizeof(data::es_hud_16), 0 },
		{ data::hud_table_addresses[3], game_language == GAME_LANGUAGE_FR ? data::fr_hud_32 :
			game_language == GAME_LANGUAGE_DE ? data::de_hud_32 : data::es_hud_32,
			game_language == GAME_LANGUAGE_FR ? sizeof(data::fr_hud_32) :
			game_language == GAME_LANGUAGE_DE ? sizeof(data::de_hud_32) : sizeof(data::es_hud_32), 0 },
		{ data::hud_table_addresses[4], data::hud64, hud64_size, 0 },
		{ data::hud_table_addresses[5], data::hud128, hud128_size, 0 }
	};
	for (const auto &table : hud_tables)
	{
		patch_code_byte(table.address + data::hud_count_offset + table.instruction_offset, static_cast<uint8_t>(table.size / sizeof(*table.records)));
		for (const auto &entry : data::hud_references)
			patch_code_dword(table.address + entry.address + table.instruction_offset, reinterpret_cast<uint32_t>(table.records) + entry.value);
	}
	memcpy_code(data::hud2_entry_address, const_cast<uint8_t *>(data::hud2_entry), sizeof(data::hud2_entry));
	replace_call(data::hud2_entry_address, reinterpret_cast<void *>(data::hud2_palette_address));
	memcpy_code(data::hud2_palette_address, const_cast<uint8_t *>(data::hud2_palette), sizeof(data::hud2_palette));
	uint32_t hud2_selected_palette = reinterpret_cast<uint32_t>(data::hud2[data::hud2_palette_rows[language]]) + data::hud_palette_offset;
	patch_code_dword(data::hud2_palette_addresses[0], hud2_selected_palette);
	patch_code_dword(data::hud2_palette_addresses[1], hud2_selected_palette + data::hud2_palette_stride);
	replace_call(data::hud2_return_call, reinterpret_cast<void *>(data::hud2_return_address));
	apply_code_patches(data::hud2_loop_patches);
	const struct {
		uint32_t entry;
		uint32_t compare;
		uint8_t (*records)[36];
		uint32_t selected;
		const data::BytePatch *copies;
		size_t count;
	} hud_rectangles[] = {
		{ data::hud_rectangle_entries[0], data::hud_rectangle_compares[0], data::hud64, data::hud64_selected_rows[language],
			data::hud64_copies, sizeof(data::hud64_copies) / sizeof(*data::hud64_copies) },
		{ data::hud_rectangle_entries[1], data::hud_rectangle_compares[1], data::hud128, data::hud128_selected_row,
			data::hud128_copies, sizeof(data::hud128_copies) / sizeof(*data::hud128_copies) }
	};
	uint32_t rectangle_code = data::hud_rectangle_address;
	for (const auto &table : hud_rectangles)
	{
		memcpy_code(table.entry, const_cast<uint8_t *>(data::hud2_entry), sizeof(data::hud2_entry));
		replace_call(table.entry, reinterpret_cast<void *>(rectangle_code));
		for (size_t index = 0; index < table.count; ++index)
		{
			const auto &copy = table.copies[index];
			uint32_t destination = reinterpret_cast<uint32_t>(table.records[table.selected + copy.value]) + data::hud_rectangle_field_offset;
			memcpy_code(rectangle_code, const_cast<uint8_t *>(data::hud_rectangle_copy), sizeof(data::hud_rectangle_copy));
			patch_code_dword(rectangle_code + data::hud_rectangle_operands[0], copy.address);
			patch_code_dword(rectangle_code + data::hud_rectangle_operands[1], destination);
			patch_code_dword(rectangle_code + data::hud_rectangle_operands[2], copy.address + data::hud_rectangle_field_offset);
			patch_code_dword(rectangle_code + data::hud_rectangle_operands[3], destination + data::hud_rectangle_field_offset);
			rectangle_code += sizeof(data::hud_rectangle_copy);
		}
		memcpy_code(rectangle_code, const_cast<uint8_t *>(data::hud_rectangle_done), sizeof(data::hud_rectangle_done));
		replace_call(rectangle_code + data::hud_rectangle_return_offset, reinterpret_cast<void *>(table.compare));
		rectangle_code += sizeof(data::hud_rectangle_done);
	}
	apply_code_patches(data::element_code_patches[language]);
	if (game_language == GAME_LANGUAGE_FR || game_language == GAME_LANGUAGE_DE)
	{
		apply_code_patches(data::status_code_patches[language]);
		for (const auto &entry : data::label_registers)
		{
			memcpy_code(entry.address, const_cast<uint8_t *>(data::label_args), sizeof(data::label_args));
			for (const auto &operand : data::label_operands)
				patch_code_byte(entry.address + operand.address, operand.value | entry.value);
			replace_call(entry.address + data::label_call_offset, reinterpret_cast<void *>(draw_text));
		}
		memcpy_code(data::label_cleanup_address, const_cast<uint8_t *>(data::label_cleanup), sizeof(data::label_cleanup));
	}
	uint8_t name_columns = data::name_columns[language];
	const char *name_characters = game_language == GAME_LANGUAGE_DE ? data::de_name_characters :
		game_language == GAME_LANGUAGE_SP ? data::es_name_characters : data::fr_name_characters;
	for (uint32_t address : data::name_column_addresses)
		patch_code_byte(address, name_columns);
	for (uint32_t address : data::name_table_addresses)
		patch_code_dword(address, reinterpret_cast<uint32_t>(name_characters));
	if (game_language != GAME_LANGUAGE_FR)
		apply_code_patches(data::name_glyph_patches[language - 1]);
	if (game_language == GAME_LANGUAGE_DE)
	{
		for (const auto &entry : data::materia_x_bytes)
		{
			patch_code_byte(entry.address, entry.value);
			patch_code_dword(entry.address - data::materia_y_offset, data::materia_y);
		}
		for (const auto &entry : data::materia_x_dwords)
		{
			patch_code_dword(entry.address, entry.value);
			patch_code_dword(entry.address - data::materia_y_offset, data::materia_y);
		}
	}
	else
	{
		for (uint32_t address : data::materia_number_addresses)
		{
			memcpy_code(address, const_cast<uint8_t *>(data::materia_number_args), sizeof(data::materia_number_args));
			replace_call(address + data::materia_number_call_offset, reinterpret_cast<void *>(data::materia_number_original));
			patch_code_byte(address + data::materia_number_cleanup.address, data::materia_number_cleanup.value);
		}
		apply_code_patches(data::materia_list_patches);
		for (uint32_t address : data::materia_symbol_addresses)
			memcpy_code(address, const_cast<uint8_t *>(data::materia_symbol_args), sizeof(data::materia_symbol_args));
	}
	if (game_language == GAME_LANGUAGE_FR)
	{
		for (uint32_t address : data::time_argument_addresses)
		{
			const uint8_t *original = reinterpret_cast<const uint8_t *>(address);
			auto time_args = data::time_args;
			time_args[1] = original[1];
			memcpy_code(reinterpret_cast<uint32_t>(time_args.data() + 6), const_cast<uint8_t *>(original + 3), 6);
			memcpy_code(address, time_args.data(), time_args.size());
		}
	}
	else
	{
		for (uint32_t address : data::time_offset_addresses)
			patch_code_byte(address, data::time_offsets[language - 1]);
	}
	if (game_language == GAME_LANGUAGE_DE)
	{
		memcpy_code(data::speed_buffer_address, const_cast<uint8_t *>(data::speed_buffer), sizeof(data::speed_buffer));
		memcpy_code(data::speed_args_address, const_cast<uint8_t *>(data::speed_args), sizeof(data::speed_args));
		memcpy_code(data::speed_digits_address, const_cast<uint8_t *>(data::speed_digits_and_draws), sizeof(data::speed_digits_and_draws));
		for (uint32_t address : data::speed_draw_calls)
			replace_call(address, reinterpret_cast<void *>(draw_ascii));
		apply_code_patches(data::speed_jump_patches);
		replace_call(data::speed_jump_address, reinterpret_cast<void *>(data::speed_draw_calls[0]));
	}
	else
	{
		const uint8_t *original = reinterpret_cast<const uint8_t *>(data::wide_speed_args_address);
		auto wide_speed_args = data::wide_speed_args;
		wide_speed_args[1] = original[1];
		uint32_t speed_offset = data::speed_offsets[game_language == GAME_LANGUAGE_FR ? 0 : 1];
		memcpy_code(reinterpret_cast<uint32_t>(wide_speed_args.data() + 2), &speed_offset, sizeof(speed_offset));
		memcpy_code(reinterpret_cast<uint32_t>(wide_speed_args.data() + 6), const_cast<uint8_t *>(original + 3), 5);
		memcpy_code(data::wide_speed_args_address, wide_speed_args.data(), wide_speed_args.size());
	}
	const struct {
		uint32_t original;
		void *wrapper;
		const uint32_t *calls;
		size_t count;
	} text_functions[] = {
		{ ff7_externals.sub_6F54A2, reinterpret_cast<void *>(text_width), data::text_width_calls, sizeof(data::text_width_calls) / sizeof(*data::text_width_calls) },
		{ reinterpret_cast<uint32_t>(ff7_externals.draw_string_from_buffer_sub_6F5B03), reinterpret_cast<void *>(draw_text),
			data::text_draw_calls, sizeof(data::text_draw_calls) / sizeof(*data::text_draw_calls) },
		{ data::text_fixed_original, reinterpret_cast<void *>(draw_text_fixed), data::text_fixed_draw_calls, sizeof(data::text_fixed_draw_calls) / sizeof(*data::text_fixed_draw_calls) },
		{ data::ascii_original, reinterpret_cast<void *>(draw_ascii), data::ascii_draw_calls, sizeof(data::ascii_draw_calls) / sizeof(*data::ascii_draw_calls) }
	};
	for (const auto &function : text_functions)
	{
		for (size_t index = 0; index < function.count; ++index)
		{
			uint32_t address = function.calls[index];
			if ((game_language == GAME_LANGUAGE_FR || game_language == GAME_LANGUAGE_DE) &&
				std::find(data::fr_de_text_exclusions.begin(), data::fr_de_text_exclusions.end(), address) != data::fr_de_text_exclusions.end())
				continue;
			if (game_language == GAME_LANGUAGE_DE && address == data::speed_jump_address)
				continue;
			if (*reinterpret_cast<const uint8_t *>(address) != 0xE8 || get_relative_call(address, 0) != function.original)
			{
				ffnx_warning("FF7 2026 multilanguage: unexpected text call at 0x%X.\n", address);
				continue;
			}
			replace_call(address, function.wrapper);
		}
	}
}

uint8_t *translate(uint8_t *text)
{
	if (!ff7_2026_rerelease)
		return text;
	auto entry = translations.find(reinterpret_cast<uint32_t>(text));
	if (entry == translations.end())
		return text;
	size_t length = 0;
	while (static_cast<uint8_t>(entry->second[length]) != 0xFF)
		++length;
	memset_code(reinterpret_cast<uint32_t>(data::text_buffer), 0, sizeof(data::text_buffer));
	memcpy_code(reinterpret_cast<uint32_t>(data::text_buffer), const_cast<char *>(entry->second), length + 1);
	return data::text_buffer;
}

}