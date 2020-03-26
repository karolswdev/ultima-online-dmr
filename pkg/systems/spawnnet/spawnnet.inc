//-----------------------------------------------------------------------------

include "include/clock";
include "include/events";
include "spawnrune";
include "include/logerror";
include "include/items";
include "include/npcs";

//use basic;

//-----------------------------------------------------------------------------

const CLICK_SOUND      := 247;

//-----------------------------------------------------------------------------
// Props
//-----------------------------------------------------------------------------

const PROP_MAXAMOUNT         := 1;
const PROP_AMOUNT            := 2;
const PROP_RANGE             := 3;
const PROP_RESTIME           := 4;
const PROP_TEMPLATE          := 5;
const PROP_TYPE              := 6;
const PROP_COLOR             := 7;

//-----------------------------------------------------------------------------

function GetRuneSpawnTypes()

  return {
    NPC_SPAWN,
    NPCGROUP_SPAWN,
    ITEM_SPAWN
  };

endfunction

//-----------------------------------------------------------------------------

function RunSpawn(rune,who := 0)

  if(IsRunning(rune))
    if(who)
      SendSysMessage(who, "Spawn zostal zatrzymany.",FONT_NORMAL,COLOR_BAD);
    endif
    SetStopped(rune);
  else
    if(!CheckRuneType(rune))
      if(who)
        SendSysMessage(who, "Spawn '"+GetRuneType(rune)+"' nie zostal ustalony!",FONT_NORMAL,COLOR_BAD);
        return;
      endif
    endif
    if(who)
      SendSysMessage(who, "Spawn zostal wlaczony.",FONT_NORMAL,COLOR_GOOD);
    endif
    SetRunning(rune);
  endif

endfunction

//-----------------------------------------------------------------------------

function ResetTime(who,rune)

  SendSysMessage(who, "Zresetowano czas spawnu.",FONT_NORMAL,COLOR_GOOD);
  SetSpawnTime(rune,ReadGameClock());

endfunction

//-----------------------------------------------------------------------------

function CheckSpawn(rune,type,template,who := 0)

  UnloadConfigFile(NPC_GROUP_CFG);
  UnloadConfigFile(ITEM_GROUP_CFG);
  var NCfg    := ReadConfigFile(NPC_GROUP_CFG);
  var ICfg    := ReadConfigFile(ITEM_GROUP_CFG);
  var Chk     := 0,i := 1,Spawns;

  case(type)
    NPC_SPAWN:      if(!GetNpcConfig(template))
                      SendChMsg("CheckSpawn: Blad templata! "+template+" nie istnieje. Runo "+GetRuneNr(rune),who);
                      return 0;
                    endif
    NPCGROUP_SPAWN: if(!NCfg)
                      SendChMsg("CheckSpawn: Konfig npc nie istnieje! ["+NPC_GROUP_CFG+"]",who);
                      return 0;
                    elseif(!NCfg[CInt(template)])
                      SendChMsg("CheckSpawn: Grupa ["+template+"] nie istnieje! Runo "+GetRuneNr(rune), who);
                      return 0;
                    endif
                    Spawns := GetconfigStringArray(NCfg[CInt(template)], "spawn");
                    if(!Spawns or !Spawns.size())
                      SendChMsg("CheckSpawn: Grupa ["+template+"] nie posiada zdefiniowanych templatow! Runo "+GetRuneNr(rune), who);
                      return 0;
                    endif
                    foreach Spawn in Spawns
                      if(!GetNpcConfig(Spawn))
                        chk := 1;
                        if(who)
                          SendSysMessage(who, "Zly wpis: "+Spawn+", nr: "+i,FONT_NORMAL,COLOR_BAD);
                        endif
                      endif
                      i := i + 1;
                    endforeach
                    if(chk)
                      SendChMsg("CheckSpawn: Bledne wpisy konfigu npc, grupa "+template,who);
                      return 0;
                    endif
    ITEM_SPAWN:     if(!ICfg)
                      SendChMsg("CheckSpawn: Konfig przedmiotow nie istnieje! ["+ITEM_GROUP_CFG+"]",who);
                      return 0;
                    endif
                    Spawns := GetConfigStringArray(ICfg[CInt(template)], "spawn");
                    if(!Spawns)
                      if(who)
                        SendSysMessage(who, "Grupa ["+template+"] nie istnieje!",FONT_NORMAL,COLOR_BAD);
                        return 0;
                      endif
                    endif
                    var MainCfg := ReadConfigFile(ITEMS_CFG);
                    foreach Spawn in Spawns
                      if(!MainCfg[CInt(Spawn)])
                        Chk := 1;
                        if(who)
                          SendSysMessage(who, "Zly wpis: "+Hex(Spawn)+", nr: "+i,FONT_NORMAL,COLOR_BAD);
                        endif
                      endif
                      i := i + 1;
                    endforeach
                    if(Chk)
                      SendChMsg("CheckSpawn: Bledne wpisy w konfigu przedmiotow, grupa "+template,who);
                      return 0;
                    endif
    default:        return 0;
  endcase

  SetCheckedType(rune,type);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function SendChMsg(Msg,who := 0)

  if(who)
    SendSysMessage(who,Msg,FONT_NORMAL,COLOR_BAD);
  else
    LogError(Msg);
  endif
  
endfunction

//-----------------------------------------------------------------------------

function AnnihilateSpawns(rune)

  var i := 0,CreatedSpawn,CreatedObjects := CheckExistingObjects(Rune);
  case(GetRuneType(Rune))
    NPC_SPAWN:
    NPCGROUP_SPAWN:
                   if(CreatedObjects)
                     foreach Serial in CreatedObjects
                       CreatedSpawn := SystemFindObjectBySerial(Serial);
                       if(CreatedSpawn and !CreatedSpawn.dead)
                         KillNPC(CreatedSpawn);
                         i := i + 1;
                       endif
                     endforeach
                   endif
    ITEM_SPAWN:    foreach Serial in CreatedObjects
                     CreatedSpawn := SystemFindObjectBySerial(Serial);
                     DestroyItem(CreatedSpawn);
                     i := i + 1;
                   endforeach
  endcase

  CheckExistingObjects(rune);
  return i;
 
endfunction

//-----------------------------------------------------------------------------

function HideRunes(who)

  var num := 0;

  foreach Rune in ListObjectsInBox(who.x-10,who.y-10,-127,who.x+10,who.y+10,127,who.realm)
    if(Rune.objtype == UOBJ_SRUNE)
      Rune.invisible := 1;
      num := num + 1;
    endif
  endforeach

  SendSysMessage(who,num + " runow zostalo schowanych.",FONT_NORMAL,COLOR_BAD);

endfunction

//-----------------------------------------------------------------------------

function ShowRunes(who)

  var num := 0;

  foreach Rune in ListObjectsInBox(who.x-10,who.y-10,-127,who.x+10,who.y+10,127,who.realm)
    if(Rune.objtype == UOBJ_SRUNE)
      Rune.invisible := 0;
      Rune.graphic   := RUNE_CH_GRAPHIC;
      num := num + 1;
    endif
  endforeach

  SendSysMessage(who,num + " runow zostalo odkrytych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function HideRune(rune)
  rune.invisible := 1;
endfunction

//-----------------------------------------------------------------------------

function ShowRune(rune)
  rune.invisible := 0;
endfunction

//-----------------------------------------------------------------------------

function ShowGlobalRunes(who)

  var i,num := 0,MaxNr := GetMaxSRuneNr(),Rune;
  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune.invisible and Rune)
      ShowRune(rune);
      num := num + 1;
    endif
  endfor
  SendSysMessage(who,num + " runow zostalo odkrytych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function HideGlobalRunes(who)

  var i,num := 0,MaxNr := GetMaxSRuneNr(),Rune;
  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(!Rune.invisible and Rune)
      HideRune(rune);
      num := num + 1;
    endif
  endfor
  SendSysMessage(who,num + " runow zostalo ukrytych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function RunGlobalRunes(who)

  var i,num := 0,MaxNr := GetMaxSRuneNr(),Rune;
  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune)
      if(!IsRunning(rune))
        RunSpawn(rune);
        num := num + 1;
      endif
    endif
  endfor
  SendSysMessage(who,num + " runow zostalo wlaczonych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function StopGlobalRunes(who)

  var i,num := 0,MaxNr := GetMaxSRuneNr(),Rune;
  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune)
      if(IsRunning(rune))
        RunSpawn(rune);
        num := num + 1;
      endif
    endif
  endfor
  SendSysMessage(who,num + " runow zostalo wylaczonych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function ShowNearRunes(who,range := 50)

  if(!range)
    range := 100;
  endif

  var Runes := array;

  foreach Rune in ListObjectsInBox(who.x-range,who.y-range,-127,who.x+range,who.y+range,127,who.realm)
    if(Rune.objtype == UOBJ_SRUNE)
      Runes.append(Rune);
    endif
  endforeach

  SendSysMessage(who, Runes.size() + " runow istnieje w zasiegu "+range+".",FONT_NORMAL,COLOR_GOOD);

  ShowSpawnRunesGump(who, Runes, "Runy w zasiegu "+range);

endfunction

//-----------------------------------------------------------------------------

function ChangeGlobalData(who,SetProp)

  var Value := SendTextEntryGump( who, "Wprowadz nowa wartosc ",TE_CANCEL_ENABLE);
  Value := CInt(Value);
  if(!Value)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_BAD);
    return;
  endif
  var Rune,i,MaxNr := GetMaxSRuneNr(),num := 0,amount := 0,Chk;

  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune)
      case(SetProp)
        PROP_AMOUNT:      num := num + CInt(SetRuneAmount(rune,value));
        PROP_RESTIME:     num := num + CInt(SetRuneRespawnTime(rune,value));
        PROP_MAXAMOUNT:   Chk := SetRuneMaxAmount(rune,value);
                          if(Chk != error)
                            num    := num + 1;
                            amount := amount + Chk;
                          endif
        PROP_RANGE:       num := num + CInt(SetRuneRange(rune,value));
      endcase
    endif
  endfor
  SendSysMessage(who,"Zmieniono "+GetRunePropName(SetProp)+" "+num+" runom na "+cint(Value)+".",FONT_NORMAL,COLOR_GOOD);
  if(amount)
    SendSysMessage(who,"Zniszczono "+amount+" spawnow.",FONT_NORMAL,COLOR_GOOD);
  endif
  
endfunction

//-----------------------------------------------------------------------------

function LoadSpawnData(who,rune)

  var SpawnProps := GetObjProperty(who, "SpawnProps");

  if(!SpawnProps)
    SendSysMessage(who, "Ostatnie dane nie istanieja!",FONT_NORMAL,COLOR_BAD);
    return;
  endif

  SetRuneRespawnTime(rune,     SpawnProps[1]);
  SetRuneMaxAmount(rune,       SpawnProps[2]);
  SetRuneAmount(rune,          SpawnProps[3]);
  SetRuneRange(rune,           SpawnProps[4]);
  SetRuneType(rune,            SpawnProps[5]);
  SetRuneTemplate(rune,        SpawnProps[6]);
  SetFlags(rune,               SpawnProps[7]);

  if(SpawnProps[8])
    SetRunning(rune);
  endif
  if(SpawnProps[9])
    SetStopped(rune);
  endif

  if(CInt(SpawnProps[10]))
    SetSpawnTime(rune,           SpawnProps[10]);
  else
    SetSpawnTime(rune,           ReadGameClock());
  endif
  if(SpawnProps[11])
    SetCheckedType(rune,         SpawnProps[11]);
  endif
  
  SetRuneFacing(Rune,SpawnProps[12]);
  SetRuneOverColor(Rune,SpawnProps[13]);

  SendSysMessage(who,"Ostatnie dane wczytane.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function SaveLastRuneData(who,Rune)

  var SpawnProps := {};

  SpawnProps[1]  := GetRuneRespawnTime(Rune);
  SpawnProps[2]  := GetRuneMaxAmount(Rune);
  SpawnProps[3]  := GetRuneAmount(Rune);
  SpawnProps[4]  := GetRuneRange(Rune);
  SpawnProps[5]  := GetRuneType(Rune);
  SpawnProps[6]  := GetRuneTemplate(Rune);
  SpawnProps[7]  := GetFlags(Rune);
  SpawnProps[8]  := IsRunning(Rune);
  SpawnProps[9]  := IsStopped(Rune);
  SpawnProps[10] := GetSpawnTime(Rune);
  SpawnProps[11] := GetCheckedType(Rune);
  SpawnProps[12] := GetRuneFacing(Rune);
  SpawnProps[13] := GetRuneOverColor(Rune);

  SetObjProperty(who,"SpawnProps",SpawnProps);
  SendSysMessage(who,"Ostatnie dane zapisane.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function GetRunePropName(PropType)

  case(PropType)
    PROP_AMOUNT:     return "Ilosc";
    PROP_MAXAMOUNT:  return "Max Ilosc";
    PROP_TEMPLATE:   return "Template";
    PROP_TYPE:       return "Typ";
    PROP_RANGE:      return "Zasieg";
    PROP_RESTIME:    return "Czas respawnu";
  endcase
  return "";

endfunction

//-----------------------------------------------------------------------------

function GetRuneOPropName(PropType)

  case(PropType)
    RUNEF_FROZEN:   return "Zamrozony";
    RUNEF_BASE_Z:   return "Bazowe Z";
    RUNEF_NOCITY:   return "No City";
    RUNEF_ONLYCITY: return "Only City";
    RUNEF_BITEMS:   return "Del BItems";
    RUNEF_NOWANDER: return "No Wander";
    RUNEF_GUARDIGNORE: return "GuardIgnore";
  endcase
  return "";

endfunction

//-----------------------------------------------------------------------------

function ShowRunesWithProp(who, WhatProp)

  var value := SendTextEntryGump( who, "Wprowadz wartosc",10);

  if(!value)
    SendSysMessage(who,"Nie podal"+ggm(who,3)+" wartosci!",FONT_NORMAL,COLOR_BAD);
    return;
  endif

  var Rune, i;
  var MaxNr := GetMaxSRuneNr();
  var Runes := array;
  var CheckInt := CInt(Value);

  if(CheckInt)
    Value := CInt(Value);
  else
    Value := lower(Value);
  endif

  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune)
      if(CheckInt)
        if(WhatProp == PROP_TEMPLATE)
          if(CInt(GetRunePropValue(Rune,WhatProp)) == Value)
            Runes.append(Rune);
          endif
        else
          if(GetRunePropValue(Rune,WhatProp) <= Value)
            Runes.append(Rune);
          endif
        endif
      else
        if(lower(GetRunePropValue(Rune,WhatProp)) == Value)
          Runes.append(Rune);
        endif
      endif
    endif
  endfor

  SendSysMessage(who,"Istnieje "+Runes.size()+" Runow, z Wl. "+GetRunePropName(WhatProp)+" mniejsza/rowna "+Value+".",FONT_NORMAL,COLOR_GREEN);

  ShowSpawnRunesGump(who, Runes, "Runy z "+GetRunePropName(WhatProp)+": "+Value);

endfunction

//-----------------------------------------------------------------------------

function GetRunePropValue(Rune,WhatProp)

  case(WhatProp)
    PROP_AMOUNT:     return GetRuneAmount(Rune);
    PROP_MAXAMOUNT:  return GetRuneMaxAmount(Rune);
    PROP_RANGE:      return GetRuneRange(Rune);
    PROP_TEMPLATE:   return GetRuneTemplate(Rune);
    PROP_TYPE:       return GetRuneType(Rune);
    PROP_RESTIME:    return GetRuneRespawnTime(Rune);
  endcase
  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckSpawnAmount(rune,who := 0)

  var CreatedObjects := CheckExistingObjects(rune);
  var num := 0,CreatedSpawn,RInt,i;

  if(len(CreatedObjects) > GetRuneMaxAmount(Rune))
    num := len(CreatedObjects) - GetRuneMaxAmount(Rune);
    for(i := 1;i <= num;i := i + 1)
      RInt := RandomInt(len(CreatedObjects))+1;
      CreatedSpawn := systemfindobjectbyserial(CreatedObjects[RInt]);
      if(CreatedSpawn.isa(POLCLASS_MOBILE))
        KillNPC(CreatedSpawn);
      elseif(CreatedSpawn.isa(POLCLASS_ITEM))
        DestroyItem(CreatedSpawn);
      endif
      CreatedObjects.erase(RInt);
    endfor
  endif
  
  if(num)
    SetObjProperty(rune, PROP_CREATED_OBJECTS,CreatedObjects);
    if(who)
      SendSysMessage(who,"Niszczenie spawnow : "+num,FONT_NORMAL,COLOR_BAD);
    endif
  endif
  if(GetRuneAmount(Rune) > GetRuneMaxAmount(Rune))
    SetRuneAmount(Rune,GetRuneMaxAmount(Rune));
  endif

  return num;

endfunction

//-----------------------------------------------------------------------------

function AnnihilateWorldSpawns(who := 0)

  var num := 0,amount := 0,Rune,i,MaxNr := GetMaxSRuneNr();
  if(who)
    if(!YesNo(who,"Napewno?"))
      SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_BAD);
      return;
    endif
  endif

  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune)
      amount := amount + len(CheckExistingObjects(Rune));
      AnnihilateSpawns(rune);
      num := num + 1;
    endif
  endfor

  if(who)
    SendSysMessage(who,"Zniszczono "+amount+" spawnow w "+num+" runach.",FONT_NORMAL,COLOR_GOOD);
  endif
  return amount;

endfunction

//-----------------------------------------------------------------------------

function SpawnGlobalRunes(who)

  if(!YesNo(who,"Napewno?"))
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  var i,num := 0,MaxNr := GetMaxSRuneNr(),Rune;
  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune)
      ForceSpawn(Rune);
      num := num + 1;
    endif
  endfor
  SendSysMessage(who,"Wymuszony spawn w "+num+" runach.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function ForceSpawn(Rune,who := 0)

  if(IsRunning(Rune))
    SetSpawnTime(Rune,ReadGameClock() - (GetRuneRespawnTime(Rune) * 60));
    if(who)
      SendSysMessage(who,"Wymuszony spawn.",FONT_NORMAL,COLOR_GOOD);
    endif
    return 1;
  elseif(who)
    SendSysMessage(who,"Runo nie jest wlaczone!",FONT_NORMAL,COLOR_BAD);
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function ChangeNote(who,Rune)

  var NewNote := SendTextEntryGump(who,"Wpisz nowa notatke.",TE_CANCEL_ENABLE);
  if(!NewNote)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_BAD);
    return;
  endif
  SetObjProperty(Rune,"note",CStr(NewNote));

endfunction

//-----------------------------------------------------------------------------

function GetRuneNote(Rune)

  if(!GetObjProperty(Rune,"note"))
    SetObjProperty(Rune,"note","");
  endif
  return GetObjProperty(Rune,"note");

endfunction

//-----------------------------------------------------------------------------

function ChangeData(who,Rune,WhatProp)

  var Value := SendTextEntryGump( who, "Wprowadz nowa wartosc",TE_CANCEL_ENABLE);
  if(!Value)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_BAD);
    return 0;
  endif

  case(WhatProp)
    PROP_TEMPLATE:   if(!CheckSpawn(Rune,GetRuneType(Rune),Value,who))
                       return 0;
                     endif
                     SetRuneTemplate(Rune,Value);
    PROP_AMOUNT:     SetRuneAmount(Rune,Value);
    PROP_MAXAMOUNT:  SetRuneMaxAmount(Rune,Value);
    PROP_RANGE:      SetRuneRange(Rune,Value);
    PROP_RESTIME:    SetRuneRespawnTime(Rune,Value);
    PROP_COLOR:      SetRuneOverColor(Rune,Value);
  endcase

  SendSysMessage(who, "Zmienil"+ggm(who,3)+" "+GetRunePropName(WhatProp)+" na "+Value,FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function ShowGlobalChests(who)

  var i,num := 0,MaxNr := GetMaxSChestNr(),Chest;
  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);
    if(Chest.invisible and Chest)
      ShowRune(Chest);
      num := num + 1;
    endif
  endfor
  SendSysMessage(who,num + " skrzyn zostalo odkrytych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function HideGlobalChests(who)

  var i,num := 0,MaxNr := GetMaxSChestNr(),Chest;
  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);
    if(!Chest.invisible and Chest)
      HideRune(Chest);
      num := num + 1;
    endif
  endfor
  SendSysMessage(who,num + " skrzyn zostalo ukrytych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function RunGlobalChests(who)

  var i,num := 0,MaxNr := GetMaxSChestNr(),Chest;
  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);
    if(Chest)
      if(!IsRunning(Chest))
        RunSpawn(Chest);
        num := num + 1;
      endif
    endif
  endfor
  SendSysMessage(who,num + " skrzyn zostalo wlaczonych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function StopGlobalChests(who)
  var i,num := 0,MaxNr := GetMaxSChestNr(),Chest;
  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);
    if(Chest)
      if(IsRunning(Chest))
        RunSpawn(Chest);
        num := num + 1;
      endif
    endif
  endfor
  SendSysMessage(who,num + " skrzyn zostalo wylaczonych.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function ShowNearChests(who,range := 50)

  if(!range)
    range := 100;
  endif

  var Chests := array;

  foreach Chest in ListObjectsInBox(who.x-range,who.y-range,-127,who.x+range,who.y+range,127,who.realm)
    if(Chest.objtype == UOBJ_SCHEST)
      Chests.append(Chest);
    endif
  endforeach

  SendSysMessage(who, Chests.size() + " skrzyn istnieje w zasiegu "+range+".",FONT_NORMAL,COLOR_GOOD);

  ShowSpawnChestsGump(who, Chests, "Skrzynie w zasiegu "+range);

endfunction

//-----------------------------------------------------------------------------

function DestroyAllChestSpawns(who)

  var i,num := 0,MaxNr := GetMaxSChestNr(),Chest;
  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);
    if(Chest)
      DestroyChestSpawns(Chest);
      num := num + 1;
    endif
  endfor
  
  SendSysMessage(who,"Zniszczono spawny w "+num+" skrzyniach.",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function ChangeChestGlobalData(who,SetProp)

  var Value := SendTextEntryGump( who, "Wprowadz nowa wartosc ",TE_CANCEL_ENABLE);
  Value := CInt(Value);
  if(!Value)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_BAD);
    return;
  endif
  var Chest,i,MaxNr := GetMaxSChestNr(),num := 0;

  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);
    if(Chest)
      case(SetProp)
        PROP_RESTIME:     num := num + SetRuneRespawnTime(Chest,value);
      endcase
    endif
  endfor
  SendSysMessage(who,"Zmieniono "+GetRunePropName(SetProp)+" "+num+" runom na "+cint(Value)+".",FONT_NORMAL,COLOR_GOOD);

endfunction

//-----------------------------------------------------------------------------

function ShowChestsWithProp(who, WhatProp)

  var value := SendTextEntryGump( who, "Wprowadz wartosc", 10);
  if(!value)
    SendSysMessage(who,"Nie podales wartosci!",FONT_NORMAL,COLOR_BAD);
    return;
  endif

  var MaxNr  := GetMaxSChestNr();
  var Chests := array;
  var Chest, i;
  
  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);

    if(Chest)
      if(CInt(Value))
        if(GetRunePropValue(Chest,WhatProp) >= CInt(Value))
          Chests.append(Chest);
        endif
      elseif(Value)
        if(GetRunePropValue(Chest,WhatProp) == CStr(Value))
          Chests.append(Chest);
        endif
      endif
    endif
  endfor

  SendSysMessage(who,"Istnieje "+Chests.size()+" Runow, z Wl. "+GetRunePropName(WhatProp)+" wieksza/rowna "+Value+".",FONT_NORMAL,COLOR_GREEN);
  ShowSpawnChestsGump(who, Chests, "Skrzynie z "+GetRunePropName(WhatProp)+": "+Value);

endfunction

//-----------------------------------------------------------------------------

function GetRunesAmount()

  var Rune,i,num := 0,MaxNr := GetMaxSRuneNr();
  for(i:=1;i<=MaxNr;i:=i+1)
    Rune := FindRuneByNr(i);
    if(Rune)
      num := num + 1;
    endif
  endfor

  return num;

endfunction

//-----------------------------------------------------------------------------

function GetChestsAmount()

  var Chest,i,num := 0,MaxNr := GetMaxSChestNr();
  for(i:=1;i<=MaxNr;i:=i+1)
    Chest := FindChestByNr(i);
    if(Chest)
      num := num + 1;
    endif
  endfor

  return num;

endfunction

//-----------------------------------------------------------------------------

function ShowTemplates(who,RuneType)

  var MainCfg,res,AllKeys, tName;

  Var GumpPos := {};
  GumpPos.+Ex := 250;
  GumpPos.+Ey := 640;
  GumpPos.+PageCount := 29;

  while(IsLoggedIn(who))
    case(RuneType)
      NPC_SPAWN:      AllKeys := GetAllNpcTemplates();

      NPCGROUP_SPAWN: UnloadConfigFile(NPC_GROUP_CFG);
                      MainCfg := ReadConfigFile(NPC_GROUP_CFG);
      ITEM_SPAWN:     UnloadConfigFile(ITEM_GROUP_CFG);
                      MainCfg := ReadConfigFile(ITEM_GROUP_CFG);
      default:        return;
    endcase

    if(MainCfg)
      AllKeys := GetConfigStringKeys(MainCfg);
    endif

    if(!AllKeys)
      SendSysMessage(who,"Nie mozna bylo znalezc konfiga ["+RuneType+"].",FONT_NORMAL,COLOR_RED);
      return;
    endif


    GInit(30,100);
    GMultiHeaderInit();
    GResPic(0, 0,5120,350,460);
    GAddPageData(330, 15, 2435, 2436, 330, 440, 2437, 2438);

    case(RuneType)
      NPC_SPAWN:        GTextLine(20,12,1000,"Wszystkie npece: "+AllKeys.size());
      NPCGROUP_SPAWN:   GTextLine(20,12,1000,"Grupy npcow: "+AllKeys.size());
      ITEM_SPAWN:       GTextLine(20,12,1000,"Grupy itemow: "+AllKeys.size());
    endcase

    GMultiPage();

    var i;
  
    for(i:=1;i<=len(AllKeys);i:=i+1)
      GCheckNextPage(20);
      GButton(20, GNPCount()*20+43, 2117, 2118, i);
      GTextLine(40,  GNPCount()*20+40, 1419, i);
      GTextLine(80,  GNPCount()*20+40, 670, AllKeys[i]);
      if(RuneType != NPC_SPAWN)
        tName := MainCfg[AllKeys[i]].name;
        if(tName)
          GTextLine(120, GNPCount()*20+40, 560, tName);
          GButton(320, GNPCount()*20+40, 2117, 2118, 1000+i);
        endif
      endif
    endfor
    
    res := GSend(who);

    if(res[0] > 1000 and res[0] <= (1000+AllKeys.size()))
      ShowCfgGroup(MainCfg, AllKeys[CInt(res[0]-1000)]);
    else
      break;
    endif
    
  endwhile

  if(res[0] > 0 and res[0] <= AllKeys.size())
    return AllKeys[CInt(res[0])];
  else
    SendSysMessage(who, "Anulowano.",FONT_NORMAL,COLOR_BAD);
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function ShowCfgGroup(MainCfg, Group)

  GInit(10,10);
  GMultiHeaderInit();
  GResPic(100,10,5120,250,600);
  GAddPageData(330,20,2435,2436,330,580,2437,2438);
  GTextLine(140,38,1000,"Grupa: "+MainCfg[Group].name);
  GButton(120,20,2117,2118,1);
  GTextLine(140,20,999, "Wroc");
  GMultiPage();

  foreach Spawn in GetConfigStringArray(MainCfg[Group],"spawn")
    GCheckNextPage(29);
    GTextLine(120,(68+(GNPCount()*18)),1000,Spawn);
  endforeach

  GSend(who);

endfunction

//-----------------------------------------------------------------------------

function EraseFromSpawnnet(Npc)

  if(GetObjProperty(Npc,"Spawnnet"))
    var Rune := FindRuneByNr(GetObjProperty(Npc,"Spawnnet"));
    if(Rune)
      var Created := CheckExistingObjects(Rune);
      if(Npc.serial in Created)
        Created.erase(retindex(Created,Npc.serial));
        SetObjProperty(Rune, PROP_CREATED_OBJECTS, Created);
        SetObjProperty(Npc,"OldSpawnnet",GetObjProperty(Npc,"Spawnnet"));
        EraseObjProperty(Npc,"Spawnnet");
        DelaySpawn(Rune);
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckSpawnnet(corpse)

  if(GetObjProperty(corpse,"Spawnnet"))
    var Rune := FindRuneByNr(GetObjProperty(corpse,"Spawnnet"));
    DelaySpawn(Rune);
  endif

endfunction

//-----------------------------------------------------------------------------

function ShowSpawnRunesGump(who, byref Runes, Title)

  if(!Runes.size())
    return;
  endif

  var i := 1;

  GInit(30,100);
  GMode(GMODE_MULTIHD, GSMODE_NORMAL);
  GResPic(0, 0,5120,550,460);
  GTextLine(20, 10, 999, Title);
  GAddPageData(530, 15, 2435, 2436, 530, 440, 2437, 2438);
  GMultiPage();

  foreach Rune in Runes
    if(Rune)
      GCheckNextPage(20);
      GButton(20, GNPCount()*20+43, 2117, 2118, GetRuneNr(Rune));
      GTextLine(40,  GNPCount()*20+40, 1419, i);
      GTextLine(80,  GNPCount()*20+40, 670, GetRuneNr(Rune));
      GTextLine(140, GNPCount()*20+40, 560, GetRuneType(Rune));
      GTextLine(230, GNPCount()*20+40, 550, GetRuneTemplate(Rune));
      GTextLine(400, GNPCount()*20+40, 1000,Rune.x + "," + Rune.y + "," + Rune.z+" "+Rune.realm);
      i := i + 1;
    endif
  endforeach

  var Res, CurPage := 1, Rune;

  while(IsLoggedIn(who))
    SetCurMultiPage(CurPage);
    Res := GSend(who);
    CurPage := GetCurMultiPage();

    if(Res[0])
      Rune := FindRuneByNr(Res[0]);
      if(!Rune)
        break;
      endif
      MoveObject(who, Rune.x, Rune.y, Rune.z, Rune.realm, MOVECHAR_FORCELOCATION, MOVETYPE_CMD);
      SendSysMessage(who, "Przenoszenie do Runa ["+GetRuneNr(Rune)+"] ["+Rune.x+","+Rune.y+","+Rune.z+" "+Rune.realm+"]", FONT_NORMAL, COLOR_GREEN);
    else
      break;
    endif
  endwhile

endfunction

//-----------------------------------------------------------------------------

function ShowSpawnChestsGump(who, byref Chests, Title)

  if(!Chests.size())
    return;
  endif

  var i := 1;
  var ChestCfg := ReadConfigFile(SCHEST_CFG);

  GInit(30,100);
  GMode(GMODE_MULTIHD, GSMODE_NORMAL);
  GResPic(0, 0,5120,550,460);
  GTextLine(20, 10, 999, Title);
  GAddPageData(530, 15, 2435, 2436, 530, 440, 2437, 2438);
  GMultiPage();

  foreach Chest in Chests
    if(Chest)
      GCheckNextPage(20);
      GButton(20, GNPCount()*20+43, 2117, 2118, GetChestNr(Chest));
      GTextLine(40,  GNPCount()*20+40, 1419, i);
      GTextLine(80,  GNPCount()*20+40, 670, GetChestNr(Chest));
      GTextLine(160, GNPCount()*20+40, 550, GetRuneTemplate(Chest));
      GTextLine(200, GNPCount()*20+40, 550, ClStr(ChestCfg[GetRuneTemplate(Chest)].name));
      GTextLine(340, GNPCount()*20+40, 1000,Chest.x + "," + Chest.y + "," + Chest.z+" "+Chest.realm);
      i := i + 1;
    endif
  endforeach

  var Res, CurPage := 1, Chest;

  while(IsLoggedIn(who))

    SetCurMultiPage(CurPage);
    Res := GSend(who);
    CurPage := GetCurMultiPage();

    if(Res[0])
      Chest := FindChestByNr(Res[0]);
      if(!Chest)
        break;
      endif
      MoveObject(who, Chest.x, Chest.y, Chest.z, Chest.realm, MOVECHAR_FORCELOCATION, MOVETYPE_CMD);
      SendSysMessage(who, "Przenoszenie do Skrzyni ["+GetChestNr(Chest)+"] ["+Chest.x+","+Chest.y+","+Chest.z+" "+Chest.realm+"]", FONT_NORMAL, COLOR_GREEN);
    else
      break;
    endif

  endwhile

endfunction

//-----------------------------------------------------------------------------


function CanBeLeader(Template, npc)

  var chance := 25;
  var Elem := GetNpcConfig(Template);
  var str := CInt(Elem.STR);

  if(str >= 600 or str <= 100 or Elem.alignment == CITYAL_NEUTRAL)
    return 0;
  endif

  // zwroci -1 jak bedzie za malo npecow, 1 jak bedzie leader
  // tylko 0 okresla, ze moze stworzyc leadera.

  if(FindLeader(npc) != 0)
    return 0;
  endif

  if(RandomInt(100)+1 <= chance)
    return 1;
  endif

  
endfunction


//-----------------------------------------------------------------------------


function FindLeader(npc)
  
  var Range := 50;
  var x := npc.x;
  var y := npc.y;
  var realm := npc.realm;
  var Cnt := 0;
  var lFound := 0;

  foreach Obj in ListObjectsInBox(x-Range, y-Range, -127, x+Range, y+Range, 127, realm)
    if(Obj.isa(POLCLASS_NPC))
      if(Obj.npctemplate == npc.npctemplate)
        Cnt += 1;

        if(GetObjProperty(Obj, "IsLeader"))
          lFound := 1;
          break;
        endif

        if(Cnt >= 3)
          break;
        endif

      endif
    endif
  endforeach

  if(lFound)
    return 1;
  endif

  if(Cnt < 3)
    return -1;
  endif

  return 0;

endfunction


//-----------------------------------------------------------------------------


function MakeLeader(npc)
  
  var dmg_mod := 1.15;
  var armor := npc.ar;
  var base_armor_mod := npc.ar_mod;

  npc.ar_mod := armor + base_armor_mod;
  IncreaseLeaderSkills(npc);
  SetMaxHp(npc, GetMaxHp(npc)*2);
  SetHp(npc, GetMaxHp(npc));
  SetObjProperty(npc, "NpcDmgMod", dmg_mod);
  SetFlag(npc, NPCF_DOUBLE_LOOT);
  npc.title_guild := "Wodz";
  SetObjProperty(npc, "IsLeader",1);

endfunction


//-----------------------------------------------------------------------------

function IncreaseLeaderSkills(npc)

  IncreaseLeaderSkillsPoints(npc, 10);

endfunction

//-----------------------------------------------------------------------------

function  IncreaseLeaderSkillsPoints(npc, points)

  var SkillCfg := ReadConfigFile(ATTRIBUTES_CFG);
  var Attributes := GetConfigStringKeys(SkillCfg);

  foreach Attribute in Attributes

    var skill_id := SkillCfg[Attribute].SkillId;
    var skill_value := GetBaseSkill(npc, skill_id);
	
	if(skill_value > 0 && skill_value < 100)
		SetBaseSkill(npc, skill_id, 100);
	endif
	
	skill_value := GetBaseSkill(npc, skill_id);
    
    if(skill_value > 0)
      SetBaseSkill(npc, skill_id, skill_value+points);
    endif
    
  endforeach

endfunction

