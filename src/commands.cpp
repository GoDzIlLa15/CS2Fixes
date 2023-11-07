/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "detours.h"
#include "common.h"
#include "utlstring.h"
#include "recipientfilters.h"
#include "commands.h"
#include "utils/entity.h"
#include "entity/cbaseentity.h"
#include "entity/ccsweaponbase.h"
#include "entity/ccsplayercontroller.h"
#include "entity/ccsplayerpawn.h"
#include "entity/cbasemodelentity.h"
#include "playermanager.h"
#include "adminsystem.h"
#include "ctimer.h"
#include "httpmanager.h"
#include "engine/igameeventsystem.h"
#include "tier0/memdbgon.h"

extern CEntitySystem *g_pEntitySystem;
extern IVEngineServer2* g_pEngineServer2;
extern ISteamHTTP* g_http;
extern CGlobalVars *gpGlobals;



void ParseChatCommand(const char *pMessage, CCSPlayerController *pController)
{
	if (!pController || !pController->IsConnected())
		return;

	CCommand args;
	args.Tokenize(pMessage + 1);

	uint16 index = g_CommandList.Find(hash_32_fnv1a_const(args[0]));

	if (g_CommandList.IsValidIndex(index))
	{
		(*g_CommandList[index])(args, pController);
	}
	else
	{
		ClientPrint(pController, HUD_PRINTTALK, CHAT_PREFIX "Unknown command: %s", args[0]);
	}
}

bool CChatCommand::CheckCommandAccess(CBasePlayerController *pPlayer, uint64 flags)
{
	if (!pPlayer)
		return false;

	int slot = pPlayer->GetPlayerSlot();

	ZEPlayer *pZEPlayer = g_playerManager->GetPlayer(slot);

	if (!pZEPlayer->IsAdminFlagSet(flags))
	{
		ClientPrint(pPlayer, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return false;
	}

	return true;
}

void ClientPrintAll(int hud_dest, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[256];
	V_vsnprintf(buf, sizeof(buf), msg, args);

	va_end(args);

	addresses::UTIL_ClientPrintAll(hud_dest, buf, nullptr, nullptr, nullptr, nullptr);
	ConMsg("%s\n", buf);
}

void ClientPrint(CBasePlayerController *player, int hud_dest, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[256];
	V_vsnprintf(buf, sizeof(buf), msg, args);

	va_end(args);

	if (player)
		addresses::ClientPrint(player, hud_dest, buf, nullptr, nullptr, nullptr, nullptr);
	else
		ConMsg("%s\n", buf);
}



CON_COMMAND_CHAT(vipinfo, "vip info")
{
	if (!player)
		return;

	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Starting health: \4 100-115.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Starting armor: \4 110-120.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Money add every round: \4 1000-3000.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Starting with: \4 defeuser, he, smoke, molotov, flashbang.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Smoke color: \4 green.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1For buying VIP, join our discord: \4 discord.gg/1tap.");
}

// make a command that prints test

CON_COMMAND_CHAT(vip, "vip info")
{
	if (!player)
		return;

	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Starting health: \4 100-115.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Starting armor: \4 110-120.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Money add every round: \4 1000-3000.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Starting with: \4 defeuser, he, smoke, molotov, flashbang.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1Smoke color: \4 green.");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"\1For buying VIP, join our discord: \4 discord.gg/1tap.");
}

CON_COMMAND_CHAT(rs, "reset your score")
{
	if (!player)
		return;

	player->m_pActionTrackingServices->m_matchStats().m_iKills = 0;
	player->m_pActionTrackingServices->m_matchStats().m_iDeaths = 0;
	player->m_pActionTrackingServices->m_matchStats().m_iAssists = 0;
	player->m_pActionTrackingServices->m_matchStats().m_iDamage = 0;
	player->m_iScore = 0;
	player->m_iMVPs = 0;

	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You successfully reset your score.");
}

CON_COMMAND_CHAT(ws, "reset your score")
{
	if (!player)
		return;


	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"-------------------------------------------------");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX" \x01 Foloseste comanda \x06skin <skin_id> \x01in CONSOLA!");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX" \x01 Pentru a gasi \x06skin_id \x01poti folosi site-ul \x06 csgostash.com !");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX" \x01 Mai multe detalii pe \x06 discord.gg/1tap!");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"-------------------------------------------------");
}

CON_COMMAND_CHAT(knife, "reset your score")
{
	if (!player)
		return;


	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"-------------------------------------------------");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX" \x01 Foloseste comanda \x06knife <nume> \x01in CONSOLA!");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX" \x01 Variantele de nume acceptate sunt \x06karambit, bayonet, css, m9, bowie, butterfly, flip, push, huntsman, falchion, gut, ursus, navaja, stiletto, talon, paracord, survival, nomad !");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX" \x01 Mai multe detalii pe \x06 discord.gg/1tap!");
	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"-------------------------------------------------");
}


CON_COMMAND_CHAT(who, "get list of all admin players online")
{
    if (!player)
        return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer* pPlayer1 = g_playerManager->GetPlayer(iCommandPlayer);


	if (!pPlayer1->IsAdminFlagSet(ADMFLAG_CHEATS))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You don't have access to this command.");
		return;
	}

    const char* pszFlagNames[] = {
        "ADMFLAG_CUSTOM1",
        "ADMFLAG_CUSTOM2",
        "ADMFLAG_CUSTOM3",
        "ADMFLAG_CUSTOM4",
        "ADMFLAG_CUSTOM5",
        "ADMFLAG_CUSTOM6",
        "ADMFLAG_CUSTOM7",
        "ADMFLAG_CUSTOM8"
    };

    const char* pszCustomNames[] = {
        "Helper",
        "Administrator",
        "Moderator",
        "Veteran",
		"Manager",
		"Co-Owner",
		"Tester",
		"Supervizor",
    };

    const int iNumFlags = sizeof(pszFlagNames) / sizeof(pszFlagNames[0]);

    uint64_t uiFlagValues[] = {
        ADMFLAG_CUSTOM1,
        ADMFLAG_CUSTOM2,
        ADMFLAG_CUSTOM3,
        ADMFLAG_CUSTOM4,
        ADMFLAG_CUSTOM5,
        ADMFLAG_CUSTOM6,
        ADMFLAG_CUSTOM7,
        ADMFLAG_CUSTOM8,
        ADMFLAG_CUSTOM9
    };

    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        CBasePlayerController* cPlayer = (CBasePlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)i);
        if (!cPlayer)
            continue;

        ZEPlayer* pPlayer = g_playerManager->GetPlayer(i - 1);
        if (!pPlayer)
            continue;

        std::string sAdminFlags = "";
        for (int j = 0; j < iNumFlags; j++)
        {
            if (pPlayer->IsAdminFlagSet(uiFlagValues[j]))
            {
                if (j < 4)
                {
                    sAdminFlags += pszCustomNames[j];
                }
                else
                {
                    sAdminFlags += pszFlagNames[j];
                }
                sAdminFlags += " ";
            }
        }

        if (sAdminFlags.empty())
            continue;

        ClientPrint(player, HUD_PRINTCONSOLE, CHAT_PREFIX "Player %s has admin flags: %s", cPlayer->GetPlayerName(), sAdminFlags.c_str());
    }
}

const char* webHookUrl7 = "https://discord.com/api/webhooks/1168484465253826631/TuDaqbAWGbBwBjSIYmaV-7AW-9GLdrarT2ULqEsLl9dN3xoy7LVsQTGkLcowJrrxH1eP";
const char* jsonTemplate7 = R"({
		"username": "[BOT] CS2.1TAP.RO",
		"avatar_url": "https://i.imgur.com/kACf2pm.png",
		"content": "@everyone Cheater reported!",
		"embeds": [
			{
				"author": {
					"name": "%s",
					"icon_url": "https://i.imgur.com/kACf2pm.png"
				},
				"description": "%s",
				"color": 16711680
			}
		]
	})";


void HttpCallback3(HTTPRequestHandle request, char* response)
{
	Message(response);
}


CON_COMMAND_CHAT(myuid, "test")
{
	if (!player)
		return;

	int iPlayer = player->GetPlayerSlot();

	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Your userid is %i, slot: %i, retrieved slot: %i", g_pEngineServer2->GetPlayerUserId(iPlayer).Get(), iPlayer, g_playerManager->GetSlotFromUserId(g_pEngineServer2->GetPlayerUserId(iPlayer).Get()));
}

CON_COMMAND_CHAT(a, "admins chat")
{
    if (!player)
        return;

    int iCommandPlayer = player->GetPlayerSlot();

    ZEPlayer *pPlayer = g_playerManager->GetPlayer(iCommandPlayer);
    if (args.ArgC() < 2)
    {
        
        ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Usage: /a <message> to admins");
        return;
    }

for (int i = 0; i < MAXPLAYERS; i++)
{
    ZEPlayer* pAdmin = g_playerManager->GetPlayer(i);
    CBasePlayerController* cPlayer = (CBasePlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)(i + 1));

    if (!cPlayer || !pAdmin || pAdmin->IsFakeClient() || !pAdmin->IsAdminFlagSet(ADMFLAG_SLAY))
        continue;
        ClientPrint(cPlayer, HUD_PRINTTALK, " \1(\2ADMINS CHAT\1) \4%s: \1 \13%s", player->GetPlayerName(), args.ArgS());
}
}

// write a function to send a discord message when writing !http in chat using the above json string
CON_COMMAND_CHAT(calladmin, "Calladmin")
{
    if (!player)
        return;

    if (args.ArgC() < 2)
    {
        ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Usage: !calladmin <message>");
        return;
    }

    char jsonStr[2048];
    snprintf(jsonStr, sizeof(jsonStr), jsonTemplate7, player->GetPlayerName(), args.GetCommandString() + strlen(args[0]) + 1);

    g_HTTPManager.POST(webHookUrl7, jsonStr, &HttpCallback3);
}


CON_COMMAND_CHAT(medic, "medic")
{
	if (!player)
		return;

	int health = 0;
	int iPlayer = player->GetPlayerSlot();

	Z_CBaseEntity* pEnt = (Z_CBaseEntity*)player->GetPawn();

	//ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIXM"pZEPlayer testing...");

	ZEPlayer* pZEPlayer = g_playerManager->GetPlayer(iPlayer);
	if (!pZEPlayer)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"pZEPlayer not valid.");
		return;
	}

	if (!pZEPlayer->IsAdminFlagSet(ADMFLAG_CONVARS))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You don't have acces to this command.");
		return;
	}

	//ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIXM"pZEPlayer valid.");



	if (pZEPlayer->WasUsingMedkit())
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You already used your medkit in this round");
		return;
	}

	if (pEnt->m_iHealth() < 1)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You need to be alive in order to use medkit.");
		return;
	}


		if (pEnt->m_iHealth() > 99)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You have enough life.");
		return;
	}

	health = pEnt->m_iHealth() + 50;

	if (health > 100)
		health = 100;

	pEnt->m_iHealth = health;

	pZEPlayer->SetUsedMedkit(true);

	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Medkit used! Your health is now \4%d", health);
	g_pEngineServer2->ClientCommand(player->GetPlayerSlot(), "play items/healthshot_success_01");
}


// Lookup a weapon classname in the weapon map and "initialize" it.
// Both m_bInitialized and m_iItemDefinitionIndex need to be set for a weapon to be pickable and not crash clients,
// and m_iItemDefinitionIndex needs to be the correct ID from weapons.vdata so the gun behaves as it should.
