//-----------------------------------------------------------------------------

include "include/events";

include "include/clock";
include "include/attributes/npcs";
include "include/objtypes/misc";
include "include/checks/stdchecks";
include "include/mobile/tamed";

include "ai/include/abilities";

//-----------------------------------------------------------------------------

const HAPPY_TIME             := 900;
const TAME_TIMER             := 300;
const CRITICAL_HAPPY_LVL     := 3;
const MIDDLE_HAPPY_LVL       := 5;
const HIGH_HAPPY_LVL         := 18;
const MAX_HAPPY_LVL          := 20;

const NPCF_TAMED_CGUARD      := 0x00001000;

const MOUNT_EXPTIME          := 345600;

const JUKA_MIN_SKVAL         := 60;

const PROP_TAMERS_LIST       := "TamersList";

//-----------------------------------------------------------------------------

function GetHappiness(me)

  if(!GetObjProperty(me, "happiness"))
    SetObjProperty(me, "happiness", MAX_HAPPY_LVL);
  endif

  return Cint(GetObjProperty(me, "happiness"));
  
endfunction

//-----------------------------------------------------------------------------

function SetHappiness(me,happiness)

  if(happiness > MAX_HAPPY_LVL)
    happiness := MAX_HAPPY_LVL;
  endif

  SetObjProperty(me,"happiness",happiness);
  
endfunction

//-----------------------------------------------------------------------------

function EraseHappiness(me)

  EraseObjProperty(me,"happiness");

endfunction

//-----------------------------------------------------------------------------

function CheckHappinessCounter(me, mount := 0)

  var Master := GetMaster(me);

  if(Master.cmdlevel or IsSleeping(me))
    return 0;
  endif

  if(!GetHappinessCounter(me))
    SetHappinessCounter(me,ReadGameClock());
  endif
  
  if(GetHappinessCounter(me) < ReadGameClock())
    SetHappiness(me,GetHappiness(me) - 1);
    if(Mechanic)
      SetHappinessCounter(me,Cint(GetHappinessCounter(me) + (HAPPY_TIME * 2)));
    else
      SetHappinessCounter(me,Cint(GetHappinessCounter(me) + (HAPPY_TIME + (HAPPY_TIME * (GetEffectiveSkill(Master, SKILLID_TAMING) * 0.01))) ) );
    endif
    
    if(mount)
      SetHappiness(mount,GetHappiness(me));
      SetHappinessCounter(mount,GetHappinessCounter(me));
    endif
    
    if(GetHappiness(me) <= MIDDLE_HAPPY_LVL)
      if(mount)
        if(Mechanic)
          PrintTextAbove(mount,"*skrzypi*");
        else
          PrintTextAbove(mount,"*glod*");
        endif
      else
        if(Mechanic)
          PrintTextAbove(me,"*skrzypi*");
        else
          PrintTextAbove(me,"*glod*");
        endif
      endif
    endif
  endif
  
  if(GetHappiness(me) < CRITICAL_HAPPY_LVL)
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function EraseHappinessCounter(me)

  EraseObjProperty(me,"HappyCounter");

endfunction

//-----------------------------------------------------------------------------

function GetHappinessCounter(me)

  return Cint(GetObjProperty(me,"HappyCounter"));

endfunction

//-----------------------------------------------------------------------------

function SetHappinessCounter(me,time)

  SetObjProperty(me,"HappyCounter",Cint(time));

endfunction

//-----------------------------------------------------------------------------

function SetTameTimer(me,time)

  Setobjproperty(me, "TameTimer",Cint(time));

endfunction

//-----------------------------------------------------------------------------

function GetTameTimer(me)

  return Cint(GetObjProperty(me,"TameTimer"));
  
endfunction

//-----------------------------------------------------------------------------

function EraseTameTimer(me)

  EraseObjProperty(me,"TameTimer");

endfunction

//-----------------------------------------------------------------------------

function GetTamersList(critter)

  var List := GetObjProperty(critter, PROP_TAMERS_LIST);

  if(!List)
    List := dictionary;
  endif
  
  if(IsVarType(List, VAR_ARRAY))
    var lNew := dictionary;
    foreach Serial in List
      lNew[Serial] := 1;
    endforeach
    
    List := lNew;
    SetObjProperty(critter, PROP_TAMERS_LIST, List);
  endif
  
  return List;
  
endfunction

//-----------------------------------------------------------------------------

function AddTamedCounter(critter, who, Amt := 1)

  if(!critter.isa(POLCLASS_NPC) or !IsPlayer(who))
    return;
  endif

  var List := GetTamersList(critter);
  
  List[who.serial] := CInt(List[who.serial]) + Amt;

  SetObjProperty(critter, PROP_TAMERS_LIST, List);

endfunction

//-----------------------------------------------------------------------------

function GetTamedCounter(critter, who)

  var List := GetTamersList(critter);
  return CInt(List[who.serial]);
  
endfunction

//-----------------------------------------------------------------------------

function SetPrevTamed(me,tamerser)

  SetObjProperty(me,"PrevTamed",Cint(tamerser));
  
endfunction

//-----------------------------------------------------------------------------

function ErasePrevTamed(me)

  EraseObjProperty(me,"PrevTamed");
  
endfunction

//-----------------------------------------------------------------------------

function CheckNecroControl(me)

  if(GetNecroControlledTime(me))
    if(GetNecroControlledTime(me) < ReadGameClock())
      return 1;
    endif
  endif
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function SetNecroControl(me, who, time)

  me.setmaster(who);
  SetObjProperty(me, "NecroControlled", time);
  SetTamed(me);
  AddTamedCounter(me, who);

endfunction

//-----------------------------------------------------------------------------

function EraseNecroControl(me)

  EraseObjProperty(me,"NecroControlled");

endfunction

//-----------------------------------------------------------------------------

function SetTamed(critter)

  SendChEvent(critter,critter,EVID_PEACEMADE);
  critter.script := "tamed";
  RestartScript(critter);
  
endfunction

//-----------------------------------------------------------------------------

function Tame(critter, Master, hLvl := MAX_HAPPY_LVL)

  if(hLvl < MIDDLE_HAPPY_LVL)
    hLvl := MIDDLE_HAPPY_LVL;
  endif

  SetHappiness(critter, hLvl);
  critter.setmaster(Master);
  SetPrevTamed(critter, Master.serial);
  AddTamedCounter(critter, Master, 2);
  SetHappinessCounter(critter, ReadGameClock() + HAPPY_TIME);
  SetCriminal(critter, Master.criminal, "Tame", "Master "+Master.name+" ["+Master.serial+"] mial krima podczas oswajania.");
  critter.SetMurderer(CInt(Master.murderer));
  SetTamed(critter);

endfunction

//-----------------------------------------------------------------------------

function UnTame(critter, utType := 0)

  if(!IsTamed(critter))
    return 0;
  endif

  var cfgElem := GetNpcConfig(critter);

  critter.script := cfgElem.script;

  if(utType)
    if(GetHappiness(critter) <= CRITICAL_HAPPY_LVL and utType == 2)
      PrintTextAbove(critter, "*Stworzenie " + critter.name + " zdecydowalo ze bedzie mu lepiej bez mistrza!*" );
    elseif(utType == 1)
      PrintTextAbove(critter, "*grrrr*");
    elseif(!GetNecroControlledTime(critter) and utType == 2)
      PrintTextAbove(critter, critter.name + " moze hasac ponownie!" );
    elseif(utType == 3)
      PrintTextAbove(critter, "*odmawia posluszenstwa*");
    endif
  endif

  var Mount := FindObjTypeInContainer(critter.backpack,UOBJ_MOUNT);
  if(Mount)
    DestroyItem(Mount);
  endif

  var Master := GetMaster(critter);

  if(Master.serial)
    SetPrevTamed(critter, Master.serial);
  endif

  if(Master)
    if(cfgElem.alignment == CITYAL_EVIL)
      var utCity := FindCity(critter);
      if(utCity)
        SetCriminal(Master, 1, "UnTame", "Uwolnil zlego Npca "+critter.name+" ["+critter.serial+"] w miescie "+utCity+", utType: "+utType+".");
      endif
    endif
  endif

  critter.setmaster(0);
  EraseObjProperty(critter, PROP_MASTER);
  EraseHappiness(critter);
  EraseHappinessCounter(critter);
  EraseNecroControl(critter);
  SetTameTimer(critter,ReadGameClock() + TAME_TIMER);
  SetObjProperty(critter,"UnTamed",ReadGameClock());
  EraseObjProperty(critter,"#Following");
  EraseObjProperty(critter,"#Followed");
  RestartScript(critter);
  
endfunction

//-----------------------------------------------------------------------------

function CanControlTamed(npctemplate, newmaster)

  if(!newmaster.isa(POLCLASS_MOBILE))
    return 0;
  endif

  var cfgElem := GetNpcConfig(npctemplate);
  var tameskill := cfgElem.tameskill;

  if(!cfgElem.tameanyctrl)
    if(tameskill > 100)
      if((tameskill - 20) > GetBaseSkill(newmaster, SKILLID_TAMING))
        return 0;
      endif
    endif

    if(tameskill > 40 and ((tameskill - 20) > GetBaseSkill(newmaster, SKILLID_ANIMALLORE)))
      return 0;
    endif
  endif
  
  var ReqSkill := cfgElem.ReqSkill;

  if(ReqSkill)
    ReqSkill := SplitWords(ReqSkill);
    if(GetBaseSkill(newmaster, CInt(ReqSkill[1])) < CInt(ReqSkill[2]))
      return 0;
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanLearnTamed(Npc, Master)

  var cfgElem := GetNpcConfig(Npc);
  return GetBaseSkill(Master, SKILLID_TAMING) > (CInt(cfgElem.tameskill) - 40);

endfunction

//-----------------------------------------------------------------------------

function CanCommandTamed(npctemplate, newmaster)

  var cfgElem := GetNpcConfig(npctemplate);

  if(!newmaster.isa(POLCLASS_MOBILE) or !cfgElem)
    return 0;
  endif

  var tameskill := cfgElem.tameskill;

  if(!cfgElem.tameanyctrl)
    if((tameskill > 40) and ((tameskill - 20) > GetBaseSkill(newmaster, SKILLID_TAMING)))
      return 0;
    endif
  endif
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function TamedNpcHavePack(Animal)

  return (Animal.graphic == CID_HORSEPACK or Animal.graphic == CID_LLAMAPACK or Animal.graphic == CID_GOLEM);

endfunction

//-----------------------------------------------------------------------------

function PutJukaOnAnimal(Juka,Animal,Master)

  if(Juka.objtype == UOBJ_JUKA)
    if(StandardItemCheck(Master,Juka))
      case(Animal.graphic)
        CID_BEIGE_HORSE:  ClearMountItem(Animal);
                          SetNpcGraphic(Animal, CID_HORSEPACK);
                          DestroyItem(Juka);
        CID_LLAMA:        ClearMountItem(Animal);
                          SetNpcGraphic(Animal, CID_LLAMAPACK);
                          DestroyItem(Juka);
      endcase
      NoLongerBusy(Master);
      Detach();
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function ClearMountItem(Npc)

  if(Npc.backpack)
    foreach Item in EnumerateItemsInContainer(Npc.backpack)
      if(Item.objtype == UOBJ_MOUNT)
        DestroyItem(Item);
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function GetHungry(hungry)

  var status := "";
  var hung;
  if(hungry < 2)
    hung := 0;
  elseif((hungry >= 2) && (hungry < 4))
    hung := 1;
  elseif((hungry >= 4) && (hungry < 5))
    hung := 2;
  elseif((hungry >= 5) && (hungry < 7))
    hung := 3;
  elseif((hungry >= 7) && (hungry < 9))
    hung := 4;
  elseif((hungry >= 9) && (hungry < 11))
    hung := 5;
  elseif((hungry >= 11) && (hungry < 13))
    hung := 6;
  elseif((hungry >= 13) && (hungry < 15))
    hung := 7;
  elseif((hungry >= 15) && (hungry < 17))
    hung := 8;
  elseif((hungry >= 17) && (hungry < 19))
    hung := 9;
  elseif((hungry >= 19) && (hungry <= 20))
    hung := 10;
  elseif(hungry > 20)
    hung := 11;
  endif

  case(hung)
	0:  status := "speszone";
	1:  status := "bardzo niezadowolone";
	2:  status := "raczej nieszczesliwe";
	3:  status := "nieszczesliwe";
	4:  status := "raczej niezadowolone";
	5:  status := "zadowolone";
	6:  status := "szczesliwe";
	7:  status := "calkiem szczesliwe";
	8:  status := "bardzo szczesliwe";
	9:  status := "nadzwyczaj szczesliwe";
	10: status := "zadowolone ze ma takiego mistrza";
	11: status := "nadzwyczaj szczesliwe z takiego wladcy";
  endcase

  return status;

endfunction

//-----------------------------------------------------------------------------
