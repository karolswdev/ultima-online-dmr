//-----------------------------------------------------------------------------

use uo;
use math;

//-----------------------------------------------------------------------------

include "include/const/props";
include "include/move";
include "include/checks/stdchecks";
include "include/objtypes/misc";
include "include/std";
include "include/player/target";
include "include/cmds/cancmd";
include "include/resurrect";
include "include/accounts";
include "include/cmds/misc";
include "include/objects/location";
include "include/mobile/age";

include ":wmtools:common";

//-----------------------------------------------------------------------------

const CDI_IGNORE_BI          := 0x00000001;
const CDI_IGNORE_HLD         := 0x00000002;
const CDI_IGNORE_HSC         := 0x00000004;
const CDI_IGNORE_GLD         := 0x00000008;
const CDI_IGNORE_TELE        := 0x00000010;
const CDI_IGNORE_GMC         := 0x00000040;
const CDI_IGNORE_WMS         := 0x00000080;

const CDI_IGNORE_ALL         := 0x0000FFFF;

//-----------------------------------------------------------------------------

function CanDestroyItem(Item, Flags := 0)

  if(Flags == CDI_IGNORE_ALL)
    return 1;
  endif

  var Err := error;

  if((Flags & CDI_IGNORE_ALL) != CDI_IGNORE_ALL)
    if(Item.objtype == UOBJ_SRUNE or Item.objtype == UOBJ_SCHEST)
      Err.errortext := "Nie mozesz niszczyc spawnrun lub spawnchestow.";
      return Err;
    elseif((Item.objtype == 0xA244 or Item.objtype == 0xA245 or Item.objtype == UOBJ_TELE) and !(Flags & CDI_IGNORE_TELE))
      Err.errortext := "Nie mozesz niszczyc teleporterow.";
      return Err;
    elseif(Item.objtype == UOBJ_WAYGATE)
      Err.errortext := "Nie mozesz niszczyc waygatow.";
      return Err;
    elseif((Item.objtype >= UOBJ_HRSIGN_START and Item.objtype <= UOBJ_HRSIGN_END))
      if(!(Flags & CDI_IGNORE_HSC))
        Err.errortext := "Nie mozesz niszczyc znakow wynajetych domow.";
        return Err;
      endif
    elseif(Item.objtype == 0xA900)
      if(!(Flags & CDI_IGNORE_HSC))
        Err.errortext := "Nie mozesz niszczyc pojemnikow wynajetych domow.";
        return Err;
      endif
    elseif(Item.objtype >= UOBJ_HSIGN_START and Item.objtype <= UOBJ_HSIGN_END) 
      if(!(Flags & CDI_IGNORE_HLD))
        Err.errortext := "Nie mozesz niszczyc znakow domow!";
        return Err;
      endif
    elseif(GetItemBINr(Item) and !(Flags & CDI_IGNORE_BI))
      Err.errortext := "Nie mozesz niszczyc BaseItemow.";
      return Err;
    elseif(GetItemWmsName(Item) and !(Flags & CDI_IGNORE_WMS))
      Err.errortext := "Nie mozesz niszczyc przedmiotow WMS.";
      return Err;
    elseif(!IsGmCreated(Item) and !IsQuestItem(Item) and !(Flags & CDI_IGNORE_GMC))
      Err.errortext := "Ten przedmiot nie zostal stworzony przez GMa.";
      return Err;
    endif
  endif

  if(IsLockedDown(Item))
    if(!(Flags & CDI_IGNORE_HLD))
      Err.errortext := "Nie mozesz niszczyc zablokowanych przedmiotow w domu.";
      return Err;
    endif
  elseif(IsSecured(Item))
    if(Flags & CDI_IGNORE_HSC)
      if(Item.item_count-1)
        Err.errortext := "Zabezpieczony pojemnik nie jest pusty!";
        return Err;
      endif
    else
      Err.errortext := "Nie mozesz niszczyc zabezpieczonych przedmiotow w domu.";
      return Err;
    endif
  elseif(Item.objtype == UOBJ_GUILD_STONE and !(Flags & CDI_IGNORE_GLD))
    Err.errortext := "Nie mozesz zniszczyc kamienia gildii.";
    return Err;
  elseif(Item.isa(POLCLASS_DOOR) and GetHouseSerial(Item))
    Err.errortext := "Nie mozesz niszczyc drzwi domow!";
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function OnDestroyItem(Item)

  if(IsLockedDown(Item))
    var House := GetHouse(Item);
    EraseItemLockDown(Item, House);
  elseif(IsSecured(Item))
    var House := GetHouse(Item);
    EraseSecured(Item, House);
  elseif(GetItemWmsName(Item))
    OnWmsItemDestroy(Item);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetDestroyItemFlags(who, byref Flags, byref text)

  Flags := 0;

  if(text["bi"])
    Flags := Flags | CDI_IGNORE_BI;
  endif
  
  if(text["wms"])
    Flags := Flags | CDI_IGNORE_WMS;
  endif
  
  if(text["hld"])
    Flags := Flags | CDI_IGNORE_HLD;
  endif
  if(text["hsc"])
    if(CanUseCmd(who, "destroy hsc"))
      Flags := Flags | CDI_IGNORE_HSC;
    endif
  endif
  if(text["gld"])
    Flags := Flags | CDI_IGNORE_GLD;
  endif
  
  if(text["gmc"])
    if(CanUseCmd(who, "destroy gmc"))
      Flags := Flags | CDI_IGNORE_GMC;
    endif
  endif

  if(IsAdmin(who))
    Flags := Flags | CDI_IGNORE_GMC;

    if(text["tele"])
      Flags := Flags | CDI_IGNORE_TELE;
    elseif(text["all"])
      Flags := Flags | CDI_IGNORE_ALL;
    endif
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Gain Watching
//-----------------------------------------------------------------------------
//=============================================================================

function SetUseSthWatcher(Watcher, Targ := 0)

  if(!Targ)
    Targ := ObjTarget(Watcher, TGTOPT_NOCHECK_LOS);
  endif

  if(!Targ or !Targ.isa(POLCLASS_MOBILE))
    SendSysMessage(Watcher,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(!CmdHaveAccess(Watcher, Targ, CMDA_READ))
    SendSysMessage(Watcher, "Nie masz dostepu do tej czynnosci.", FONT_NORMAL, COLOR_RED);
    return;
  endif

  EraseUseSthWatcher(Watcher);
  SetObjProperty(Targ, PROP_USESTH_WATCHER, Watcher.serial);
  SetObjProperty(Watcher, PROP_USESTH_WATCH, Targ.serial);
  SendSysmessage(Watcher, "Sledzenie Use: "+Targ.name+".",FONT_NORMAL,COLOR_BLUE);

endfunction

//-----------------------------------------------------------------------------

function EraseUseSthWatcher(Watcher)

  if(GetObjProperty(Watcher, PROP_USESTH_WATCH))
    var Targ := FindPlayerBySerial(GetObjProperty(Watcher, PROP_USESTH_WATCH));
    if(Targ)
      SendSysMessage(Watcher, "Koniec sledzenia Use: "+Targ.name+".", FONT_NORMAL, COLOR_RED);
      EraseObjProperty(Targ, PROP_USESTH_WATCHER);
      EraseObjProperty(Watcher, PROP_USESTH_WATCH);
      return 1;
    endif
  endif

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Misc
//-----------------------------------------------------------------------------
//=============================================================================

function ChangeCmdLevel(who,bywho)

  if(!IsAdmin(bywho))
    SendSysMessage(bywho,"Nie mozesz tego zrobic.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var NewValue := SendTextEntryGump( bywho, "Wprowadz nowa wartosc ",TE_CANCEL_ENABLE,TE_STYLE_NUMERICAL,6);
  if(!NewValue)
    SendSysMessage(bywho,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(NewValue == 5 and !IsDev(bywho))
    SendSysMessage(bywho,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  NewValue := Cint(NewValue);

  who.cmdlevel := NewValue;
  SendSysMessage(bywho,who.name +" jest teraz "+GetCmdLevelName(newvalue)+".",FONT_NORMAL,COLOR_GREEN);

  return NewValue;

endfunction

//-----------------------------------------------------------------------------

function KickPlayer(victim,bywho)

  if(IsPlayer(victim))
    SendSysMessage(bywho,"Wykopal"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_GREEN);
    InstantDisconnect(victim);
  else
    SendSysMessage(bywho,"Tego nie mozesz wykopac.",FONT_NORMAL,COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function FreezePlayer(victim,bywho)

  if(victim.frozen)
    SendSysMessage(bywho,"Odmrozil"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_BLUE);
    victim.frozen := 0;
    if(victim.isa(POLCLASS_NPC))
      UnSetFlag(victim, NPCF_FROZEN);
    endif
  else
    SendSysMessage(bywho,"Zamrozil"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_BLUE);
    victim.frozen := 1;
    if(victim.isa(POLCLASS_NPC))
      SetFlag(victim, NPCF_FROZEN);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function ParalyzePlayer(victim,bywho)

  if(victim.paralyzed)
    SendSysMessage(bywho,"Odparalizowal"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_BLUE);
    victim.paralyzed := 0;
  else
    SendSysMessage(bywho,"Sparalizowal"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_BLUE);
    victim.paralyzed := 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function CmdHidePlayer(victim,bywho)

  if(victim.hidden)
    SendSysMessage(bywho,"Odkryl"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_BLUE);
    UnhidePlayer(victim);
  else
    SendSysMessage(bywho,"Schowal"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_BLUE);
    HidePlayer(victim);
  endif

endfunction

//-----------------------------------------------------------------------------

function ToggleNpcConceal(Npc, bywho)

  if(!Npc.isa(POLCLASS_NPC))
    return;
  endif

  if(Npc.concealed)
    SendSysMessage(bywho,"Ujawnil"+ggm(bywho,3)+" NPCa "+Npc.name+".",FONT_NORMAL,COLOR_BLUE);
    Npc.concealed := 0;
  else
    SendSysMessage(bywho,"Ukryl"+ggm(bywho,3)+" NPCa "+Npc.name+".",FONT_NORMAL,COLOR_BLUE);
    Npc.concealed := 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function SummonPlayer(victim,bywho)

  SendSysMessage(bywho,"Przyciaganie osoby "+victim.name+".",FONT_NORMAL,COLOR_GREEN);
  MoveObject(victim,bywho.x,bywho.y,bywho.z,bywho.realm,MOVECHAR_FORCELOCATION);

endfunction

//-----------------------------------------------------------------------------

function KillPlayer(victim,bywho)

  if(IsStaff(victim))
    SendSysMessage(bywho,"Ekipy nie zabija sie pacanie!",FONT_NORMAL,COLOR_GREEN);
    return;
  endif

  SendSysMessage(bywho,"Zabil"+ggm(bywho,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_GREEN);

  LogMsg(KILL_LOG,bywho,victim);
  RevokePrivilege( victim, "invul" );
  if(victim.isa(POLCLASS_NPC))
    SetFlag(victim, NPCF_GUARD_KILL);
  endif
  DealDamage( victim, GetMaxHp(victim)+3, bywho );

endfunction

//-----------------------------------------------------------------------------

function LightKill(victim,bywho,LDmg := 0)

  if(IsStaff(victim))
    SendSysMessage(bywho,"Ekipy nie zabija sie pacanie!",FONT_NORMAL,COLOR_GREEN);
    return;
  endif

  if(!LDmg)
    LDmg := SendTextEntryGump(bywho,"Jaka ma byc sila pioruna ?",TE_CANCEL_ENABLE,TE_STYLE_NUMERICAL,8000);
    LDmg := Cint(LDmg);
    if(!LDmg)
      SendSysMessage(bywho,"Anulowano.",FONT_NORMAL,COLOR_RED);
      return;
    endif
  endif

  PlayLightningBoltEffect( victim );
  PlaySoundEffect( victim, SFX_SPELL_LIGHTNING );

  LogMsg(KILL_LOG,bywho,victim);
  RevokePrivilege( victim, "invul" );
  if(victim.isa(POLCLASS_NPC))
    SetFlag(victim, NPCF_GUARD_KILL);
  endif
  DealDamage( victim, LDmg , bywho );

endfunction

//-----------------------------------------------------------------------------

function QuiteKillNPC(victim,bywho)

  if(!victim.isa(POLCLASS_NPC))
    SendSysMessage(bywho,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(!KillNPC(victim))
    SendSysMessage(bywho,"Nie udalo sie zabic "+victim.name+".",FONT_NORMAL,COLOR_RED);
    return;
  endif

endfunction

//-----------------------------------------------------------------------------

function RessPlayer(who,bywho)

  if(!IsPlayer(who))
    SendSysMessage(bywho,"Tego nie mozesz wskrzesic.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(!who.dead)
    SendSysMessage(bywho,"Ta osoba wciaz zyje.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  ResurrectPlayer(who);
  SendSysMessage(bywho,"Wskrzesil"+ggm(bywho,3)+" osobe "+who.name+".",FONT_NORMAL,COLOR_BLUE);

endfunction

//-----------------------------------------------------------------------------

function MoveToWrittenLocation(Obj,bywho)

  if(!CanModifyItem(bywho, Obj))
    SendSysMessage(bywho,"Nie mozesz przenosic tego obiektu.",FONT_NORMAL,COLOR_GREEN);
    return;
  endif

  SendSysMessage(bywho,"Wpisz lokacje, X Y Z.",FONT_NORMAL,COLOR_GREEN);
  var Loc := RequestInput(bywho,bywho.backpack,"");
  if(!Loc)
    SendSysMessage(bywho,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  Loc := SplitWords(Loc);
  var x := Cint(Loc[1]);
  var y := Cint(Loc[2]);
  var z := Cint(Loc[3]);

  var sx := Obj.x;
  var sy := Obj.y;
  var sz := Obj.z;

  var Res := CanCmdMoveTo(bywho, Obj, BuildLocStruct(x, y, z, bywho.realm));
  if(!Res)
    SendSysMessage(bywho, "Blad: "+Res.errortext);
    return;
  endif

  Res := 0;

  LogMsg(MOVE_LOG,bywho,{Obj,x,y,z,bywho.realm});

  if(Obj.isa(POLCLASS_MOBILE))
    Res := MoveObject(Obj,x,y,z,bywho.realm,MOVECHAR_FORCELOCATION);
    if(Res)
      SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" osobe "+Obj.name+" z miejsca sx: "+sx+" sy: "+sy+" sz: "+sz+", do ex: "+Obj.x+" ey: "+Obj.y+" ez: "+Obj.z,FONT_NORMAL,COLOR_GREEN);
    endif
  elseif(Obj.isa(POLCLASS_BOAT))
    Res := MoveBoatXY(Obj,x,y);
    if(Res)
      SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" lodz "+Obj.name+" z miejsca sx: "+sx+" sy: "+sy+" sz: "+sz+", do ex: "+Obj.x+" ey: "+Obj.y+" ez: "+Obj.z,FONT_NORMAL,COLOR_GREEN);
    endif
  elseif(Obj.isa(POLCLASS_MULTI))
    Res := MoveObject(Obj,x,y,z,bywho.realm,MOVECHAR_FORCELOCATION);
    if(Res)
      SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" multi "+Obj.name+" z miejsca sx: "+sx+" sy: "+sy+" sz: "+sz+", do ex: "+Obj.x+" ey: "+Obj.y+" ez: "+Obj.z,FONT_NORMAL,COLOR_GREEN);
    endif
  else
    Res := MoveObject(Obj,x,y,z,bywho.realm,MOVEITEM_FORCELOCATION | MOVEITEM_IGNOREMOVABLE);
    if(Res)
      SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" item "+Obj.name+" z miejsca sx: "+sx+" sy: "+sy+" sz: "+sz+", do ex: "+Obj.x+" ey: "+Obj.y+" ez: "+Obj.z,FONT_NORMAL,COLOR_GREEN);
    endif
  endif

  if(!Res)
    SendSysMessage(bywho,"Blad podczas przenoszenia: "+Res.errortext, FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function MoveToSelectedLocation(Obj, bywho)

  if(!CanModifyItem(bywho, Obj))
    SendSysMessage(bywho,"Nie mozesz przenosic tego obiektu.",FONT_NORMAL,COLOR_GREEN);
    return;
  endif

  SendSysMessage(bywho,"Wybierz miejsce.",FONT_NORMAL,COLOR_GREEN);
  var Loc := LocTarget(bywho);

  if(!Loc)
    SendSysMessage(bywho,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var x := Cint(Loc.x);
  var y := Cint(Loc.y);
  var z := Cint(Loc.z);

  var sx   := Obj.x;
  var sy   := Obj.y;
  var sz   := Obj.z;
  var sRlm := Obj.realm;

  var Res := CanCmdMoveTo(bywho, Obj, BuildLocStruct(x, y, z, bywho.realm));
  if(!Res)
    SendSysMessage(bywho, "Blad: "+Res.errortext);
    return;
  endif

  Res := 0;

  LogMsg(MOVE_LOG,bywho,{Obj,x,y,z,bywho.realm});

  if(Obj.isa(POLCLASS_MOBILE))

    Res := MoveObject(Obj,x,y,z,bywho.realm,MOVECHAR_FORCELOCATION);
    if(Res)
      SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" osobe "+Obj.name+" z ["+sx+","+sy+","+sz+" "+sRlm+"] do ["+Obj.x+","+Obj.y+","+Obj.z+" "+Obj.realm+"]",FONT_NORMAL,COLOR_GREEN);
    endif

  elseif(Obj.isa(POLCLASS_BOAT))

    Res := MoveBoatXY(Obj,x,y);

    if(Res)
      SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" lodz "+Obj.desc+" z ["+sx+","+sy+","+sz+" "+sRlm+"] do ["+Obj.x+","+Obj.y+","+Obj.z+" "+Obj.realm+"]",FONT_NORMAL,COLOR_GREEN);
    endif

  elseif(Obj.isa(POLCLASS_MULTI))

    Res := MoveObject(Obj,x,y,z,bywho.realm, MOVEITEM_FORCELOCATION);

    if(Res)
      SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" multi "+Obj.desc+" z ["+sx+","+sy+","+sz+" "+sRlm+"] do ["+Obj.x+","+Obj.y+","+Obj.z+" "+Obj.realm+"]",FONT_NORMAL,COLOR_GREEN);
    endif

  else

    if(Loc.item.isa(POLCLASS_CONTAINER))
      var movable := Obj.movable;
      Obj.movable := 1;

      Res := MoveItemToContainer(Obj,Loc.item);
      if(Res)
        SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" przedmiot "+Obj.desc+" z ["+sx+","+sy+","+sz+" "+sRlm+"], do kontenera "+Loc.item.desc+" ["+Loc.item.serial+"]",FONT_NORMAL,COLOR_GREEN);
      else
        Obj.movable := movable;
      endif
    else
      Res := MoveObject(Obj,x,y,z,bywho.realm, MOVEITEM_FORCELOCATION | MOVEITEM_IGNOREMOVABLE);

      if(Res)
        SendSysMessage(bywho,"Przeniosl"+ggm(bywho,3)+" przedmiot "+Obj.desc+" z ["+sx+","+sy+","+sz+" "+sRlm+"] do ["+Obj.x+","+Obj.y+","+Obj.z+" "+Obj.realm+"]",FONT_NORMAL,COLOR_GREEN);
      endif
    endif
  endif

  if(!Res)
    SendSysMessage(bywho,"Blad podczas przenoszenia: "+Res.errortext, FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function MoveToHimLocation(towho,who)

  var Res := CanCmdMoveTo(towho, who, towho);
  if(!Res)
    SendSysMessage(towho, "Blad: "+Res.errortext);
    return;
  endif

  SendSysMessage(who,"Przenoszenie do osoby "+towho.name+" x: "+towho.x+" y: "+towho.y+" z: "+towho.z+".",FONT_NORMAL,COLOR_GREEN);
  LogMsg(GOTO_LOG,who,{towho,towho.x,towho.y,towho.z,towho.realm});
//  LogNotice("["+GetRealName(who)+"/"+who.acctname+"] przeniosl sie do postaci "+GetRealName(towho)+"/"+towho.acctname+"] do lokacji "+towho.x+","+towho.y+","+towho.z+","+towho.realm, LOGF_FORCE_ALERT | LOGF_ALERT_ADMINS | LOGF_SCRIPTS);
  MoveObject(who,towho.x,towho.y,towho.z,towho.realm,MOVECHAR_FORCELOCATION);

endfunction

//-----------------------------------------------------------------------------

function MoveCharToItemInContainer(who,item)

  var source := item;
  while(source.container)
    source := source.container;
  endwhile

  if(source.x and source.y and source.z != error)
    SendSysMessage(who,"Przenoszenie do "+source.name+" x: "+source.x+" y: "+source.y+" "+source.z+".",FONT_NORMAL,COLOR_GREEN);
    LogMsg(GOTO_LOG,who,{source,source.x,source.y,source.z,source.realm});
    MoveObject(who,source.x,source.y,source.z,source.realm,MOVECHAR_FORCELOCATION);
  else
    SendSysMessage(who,"Nie znaleziono celu.",FONT_NORMAL,COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function DuplicateItemToContainer(item, Cont, who)

  if(!Item.movable)
    SendSysMessage(who, "Ten przedmiot nie jest ruchomy.", FONT_NORMAL, COLOR_RED);
    return;
  endif

  var DupItem := CreateItemCopyAtLoc(CR_TEMP_ITEM_X, CR_TEMP_ITEM_Y, CR_TEMP_ITEM_Z, Item, CR_TEMP_ITEM_REALM);
  DupItem.movable := 1;

  if(!MoveItemToContainer(DupItem, Cont))
    DestroyItem(DupItem);
  elseif(Item.isa(POLCLASS_CONTAINER))
    DuplicateContainerToCont(Item, DupItem, who);
  endif

endfunction

//-----------------------------------------------------------------------------

function DuplicateContainerToCont(Cont, inCont, who)

  var DupItem;

  foreach Item in ListRootItemsInContainer(Cont)
    DupItem := CreateItemCopyAtLoc(CR_TEMP_ITEM_X, CR_TEMP_ITEM_Y, CR_TEMP_ITEM_Z, Item, CR_TEMP_ITEM_REALM);
    DupItem.movable := 1;

    if(!MoveItemToContainer(DupItem, inCont))
      DestroyItem(DupItem);
    elseif(Item.isa(POLCLASS_CONTAINER))
      DuplicateContainerToCont(Item, DupItem, who);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function DeleteChar(acc,nr)

  if(!acc or !nr)
    return 0;
  endif

  nr := Cint(nr);

  var char := acc.GetCharacter(nr);

  if(char)
    if(IsLoggedIn(char))
      InstantDisconnect(char);
      Sleepms(400);
    endif

    var i;
    for(i:=0; i<=10; i+=1)
      var Res := acc.DeleteCharacter(nr);
      if(Res)
        return 1;
      else
        LogError("Delete char "+GetName(acc.GetCharacter(nr))+" error: "+Res);
      endif
      Sleepms(100);
    endfor
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetPrivs()

  return {
    "attackhidden",
    "clotheany",
    "dblclickany",
    "firewhilemoving",
    "freemove",
    "hearghosts",
    "hiddenattack",
    "ignoredoors",
    "invul",
    "losany",
    "moveany",
    "moveanydist",
    "plogany",
    "renameany",
    "seeghosts",
    "seehidden",
    "seeinvisitems",
    "showadv"
  };

endfunction

//-----------------------------------------------------------------------------

function SwitchPriv(who, bywho, PrivId)

  var Privs;

  if(CmdHaveAccess(bywho, who, CMDA_ADMIN))
    var PrivNames := GetPrivs();
    Privs := dictionary;
    foreach Name in PrivNames
      Privs[Name] := whom.enabled(Name);
    endforeach
  elseif(bywho == whom)
    Privs := whom.Privileges();
  else
    exit;
  endif

  var PrivName := Privs.keys()[PrivId];

  if(CmdHaveAccess(bywho, who, CMDA_ADMIN))
    if(who.enabled(PrivName))
      RevokePrivilege(who,PrivName);
      who.disable(PrivName);
      SendSysMessage(bywho,"Wylaczyl"+ggm(bywho,3)+" przywilej "+PrivName+" osobie "+who.name+".",FONT_NORMAL,COLOR_RED);
    else
      GrantPrivilege(who,PrivName);
      who.enable(PrivName);
      SendSysMessage(bywho,"Wlaczyl"+ggm(bywho,3)+" przywilej "+PrivName+" osobie "+who.name+".",FONT_NORMAL,COLOR_BLUE);
    endif
  else
    if(who.enabled(PrivName))
      who.disable(PrivName);
      SendSysMessage(bywho,"Wylaczyl"+ggm(bywho,3)+" sobie przywilej "+PrivName+".",FONT_NORMAL,COLOR_RED);
    else
      who.enable(PrivName);
      SendSysMessage(bywho,"Wlaczyl"+ggm(bywho,3)+" sobie przywilej "+PrivName+".",FONT_NORMAL,COLOR_BLUE);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function IsSearchChar(who, Char, byref text, byref Vals)

  var Name := lower(GetRealName(Char));

  if(text[2] == "nm")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3];
      Vals.sname := "Imie";
    endif

    if(!Vals.name)
      Vals.+name;
      Vals.name  := lower(text[3]);
    endif
    if(Name[Vals.name])
      return 1;
    else
      var WolfName := GetObjProperty(Char, "WolfName");

      if(WolfName)
        WolfName := lower(WolfName);
        if(WolfName[Vals.name])
          return 1;
        endif
      endif
    endif

  elseif(text[2] == "st")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3]+" "+text[4]+" "+CInt(text[5]);
      Vals.sname := "Statystyka";
    endif

    if(Char.cmdlevel)
      return 0;
    endif

    case(text[3])
      "str":    return ProcessSearchType(text[4], GetBaseStrength(Char), CInt(text[5]));
      "dex":    return ProcessSearchType(text[4], GetBaseDexterity(Char), CInt(text[5]));
      "int":    return ProcessSearchType(text[4], GetBaseIntelligence(Char), CInt(text[5]));

      default:  var Err := error;
                Err.errortext := "Bledna nazwa statystyki.";
                return Err;
    endcase

  elseif(text[2] == "sk")

    if(!Vals.skid and Vals.skid != 0)
      Vals.+skid;
      Vals.skid := FindSkIdByShortName(text[3]);
    endif

    if(!Vals.sname and Vals.skid != error)
      Vals.+sname;
      Vals.+value;
      Vals.value := GetSkillConfigElem(Vals.skid).Name+" "+text[4]+" "+CInt(text[5]);
      Vals.sname := "Umiejetnosc";
    endif

    if(Vals.skid != error)
      if(Char.cmdlevel)
        return 0;
      endif
      return ProcessSearchType(text[4], GetBaseSkill(Char, Vals.skid), CInt(text[5]));
    else
      var Err := error;
      Err.errortext := "Bledna nazwa umiejetnosci.";
      return Err;
    endif

  elseif(text[2] == "age")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3]+" "+CInt(text[4]);
      Vals.sname := "Wiek";
    endif

    if(Char.cmdlevel)
      return 0;
    endif

    return ProcessSearchType(text[3], GetRealAge(Char), CInt(text[4]));

  elseif(text[2] == "ontimer")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3]+" "+CStr(CInt(text[4]))+text[5];
      Vals.sname := "Online Timer";
    endif

    if(Char.cmdlevel)
      return 0;
    endif

    return ProcessSearchTimeType(text[5], text[3], CInt(GetObjProperty(Char, PROP_ONLINETIMER)), CInt(text[4]));

  elseif(text[2] == "lastlog")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3]+" "+CStr(CInt(text[4]))+text[5];
      Vals.sname := "Last Login";
    endif

    return ProcessSearchTimeType(text[5], text[3], ReadGameClock() - CInt(GetObjProperty(Char, "LastLog")), CInt(text[4]));

  elseif(text[2] == "extime")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3]+" "+CStr(CInt(text[4]))+text[5];
      Vals.sname := "Czas istnienia";
    endif

    return ProcessSearchTimeType(text[5], text[3], ReadGameClock() - Char.createdat, CInt(text[4]));

  elseif(text[2] == "murd")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3]+" "+CStr(CInt(text[4]));
      Vals.sname := "Ilosc morderstw";
    endif

    var Murders := GetObjProperty(Char,"Murders");

    return ProcessSearchType(text[3], CInt(Murders.size()), CInt(text[4]));

  elseif(text[2] == "murdtime")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3]+" "+CStr(CInt(text[4]))+text[5];
      Vals.sname := "Czas mordercy";
    endif

    return ProcessSearchTimeType(text[5], text[3], CInt(CInt(GetObjProperty(Char, "decaylongcountat")) - GetObjProperty(Char, PROP_ONLINETIMER)), CInt(text[4]));

  elseif(text[2] == "mwgid" and Vals.canmwgid)

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := CStr(CInt(text[3]));
      Vals.sname := "MwGID";
    endif

    Text[3] := CInt(Text[3]);
    Vals.AddInfo := 0;

    var AccIds := Char.acct.GetProp("Logs");

    if(!AccIds)
      AccIds := dictionary;
      var LastIps := ReadFile(LASTLOG_PKG + Char.acct.name);

      foreach LIp in LastIps
        LIp := UnPack(LIp);
        LIp[5] := CInt(LIp[5]);

        if(LIp[5] > 0)
          AccIds[LIp[5]] := CInt(AccIds[LIp[5]]) + 1;
        endif
      endforeach

      Char.acct.SetProp("Logs", AccIds);
    endif

    Vals.AddInfo := CInt(AccIds[text[3]]);

    if(Vals.AddInfo)
      return 2;
    endif

  elseif(text[2] == "pdid" and Vals.canmwgid)

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[3];
      Vals.sname := "ProdID";
    endif

    var LastIps := ReadFile(LASTLOG_PKG + Char.acct.name);
    var Stealth := Char.acct.GetProp("StlId");

    Vals.AddInfo := 0;

    foreach LIp in LastIps
      LIp := UnPack(LIp);
      if(LIp[6])
        if(Stealth and who.cmdlevel < 4)
          if(Stealth[2] == Text[3])
            Vals.AddInfo := Vals.AddInfo + 1;
          endif
        elseif(LIp[6] == Text[3])
          Vals.AddInfo := Vals.AddInfo + 1;
        endif
      endif
    endforeach

    if(Vals.AddInfo)
      return 2;
    endif

  elseif(text[2] == "maxskillabuse")

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := 0;
      Vals.sname := "MaxSkillAbuse";
    endif

    var maxSkId := GetMaxSkillId();
    var SkArr := array;
    var SkID;

    if(!Char.cmdlevel)
      for(SkID:=0; SkID<=maxSkId; SkID+=1)
        if(GetBaseSkillBaseValue(Char, SkID) > GetSkillBaseMaxValue(Char, SkID))
          SkArr.append(SkID);
        endif
      endfor
    endif

    if(SkArr.size())
      if(!Vals.AddInfo)
        Vals.+AddInfo;
      endif
      Vals.AddInfo := "";

      foreach sk in SkArr
        Vals.AddInfo := Vals.AddInfo+Cstr(sk)+" ";
      endforeach
      return 1;
    endif

  else

    if(!Vals.sname)
      Vals.+sname;
      Vals.+value;
      Vals.value := text[2];
      Vals.sname := "Imie";
    endif

    if(!Vals.name)
      Vals.+name;
      Vals.name := lower(text[2]);
    endif

    if(Name[Vals.name])
      return 1;
    else
      var WolfName := GetObjProperty(Char, "WolfName");

      if(WolfName)
        WolfName := lower(WolfName);
        if(WolfName[Vals.name])
          return 1;
        endif
      endif
    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ProcessSearchTimeType(tType, Type, BaseTime, CompTime)

  case(tType)
    "s":     return ProcessSearchType(Type, BaseTime, CompTime);
    "m":     return ProcessSearchType(Type, BaseTime / 60, CompTime);
    "h":     return ProcessSearchType(Type, BaseTime / 3600, CompTime);
    "d":     return ProcessSearchType(Type, BaseTime / DAY_LENGTH, CompTime);
    default: var Err := error;
             Err.errortext := "Bledny przedzial czasowy: [s/m/h/d].";
             return Err;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ProcessSearchType(Type, BaseVal, CompVal)

  case(Type)
    "=":      if(BaseVal == CompVal)
                return 1;
              endif
    ">":      if(BaseVal > CompVal)
                return 1;
              endif
    "<":      if(BaseVal < CompVal)
                return 1;
              endif
    default:  var Err := error;
              Err.errortext := "Blednie okreslone dzialanie [= > <].";
              return Err;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ShowCharsSelection(who, byref AllChars, Title)

  if(AllChars.size() == 1)
    return FindPlayerBySerial(AllChars[1][3]);
  endif

  GInit();
  GMultiHeaderInit();
  GResPic(0,  20, 2520,600,500);
  GResPic(28, 54, 3600,544,430);
  GTextLine(60,30, 999, Title);
  GAddPageData(565,55,2650,2651,565,463,2648,2647);
  GMultiPage();
  var res;
  var i;

  for(i:=1;i<=AllChars.size();i:=i+1)
    GCheckNextPage(20);
    GButton(50, GNPCount()*20+74, 2117,2118, i);
    GTextLine(75,  GNPCount()*20+70, 550, AllChars[i][1]);
    if(AllChars[i][4])
      GTextLine(240, GNPCount()*20+70, 1419, AllChars[i][4]);
    endif
    GTextLine(280, GNPCount()*20+70, 610, AllChars[i][2]);
    
    var Player := FindPlayerBySerial(AllChars[i][3]);
    GTextLine(380, GNPCount()*20+70, 560, DescRealTime(ReadGameClock() - CInt(GetObjProperty(Player, PROP_LASTLOG))) );
  endfor

  res := GSend(who)[0];

  return FindPlayerBySerial(AllChars[Res][3]);

endfunction

//-----------------------------------------------------------------------------

function SearchAllChars(who, split, byref Vals, byref AllChars)

  var AllAcc   := ListAccounts();
  var sRes,i,Char;

  if(split[2] == "mwgid" or split[2] == "pdid")
    if(CanUseCmd(who, "jail sr mwgid"))
      Vals.+canmwgid := 1;
      Vals.+AddInfo  := 0;
    endif
  endif

  foreach Acc in AllAcc
    Acc := FindAccount(Acc);

    for(i:=1;i<=5;i:=i+1)

      Char := Acc.GetCharacter(i);

      if(Char)
        sRes := IsSearchChar(who, Char, split, Vals);
        if(sRes)
          AllChars.append(array(GetRealName(Char), Acc.name, Char.serial, Vals.AddInfo));
          if(sRes == 2)
            break;
          endif
        elseif(sRes == error)
          SendSysMessage(who, "Blad: "+sRes.errortext, FONT_NORMAL, COLOR_RED);
          return 0;
        endif
      endif

    endfor

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetLastMwGidForAcc(AccName)

  var LastIPs := ReadFile(LASTLOG_PKG + AccName);

  LastIps.reverse();

  foreach IP in LastIPs
    IP := UnPack(IP);
    return CInt(IP[5]);
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetCmdTarget(who, Text)

  var Targ := 0;

  if(CInt(Text))
    Targ := FindPlayerBySerial(Text);
  elseif(Text)
    Targ := FindCharacterByName(Text);
  endif

  if(!Targ)
    Targ := Target(who);
  endif

  return Targ;

endfunction

//-----------------------------------------------------------------------------

function GetCmdLoc(who, bywho)

  if(who.concealed > bywho.cmdlevel)
    var Loc := GetObjProperty(who, "ConcLoc");
    if(Loc)
      return Loc;
    endif
  endif

  var Loc := GetLocStruct(who);

  return Loc;

endfunction

//-----------------------------------------------------------------------------

function GetAttrRawArray(who, AttrId)

  var RawPoints := GetRawAttributeValue(who, AttrId);

  if(CInt(RawPoints) < 0)
    RawPoints := 0;
  endif

  var BaseVal := GetAttributeBaseValue(who, AttrId);
  var BaseRaw := BaseToRawSkill(BaseVal);
  var NextRaw := BaseToRawSkill(BaseVal + 1);

  var GainPoints := RawPoints - BaseRaw;
  var LeftPoints := NextRaw - BaseRaw - GainPoints;

  var RawAttr := struct;

  RawAttr.+baseVal    := BaseVal;
  RawAttr.+gainPoints := GainPoints;
  RawAttr.+leftPoints := LeftPoints;
  RawAttr.+basePoints := BaseRaw;
  RawAttr.+nextPoints := NextRaw;
  RawAttr.+curPoints  := RawPoints;

  return RawAttr;

endfunction
                         
//-----------------------------------------------------------------------------

function CopyLookAll(FromMob, ToMob)

  SetTitlePrefix(ToMob, GetTitlePrefix(FromMob));
  SetTitleSuffix(ToMob, GetTitleSuffix(FromMob));
  SetTitleGuild(ToMob, GetTitleGuild(FromMob));
  ToMob.title_race   := FromMob.title_race;

  var i, maxSkId := GetMaxSkillId();

  for(i:=0; i<=maxSkId; i+=1)
    SetBaseSkillBaseValue(ToMob, i, GetBaseSkillBaseValue(FromMob, i));
  endfor

  SetBaseStrength(ToMob, GetBaseStrength(FromMob));
  SetBaseDexterity(ToMob, GetBaseDexterity(FromMob));
  SetBaseIntelligence(ToMob, GetBaseIntelligence(FromMob));

  SetMaxHp(ToMob, GetMaxHp(FromMob));
  SetMaxMana(ToMob, GetMaxMana(FromMob));
  SetMaxStamina(ToMob, GetMaxStamina(FromMob));

  SetHp(ToMob, GetHp(FromMob));
  SetMana(ToMob, GetMana(FromMob));
  SetStamina(ToMob, GetStamina(FromMob));

  ToMob.run_speed := FromMob.run_speed;

  var PrivNames := GetPrivs();

  foreach PrivName in PrivNames
    if(FromMob.enabled(PrivName))
//      GrantPrivilege(ToMob, PrivName);
      ToMob.enable(PrivName);
    elseif(ToMob.enabled(PrivName))
      ToMob.disable(PrivName); // zamiast Revoke zrobic lepiej disable, wtedy bedzie mogl sobie sam pozniej wlaczyc, o ile go ma.
      //RevokePrivilege(ToMob, PrivName);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function CanCmdMoveTo(who, what, where)

  if(IsAdmin(who))
    return 1;
  endif

  if(FindEventAreaByLoc(what))
    return RetError("Nie mozesz przeniesc "+what.name+" z lokacji AutoEventu.");
  elseif(FindEventAreaByLoc(where))
    return RetError("Nie mozesz przeniesc "+what.name+" do lokacji AutoEventu!!! Musi sam wejsc przez portal!");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
