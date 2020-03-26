//-----------------------------------------------------------------------------

const CRF_NAME              := 0x01;
const CRF_GENDER            := 0x02;
const CRF_CLASS             := 0x04;
const CRF_RACE              := 0x08;
const CRF_COLOR             := 0x10;

const CRF_SKILLS            := 0x20;
const CRF_STATS             := 0x40;

const CRF_CANT_QUIT         := 0x100;
const CRF_START             := 0x200;
const CRF_NO_EQUIP          := 0x400;

const CRF_ALL               := 0xFF;

const CHAR_COLOR_BASE       := 32769;

const CR_MAX_SKILLS         := 4;

include "include/finding";

//-----------------------------------------------------------------------------

function DefineChooseData()

  var Data := struct;

  Data.+cfgFileClass := ReadConfigFile(":character:classes");
  Data.+cfgFileRace  := ReadConfigFile(":character:races");

  Data.+cfgRace   := 0;
  Data.+cfgClass  := 0;
  Data.+RaceId    := 0;
  Data.+ClassId   := 0;
  Data.+Gender    := 0;
  Data.+GenderTitle := "";
  Data.+CharName  := "";
  Data.+oldName   := "";
  Data.+Color     := 0;

  Data.+Stats     := array(10, 10, 10);
  Data.+Skills    := dictionary;

  Data.+curStage  := 1;
  Data.+maxStages := 2;

  Data.+Flags     := CRF_ALL;

  return Data;

endfunction

//-----------------------------------------------------------------------------

function PrepareDefaultSkills(Acc, byref Data)

  Data.Stats[1] := CInt(Data.cfgClass.DefStr);
  Data.Stats[2] := CInt(Data.cfgClass.DefDex);
  Data.Stats[3] := CInt(Data.cfgClass.DefInt);

  var Skills := GetConfigStringArray(Data.cfgClass, "DefAttribute");

  Data.Skills := dictionary;
  var MaxSkillVal := CInt(GetCreationConfig().MaxSkillVal);
  var MaxSkill := GetCreationMaxSkills(Acc);

  foreach Skill in Skills
    ModifySkillData(Data, GetSkillIdByAttributeId(Skill), MaxSkillVal, MaxSkill);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function PrepareChooseDataFor(who, byref Data, limitData := 0)
  
  if(!IsInStartLocation(who))

    Data.CharName := GetRealName(who);
    Data.oldName := Data.CharName;
    SetDataGender(Data, who.gender+1);
    Data.Flags := 0;

    if(GetObjProperty(who, "CharData"))
      SetDataClass(Data, GetCharClass(who), who.acct);
      SetDataRace(Data, GetCharRace(who));
    else

      if(IsArcherBySkill(who))
        SetDataClass(Data, CLASS_ARCHER, who.acct);
      elseif(IsMageBySkill(who))
        SetDataClass(Data, CLASS_MAGE, who.acct);
      else
        SetDataClass(Data, CLASS_WARRIOR, who.acct);
      endif

      var oldRace := GetObjProperty(who, "Race");

      if(oldRace)
        case(oldRace)
          1:  SetDataRace(Data, ELF_RACE);
          2:  SetDataRace(Data, DWARF_RACE);
          3:  SetDataRace(Data, ISTARI_RACE);
          4:  SetDataRace(Data, HUMAN_RACE);
        endcase
      endif

      Data.Flags := CRF_RACE | CRF_COLOR | CRF_CANT_QUIT;
    endif

    // nie bedzie przeciez wybierac skilli...
    Data.maxStages := 1;

  endif

  if(limitData)
    if(limitData.ClassId)
      SetDataClass(Data, limitData.ClassId, who.acct);
      SetBitOff(Data.Flags, CRF_CLASS);

      if(IsInStartLocation(who))
        SetBitOn(Data.Flags, CRF_START);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SetDataClass(byref Data, Class, Acc)

  Data.ClassId  := Class;
  Data.cfgClass := Data.cfgFileClass[Class];

  PrepareDefaultSkills(Acc, Data);

  SetDataRace(Data, 0);

endfunction

//-----------------------------------------------------------------------------

function SetDataRace(byref Data, Race)

  Data.RaceId  := Race;

  if(Race)
    Data.cfgRace := Data.cfgFileRace[Race];

    if(Data.Color)
      var Colors   := SplitWords(Data.cfgRace.Colors);
      Data.Color   := CInt(Colors[1]);
    endif
  else
    Data.Race := 0;
    Data.cfgRace := 0;
    
    if(Data.Color)
      Data.Color := CHAR_COLOR_BASE;
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SetDataGender(byref Data, Gender)

  Data.Gender := Gender;

  case(Gender)
    1:  Data.GenderTitle := "Mezczyzna";
    2:  Data.GenderTitle := "Kobieta";
    default: 
      Data.Gender := 0;
      Data.GenderTitle := "";
  endcase

endfunction

//-----------------------------------------------------------------------------

function ModifyStatValue(byref Stats, Idx, Mod, MaxVal)

  var Sum := GetArraySum(Stats);
  var MaxStat := CInt(GetCreationConfig().MaxStatVal);
  
  if(Stats[Idx] + Mod > MaxStat)
    Mod := MaxStat - Stats[Idx];
  elseif(Stats[Idx] + Mod < 10)
    Mod := Min(10 - Stats[Idx], 0);
  endif

  if(Sum + Mod > MaxVal)
    Mod := MaxVal - Sum;
  endif

  if(!Mod)
    return;
  endif

  Stats[Idx] += Mod;

endfunction

//-----------------------------------------------------------------------------

function ModifySkillData(byref Data, SkillId, Mod, MaxVal)

  var Sum := GetArraySum(Data.Skills);
  var MaxSkill := CInt(GetCreationConfig().MaxSkillVal);

  if(Data.Skills.exists(SkillId))
    if(Data.Skills[SkillId] + Mod > MaxSkill)
      Mod := MaxSkill - Data.Skills[SkillId];
    elseif(Data.Skills[SkillId] + Mod < 0)
      Mod := Min(0 - Data.Skills[SkillId], 0);
    endif
  endif

  if(Sum + Mod > MaxVal)
    Mod := MaxVal - Sum;
  endif

  if(!Data.Skills.exists(SkillId))
    Data.Skills[SkillId] := Mod;
  else
    Data.Skills[SkillId] += Mod;
  endif

endfunction

//-----------------------------------------------------------------------------

function ChangeStatValue(byref Data, butId, MaxStats)

  var Idx := CInt(butId / 10);
  butId := Modulo(butId, 10);

  case(butId)
    1:  ModifyStatValue(Data.Stats, Idx, -1, MaxStats);
    2:  ModifyStatValue(Data.Stats, Idx, -5, MaxStats);
    3:  ModifyStatValue(Data.Stats, Idx,  1, MaxStats);
    4:  ModifyStatValue(Data.Stats, Idx,  5, MaxStats);
  endcase

endfunction

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Processing character creation!
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

function ProcessCreationData(who, byref Data)

  // starych graczy zafrozuje na starcie, aby wybrali pod przymusem nowa.
  who.frozen := 0;

  if(IsBitOn(Data.Flags, CRF_GENDER))
    who.gender := Data.Gender-1;
  endif

  if(IsBitOn(Data.Flags, CRF_CLASS) or IsBitOn(Data.Flags, CRF_RACE))
    UndoMod(who, MOD_GRAPHIC, UMDF_WAIT);

    var CharData := array(Data.ClassId, Data.RaceId);
    SetObjProperty(who, "CharData", CharData);
    SendAttrRefresh(who);

    who.title_race := GetProperRaceTitle(who);

    if(who.dead)
      who.graphic := CInt(Data.cfgRace.Ghost);
    else
      case(Data.Gender)
        1: who.graphic := CInt(Data.cfgRace.MaleGraphic);
        2: who.graphic := CInt(Data.cfgRace.FemaleGraphic);
      endcase
    endif

     var hair  := GetEquipmentByLayer( who, LAYER_HAIR );
     DestroyItem(hair);
     hair := CreateItemAtTempLoc(Data.cfgRace.ShortHairs);
     if(hair)
       EquipItem(who, hair);
     endif

     if(!IsOnDMRTest())
       var Time := GetAccountExistsTime(who.acct);
       var Cfg  := GetCreationConfig();

       if(Time > 0 and Time < CInt(Cfg.PrivMaxTime))
         SetYoung(who);
       endif
     endif
  endif

  if(IsBitOn(Data.Flags, CRF_COLOR) and !IsVampire(who))
    UndoMod(who, MOD_COLOR, UMDF_WAIT);
    who.truecolor := Data.Color;
    who.color := Data.Color;
  endif

  if(IsBitOn(Data.Flags, CRF_NAME))
    UndoMod(who, MOD_INCO, UMDF_WAIT);
    SetName(who, Data.CharName);
  endif

  if(IsBitOn(Data.Flags, CRF_STATS))
    SetBaseStrength(who, Data.Stats[1]);
    SetBaseDexterity(who, Data.Stats[2]);
    SetBaseIntelligence(who, Data.Stats[3]);
    
    SetStamina(who, GetMaxStamina(who));
    SetMana(who, GetMaxMana(who));
    SetHp(who, GetMaxHp(who));
  endif

  if(IsBitOn(Data.Flags, CRF_SKILLS))

    var i;

    for(i:=0; i<=SKILLID__HIGHEST; i+=1)
      SetBaseSkill(who, i, 0);
    endfor

    foreach SkillVal in (Data.Skills)
      SetBaseSkill(who, _SkillVal_iter, SkillVal);
    endforeach

  endif

  if((IsBitOn(Data.Flags, CRF_CLASS) or IsBitOn(Data.Flags, CRF_START)) and !IsBitOn(Data.Flags, CRF_NO_EQUIP))
    CreateClassEquipment(who, Data);

    var Loc := GetLocFromString(Data.cfgClass.StartLoc);
    SendSysMessage(who, "Pomyslnie ukonczyl"+ggm(who,3)+" tworzenie postaci.", FONT_NORMAL, COLOR_WHITE);
    SendSysMessage(who, "Zostal"+ggm(who,3)+" przeniesion"+ggm(who,6)+" w lokacje startowa dla wybranej klasy.", FONT_NORMAL, COLOR_GREEN);
    var Res := MoveObject(who, Loc.x, Loc.y, Loc.z, Loc.realm, MOVECHAR_FORCELOCATION);

    if(!Res)
      return Res;
    endif
    
    TriggerTipEvent(who, TEID_BEGINING, TWPTF_ONETIME_EVENT | TWPTF_CHECK_CLASS);

  elseif(Data.Flags & CRF_RACE)
    SendSysMessage(who, "Pomyslnie zakonczono wybor nowej rasy.", FONT_NORMAL, COLOR_WHITE);
  endif

  // to jakby pozostalo po starociach.
  EraseObjProperty(who, "Race");

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CreateClassEquipment(who, byref Data)

  var Equip  := GetConfigStringArray(Data.cfgRace, "Equip");
  var ClCfg  := ReadConfigFile(COLORS_CFG);
  var Amt,Itm,Cls;

  for i:=1 to LAYER_MOUNT
    if(i != LAYER_PACK)
      Itm := GetEquipmentByLayer(who, i);
      if(Itm)
        DestroyItem(Itm);
      endif
    endif
  endfor

  var Hairs := CInt(Data.cfgRace.ShortHairs);

  if(Hairs)
    Hairs := CreateItemAtTempLoc(Hairs);
    if(!EquipItem(who, Hairs))
      DestroyItem(Hairs);
    endif
  endif
  
  foreach Item in Equip
    Item := SplitWords(Item);
    Amt  := Item[2];
    if(!CInt(Amt) and Amt != error)
      Amt := ClCfg[Amt];
      if(Amt)
        Cls := GetConfigStringArray(Amt, "color");
        Amt := Cls[RandomInt(Cls.size())+1];
        if(!Amt)
          Amt := Cls[1];
        endif
      endif
    endif
    Itm  := CreateItemInPack(who,Item[1]);
    if(CInt(Amt))
      Itm.color := CInt(Amt);
    endif
    EquipItem(who,Itm);
  endforeach

  var Keys := Data.Skills.keys();
  
  foreach SkId in Keys
    CreateLoot(who.backpack, SkId, STEQP_CFG);
  endforeach
    
  CreateLoot(who.backpack, "BackpackItems", STEQP_CFG);
  
  var gold := FindObjTypeInContainer(who.backpack, UOBJ_GOLDCOIN);
  
  if(gold)
	gold.locked := 1;
	gold.movable := 0;
	gold.newbie := 1;
	SetObjProperty(gold, "StartingGold", 1);
	SetObjProperty(gold, "ToolTip", "Startowe zloto");
  endif

endfunction

//-----------------------------------------------------------------------------
