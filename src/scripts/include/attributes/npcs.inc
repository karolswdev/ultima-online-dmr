//--------------------------------------------------------------------------------

include "include/attributes/main";
include "include/attributes/raw";
include "include/mobile/tamed";
include "include/mobile/npcs/utils";

//================================================================================
//--------------------------------------------------------------------------------
// Npc Attributes
//--------------------------------------------------------------------------------
//================================================================================

const NPCSKID_ARCHERY            := 0;
const NPCSKID_SWORDSMANSHIP      := 1;
const NPCSKID_MACEFIGHTING       := 2;
const NPCSKID_FENCING            := 3;
const NPCSKID_WRESTLING          := 4;
const NPCSKID_TACTICS            := 5;
const NPCSKID_DRUID              := 6;
const NPCSKID_MAGERY             := 7;
const NPCSKID_MAGICRESISTANCE    := 8;
const NPCSKID_VAMPIRE            := 9;
const NPCSKID_NECRO              := 10;
const NPCSKID_EVALINT            := 11;
const NPCSKID_SPIRITSPEAK        := 12;

const NPCSKID__HIGHEST           := NPCSKID_SPIRITSPEAK;

//--------------------------------------------------------------------------------
// NPC Count Attacker Gain
//--------------------------------------------------------------------------------

const PROP_CAG_PREFIX  := "Cag_";
const CAG_HEAL_PERCENT := 60;

//--------------------------------------------------------------------------------

const NPC_MAX_SKILL_GAIN         := 100;
const NPC_MAX_SK_VALUE           := 1100;

//--------------------------------------------------------------------------------

function AwardNpcPoints(Npc, SkillId, SkillPoints)

  SkillPoints := CInt(SkillPoints);

  if(!Npc.isA(POLCLASS_NPC) or SkillId == error or !SkillPoints)
    return 0;
  endif

  if(!IsNpcSkillGainOn(SkillId))
    return 0;
  endif

  if(IsTamed(Npc))
    if(!IsReallyTamed(Npc))
      return 0;
    endif

    SkillPoints := SkillPoints * 4;
  elseif(GetPrevTamed(Npc))
    SkillPoints := SkillPoints * 4;
  else
    SkillPoints := SkillPoints * 16;
  endif

  var nElem := GetNpcConfig(Npc);

  if(!nElem or !Npc)
    return 0;
  endif

  if(nElem.NoGain)
    return 0;
  endif

  var AttrId       := GetAttributeIdBySkillId(SkillId);

  if(!AttrId)
    return 0;
  endif

  var RawValue     := GetRawAttributeValue(Npc, AttrId);
  var BaseValue    := RawToBaseSkill(RawValue);
  var MaxVal       := CInt(GetConfigInt(nElem, AttrId)) * 10 + NPC_MAX_SKILL_GAIN;

  if(IsSetAttrFlag(Npc, AttrId, ATTRF_BLOCKED))
    return;
  endif

  if(SkillPoints > 0)
    if(BaseValue >= NPC_MAX_SK_VALUE)
      return 0;
    endif

    if(!nElem.Mechanic)
      if(IsReallyTamed(Npc))
        MaxVal := MaxVal + CInt(GetEffectiveSkillBaseValue(GetMaster(Npc), SKILLID_VETERINARY)/5);
      endif
    endif

    if(BaseValue >= MaxVal)
      return 0;
    endif
  elseif(BaseValue == 0)
    return 0;
  endif

  SetRawAttributeValue(Npc, AttrId, RawValue + SkillPoints);
  CheckGainAttributeWatcher(Npc, AttrId, SkillPoints);

  return 1;

endfunction

//--------------------------------------------------------------------------------

function IsNpcSkillGainOn(SkID)

  var AttrCfg := ReadConfigFile(ATTRIBUTES_CFG);

  return CInt(AttrCfg[GetAttributeIdBySkillID(SkID)].NpcGain);

endfunction

//--------------------------------------------------------------------------------

function SetNpcBaseSkillBaseValue(Npc, SkID, SkVal)

  SkID  := CInt(SkID);
  SkVal := CInt(SkVal);

  var CanGain := IsNpcSkillGainOn(SkID);

  if(CanGain)
    if(IsTamed(Npc))
      if(!IsReallyTamed(Npc))
        CanGain := 0;
      endif
    endif
  endif

  var AttrId := GetAttributeIdBySkillId(SkID);

  if(CanGain)
    _SetRawAttributeValue(Npc, AttrId, BaseToRawSkill(SkVal));
  endif

  return SetAttributeBaseValue(Npc, AttrId, SkVal);

endfunction

//--------------------------------------------------------------------------------

function SaveNpcAttrData(Npc, OnObj)

  if(!Npc.isa(POLCLASS_NPC) or !OnObj.isa(POLCLASS_UOBJECT))
    return 0;
  endif

  SetObjProperty(OnObj, PROP_NPCTEMP, Npc.npctemplate);
  SetObjProperty(OnObj, "Graphic", Npc.graphic);
  SetObjProperty(OnObj, "SetColor", CInt(GetObjProperty(Npc, "SetColor")));

  var rName  := GetNpcConfig(Npc).Name;

  if(rName != Npc.name)
    SetObjProperty(OnObj, "NpcName", Npc.name);
  endif

  var arMod := GetStaticMod(Npc, MOD_AR);

  if(arMod)
    SetObjProperty(OnObj, PROP_STATIC_MOD + MOD_AR, arMod);
  endif

  if(IsTamed(Npc))
    SetObjProperty(OnObj, PROP_MASTER, GetMasterSerial(Npc));
    SetHappiness(OnObj, GetHappiness(Npc));
  endif

  SetObjProperty(OnObj, "Color", Npc.color);
  SetObjProperty(OnObj, "RessurectTime",  CInt(GetObjProperty(Npc, "RessurectTime")));
  SetObjProperty(OnObj, "ResurrectCount", CInt(GetObjProperty(Npc, "ResurrectCount")));
  SetObjProperty(OnObj, "Vitals", {GetHp(Npc), GetStamina(Npc), GetMana(Npc)});
  SetObjProperty(OnObj, "NpcFlags", GetFlags(Npc));

  if(IsGmCreated(Npc))
    SetPropCreated(GetObjCreator(Npc),OnObj);
  elseif(IsQuestItem(Npc))
    SetPropQuestItem(GetObjQuestMaster(Npc),OnObj);
  endif

  var i, AttrId, maxSkId := GetMaxSkillId();
  var Attributes := dictionary;

  for(i:=0;i<=maxSkId;i:=i+1)
    if(IsNpcSkillGainOn(i))
      AttrId := GetAttributeIdBySkillid(i);
      Attributes[AttrId] := GetRawAttributeValue(Npc, AttrId);
    endif
  endfor

  SetObjProperty(OnObj, "Attributes", Attributes);
  SetObjProperty(OnObj, PROP_NPC_SPECABL, GetRawSpecialAbilities(Npc));

  if(CInt(GetObjProperty(Npc, PROP_CREATEDAT)))
    SetObjProperty(OnObj, PROP_CREATEDAT, GetObjProperty(Npc, PROP_CREATEDAT));
  endif

  var Props := GetObjPropertyNames(Npc);

  foreach PropName in Props 

    if(SubStr(PropName, 1, Len(PROP_CAG_PREFIX)) == PROP_CAG_PREFIX)
      SetObjProperty(OnObj, PropName, GetObjProperty(Npc, PropName));
    endif

  endforeach

  var List := GetObjProperty(Npc, PROP_TAMERS_LIST);

  if(List)
    SetObjProperty(OnObj, PROP_TAMERS_LIST, List);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function LoadNpcAttrData(Npc, FromObj, NewMaster := 0)

  if(!Npc.isa(POLCLASS_NPC) or !FromObj.isa(POLCLASS_UOBJECT))
    return 0;
  endif

  var Graphic := CInt(GetObjProperty(FromObj, "Graphic"));
  var Color   := GetObjProperty(FromObj, "SetColor");
  var Name    := GetObjProperty(FromObj, "NpcName");
  var arMod   := GetObjProperty(FromObj, PROP_STATIC_MOD + MOD_AR);
  var clr := GetObjProperty(FromObj, "Color");
 
  if(arMod)
    SetStaticMod(Npc, MOD_AR, arMod);
  endif

  if(Graphic)
    SetNpcGraphic(Npc, Graphic);
  endif

  if(Color != error)
    SetNpcColor(Npc, CInt(Color));
  endif
  
  if(clr != error)
	SetNpcColor(Npc, CInt(clr));
  endif

  if(Name)
    SetName(Npc, Name);
  endif

  __SetSpecialRawAbilities(Npc, GetRawSpecialAbilities(FromObj));

  if(GetObjProperty(FromObj, PROP_MASTER) or NewMaster.isa(POLCLASS_MOBILE))
    if(!NewMaster)
      NewMaster := GetMaster(FromObj);
    endif

    if(NewMaster.isa(POLCLASS_MOBILE))
      Tame(Npc, NewMaster, GetHappiness(FromObj));
      if(IsGmCreated(FromObj))
        SetPropCreated(GetObjCreator(FromObj),Npc);
      elseif(IsQuestItem(FromObj))
        SetPropQuestItem(GetObjQuestMaster(FromObj),Npc);
      endif
    endif
  endif

  SetObjProperty(Npc, "RessurectTime",  CInt(GetObjProperty(FromObj,  "RessurectTime")));
  SetObjProperty(Npc, "ResurrectCount", CInt(GetObjProperty(FromObj,  "ResurrectCount")));
  var Vitals := GetObjProperty(FromObj, "Vitals");

  if(Vitals[1] and Vitals.size() == 3)
    SetHp(Npc, Vitals[1]);
    SetStamina(Npc, Vitals[2]);
    SetMana(Npc, Vitals[3]);
  endif

  var Flags;

  if(FromObj.objtype == UOBJ_CORPSE)
    Flags := GetFlags(FromObj);
  else
    Flags := CInt(GetObjProperty(FromObj, "NpcFlags"));
  endif

  SetFlags(Npc, FromObj);

  var Attributes := GetObjProperty(FromObj, "Attributes");

  if(Attributes)
    foreach Attr in Attributes
      SetRawAttributeValue(Npc, _Attr_iter, Attr);
    endforeach
  else
    var i, AttrId, Val, maxSkId := GetMaxSkillId();
    for(i:=0;i<=maxSkId;i:=i+1)
      if(IsNpcSkillGainOn(i))
        AttrId := GetAttributeIdBySkillid(i);
        Val    := GetRawAttributeValue(FromObj, AttrId);
        if(Val != error)
          SetRawAttributeValue(Npc, AttrId, Val);
        endif
      endif
    endfor
  endif

  if(CInt(GetObjProperty(FromObj, PROP_CREATEDAT)))
    SetObjProperty(Npc, PROP_CREATEDAT, GetObjProperty(FromObj, PROP_CREATEDAT));
  endif

  // a tutaj przywracamy Cag - czyli count attacker gain = brak sparringu!

  var Props := GetObjPropertyNames(FromObj);

  foreach PropName in Props 

    if(SubStr(PropName, 1, Len(PROP_CAG_PREFIX)) == PROP_CAG_PREFIX)
      SetObjProperty(Npc, PropName, GetObjProperty(FromObj, PropName));
    endif

  endforeach

  var List := GetObjProperty(FromObj, PROP_TAMERS_LIST);

  if(List)
    SetObjProperty(Npc, PROP_TAMERS_LIST, List);
  endif

endfunction

//--------------------------------------------------------------------------------
