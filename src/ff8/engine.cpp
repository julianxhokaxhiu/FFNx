/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2023 myst6re                                            //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//    Copyright (C) 2023 Tang-Tang Zhou                                     //
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

#include "engine.h"
#include "../ff8.h"
#include "../log.h"

static const std::vector<std::string> ff8_table = {
  " ","0","1","2","3","4","5","6","7","8","9","%","/",":","!","?",
  "…","+","-","=","*","&","「","」","(",")","·",".",",","~","”","“",
  "‘","#","$","'","_","A","B","C","D","E","F","G","H","I","J","K",
  "L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","a",
  "b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q",
  "r","s","t","u","v","w","x","y","z","À","Á","Â","Ä","Ç","È","É",
  "Ê","Ë","Ì","Í","Î","Ï","Ñ","Ò","Ó","Ô","Ö","Ù","Ú","Û","Ü","Œ",
  "ß","à","á","â","ä","ç","è","é","ê","ë","ì","í","î","ï","ñ","ò",
  "ó","ô","ö","ù","ú","û","ü","œ","","[","]","■","○","♦","【","】",
  "□","","『","』","",";","","¯","×","","","↓","°","¡","¿","─","«",
  "»","±","♫","","↑","","","","™","<",">","","","","","","","","",
  "","","","","","","","","","","","","","","®","","","","","",
  "{in}","{e }","{ne}","{to}","{re}","{HP}","{l }","{ll}","{GF}",
  "{nt}","{il}","{o }","{ef}","{on}","{ w}","{ r}","{wi}","{fi}",
  "{EC}","{s }","{ar}","{FE}","{ S}","{ag}"
};

static const std::vector<std::string> ff8_names = {
	"Squall",
  "Zell",
  "Irvine",
  "Quistis",
  "Rinoa",
  "Selphie",
  "Seifer",
  "Edea",
  "Laguna",
  "Kiros",
  "Ward",
  "Angelo",
  "Griever",
  "Boko"
};

int ff8_manage_time_engine(int enable_rdtsc)
{
  // Force rdtsc
  return ff8_externals.enable_rdtsc_sub_40AA00(1);
}

std::string ff8_decode_text(const char* encoded_text)
{
  std::string ret{};
  int index = 0;
  char current_char = NULL, last_char = NULL;

  if (encoded_text != NULL)
  {
    while (current_char = encoded_text[index++], current_char != char(NULL))
    {
      // Control char, save it and continue
      if (current_char < 0x20)
      {
        last_char = current_char;
        continue;
      }

      // If it was a control char, evaluate
      if (last_char != char(NULL) && last_char < 0x20)
      {
        switch (last_char){
          case 0x2:
            ret.append(" ");
            ret.append(ff8_table[current_char - 0x20]);
            break;
          case 0x3:
            if(current_char>=0x30 && current_char<=0x3a)
              ret.append(ff8_names[current_char-0x30]);
            else if(current_char==0x40)
              ret.append(ff8_names[11]);
            else if(current_char==0x50)
              ret.append(ff8_names[12]);
            else if(current_char==0x60)
              ret.append(ff8_names[13]);
            break;
        }

        last_char = NULL;

        continue;
      }

      // Normal string char, append
      ret.append(ff8_table[current_char - 0x20]);
    }
  }

  return ret;
}
