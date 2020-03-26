//-----------------------------------------------------------------------------

include ":quests:quest_ctrl";
include "include/findcity";
include ":npcguilds:common";
include "ai/include/guild_npcs";
include "../pkg/items/lostitems/lostitems";
include "include/buildings";
include "ai/include/citynpcs";

include ":craftquests:craftquests";
include ":character:class_speech";

//-----------------------------------------------------------------------------

const NSPF_CANT_SPEAK        := 0x00000001;

//-----------------------------------------------------------------------------

var SpCfg, Speech, All, SpFlags := 0;

//-----------------------------------------------------------------------------

function CanIHearPlayer(who)

  if(who.dead or me.concealed)
    return 0;
  endif

  return (CheckLineOfSight(me, who) and DistZ(me,who) < 20 and Distance(me, who) < 10);

endfunction

//-----------------------------------------------------------------------------

function CanNpcSpeakWith(who)

  if(!CanTrade(who))
    SetBitOn(SpFlags, NSPF_CANT_SPEAK);
    return error;
  endif

  var Data := struct;
  Data.+npc := me;
  Data.+source := who;

  if(run_script(":ngd_npcs:is_member", Data) == 0)
    SetBitOn(SpFlags, NSPF_CANT_SPEAK);
    return 0;
  endif

  SetBitOff(SpFlags, NSPF_CANT_SPEAK);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CheckNpcSpeech(who, byref text)

  if(!CanIHearPlayer(who))
    return error;
  endif

  SetBitOff(SpFlags, NSPF_CANT_SPEAK);

  if(CheckThisNpcSpeech(who, text))
    if(IsWalking())
      SetWalkPause(SHORT_WALK_PAUSE);
    endif
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function NpcShowCraftReceptures(who)

  var Data := struct;
  Data.+npc := me;
  Data.+source := who;

  if(run_script(":craftquests:show_receptures", Data))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function NpcIdentifyCraftScroll(who)

  var Data := struct;
  Data.+npc := me;
  Data.+source := who;

  if(start_script(":crafting:ident_secret", Data))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckCraftQuests(who, me)

  var Data := struct;
  Data.+npc := me;
  Data.+source := who;

  if(run_script(":craftquests:show_quests", Data))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckThisNpcSpeech(who, byref text)

  var ltext := lower(text);

  CheckNpcGuildSpeech(who, text);
  if(CheckCharClassSpeech(who, text))
    return 1;
  endif

  if(ltext["witaj"] or ltext["witam"])

    var Data := struct;
    Data.+npc := me;
    Data.+source := who;

    if(CheckSpokenName(ltext))
      if(run_script(":mq_npcs:hello", Data) > 0)
        return 1;
      endif
    endif

    if(run_script(":quests:npc_speech", Data) > 0)
      return 1;
    endif

  elseif(ltext["pomoc"] or ltext["pomog"])

    if(CheckSpokenName(ltext))
      var Data := struct;
      Data.+npc := me;
      Data.+source := who;
      start_script(":mq_npcs:give_quest", Data);
    endif
    
  elseif(ltext["receptur"])

    if(MeHaveFlag("CanGiveCraftQuest"))
      if(CheckSpokenName(ltext))
        TurnToward(who);
        if(len(ltext) < 20)
          Say("Hm.. co z recepturami?");
        elseif(NpcShowCraftReceptures(who))
          return 1;
        endif
      endif
    endif
    
  elseif(ltext["identyfik"] and ltext["tajnik"])

    if(MeHaveFlag("CanGiveCraftQuest"))
      if(CheckSpokenName(ltext))
        TurnToward(who);
        if(len(ltext) < 30)
          Say("Hm.. co tam marmolisz?");
        elseif(NpcIdentifyCraftScroll(who))
          return 1;
        endif
      endif
    endif

  elseif(ltext["kiedy"] and ltext["zamowien"] and ltext["specjaln"])
    
    if(CheckSpokenName(ltext))
      TurnToward(who);

      var Data := struct;
      Data.+npc := me;
      Data.+source := who;

      if(run_script(":bulks:process_info", Data))
        return 1;
      endif
    endif

  elseif(ltext["zamowien"])

    if(MeHaveFlag("CanGiveCraftQuest"))
      if(CheckSpokenName(ltext))
        TurnToward(who);
        if(len(ltext) < 20)
          Say("Hm.. zamowienie?");
        elseif(CheckCraftQuests(who, me))
          return 1;
        endif
      endif
    endif

  elseif(ltext["przedmiot"])

    if(CheckSpokenName(ltext))
      var Data := struct;
      Data.+npc := me;
      Data.+source := who;

      TurnToward(who);
      if(len(ltext) < 20)
        Say("Hm.. przedmiot?");
      elseif(run_script(":lostitems:check_lostitem", Data))
        return 1;
      endif
    endif

  elseif(ltext["przypomnij"])

    if(CheckSpokenName(ltext))
      var Data := struct;
      Data.+npc := me;
      Data.+source := who;
      start_script(":mq_npcs:remind", Data);
    endif

  elseif(ltext["przesun"])
    if(CheckSpokenName(ltext))
      MoveSelf();
      return 1;
    endif

  endif

  if(CheckMySpeech(who, text))
    if(!(SpFlags & NSPF_CANT_SPEAK))
      CheckOtherSpeech(who, text);
    endif
    return 1;
  endif

  if(!(SpFlags & NSPF_CANT_SPEAK))
    CheckOtherSpeech(who, text);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckOtherSpeech(who, Text)

  if(!SpCfg)
    SpCfg  := ReadConfigFile(NPCSPEECH_CFG);
    if(SpCfg[me.npctemplate])
      Speech := ListConfigElemProps(SpCfg[me.npctemplate]);
    endif
    if(SpCfg["all"])
      All    := ListConfigElemProps(SpCfg["all"]);
    endif
  endif

  if(Speech)
    foreach Sp in Speech
      if(text[Sp])
        TurnToward(who);
        Text := GetConfigStringArray(SpCfg[me.npctemplate],Sp);
        Text := Text[RandomInt(Text.size() + 1)];
        if(Text)
          Say(Text);
        endif
        Text := 0;
        return 1;
      endif
    endforeach
    
    foreach Sp in All
      if(text[Sp])
        TurnToward(who);
        Text := GetConfigStringArray(SpCfg[me.npctemplate],Sp);
        Text := Text[RandomInt(Text.size() + 1)];
        if(Text)
          Say(Text);
        endif
        Text := 0;
        return 1;
      endif
    endforeach
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function MoveSelf()

  if(IsSetFlag(me,NPCF_NO_WANDER))
    if(RandomInt(100) < 50)
      Say("Zostaw mnie w spokoju.");
    elseif(RandomInt(100) < 30)
      Say("Nie mam zamiaru sie przesuwac.");
    endif
    return;
  endif

  if(MoveIfBlocking(ev.source))
    case(RandomInt(3))
      1:       Say("Juz sie ruszam..");
      2:       Say("Ah, zamyslil"+ggm(me,8)+" sie.");
      default: Say("Oj, juz sie usuwam z przejscia.");
    endcase
  else
    TurnToward(ev.source);
    case(RandomInt(3))
      1:       Say("Czemu chcesz bym sie przesunal?");
      2:       Say("Zostaw mnie w spokoju.");
    endcase
  endif

endfunction

//-----------------------------------------------------------------------------

function NpcYesNo(who)

  TurnToward(who);
  
  var Yes  := error;
  var Time := ReadGameClock() + 30;
  var nEv;

  while(Time > ReadGameClock() and Yes == error)
    nEv := wait_for_event(10);
    case(nEv.type)
      EVID_SPEECH:     if(CheckLineOfSight(me, nEv.source))
                         TurnToward(nEv.source);
                         if(nEv.source == who)
                           if(lower(nEv.text)["tak"])
                             Yes := 1;
                           elseif(lower(nEv.text)["nie"])
                             Yes := 0;
                           endif
                         else
                           Say(GetName(nEv.source)+", nie tobie zadal"+ggm(me, 8)+" pytanie...");
                         endif
                       endif

      EVID_ITEM_GIVEN: MoveToPack(ev.item, ev.source);

      EVID_QUEST_MSG:      var Data := struct;
                           Data.+npc := me;
                           Data.+param := ev;
                           
                           start_script(":ngd_npcs:quest_msg", Data);
    endcase
  endwhile

  TurnToward(who);
  return Yes;

endfunction

//-----------------------------------------------------------------------------

function CheckSpokenName(byref text)

  if(text[lower(me.name[1,find(me.name," ",1)-1])])
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
