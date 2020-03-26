//-----------------------------------------------------------------------------

use uo;
use os;
use cfgfile;

//-----------------------------------------------------------------------------

include "include/attributes/main";
include "include/const/props";
include "include/client/fonts";
include "include/checks/stdchecks";
include "include/coords";
include "include/finding";
include "include/items";
include "include/items/basic";

//-----------------------------------------------------------------------------

const NOCHECK_DEAD           := 0x00000001;
const NO_MOVABLE_CHECK       := 0x00000002;
const NO_BUSY_CHECK          := 0x00000004;
const NO_DIST_CHECK          := 0x00000008;
const NO_OWNER_CHECK         := 0x00000010;
const NO_ITEMCONT_CHECK      := 0x00000020;
const NO_USE_CHECK           := 0x00000040;
const NO_WORK_BREAK          := 0x00000080;
const NO_LOCSPEC_CHECK       := 0x00000200;
const NO_CHECK_SPECLOS       := 0x00000400;
const CAN_DAMAGE_HIDDEN      := 0x00000800;
const NO_BOUND_CHECK         := 0x00001000;
const NO_ESCROW_CHECK        := 0x00002000;

const NEW_DIST_CHECK         := 0x00000100;



//=============================================================================
//-----------------------------------------------------------------------------
// Standard Functions Item and Mobile Check
//-----------------------------------------------------------------------------
//=============================================================================

function StandardItemCheck(who,item, Flags := 0, Param := 0)

  if(!item or who.dead or who.paralyzed or who.frozen)
    SendSysMessage(who, "Anulowano.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!item.isa(POLCLASS_ITEM))
    SendSysMessage(who,"To nie przedmiot.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!(Flags & NO_ESCROW_CHECK) and Item.objtype == UOBJ_ESCROWPACK)
    SendSysMessage(who, "Nie mozesz uzywac tego przedmiotu.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!CanUseItem(who, Item))
    return 0;
  endif

  if(!(Flags & NO_OWNER_CHECK))
    if(!CanAccess(who, item))
      return 0;
    endif
  endif

  if(!(Flags & NO_BOUND_CHECK))
    if(!CheckBound(who, item))
      SendSysMessage(who, "Nie mozesz tego uzyc.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endif

  if(!item.movable and !(Flags & NO_MOVABLE_CHECK))
    SendSysMessage(who, "Nie mozesz tego uzyc.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(CInt(GetObjProperty(item,"#BeingSnoop")) == who.serial)
    SendSysMessage(who, "Nie mozesz tego uzyc.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!Accessible(who, item))
    SendSysMessage(who, "Nie dosiegniesz tego!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!ReserveItem(item))
    SendSysMessage(who,"Nie mozesz tego teraz uzyc.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!CheckLineOfSight(who,item) and !item.container)
    SendSysMessage(who,"Nie widzisz tego przedmiotu.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!item.container)
    if((Flags & NEW_DIST_CHECK) and Param)
      if(Distance(who,Item) > Param)
        SendSysMessage(who,"Stoisz za daleko.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif
    endif
  else
    if(!CanAccess(who,item.container))
      return 0;
    endif
  endif

  if(!(Flags & NO_BUSY_CHECK))
    if(IsBusy(who,NO_BUSY_DELAY_CHECK | Flags))
      return 0;
    endif
  endif

  CheckUseSthWatch(who,item);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function StandardMobCheck(who, mob, Flags := 0, Param := 0)

  if(!mob or who.dead or who.paralyzed or who.frozen)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!mob.isa(POLCLASS_MOBILE))
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(Flags & NO_CHECK_SPECLOS)
    if(!CheckLineOfSight(who,mob) or mob.hidden or mob.concealed)
      SendSysMessage(who,"Nie widzisz tego.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  else
    if(!CheckSpecialLos(who,mob) or mob.hidden or mob.concealed)
      SendSysMessage(who,"Nie widzisz tego.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endif

  if((Flags & NEW_DIST_CHECK) and Param)
    if(Distance(who,mob) > Param)
      SendSysMessage(who,"Stoisz za daleko.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  elseif(Distance(who,mob) > 4)
    if(!(Flags & NO_DIST_CHECK))
      SendSysMessage(who,"Stoisz za daleko.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endif

  if(mob.dead)
    if(!(Flags & NOCHECK_DEAD))
      SendSysMessage(who,"To stworzenie nie zyje.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endif

  if(!(Flags & NO_BUSY_CHECK))
    if(IsBusy(who,NO_BUSY_DELAY_CHECK | Flags))
      return 0;
    endif

    if(!(Flags & NO_USE_CHECK))
      if(CInt(GetObjProperty(mob,"#UserPid")))
        if(GetProcess(CInt(GetObjProperty(mob,"#UserPid"))) and CInt(GetObjProperty(mob,"#UserPid")) != GetPid())
          SendSysMessage(who,"Nie mozesz tego teraz uzyc.",FONT_NORMAL,COLOR_RED);
          return 0;
        endif
      endif
      SetObjProperty(mob,"#UserPid",GetPid());
    endif
  endif

  CheckUseSthWatch(who,mob);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function StandardLocCheck(who,byref Loc, Flags := 0, Param := 0)

  if(!Loc or who.dead or Loc.container or who.paralyzed or who.frozen)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(Loc.objtype and !Loc.item.container)
    var Checked := 0;

    if(Loc.item)
      if(Loc.item.graphic != Loc.objtype)
        Checked := 1;
      else
        Checked := 2;
      endif
    endif

    if(Loc.mobile)
      if(Loc.mobile.graphic != Loc.objtype)
        Checked := 1;
      else
        Checked := 2;
      endif
    endif

    if(!Checked)
      Checked := 1;
      foreach Item in ListStaticsAtLocation(Loc.x, Loc.y, LIST_IGNORE_Z, 0, who.realm)
        if(Abs(Item.objtype - Loc.objtype) < 5)
          Checked := 0;
          break;
        endif
      endforeach
    endif

    if(Checked == 1)
      SendSysMessage(who, "Nie widzisz tego.", FONT_NORMAL, COLOR_RED);
      return 0;
    endif
  endif

  if(!(Flags & NO_LOCSPEC_CHECK))
    if(Loc.item)
      Loc := Loc.item;
    elseif(Loc.mobile)
      Loc := Loc.mobile;
      if(Loc.hidden or Loc.concealed)
        SendSysMessage(who,"Nie widzisz tego.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif
    endif
  endif

  if(Loc.container or Loc.item.container)
    if(!(Flags & NO_ITEMCONT_CHECK))
      SendSysMessage(who,"Musisz wskazac miejsce.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  else
    if((Flags & NEW_DIST_CHECK) and Param)
      if(coordist(who.x, who.y, Loc.x, Loc.y) > Param)
        SendSysMessage(who,"Stoisz za daleko.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif
    endif

    if(!(Flags & NO_DIST_CHECK))
      if(Flags & NO_CHECK_SPECLOS)
        if(!CheckLosAt(who, Loc.x, Loc.y, Loc.z))
          SendSysMessage(who,"Nie widzisz tego.",FONT_NORMAL,COLOR_RED);
          return 0;
        endif
      else
        if(!CheckSpecialLos(who, Loc))
          SendSysMessage(who,"Nie widzisz tego.",FONT_NORMAL,COLOR_RED);
          return 0;
        endif
      endif
    elseif(coordist(who.x,who.y,Loc.x,Loc.y) > 15)
      SendSysMessage(who,"Nie widzisz tego.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

    if(!(Flags & NO_BUSY_CHECK))
      if(IsBusy(who,NO_BUSY_DELAY_CHECK | Flags))
        return 0;
      endif
    endif
  endif

  CheckUseSthWatch(who,Loc);
  return 1;

endfunction

//--------------------------------------------------------------------------------

function CheckUseSthWatch(who, Obj)

  if(!Obj or !who)
    return;
  endif

  var Msg  := "";
  var Type := "";

  if(Obj.isa(POLCLASS_MOBILE))
    Msg := obj.name + " Ser: "+CInt(obj.serial)+" Loc: "+obj.x+","+obj.y+","+obj.z+" "+obj.realm[1,2];
    Type := "Mob";
  elseif(Obj.isa(POLCLASS_ITEM))
    Msg := obj.desc + " Ser: "+CInt(obj.serial);
    if(obj.container)
      Msg := Msg + " Cont: "+obj.container.serial;
    else
      Msg := Msg + " Loc: "+obj.x+","+obj.y+","+obj.z+" "+obj.realm[1,2];
    endif
    Type := "Item";
  else
    Msg  := "Loc: "+obj.x+","+obj.y+","+obj.z+" "+who.realm[1,2];
    Type := "Loc";
  endif

  SetLastUse(who, LAST_USE_TARGET, Type, Msg);

  if(GetObjProperty(who, PROP_USESTH_WATCHER))
    var Watcher := FindPlayerBySerial(GetObjProperty(who,PROP_USESTH_WATCHER));
    if(!Watcher)
      EraseObjProperty(who, PROP_USESTH_WATCHER);
    elseif(!IsLoggedIn(watcher) or !watcher.cmdlevel)
      EraseObjProperty(who, PROP_USESTH_WATCHER);
      EraseObjProperty(watcher, PROP_USESTH_WATCH);
    else
      SendSysMessage(Watcher, "Osoba : " + who.name + " Typ : " + type);
      SendSysMessage(Watcher, Msg, FONT_NORMAL, COLOR_GREEN);
    endif
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Busy
//-----------------------------------------------------------------------------
//=============================================================================

const NO_BUSY_MSG         := 0x00010000;
const NO_BUSY_DELAY_CHECK := 0x00020000;
const NO_COMBAT_CHECK     := 0x00040000;
const NO_BUSY_SET         := 0x00080000;

//-----------------------------------------------------------------------------

function SetBusyTime(who, Time)

  if(!Cint(Time))
    return;
  endif

  SetObjProperty(who,"#busy",ReadGameClock() + Cint(Cint(Time) - 1));

endfunction

//-----------------------------------------------------------------------------

function IsBusy(who, Flags := 0)

  if(who.isa(POLCLASS_NPC))
    return 0;
  endif

  if(who.paralyzed)
    SendSysMessage(who, "Jestes sparalizowan" + ggm(who,6) + "." ,FONT_NORMAL,COLOR_RED);
    return 1;
  endif

  if(!(Flags & NO_WORK_BREAK))
    EraseObjProperty(who,"#HealTimer");
    EraseObjProperty(who,"#MedTimer");
    if(!(Flags & NO_COMBAT_CHECK))
      EraseObjProperty(who,"TrackingCounter");
    endif
  endif

  if(!who.backpack)
    return 1;
  endif

  if(!ReserveItem(who.backpack))
    if(!(Flags & NO_BUSY_CHECK))
      if(!(Flags & NO_BUSY_MSG))
        SendSysMessage(who,"Juz cos robisz.",FONT_NORMAL,COLOR_RED);
      endif
      return 1;
    endif
  elseif(Flags & NO_BUSY_SET)
    ReleaseItem(who.backpack);
  endif

  if((GetObjProperty(who,"#busy") and (Cint(GetObjProperty(who,"#busy")) > ReadGameClock()) and !(Flags & NO_BUSY_DELAY_CHECK) ) or
    (GetObjProperty(who,"#CombatBusy") and (Cint(GetObjProperty(who,"#CombatBusy")) > ReadGameClock()) and !(Flags & NO_COMBAT_CHECK) ) )

    if(!(Flags & NO_BUSY_MSG))
      SendSysMessage(who,"Juz cos robisz.",FONT_NORMAL,COLOR_RED);
    endif

    return 1;
  endif

  /*
  Nie pamietam po grzyb to sprawdzenie  jest  - dlatego poki co comment.
  if(CInt(GetGlobalProperty("#:saver:globalsave")))
    var lastsave := ReadGameClock() - CInt(GetGlobalProperty("#:saver:globalsave"));
    if(lastsave <= 2)
      if(!(Flags & NO_BUSY_MSG))
        SendSysMessage(who,"Juz cos robisz.",FONT_NORMAL,COLOR_RED);
      endif
      return 1;
    endif
  endif
  */

  return 0;

endfunction

//-----------------------------------------------------------------------------

function NoLongerBusy(who)

  ReleaseItem(who.backpack);
  EraseObjProperty(who,"#UserPid");

endfunction

//---------------------------------------------------------------------------------------
