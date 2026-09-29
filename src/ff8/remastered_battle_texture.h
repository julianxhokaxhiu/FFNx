#pragma once

#include <span>
#include <string>

namespace remastered_battle_texture
{
enum class MappingKind
{
	NamedOverride,
	BlockedPrefix,
	SourceAlias,
	FamilyVariant,
	ImageLayout
};

enum class CompanionPosition
{
	None,
	Upper,
	Lower
};

struct PaletteLayer
{
	int firstPaletteId;
	int lastPaletteId;
	int imageIndex;
	bool alignRight;
	bool alignBottom;

	constexpr bool matchesPalette(int paletteId) const
	{
		return paletteId >= firstPaletteId && paletteId <= lastPaletteId;
	}
};

struct ImageLayout
{
	int imageIndex;
	int sourceWidthDivisor;
	int sourceHeightDivisor;
	int companionIndex;
	CompanionPosition companionPosition;
	bool alignRight;
	bool alignBottom;
	bool useTargetPosition;
	bool palettePageImages;
	std::span<const PaletteLayer> paletteLayers{};
};

struct Mapping
{
	MappingKind kind;
	const char *battleName;
	int resourceId;
	int variant;
	const char *family;
	int outputIndex;
	ImageLayout layout;
};

constexpr ImageLayout noLayout()
{
	return { -1, 1, 1, -1, CompanionPosition::None, false, false, false, false };
}

constexpr Mapping sourceAlias(const char *battleName, int resourceId)
{
	return { MappingKind::SourceAlias, battleName, resourceId, -1, nullptr, -1, noLayout() };
}

constexpr Mapping namedOverride(const char *battleName, const char *family, int outputIndex)
{
	return { MappingKind::NamedOverride, battleName, -1, -1, family, outputIndex, noLayout() };
}

constexpr Mapping blockedPrefix(const char *battleName)
{
	return { MappingKind::BlockedPrefix, battleName, -1, -1, nullptr, -1, noLayout() };
}

constexpr Mapping familyVariant(int resourceId, int variant, const char *family)
{
	return { MappingKind::FamilyVariant, nullptr, resourceId, variant, family, -1, noLayout() };
}

constexpr Mapping imageLayout(const char *family, int imageIndex, int sourceWidthDivisor, int sourceHeightDivisor,
	int companionIndex, CompanionPosition companionPosition, bool alignRight, bool alignBottom,
	bool useTargetPosition, bool palettePageImages, std::span<const PaletteLayer> paletteLayers = {})
{
	return { MappingKind::ImageLayout, nullptr, -1, -1, family, -1,
		{ imageIndex, sourceWidthDivisor, sourceHeightDivisor, companionIndex, companionPosition,
			alignRight, alignBottom, useTargetPosition, palettePageImages, paletteLayers } };
}

template<PaletteLayer... Layers>
inline constexpr PaletteLayer layerList[] = { Layers... };

inline constexpr Mapping mappings[] = {
	sourceAlias("battle/MAG095_B.1T2", 99),
	sourceAlias("battle/MAG089_B.1T0", 95),
	sourceAlias("battle/MAG201_B.04", 991),
	sourceAlias("battle/MAG202_B.03", 201),
	sourceAlias("battle/MAG203_B.05", 202),
	sourceAlias("battle/MAG005_B.03", 203),
	sourceAlias("battle/MAG186_A.DAT", 902),
	sourceAlias("battle/MAG325_A.DAT", 186),
	sourceAlias("battle/MAG325_F.DAT", 325),
	sourceAlias("battle/MAG326_A.DAT", 990),
	sourceAlias("battle/MAG326_E.DAT", 326),
	sourceAlias("battle/MAG217_A.DAT", 326),
	blockedPrefix("battle/MAG095_B.1T0"),
	blockedPrefix("battle/MAG095_B.1T1"),
	namedOverride("battle/MAG095_B.1T2", "mag095", 0),
	namedOverride("battle/MAG095_B.1T3", "mag095", 1),
	namedOverride("battle/MAG325_F.DAT", "mag325", 4),
	namedOverride("battle/MAG326_B.DAT", "mag326", 0),
	blockedPrefix("battle/MAG326_C.DAT"),
	namedOverride("battle/MAG326_E.DAT", "mag326", 6),
	blockedPrefix("battle/MAG326_K.DAT"),
	namedOverride("battle/MAG099_B.4T0", "mag099", 1),
	namedOverride("battle/MAG099_B.4T1", "mag099", 2),
	blockedPrefix("battle/MAG099_B."),
	namedOverride("battle/MAG139_H.00", "mag139", 0),
	namedOverride("battle/MAG139_H.01", "mag139", 1),
	namedOverride("battle/MAG139_H.06", "mag139", 1),
	blockedPrefix("battle/MAG139"),
	familyVariant(5, 0, "mag005"),
	familyVariant(85, 0, "mag085"), { MappingKind::FamilyVariant, nullptr, 86, 0, "mag085", -1, noLayout() },
	familyVariant(87, 0, "mag085"), familyVariant(88, 0, "mag085"),
	familyVariant(90, 0, "mag085"), familyVariant(91, 0, "mag085"),
	familyVariant(92, 0, "mag085"), familyVariant(93, 0, "mag085"),
	familyVariant(1056, -1, "mag089"), familyVariant(89, 0, "mag089"), familyVariant(991, 1, "mag089"),
	familyVariant(94, 0, "mag094"), familyVariant(94, 1, "mag094"), familyVariant(95, 0, "mag095"),
	familyVariant(99, -1, "mag099"), familyVariant(115, 0, "mag115"), familyVariant(115, 1, "mag115"),
	familyVariant(139, 0, "mag139"), familyVariant(139, 1, "mag139"),
	familyVariant(184, -1, "mag184"), familyVariant(190, -1, "mag190"),
	familyVariant(1061, -1, "mag200"), familyVariant(200, 0, "mag200"),
	familyVariant(1067, -1, "mag201"), familyVariant(201, 0, "mag201"), familyVariant(201, 1, "mag201"),
	familyVariant(1065, -1, "mag202"), familyVariant(202, 0, "mag202"), familyVariant(202, 1, "mag202"),
	familyVariant(203, 0, "mag203"), familyVariant(203, 1, "mag203"),
	familyVariant(204, 0, "mag204"), familyVariant(204, 1, "mag204"), familyVariant(205, -1, "mag205"),
	familyVariant(217, 2, "mag217"), familyVariant(277, 0, "mag277"),
	familyVariant(290, 0, "mag290"), familyVariant(290, 1, "mag290"),
	familyVariant(1066, -1, "mag324"), familyVariant(324, -1, "mag324"),
	familyVariant(1070, -1, "mag325"), familyVariant(186, 0, "mag325"),
	familyVariant(186, 1, "mag325"), familyVariant(186, 2, "mag325"), familyVariant(186, 3, "mag325"),
	familyVariant(325, 0, "mag325"), familyVariant(325, 1, "mag325"), familyVariant(990, 2, "mag325"),
	familyVariant(326, 0, "mag326"), familyVariant(326, 1, "mag326"),
	familyVariant(326, 2, "mag326"), familyVariant(326, 3, "mag326"),
	familyVariant(217, 0, "mag326"), familyVariant(217, 1, "mag326"),
	familyVariant(1055, -1, "mag900"), familyVariant(900, 0, "mag900"),
	familyVariant(901, 0, "mag901"), familyVariant(902, -1, "mag902"),
	imageLayout("mag095", 0, 1, 1, -1, CompanionPosition::None, false, false, true, false),
	imageLayout("mag095", 1, 1, 1, -1, CompanionPosition::None, false, true, true, false),
	imageLayout("mag099", 1, 2, 2, 0, CompanionPosition::Upper, false, false, false, false),
	imageLayout("mag099", 2, 2, 2, 3, CompanionPosition::Lower, false, false, false, false),
	imageLayout("mag139", 0, 1, 2, -1, CompanionPosition::None, false, false, true, false,
		layerList<PaletteLayer{ 0, 1, 0, false, false }, PaletteLayer{ 0, 1, 2, false, true }>),
	imageLayout("mag139", 1, 1, 1, -1, CompanionPosition::None, false, false, true, false,
		layerList<PaletteLayer{ 0, 0, 1, false, false }>),
	imageLayout("mag325", 0, 3, 2, -1, CompanionPosition::None, false, false, true, true),
	imageLayout("mag325", 1, 3, 2, -1, CompanionPosition::None, false, true, true, true),
	imageLayout("mag325", 2, 3, 2, -1, CompanionPosition::None, false, false, true, true),
	imageLayout("mag325", 3, 3, 2, -1, CompanionPosition::None, false, true, true, true),
	imageLayout("mag325", 4, 1, 1, -1, CompanionPosition::None, false, false, true, false),
	imageLayout("mag326", 0, 3, 2, -1, CompanionPosition::None, false, false, true, false,
		layerList<
			PaletteLayer{ 0, 1, 0, false, false }, PaletteLayer{ 0, 1, 1, false, true },
			PaletteLayer{ 2, 3, 2, false, false }, PaletteLayer{ 2, 2, 3, false, true }, PaletteLayer{ 2, 3, 0, true, false },
			PaletteLayer{ 4, 5, 4, true, false },
			PaletteLayer{ 6, 10, 4, false, false }>),
	imageLayout("mag326", 6, 1, 2, -1, CompanionPosition::None, false, false, true, false,
		layerList<
			PaletteLayer{ 0, 1, 6, false, false },
			PaletteLayer{ 0, 0, 1, false, true }, PaletteLayer{ 1, 1, 7, false, true }>),
};

const ImageLayout *findImageLayout(const std::string &remasteredName, int imageIndexOverride = -1);
}