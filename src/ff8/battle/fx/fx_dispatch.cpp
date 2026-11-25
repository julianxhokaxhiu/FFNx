/****************************************************************************/
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//    Copyright (C) 2026 HobbitDur                                          //
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

// Native battle effects: installation

#include "fx_port.h"
#include "../../../patch.h"
#include "../../../log.h"
#include "../../../common.h"

namespace ff8fx
{
static bool g_installed = false;

void install()
{
    if (g_installed)
        return;
    g_installed = true;
    if (!address_init())
    {
        ffnx_info("FF8 battle fx: native effect code not available for this release\n");
        return;
    }
    register_all();
    set_ports(true);
    ffnx_info("FF8 battle fx: native effect code on\n");
}
} // namespace ff8fx
