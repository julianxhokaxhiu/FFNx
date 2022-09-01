/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
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

#include <array>

namespace ff7_field {

    struct field_bank_address
    {
        byte bank;
        byte address;

        uint32_t getID() const
        {
            return bank * 256 + address;
        }

        bool operator< (const field_bank_address& f) const
        {
            return this->getID() < f.getID();
        }

        bool operator== (const field_bank_address& f) const
        {
            return this->getID() == f.getID();
        }
    };

    std::array<uint32_t, 256> original_opcode_table {0};

    short ff7_opcode_multiply_get_bank_value(short bank, short address);
    short ff7_opcode_divide_get_bank_value(short bank, short address);
    int opcode_script_partial_animation_wrapper();
    int opcode_script_SHAKE();
    int opcode_script_WAIT();
    int opcode_script_MVIEF();
    int opcode_script_BGMOVIE();
    uint8_t opcode_IFSW_compare_sub();
    int opcode_script_FADE();

    // Thanks for myst6re https://github.com/myst6re/makoureactor/blob/5231723307901043941356ad1e42d26725305edf/core/field/Opcode.h#L71
    enum FieldOpcode {
        RET=0, REQ, REQSW, REQEW,
        PREQ, PRQSW, PRQEW, RETTO,
        JOIN, SPLIT, SPTYE, GTPYE,
        Unknown1, Unknown2, DSKCG, SPECIAL,

        JMPF, JMPFL, JMPB, JMPBL,
        IFUB, IFUBL, IFSW, IFSWL,
        IFUW, IFUWL, Unknown3, Unknown4,
        Unknown5, Unknown6, Unknown7, Unknown8,

        MINIGAME, TUTOR, BTMD2, BTRLD,
        WAIT, NFADE, BLINK, BGMOVIE,
        KAWAI, KAWIW, PMOVA, SLIP,
        BGPDH, BGSCR, WCLS, WSIZW,

        IFKEY, IFKEYON, IFKEYOFF, UC,
        PDIRA, PTURA, WSPCL, WNUMB,
        STTIM, GOLDu, GOLDd, CHGLD,
        HMPMAX1, HMPMAX2, MHMMX, HMPMAX3,

        MESSAGE, MPARA, MPRA2, MPNAM,
        Unknown9, MPu, Unknown10, MPd,
        ASK, MENU, MENU2, BTLTB,
        Unknown11, HPu, Unknown12, HPd,

        WINDOW, WMOVE, WMODE, WREST,
        WCLSE, WROW, GWCOL, SWCOL,
        STITM, DLITM, CKITM, SMTRA,
        DMTRA, CMTRA, SHAKE, NOP,

        MAPJUMP, SCRLO, SCRLC, SCRLA,
        SCR2D, SCRCC, SCR2DC, SCRLW,
        SCR2DL, MPDSP, VWOFT, FADE,
        FADEW, IDLCK, LSTMP, SCRLP,

        BATTLE, BTLON, BTLMD, PGTDR,
        GETPC, PXYZI, PLUSX, PLUS2X,
        MINUSX, MINUS2X, INCX, INC2X,
        DECX, DEC2X, TLKON, RDMSD,

        SETBYTE, SETWORD, BITON, BITOFF,
        BITXOR, PLUS, PLUS2, MINUS,
        MINUS2, MUL, MUL2, DIV,
        DIV2, MOD, MOD2, AND,

        AND2, OR, OR2, XOR,
        XOR2, INC, INC2, DEC,
        DEC2, RANDOM, LBYTE, HBYTE,
        TOBYTE, SETX, GETX, SEARCHX,

        PC, CHAR, DFANM, ANIME1,
        VISI, XYZI, XYI, XYZ,
        MOVE, CMOVE, MOVA, TURA,
        ANIMW, FMOVE, ANIME2, ANIMX1,

        CANIM1, CANMX1, MSPED, DIR,
        TURNGEN, TURN, DIRA, GETDIR,
        GETAXY, GETAI, ANIMX2, CANIM2,
        CANMX2, ASPED, Unknown13, CC,

        JUMP, AXYZI, LADER, OFST,
        OFSTW, TALKR, SLIDR, SOLID,
        PRTYP, PRTYM, PRTYE, IFPRTYQ,
        IFMEMBQ, MMBud, MMBLK, MMBUK,

        LINE, LINON, MPJPO, SLINE,
        SIN, COS, TLKR2, SLDR2,
        PMJMP, PMJMP2, AKAO2, FCFIX,
        CCANM, ANIMB, TURNW, MPPAL,

        BGON, BGOFF, BGROL, BGROL2,
        BGCLR, STPAL, LDPAL, CPPAL,
        RTPAL, ADPAL, MPPAL2, STPLS,
        LDPLS, CPPAL2, RTPAL2, ADPAL2,

        MUSIC, SOUND, AKAO, MUSVT,
        MUSVM, MULCK, BMUSC, CHMPH,
        PMVIE, MOVIE, MVIEF, MVCAM,
        FMUSC, CMUSC, CHMST, GAMEOVER,
        OPCODE_COUNT
    };
}