//================================================================================
//--------------------------------------------------------------------------------
// Traps
//--------------------------------------------------------------------------------
//================================================================================

include "include/getobj";

//--------------------------------------------------------------------------------

function ArmTrap(who, Item)

  var Parms := {};
  Parms[1] := who;
  Parms[2] := Item;
  Parms[3] := CInt(GetTrapValue(Item,"TrapStr"));
  Parms[4] := CInt(GetTrapValue(Item,"TrapType"));
  
  start_script(":tinkering:armtrap",Parms);

endfunction

//--------------------------------------------------------------------------------

function ResistedTrap(who, Dmg)

  if(who.dead or !who)
    return 0;
  endif
  
  if(CheckSkill(who, SKILLID_MAGICRESISTANCE, CInt(Dmg * 9), 0) )
    SendSysMessage(who, "Czujesz swoja odpornosc na magie!");
    PlaySoundEffect(who, SFX_SPELL_WEAKEN);
    PlayObjectCenteredEffect(who, FX_CURSE_EFFECT, 5,5);
    Dmg := Dmg / 2;
  endif

  if(Dmg < 1)
    Dmg := 1;
  endif

  return Dmg;

endfunction

//--------------------------------------------------------------------------------

function SetTrap(Trapper,Item,TrapType,TrapStr)

  var TrapInfo := {};
  if(GetObjProperty(Item,"TrapInfo"))
    return 0;
  endif

  if(!Item.usescript)
    Item.usescript := "";
  endif

  if(TrapStr > 10)
    TrapStr := 10;
  elseif(TrapStr < 1)
    TrapStr := 1;
  endif

  TrapInfo[1] := TrapType;
  TrapInfo[2] := TrapStr;
  TrapInfo[3] := Trapper.serial;
  TrapInfo[4] := Item.usescript;

  SetObjProperty(Item,"TrapInfo",TrapInfo);
  Item.usescript := ":tinkering:tinkerTraps";

  return 1;

endfunction

//--------------------------------------------------------------------------------

function UnTrap(Item)

  var TrapInfo := GetObjProperty(Item,"TrapInfo");
  if(!TrapInfo)
    return 0;
  endif
  
  if(TrapInfo[4])
    Item.usescript := TrapInfo[4];
  else
    Item.usescript := "";
  endif
  
  EraseObjProperty(Item,"TrapInfo");
  
  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetTrapValue(Item,Type)

  var TrapInfo := GetObjProperty(Item,"TrapInfo");
  Case(Type)
    "TrapType":  return TrapInfo[1];
    "TrapStr":   return TrapInfo[2];
    "Trapper":   return TrapInfo[3];
    "OldScript": return TrapInfo[4];
  endcase

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsTrappedItem(Item)

  if(GetObjProperty(Item,"TrapInfo"))
    return 1;
  endif
  
  return 0;
  
endfunction

//--------------------------------------------------------------------------------
