﻿#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include <time.h>
#include "BaseClient.h"


std::string animatedclantag;
int iLastTime;
bool bDone=false;
void misc::OnCreateMove(CInput::CUserCmd *cmd, IClientEntity *local)
{

    if (menu.Misc.Bhop && local->IsAlive())
    {
        if (cmd->buttons & IN_JUMP && !(local->GetMoveType() & MOVETYPE_LADDER))
        {

            int iFlags = local->GetFlags();
            if (!(iFlags & FL_ONGROUND))
                cmd->buttons &= ~IN_JUMP;
        }
    }
    if(menu.Misc.syncclantag)
    {
        if (int(g_Globals->curtime) != iLastTime)
        {
            switch (int(g_Globals->curtime) % 20)
            {
            case 0: setclantag(XorStr("thook         p")); break;
            case 1: setclantag(XorStr("hook         pe")); break;
            case 2: setclantag(XorStr("ook         per")); break;
            case 3: setclantag(XorStr("ok         perf")); break;
            case 4: setclantag(XorStr("k         perfe")); break;
            case 5: setclantag(XorStr("         perfec")); break;
            case 6: setclantag(XorStr("        perfect")); break;
            case 7: setclantag(XorStr("       perfecth")); break;
            case 8: setclantag(XorStr("      perfectho")); break;
            case 9: setclantag(XorStr("     perfecthoo")); break;
           case 10: setclantag(XorStr("    perfecthook")); break;
           case 11: setclantag(XorStr("   perfecthook ")); break;
           case 12: setclantag(XorStr("  perfecthook  ")); break;
           case 13: setclantag(XorStr(" perfecthook   ")); break;
           case 14: setclantag(XorStr("perfecthook    ")); break;
           case 15: setclantag(XorStr("erfecthook     ")); break;
           case 16: setclantag(XorStr("rfecthook      ")); break;
           case 17: setclantag(XorStr("fecthook       ")); break;
           case 18: setclantag(XorStr("ecthook        ")); break;
           case 19: setclantag(XorStr("cthook         ")); break;
            }
        }

        iLastTime = int(g_Globals->curtime);
    }
    if (!menu.Misc.animatedclantag && animatedclantag.c_str() != G::AnimatedClantag)
    {
        animatedclantag = G::AnimatedClantag;
    }
    if (menu.Misc.animatedclantag && animatedclantag.length() > 1)
    {
        if (int(g_Globals->curtime) != iLastTime)
        {
            auto length = animatedclantag.length();
            animatedclantag.insert(0, 1, animatedclantag[length - 2]);
            animatedclantag.erase(length - 1, 1);

            setclantag(animatedclantag.c_str());
            iLastTime = int(g_Globals->curtime);
        }
    }
    static size_t lastTime = 0;
    static int counter = 0;
    if (GetTickCount() > lastTime)
    {

        counter = counter + 1;
        if (counter >= 6)
            counter = 0;
        switch (menu.Misc.spammer)
        {
        case 1:
            SayInChat(tuxlist[counter]);
            break;
        case 2:
            SayInChat(ezfragslist[counter]);
            break;
        }

        lastTime = GetTickCount() + 850;


    }
    if (menu.Misc.AutoStrafe)
        AutoStrafe(cmd, local);

    if(menu.Misc.silentstealer)
    {
        bool bDidMeme = false;
        int iNameIndex = -1;
        char chName[130];
        static ConVar* name = g_CVar->FindVar("name");
        {
            for (int iPlayerIndex = 0; iPlayerIndex < g_Engine->GetMaxClients(); iPlayerIndex++)
            {
                IClientEntity *pEntity = g_EntityList->GetClientEntity(iPlayerIndex);
                if (!pEntity || pEntity == local || iPlayerIndex == g_Engine->GetLocalPlayer())
                    continue;
                if (rand() % 3 == 1)
                {
                    iNameIndex = pEntity->GetIndex();
                    bDidMeme = true;
                }
            }
            if (bDidMeme)
            {
                player_info_t pInfo;
                g_Engine->GetPlayerInfo(iNameIndex, &pInfo);
                sprintf(chName, "%s ", pInfo.name);
                name->SetValue(chName);
            }
        }

    }
}




void misc::AutoStrafe(CInput::CUserCmd *cmd, IClientEntity *local)
{


    static float move = 450; //4?.f; // move = max(move, (abs(cmd->move.x) + abs(cmd->move.y)) * 0.5f);
    float s_move = move * 0.5065f;
    if (local->GetMoveType() & (MOVETYPE_NOCLIP | MOVETYPE_LADDER))
        return;
    if (cmd->buttons & (IN_FORWARD | IN_MOVERIGHT | IN_MOVELEFT | IN_BACK))
        return;

    if (cmd->buttons & IN_JUMP || !(local->GetFlags() & FL_ONGROUND))
    {
        if (local->GetVelocity().Length2D() >= menu.Misc.MinVel)
        {
            cmd->forwardmove = move * 0.015f;
            cmd->sidemove += (float)(((cmd->tick_count % 2) * 2) - 1) * s_move;

            if (cmd->mousedx)
                cmd->sidemove = (float)clamp(cmd->mousedx, -1, 1) * s_move;

            static float strafe = cmd->viewangles.y;

            float rt = cmd->viewangles.y, rotation;
            rotation = strafe - rt;

            if (rotation < FloatNegate(g_Globals->interval_per_tick))
                cmd->sidemove = -s_move;

            if (rotation > g_Globals->interval_per_tick)
                cmd->sidemove = s_move;

            strafe = rt;
        }
    }
}