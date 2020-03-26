//-----------------------------------------------------------------------------

include "ai/include/tamed";

//-----------------------------------------------------------------------------

function AlarmOthers(who,Victim)

  foreach Mob in ListMobilesNearLocation(who.x,who.y,who.z,4,who.realm)
    if(Mob == Victim)
      SendSysMessage(Mob,who.name + " chcial"+ggm(who,4)+" Cie okrasc!",FONT_NORMAL,COLOR_RED);
    elseif(IsPlayer(Mob) and Mob != who)
      SendSysMessage(Mob,who.name + " chcial"+ggm(who,4)+" okrasc "+Victim.name+"!",FONT_NORMAL,COLOR_RED);
    endif
  endforeach
  UnhidePlayer(who);
  SetCriminal(who, 1, "AlarmOthers", "Okradal"+ggm(who,4)+" stworzenie "+Victim.name+" ["+Victim.serial+"] i sie nie powiodlo.");

endfunction

//-----------------------------------------------------------------------------

function SnoopAlarmOthers(who,Victim)

  if(!CheckSkill(who,SKILLID_SNOOPING,90,0))
    foreach Mob in ListMobilesNearLocation(who.x,who.y,who.z,4,who.realm)
      if(Mob == Victim)
        SendSysMessage(Mob,who.name + " chcial"+ggm(who,4)+" zajrzec do twojego plecaka!",FONT_NORMAL,COLOR_RED);
      elseif(IsPlayer(Mob) and Mob != who)
        SendSysMessage(Mob,who.name + " chcial"+ggm(who,4)+" zajrzec do plecaka "+Victim.name+"!",FONT_NORMAL,COLOR_RED);
      endif
    endforeach
    UnhidePlayer(who);
  endif
  
endfunction

//-----------------------------------------------------------------------------

function CanSnoop(who)
  
  var ItemCfg  := ReadConfigFile(ITEMS_CFG);

  if(GetEquipmentByLayer(who,LAYER_HAND1) or GetEquipmentByLayer(who,LAYER_HAND2))
    SendSysMessage(who,"Aby cos ukrasc musisz miec puste rece!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  foreach item in ListEquippedItems(who)
    if(ItemCfg[item.objtype].blockcircle and CInt(ItemCfg[item.objtype].blockcircle) < 6)
      SendSysMessage(who,"W takim ekwipunku ciezko bedzie ci cos ukrasc po cichu!",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetSnoopDiff(who,Victim)

  var SnoopCfg := ReadConfigFile(":snooping:snoop");
  var Diff := -1;

  if(Victim.isa(POLCLASS_NPC))
    if(IsTamed(Victim))
      Diff := 50;
    else
    
      var Grap := 0;
      
      foreach NpcTemp in GetConfigStringKeys(SnoopCfg)
        if(lower(NpcTemp) == lower(Victim.npctemplate))
          Grap := CInt(SnoopCfg[Victim.npctemplate].Grap);
          if(Grap == Victim.graphic or !Grap)
            Diff := CInt(SnoopCfg[Victim.npctemplate].Diff);
          endif
        endif
      endforeach
      
      if(Diff == -1)
        Diff := 150;
      endif
      
    endif
  endif

  if(IsPlayer(Victim))
    Diff := 5 + CInt((GetDexterity(Victim) * 100) / GetDexterity(who));
  endif
  
  return Diff;

endfunction

//-----------------------------------------------------------------------------

function GetSnoopPack(Victim)
  
  if(IsTamed(Victim))
    if(TamedNpcHavePack(Victim))
      return GetNpcBackpack(Victim);
    endif
  else
    return Victim.backpack;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
