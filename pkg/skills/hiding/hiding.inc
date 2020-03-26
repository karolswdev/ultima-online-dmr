//=============================================================================
//-----------------------------------------------------------------------------
// Hiding
//-----------------------------------------------------------------------------
//=============================================================================

include "../pkg/systems/wearwolf/common";

//=============================================================================

function CanHide(who)
 
  var circle;
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(who.frozen or who.paralyzed)
    return 0;
  endif

  foreach item in ListEquippedItems(who)
    if(item)
      circle := ItemCfg[item.objtype].blockcircle;
      if(circle == 1)
        if(who.weapon != item and !ItemCfg[item.objtype].shield)
          if(GetBaseSkill(who,SKILLID_HIDING) < 95 or GetBaseDexterity(who) < 55)
            SendSysMessage(who, "Jeszcze nie wiesz jak schowac sie w tak ciezkiej i widocznej zbroi!",FONT_NORMAL,COLOR_RED);
            return 0;
          endif
        endif
      endif
    endif
  endforeach

  while(GetObjProperty(who, "#Mounting"))
    Sleepms(50);
  endwhile

  var Mount := GetEquipmentByLayer(who, LAYER_MOUNT);
  var MountSpeedMod := (CanMod(who, MOD_RUN_SPEED) == 0);
  
  if(!IsWearWolfMorphed(who))
    if(GetObjProperty(who, "#Mounting"))
      SendSysMessage(who,"Nie uda Ci sie schowac wierzchowca!",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endif
  
  if(Mount or MountSpeedMod)
    if(GetBaseSkill(who,SKILLID_PALADINISM) or GetBaseSkill(who, SKILLID_FANATICISM) or GetBaseSkillBaseValue(who, SKILLID_KENSAI) or GetBaseSkillBaseValue(who, SKILLID_BARBARIAN))
      SendSysMessage(who,"Nie umiesz schowac wierzchowca.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

    var NpcTemp := GetObjProperty(Mount, PROP_NPCTEMP);

    if(!NpcTemp or MountSpeedMod)
      var Item := GetEquipmentByLayer(who, LAYER_HELM);
      if(who.graphic == CID_WOLF or ((Item.objtype != UOBJ_TOTEMSKALP and Item.objtype != 0xAA14) or GetBaseSkill(who,SKILLID_HIDING) < 100 or GetBaseDexterity(who) < 50))
        SendSysMessage(who,"Nie jestes jeszcze na to gotow"+ggm(who,6)+"...",FONT_NORMAL,COLOR_RED);
        return error;
      endif
    elseif(!GetNpcConfig(NpcTemp).CanHide)
      if(IsWolfMorphed(who))
        SendSysMessage(who,"Nie uda ci sie schowac w tej formie.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif
      SendSysMessage(who,"Nie uda Ci sie schowac wierzchowca!",FONT_NORMAL,COLOR_RED);
      return 0;
    elseif(GetBaseSkill(who,SKILLID_HIDING) < 50 or GetBaseDexterity(who) < 30)
      SendSysMessage(who,"Jestes za malo zreczn"+ggm(who,6)+" by sie schowac na wierzchowcu!",FONT_NORMAL,COLOR_RED);
      return 0;
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function TimeHidding(who,Dur)

  HidePlayer(who);

  SetObjProperty(who,"#InvisPid",GetPid());
  EraseObjProperty(who,"#HealTimer");
  Detach();
  NoLongerBusy(who);

  var Time := ReadGameClock() + Dur;

  while(Time > ReadGameClock())

    if(GetObjProperty(who, "#InvisPid") != GetPid())
      return;
    endif

    if(!who.hidden)
      EraseObjProperty(who, "#AddHideTime");
      return;
    endif

    Sleep(1);

    if(GetObjProperty(who, "#AddHideTime"))
      Time := Time + CInt(GetObjProperty(who, "#AddHideTime"));
      EraseObjProperty(who, "#AddHideTime");
    endif

  endwhile

  if(GetObjProperty(who,"#InvisPid"))
    if(CInt(GetObjProperty(who,"#InvisPid")) == GetPid())
      UnhidePlayer(who);
      EraseObjProperty(who,"#InvisPid");
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
