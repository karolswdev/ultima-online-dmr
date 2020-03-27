//-----------------------------------------------------------------------------

include "include/getobj";
include "include/misc";
include "include/items";
include "include/npcs";
include "include/finding";
include "include/effects";
include "include/checks/objchecks";
include "include/checks/stdchecks";
include "include/client/fonts";
include "include/client/fonts";
include ":blacksmithy:lore";

//-----------------------------------------------------------------------------

function SkillItemHealth(who, skill, config1, config2 := 0)

  if(IsBusy(who))
    return;
  endif

  SetBusyTime(who,4);

  var WhoTinker := GetEffectiveSkill(who,skill);

  if(WhoTinker < 35)
    SendSysMessage(who, "Nie potrafisz oceniac przedmiotow.", FONT_NORMAL, COLOR_RED);
    return;
  endif

  SendSysMessage(who, "Wskaz przedmiot do ocenienia.", FONT_NORMAL, COLOR_GREEN);
  
  var item := ObjTarget(who);
  
  var madeby := GetItemMakerName(item);
  if(!madeby)
    madeby := "nieznana osobe";
  else
    madeby := "osobe zwana "+madeby;
  endif
  
  InitSkillGlob(who, config1, skill);
  if(!SkillGlob.cfg[item.objtype].health)
    if(config2)
      InitSkillGlob(who, config2, skill);
      if(!SkillGlob.cfg[item.objtype].health)
        SendSysMessage(who, "Nie potrafisz ocenic tego przedmiotu.", FONT_NORMAL, COLOR_RED);
        return;
      endif
    else
      SendSysMessage(who, "Nie potrafisz ocenic tego przedmiotu.", FONT_NORMAL, COLOR_RED);
      return;
    endif
  endif

  SendSysMessage(who, "Ocenil" + ggm(who,3) + " stan przedmiotu.", FONT_NORMAL, COLOR_GREEN);
  
  var Condition := GetItemCondition(item);
  SetToolTipProp(item, "Stan", Condition);

  SendSysmessage(who,"Ten przedmiot jest " + Condition + ".",FONT_NORMAL,COLOR_GREEN);

  if(WhoTinker > 70)
    SendSysMessage(who,"Zostal wykonany przez " + madeby + ".",FONT_NORMAL,COLOR_GREEN);
    SendSysMessage(who,GetQualityMsg(item),FONT_NORMAL,COLOR_BLUE);
  endif

endfunction

//--------------------------------------------------------------------------------

function DelaySkillUse(who, SkillID, delayTime)

  SetObjProperty(who, "#SkillDelay_"+SkillID, ReadGameClock() + delayTime);

endfunction

//--------------------------------------------------------------------------------

function CheckSkillUse(who, SkillID)

  if(IsBusy(who))
    return 0;
  endif

  var Delay := GetObjProperty(who, "#SkillDelay_"+SkillID);

  if(Delay != error)
    if(CInt(Delay) > ReadGameClock())
      SendSysMessage(who, "Musisz chwile odczekac...", FONT_NORMAL, COLOR_ORANGE);
      return 0;
    endif

    EraseObjProperty(who, "#SkillDelay_"+SkillID);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------
