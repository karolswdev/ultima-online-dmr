//-----------------------------------------------------------------------------

include "include/packets";
include "include/mount";
include "include/reputation/criminal";

include "common";
include ":warcasts:warcasts";

//-----------------------------------------------------------------------------

const WEARWOLF_SOUND_MORPH             := 0xE6;
const WEARWOLF_SOUND_SCREAM            := 0xE7;
const WW_RUNES_CFG                     := ":wearwolf:runes";

const WW_START_HOUR                    := 21;
const WW_END_HOUR                      := 9;
const WW_DURATION                      := 70;

const WW_CONS_STAM                     := 1;
const WW_CONS_MANA                     := 2;

const WW_TYPE_BASE                     := 1;
const WW_TYPE_MAGE                     := 2;
const WW_TYPE_WARRIOR                  := 3;

//-----------------------------------------------------------------------------

const WWF_WOLF_MORPHED      := 0x01;
const WWF_WEARWOLF_MORPHED  := 0x02;

//-----------------------------------------------------------------------------

function CanBeWearWolf(who)
  return CInt(GetObjProperty(who,"CanBeWearWolf"));
endfunction

//-----------------------------------------------------------------------------

function InitiateMorph(who)

  if(who.dead or !who)
    return;
  endif

  if(CInt(GetObjProperty(who,"#WWInitiateComplete")) < ReadGameClock())
    EraseObjProperty(who,"#EndedMorph");
    SetObjProperty(who,"#WearWolfStartSkill",GetEffectiveSkill(who,SKILLID_WEARWOLF));
    UnDoMod(who, MOD_STR);
    UnDoMod(who, MOD_DEX);

    ModifyMod(who, MOD_STR, 0 - (12 - Cint(GetWStSk(who) / 10)), 600);
    ModifyMod(who, MOD_DEX, 0 - (12 - Cint(GetWStSk(who) / 10)), 600);

    SetObjProperty(who,"#WWInitiateComplete",ReadGameClock()+3600);
  endif

endfunction

//-----------------------------------------------------------------------------

function StartMorph(who)

  if(who.dead or !who)
    return;
  endif

  if(!IsWearWolfMorphed(who) or (IsHuman(who) and CanMod(who, MOD_MOUNT)))
    if(GetBaseSkill(who, SKILLID_WEARWOLF) >= 80 and !GetObjProperty(who, "WolfName"))
      start_script(":wearwolf:choose_name", who);
    endif

    SetObjProperty(who,"#WearWolfStartSkill",GetEffectiveSkill(who,SKILLID_WEARWOLF));
    EraseObjProperty(who,"#WWInitiateComplete");
    SetCriminal(who, 1, "StartMorph", "Przemiana w wilkolaka.");
    PlaySoundEffect(who, WEARWOLF_SOUND_MORPH);

    var i;
    for(i:=1;i<=5;i:=i+1)
      PlayPcEffect(who,FX_HEAL_EFFECT,1424,0x1f,0x1f,3);
      sleep(1);
    endfor

    PlayPcEffect(who,FX_MARK_EFFECT,1424,0x1,0xf,3);
    sleepms(700);

    UnEquipToMorph(who);
    Dismount(who);

    UndoAllMods(who, UMDF_WAIT);

    DoMod(who, MOD_INCO,         GetWearwolfName(who),                          CInt(WW_DURATION * 60));
    DoMod(who, MOD_GRAPHIC,      CID_WEARWOLF,                                  CInt(WW_DURATION * 60));
    DoMod(who, MOD_STR,          2 + Cint((GetWstSk(who) / 4) + RandomInt(3)),  CInt(WW_DURATION * 60));
    DoMod(who, MOD_DEX,          2 + CInt((GetWStSk(who) / 5) + RandomInt(10)),  CInt(WW_DURATION * 60));
    DoMod(who, MOD_COLOR,        GetMorphColor(who),                            CInt(WW_DURATION * 60));
    DoMod(who, MOD_STATUS,       3,                                             CInt(WW_DURATION * 60));
    DoMod(who, MOD_AR,           CInt(GetWStSk(who) / 3),                       CInt(WW_DURATION * 60));
    DoMod(who, MOD_TPREFIX,      "",                                            CInt(WW_DURATION * 60));
    DoMod(who, MOD_TSUFFIX,      "",                                            CInt(WW_DURATION * 60));
    DoMod(who, MOD_TGUILD,       "",                                            CInt(WW_DURATION * 60));
    DoMod(who, MOD_TRACE,        "",                                            CInt(WW_DURATION * 60));
    DoMod(who, MOD_LIGHT,        5 - (GetWStSk(who)/25),                        CInt(WW_DURATION * 60));
    DoMod(who, MOD_RUN_SPEED,    RUNSPEED_FAST,                                 CInt(WW_DURATION * 60));

    SetObjProperty(who,"#WearWolfMorph",ReadGameClock());
  else
    if(CanMod(who, MOD_STR))
      DoMod(who, MOD_STR,          2 + Cint((GetWstSk(who) / 4) + RandomInt(3)),  CInt(WW_DURATION * 60));
    endif
    
    if(CanMod(who, MOD_DEX))
      DoMod(who, MOD_DEX,          2 + CInt((GetWStSk(who) / 5) + RandomInt(3)),  CInt(WW_DURATION * 60));
    endif

    if(CanMod(who, MOD_AR))
      DoMod(who, MOD_AR,           CInt(GetWStSk(who) / 4),                       CInt(WW_DURATION * 60));
    endif
    
    if(CanMod(who, MOD_LIGHT))
      DoMod(who, MOD_LIGHT,        5 - (GetWStSk(who)/25),                        CInt(WW_DURATION * 60));
    endif
    
    if(CanMod(who, MOD_RUN_SPEED))
      DoMod(who, MOD_RUN_SPEED,    RUNSPEED_FAST,                                 CInt(WW_DURATION * 60));
    endif

  endif

endfunction

//-----------------------------------------------------------------------------

function FinishMorph(who)

  if(IsWearWolfMorphed(Who))
    UndoAllMods(who, UMDF_WAIT);
    EraseObjProperty(who,"#WearWolfMorph");

    if(!who.murderer)
      SetCriminal(who, 0);
    endif

    if(CInt(GetObjProperty(who,"#EndedMorph")) < ReadGameClock())

      ModifyMod(who, MOD_STR, 0 - (12 - Cint(GetWStSk(who) / 10)), 600);
      ModifyMod(who, MOD_DEX, 0 - (12 - Cint(GetWStSk(who) / 10)), 600);

      EraseObjProperty(who,"#WearWolfStartSkill");
      SetObjProperty(who,"#EndedMorph",ReadGameClock()+3600);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function EndMorph(who)

  if(IsWearWolfMorphed(Who))
    UndoAllMods(who, UMDF_WAIT);
    EraseObjProperty(who,"#WearWolfMorph");
    EraseObjProperty(who,"#WearWolfStartSkill");
    EraseObjProperty(who,"#EndedMorph");
    if(!who.murderer)
      SetCriminal(who, 0);
    endif

  else
    if(GetObjProperty(who,"#WWInitiateComplete"))
      EraseObjProperty(who,"#WWInitiateComplete");
    endif

    if(GetObjProperty(who,"#EndedMorph"))
      UndoAllMods(who, UMDF_WAIT);
      EraseObjProperty(who,"#EndedMorph");
    endif

  endif

endfunction

//-----------------------------------------------------------------------------

function GetWolfMorphTemplate(who)

  var template;

  if(IsWolfMorphed(who))
    template := "summonedwearwolf";

    case(who.graphic)
      CID_WOLF:        template := "wolf";
      CID_GRIZZLY:     template := "grizzly"
    endcase
  endif

  return template;

endfunction

//-----------------------------------------------------------------------------

function CheckWearWolfAcceptation(who)

  if(!ExPid(GetObjProperty(who,"#StartedMorph")))
    if(GetHour() == 20)
      start_script(":wearwolf:firstmorph",who);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function UnEquipToMorph(who)

  UnEquipMagicItems(who, array(MAGICMOD_STRENGTH, MAGICMOD_DEXTERITY, MAGICMOD_INTELLIGENCE, MAGICMOD_LIGHT));

  var itemcfg := ReadConfigFile(ITEMS_CFG);
  var i,item;

  for(i:=1;i<=24;i:=i+1)
    if(!(i in {LAYER_PACK,LAYER_HAIR,LAYER_BEARD}))
      item := GetEquipmentByLayer(who,i);
      if((itemcfg[item.objtype].blockcircle and itemcfg[item.objtype].blockcircle <= 5) or i == LAYER_HAND1 or i == LAYER_HAND2)
        MoveToPack(Item, who);
      endif
    endif
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function WearWolfRuneCast(byref Parms, wcId, rType, wwFlags := 0, TrgType := TRGT_NONE, TrgNoto := TRGO_NONE)

  var wcFlags := 0;

  if(!Parms.Caster.isa(POLCLASS_NPC) and !Parms.Caster.cmdlevel)
    if(wwFlags & WWF_WOLF_MORPHED)
      if(!IsWolfMorphed(Parms.Caster))
        SendSysMessage(Parms.Caster, "Musisz byc w formie wilka.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif
    endif

    if(wwFlags & WWF_WEARWOLF_MORPHED)
      if(!IsWearWolfMorphed(Parms.Caster))
        SendSysMessage(Parms.Caster, "Nie jestes teraz wilkolakiem.",FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif
  endif

  if(GetObjProperty(Parms.Caster, "WearwolfStopped") and rType != WW_TYPE_BASE)
    SendSysMessage(Parms.Caster, "Przemian zostala wstrzymana. Nie jestes teraz wilkolakiem.",FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  case(rType)
    WW_TYPE_BASE:     wcFlags := WcFlags | WCF_CONS_STAMINA;

    WW_TYPE_MAGE:     if(!Parms.Caster.isa(POLCLASS_NPC))
                        if(!IsCharClass(Parms.Caster, CLASS_MAGE) and !Parms.Caster.cmdlevel)
                          SendSysMessage(Parms.Caster, "Te runo jest niezwykle tajemnicze dla Ciebie...", FONT_NORMAL, COLOR_RED);
                          return 0;
                        endif
                      endif

                      wcFlags := wcFlags | WCF_CONS_MANA;

    WW_TYPE_WARRIOR:  if(!Parms.Caster.isa(POLCLASS_NPC))
                        if(IsCharClass(Parms.Caster, CLASS_MAGE) and !Parms.Caster.cmdlevel)
                          SendSysMessage(Parms.Caster, "Te runo jest niezwykle tajemnicze dla Ciebie...", FONT_NORMAL, COLOR_RED);
                          return 0;
                        endif
                      endif

                      wcFlags := wcFlags | WCF_CONS_STAMINA;
  endcase

  return WarCast(Parms, wcId, wcFlags, TrgType, TrgNoto);

endfunction

//-----------------------------------------------------------------------

function ChangeIntoWearwolf(who)

  UndoMod(who, MOD_GRAPHIC, UMDF_WAIT);
  UndoMod(who, MOD_INCO,    UMDF_WAIT);
  UndoMod(who, MOD_COLOR,   UMDF_WAIT);
  DoMod(who, MOD_INCO,     GetWearwolfName(who),  CInt(WW_DURATION * 60));
  DoMod(who, MOD_COLOR,    GetMorphColor(who),    CInt(WW_DURATION * 60));
  DoMod(who, MOD_GRAPHIC,  CID_WEARWOLF,          CInt(WW_DURATION * 60));

endfunction

//-----------------------------------------------------------------------------

function SetWearWolf(who)

  if(IsWearWolf(who))
    return 0;
  endif

  SetObjProperty(who, "IsWearWolf", ReadGameClock());

  return 1;

endfunction

//-----------------------------------------------------------------------------

function EraseWearWolf(who)

  var Skill := GetBaseSkillBaseValue(who, SKILLID_WEARWOLF);

  if(!IsWearWolf(who) and !Skill)
    return 0;
  endif

  EraseObjProperty(who, "IsWearWolf");
  UndoAllMods(who, UMDF_WAIT);
  EraseObjProperty(who, "#WearWolfMorph");
  EraseObjProperty(who, "#WearWolfStartSkill");
  EraseObjProperty(who, "#EndedMorph");
  EraseObjProperty(who, "#WWInitiateComplete");
  EraseObjProperty(who, "CanBeWearWolf");
  EraseObjProperty(who, "WolfName");
  SetBaseSkill(who, SKILLID_WEARWOLF, 0);
  if(!who.murderer)
    SetCriminal(who, 0);
  endif

  return 1;

endfunction