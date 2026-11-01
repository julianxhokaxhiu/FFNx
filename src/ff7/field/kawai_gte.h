/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
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

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstddef>
#include <optional>

namespace ff7::field::kawai::gte
{
    using Vector = std::array<int16_t, 3>;
    using LongVector = std::array<int32_t, 3>;
    using Matrix = std::array<Vector, 3>;

    struct Affine
    {
        Matrix rotation = {{{4096, 0, 0}, {0, 4096, 0}, {0, 0, 4096}}};
        LongVector translation{};
    };

    struct Plane
    {
        Vector normal{};
        Vector anchor{};
        Matrix rotation{};
    };

    constexpr int16_t word(int64_t value)
    {
        return std::bit_cast<int16_t>(static_cast<uint16_t>(value));
    }

    constexpr int32_t long_word(int64_t value)
    {
        return std::bit_cast<int32_t>(static_cast<uint32_t>(value));
    }

    constexpr int64_t mac44(int64_t value)
    {
        const uint64_t bits = static_cast<uint64_t>(value) & ((uint64_t{1} << 44) - 1);
        return (bits & (uint64_t{1} << 43)) ? static_cast<int64_t>(bits) - (int64_t{1} << 44) :
            static_cast<int64_t>(bits);
    }

    constexpr int16_t ir(int64_t value, bool positive = false)
    {
        return static_cast<int16_t>(std::clamp<int64_t>(value, positive ? 0 : -32768, 32767));
    }

    constexpr uint32_t integer_sqrt(uint32_t value)
    {
        uint32_t result = 0;
        for (uint32_t bit = uint32_t{1} << 30; bit != 0; bit >>= 2)
        {
            if (value >= result + bit)
            {
                value -= result + bit;
                result = (result >> 1) + bit;
            }
            else result >>= 1;
        }
        return result;
    }

    // These formulas reproduce all 192 entries of the USA PsyQ SQRT/rsqrt tables
    // at 8004B668/8004B7FC, including the table-index quantization.
    constexpr std::optional<int32_t> square_root(uint32_t value)
    {
        if (value > 0x7FFFFFFF) return std::nullopt;
        if (value == 0) return 0;
        const int leading = std::countl_zero(value) & ~1;
        const uint32_t normalized = leading >= 24 ? value << (leading - 24) : value >> (24 - leading);
        const uint32_t table = integer_sqrt(normalized << 18);
        return static_cast<int32_t>((table << ((31 - leading) / 2)) >> 12);
    }

    constexpr std::optional<Vector> normalize(const Vector& vector)
    {
        uint32_t squared = 0;
        for (int32_t component : vector) squared += static_cast<uint32_t>(component * component);
        if (squared > 0x7FFFFFFF) return std::nullopt;
        if (squared == 0) return Vector{};
        const int leading = std::countl_zero(squared) & ~1;
        const uint32_t normalized = leading >= 24 ? squared << (leading - 24) : squared >> (24 - leading);
        const int32_t factor = static_cast<int32_t>(integer_sqrt((uint32_t{1} << 30) / normalized));
        Vector result{};
        for (size_t i = 0; i < 3; ++i)
            result[i] = word((static_cast<int32_t>(vector[i]) * factor) >> ((31 - leading) / 2));
        return result;
    }

    constexpr LongVector transform(const Affine& matrix, const Vector& vector)
    {
        LongVector result{};
        for (size_t i = 0; i < 3; ++i)
        {
            int64_t accumulator = mac44(static_cast<int64_t>(matrix.translation[i]) * 4096);
            for (size_t j = 0; j < 3; ++j)
                accumulator = mac44(accumulator + static_cast<int32_t>(matrix.rotation[i][j]) * vector[j]);
            result[i] = long_word(accumulator >> 12);
        }
        return result;
    }

    constexpr Vector transform_ir(const Affine& matrix, const Vector& vector, bool positive = false)
    {
        const auto values = transform(matrix, vector);
        return {ir(values[0], positive), ir(values[1], positive), ir(values[2], positive)};
    }

    constexpr LongVector transform_world(const Affine& matrix, const LongVector& vector)
    {
        LongVector result{};
        for (size_t i = 0; i < 3; ++i)
        {
            int64_t accumulator = mac44(static_cast<int64_t>(matrix.translation[i]) * 4096);
            for (size_t j = 0; j < 3; ++j)
                accumulator = mac44(accumulator + static_cast<int64_t>(matrix.rotation[i][j]) * vector[j]);
            result[i] = long_word(accumulator >> 12);
        }
        return result;
    }

    constexpr Matrix transpose(const Matrix& matrix)
    {
        Matrix result{};
        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < 3; ++j) result[i][j] = matrix[j][i];
        return result;
    }

    constexpr Affine compose(const Affine& left, const Affine& right)
    {
        Affine result;
        const Affine rotation = {left.rotation, {}};
        for (size_t column = 0; column < 3; ++column)
        {
            const auto values = transform_ir(rotation,
                {right.rotation[0][column], right.rotation[1][column], right.rotation[2][column]});
            for (size_t row = 0; row < 3; ++row) result.rotation[row][column] = values[row];
        }
        const auto translation = transform_ir(left,
            {word(right.translation[0]), word(right.translation[1]), word(right.translation[2])});
        for (size_t i = 0; i < 3; ++i) result.translation[i] = translation[i];
        return result;
    }

    // INTPL (980011), then its saturated IR values become GPL0's MAC base.
    // MTC2 sign-extends IR0; it does not clamp the interpolation factor to 0..4096.
    constexpr Vector interpolate(const Vector& start, const LongVector& far_color, int16_t factor)
    {
        Vector result{};
        for (size_t i = 0; i < 3; ++i)
        {
            const int64_t base = static_cast<int64_t>(start[i]) * 4096;
            const auto difference = ir((static_cast<int64_t>(far_color[i]) * 4096 - base) >> 12);
            result[i] = ir((base + static_cast<int32_t>(factor) * difference) >> 12);
        }
        return result;
    }

    constexpr uint32_t normal_color(uint32_t color, const Matrix& directions, const Matrix& colors,
        const LongVector& background, const Vector& normal)
    {
        const auto light = transform_ir({directions, {}}, normal, true);
        const auto illumination = transform_ir({colors, background}, light, true);
        uint32_t result = color & 0xFF000000;
        for (size_t i = 0; i < 3; ++i)
        {
            const size_t shift = 16 - 8 * i;
            const int32_t mac = (static_cast<int32_t>((color >> shift) & 255) * 16 * illumination[i]) >> 12;
            result |= static_cast<uint32_t>(std::clamp(mac >> 4, 0, 255)) << shift;
        }
        return result;
    }

    constexpr std::optional<Plane> make_plane(const Vector& normal, int16_t distance)
    {
        const auto unit = normalize(normal);
        if (!unit || *unit == Vector{}) return std::nullopt;
        Plane plane;
        plane.normal = *unit;
        const size_t component = (*unit)[2] ? 2 : ((*unit)[1] ? 1 : 0);
        plane.anchor[component] = word(static_cast<int32_t>(distance) * 4096 / (*unit)[component]);
        const Vector axis = {(*unit)[1], word(-(*unit)[0]), 0};
        const auto unit_axis = normalize(axis);
        const auto sine = square_root(static_cast<uint32_t>(axis[0] * axis[0] + axis[1] * axis[1]));
        if (!unit_axis || !sine) return std::nullopt;
        if (*unit_axis == Vector{})
        {
            plane.rotation = Affine{}.rotation;
            return plane;
        }
        const int32_t x = (*unit_axis)[0], y = (*unit_axis)[1], z = (*unit_axis)[2];
        const int32_t cosine = (*unit)[2], complement = 4096 - cosine;
        const int32_t xx = x * x, yy = y * y, zz = z * z, xy = x * y, xz = x * z, yz = y * z;
        const int32_t xs = x * *sine, ys = y * *sine, zs = z * *sine;
        plane.rotation = {{
            {word((xx + cosine * ((4096 * 4096 - xx) / 4096)) / 4096),
                word(((xy / 4096) * complement - zs) / 4096), word(((xz / 4096) * complement + ys) / 4096)},
            {word(((xy / 4096) * complement + zs) / 4096),
                word((yy + cosine * ((4096 * 4096 - yy) / 4096)) / 4096), word(((yz / 4096) * complement - xs) / 4096)},
            {word(((xz / 4096) * complement - ys) / 4096), word(((yz / 4096) * complement + xs) / 4096),
                word((zz + cosine * ((4096 * 4096 - zz) / 4096)) / 4096)}
        }};
        return plane;
    }

    constexpr Affine to_plane(const Plane& plane, const Affine& model)
    {
        Affine translation;
        for (size_t i = 0; i < 3; ++i) translation.translation[i] = -plane.anchor[i];
        return compose({plane.rotation, {}}, compose(translation, model));
    }

    constexpr Affine from_plane(const Plane& plane, const Affine& camera)
    {
        Affine translation;
        for (size_t i = 0; i < 3; ++i) translation.translation[i] = plane.anchor[i];
        return compose(compose(camera, translation), {transpose(plane.rotation), {}});
    }

    constexpr bool project(Vector& position, const Plane& plane)
    {
        const bool reverse = plane.normal == Vector{0, 0, -4096};
        const bool visible = reverse ? position[2] < 0 : position[2] > 0;
        if (!visible) position[2] = 0;
        return visible;
    }

    constexpr std::optional<Vector> point_direction(const Vector& light, int16_t radius,
        const LongVector& position, const Matrix& rotation)
    {
        if (radius == 0) return std::nullopt;
        LongVector delta{};
        uint32_t squared = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            delta[i] = long_word(static_cast<int64_t>(light[i]) - position[i]);
            squared += static_cast<uint32_t>(static_cast<int64_t>(delta[i]) * delta[i]);
        }
        const auto root = square_root(squared);
        if (!root) return std::nullopt;
        const int32_t distance = *root + 2;
        const int32_t attenuation = long_word(static_cast<int64_t>(std::max<int32_t>(radius - distance, 0)) * 4096) / radius;
        Vector direction{};
        for (size_t i = 0; i < 3; ++i)
        {
            const auto component = word(long_word(static_cast<int64_t>(delta[i]) * 4096) / distance);
            direction[i] = word((static_cast<int32_t>(component) * word(attenuation)) >> 12);
        }
        return transform_ir({transpose(rotation), {}}, direction);
    }
}
