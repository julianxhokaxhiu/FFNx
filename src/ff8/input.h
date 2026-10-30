#pragma once

#include <cstdint>

constexpr int ff8_japanese_action_to_engine(int action)
{
    // Japanese: Menu, Confirm, Cancel. Western engine: Cancel, Menu, Confirm.
    return action == 4 ? 5 : action == 5 ? 6 : action == 6 ? 4 : action;
}

constexpr uint32_t ff8_japanese_buttons_to_engine(uint32_t buttons)
{
    uint32_t result = buttons & ~0x70u;

    for (int action = 4; action <= 6; ++action)
    {
        if (buttons & (1u << action))
            result |= 1u << ff8_japanese_action_to_engine(action);
    }

    return result;
}
