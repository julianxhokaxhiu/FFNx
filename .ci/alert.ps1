#*****************************************************************************#
#    Copyright (C) 2009 Aali132                                               #
#    Copyright (C) 2018 quantumpencil                                         #
#    Copyright (C) 2018 Maxime Bacoux                                         #
#    Copyright (C) 2020 Julian Xhokaxhiu                                      #
#    Copyright (C) 2020 myst6re                                               #
#    Copyright (C) 2020 Chris Rizzitello                                      #
#    Copyright (C) 2020 John Pritchard                                        #
#                                                                             #
#    This file is part of FFNx                                                #
#                                                                             #
#    FFNx is free software: you can redistribute it and\or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    FFNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

$downloadUrl = "https://github.com/julianxhokaxhiu/FFNx/releases/latest"

if ($env:_IS_BUILD_CANARY -eq "true")
{
  $downloadUrl = "https://github.com/julianxhokaxhiu/FFNx/releases/tag/canary"
}

# Initial template from https://discohook.org/
$discordPost = @"
{
  "username": "FFNx",
  "avatar_url": "https://avatars3.githubusercontent.com/ml/1303",
  "content": "Release **${env:_RELEASE_VERSION}** has just been published!\n\nDownload Url: ${downloadUrl}\n\nIf you find something broken or unexpected, feel free to check existing ones first here https://github.com/julianxhokaxhiu/FFNx/issues.\nIf non existing, then report your issue here https://github.com/julianxhokaxhiu/FFNx/issues/new.\n\nThank you for using FFNx!",
  "embeds": [
    {
      "title": "How to install",
      "description": "Feel free to follow instructions at this link, depending on which version of the game you own: https://github.com/julianxhokaxhiu/FFNx#how-to-install",
      "color": 7506394
    },
    {
      "title": "FFNx is FOSS Software!",
      "description": "FFNx is released under GPLv3 license. More info here: https://github.com/julianxhokaxhiu/FFNx#license",
      "color": 15746887
    }
  ]
}
"@

Invoke-RestMethod -Uri $env:_MAP_FFNX_TSUNAMODS -ContentType "application/json" -Method Post -Body $discordPost
Invoke-RestMethod -Uri $env:_MAP_FFNX_QHIMM_FF7 -ContentType "application/json" -Method Post -Body $discordPost
Invoke-RestMethod -Uri $env:_MAP_FFNX_QHIMM_FF8 -ContentType "application/json" -Method Post -Body $discordPost
