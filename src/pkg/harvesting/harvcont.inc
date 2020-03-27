//-----------------------------------------------------------------------------

include "ai/include/tamed";

//-----------------------------------------------------------------------------

function SetHarvestContainerFromLoc(who, Loc, HarvSkillID)

  var SkElem := GetSkillConfigElem(HarvSkillID);
  var SkName := SkElem.Name+"_Cont";
  var Obj;

  if(Loc.mobile)
    Obj := Loc.mobile;
  elseif(Loc.item)
    Obj := Loc.item;
  else
    Obj := Loc;
  endif

  if(Obj == who.backpack)
    EraseObjProperty(who, SkName);
    SendGood(who, "Zresetowano pojemnik "+GetHarvContSentence(HarvSkillID)+" na domyslny - wlasny plecak.", COLOR_BLUE);
    return 1;
  elseif(GetMaster(Obj) == who and TamedNpcHavePack(Obj))
    SetObjProperty(who, SkName, Obj.backpack.serial);
    SendGood(who, "Ustawiono pojemnik "+GetHarvContSentence(HarvSkillID)+" na juke zwierzecia "+GetName(Obj), COLOR_BLUE);
    return 1;
  elseif(Obj.Isa(POLCLASS_CONTAINER))
    var topCont := GetTopCont(Obj);
    var topMob := GetTopContMob(Obj);

    if(topCont == who.backpack or (topMob and GetMaster(topMob) == who and Distance(topMob, who) < 3))
      SetObjProperty(who, SkName, Obj.serial);
      SendGood(who, "Ustawiono pojemnik "+GetHarvContSentence(HarvSkillID)+" na wskazany przedmiot.", COLOR_BLUE);
      return 1;
    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHarvContSentence(HarvSkilLID) 

  var Sentence := "docelowy";

  case(HarvSkilLID)
    SKILLID_MINING:
      Sentence := "kopania";
    SKILLID_LUMBERJACKING:
      Sentence := "rabania";
    SKILLID_FISHING:
      Sentence := "lowienia";
    SKILLID_FARMING:
    SKILLID_HERDING:
      Sentence := "zbierania";
  endcase

  return Sentence;

endfunction

//-----------------------------------------------------------------------------

function CreateHarvResource(who, HarvSkillID, ObjType, Amt := 1)

  var Cont := GetHarvestCont(who, HarvSkillID);

  return CreateItemInCont( Cont, ObjType, Amt);

endfunction

//-----------------------------------------------------------------------------

function GetHarvestCont(who, HarvSkillID)

  var SkElem := GetSkillConfigElem(HarvSkillID);
  var Serial := GetObjProperty(who, SkElem.Name+"_Cont");

  if(!Serial)
    return who.backpack;
  endif

  var Cont := FindItemBySerial(Serial);

  var Sentence := GetHarvContSentence(HarvSkillID);

  if(Cont)
    var topCont := GetTopCont(Cont);
    var topMob := GetTopContMob(Cont);
    if(topCont == who.backpack)
      return Cont;
    elseif(topMob)
      if(GetMaster(topMob) == who and Distance(topMob, who) < 3)
        return Cont;
      else
        SendBad(who, "Zwierze oddalilo sie lub ucieklo!");
      endif
    else
      SendBad(who, "Pojemnik "+Sentence+" nie jest juz w Twoim plecaku!");
    endif
  else
    SendBad(who, "Pojemnik nie istnieje!");
  endif
   
  EraseObjProperty(who, "MiningCont");
  return who.backpack;

endfunction

//-----------------------------------------------------------------------------
