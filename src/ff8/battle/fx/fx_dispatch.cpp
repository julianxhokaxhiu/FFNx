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
#include "../../../cfg.h"
#include "../../../common.h"

namespace ff8fx
{
	void verify_install();
	void (*g_queue_seen)(TaskQueue *q) = nullptr;
	static bool g_installed = false;

	// Replaces the game's task queue executor only while verifying, so the verifier sees every queue that runs
	static int __cdecl execute_task_queue(TaskQueue *q)
	{
		if (g_queue_seen) g_queue_seen(q);
		TaskNode *prev = nullptr;
		int kept = 0;
		for (TaskNode *cur = q->head; cur; cur = cur->next)
		{
			if (cur->func(cur) & 2)
			{
				cur->flags = 0;
				if (prev) prev->next = cur->next;
				else q->head = cur->next;
			}
			else
			{
				prev = cur;
				kept++;
			}
		}
		q->tail = prev;
		return kept;
	}

	void install()
	{
		if (g_installed || !ff8_battle_fx_native) return;
		g_installed = true;
		if (!address_init())
		{
			ffnx_info("FF8 battle fx: native effect code not available for this release\n");
			return;
		}
		register_all();
		set_ports(true);
		ffnx_info("FF8 battle fx: native effect code on%s\n", ff8_battle_fx_verify ? ", verified against the original" : "");
		if (ff8_battle_fx_verify)
		{
			replace_function(game_addr(game::ExecuteTaskQueue), (void *)execute_task_queue);
			prim::install_verify();
			verify_install();
		}
	}
}
