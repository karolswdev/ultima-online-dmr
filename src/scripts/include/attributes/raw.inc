//--------------------------------------------------------------------------------

include "caps";

//--------------------------------------------------------------------------------

const PROP_ATTRIBUTE         := "At_";

//--------------------------------------------------------------------------------
// Attribute Flags
//--------------------------------------------------------------------------------

const ATTRF_BLOCKED          := 0x0001;

//================================================================================
//--------------------------------------------------------------------------------
// Attribute Core Messages
//--------------------------------------------------------------------------------
//================================================================================

function RebuildAttributeSums(who)

  var SkillSum    := array(0, 0);
  var StatSum     := 0;
  var i;

  var maxSkId := GetMaxSkillId();

  for(i:=0;i<=maxSkId;i:=i+1)
    var AttrId := GetAttributeIdBySkillId(i);

    if(IsCraftHarvestAttribute(AttrId))
      SkillSum[2] := SkillSum[2] + GetBaseSkillBaseValue(who, i);
    else
      SkillSum[1] := SkillSum[1] + GetBaseSkillBaseValue(who, i);
    endif
  endfor

  StatSum += GetRawAttributeValue(who, ATTRIBUTEID_STRENGTH);
  StatSum += GetRawAttributeValue(who, ATTRIBUTEID_DEXTERITY);
  StatSum += GetRawAttributeValue(who, ATTRIBUTEID_INTELLIGENCE);

  SetObjProperty(who, PROP_SKILLSUM, SkillSum);
  SetObjProperty(who, PROP_STATSUM, StatSum);

endfunction

//-----------------------------------------------------------------------------

function SetOffLineSkillValue(who, AttrId, Val)

  if(!AttrId)
    return 0;
  endif

  RebuildAttributeSums(who);

  var SkId     := GetSkillIdByAttributeId(AttrId);
  var SkMax    := GetSkillBaseMaxValue(who, SkId);
  var SkillSum := GetObjProperty(who, PROP_SKILLSUM);
  var SkillCap := GetSkillCap(who);
  var CurVal   := GetAttributeBaseValue(who, AttrId);
  var Diff     := (Val - CurVal);

  var IsCraft := IsCraftHarvestAttribute(AttrId);

  if(who.cmdlevel)
    Val := MinMax(CInt(Val), CORE_SETTINGS_MIN_ATTRIBUTE, CORE_SETTINGS_MAX_ATTRIBUTE);
  else
    Val := MinMax(CInt(Val), CORE_SETTINGS_MIN_ATTRIBUTE, SkMax);

    var CurCap := SkillCap;
    var CurSum := SkillSum[1];

    if(IsCraft)
      CurCap += SK_CRAFT_SKILLCAP_ADD;
      CurSum += SkillSum[2];
    else
      CurSum += Max(SkillSum[2] - SK_CRAFT_SKILLCAP_ADD, 0); // dodajemy, ale odejmujac 100% z rzemiosla
    endif

    if(IsCraft)
      CurCap += SK_CRAFT_SKILLCAP_ADD;
    endif

    if(CurSum + Diff > CurCap)
      if(Diff > 0)
        if(!GetObjProperty(who, PROP_SKILLCAP))
          SetObjProperty(who, PROP_SKILLCAP, 1+IsCraft);
        endif

        Diff := (CurCap - CurSum);
        Val  := CurVal + Diff;

        if(Val == CurVal)
          return 0;
        endif
      endif

    elseif(CurSum == CurCap)

      if(Diff < 0)
        EraseObjProperty(who,PROP_SKILLCAP);
      else
        if(!GetObjProperty(who,PROP_SKILLCAP))
          SetObjProperty(who,PROP_SKILLCAP,1+IsCraft);
        endif
        return 0;
      endif

    elseif(GetObjProperty(who,PROP_SKILLCAP))
      EraseObjProperty(who,PROP_SKILLCAP);
    endif

  endif

  _SetRawAttributeValue(who, AttrId, BaseToRawSkill(Val));
  SkillSum[1+IsCraft] += Diff;
  SetAttributeBaseValue(who, AttrId, Val);
  SetObjProperty(who, PROP_SKILLSUM, SkillSum);

  return 1;

endfunction

//--------------------------------------------------------------------------------

function SetOffLineStatValue(who, AttrId, Val)

  if(!AttrId)
    return 0;
  endif

  if(who.cmdlevel)
    Val := MinMax(CInt(Val), CORE_SETTINGS_MIN_ATTRIBUTE, CORE_SETTINGS_MAX_ATTRIBUTE);
  else
    Val := MinMax(CInt(Val), 100, 1000);
  endif

  var CurVal   := GetAttributeBaseValue(who, AttrId);
  var RwVal    := BaseToRawSkill(Val);
  var Diff     := (Val - CurVal);

  var StSum    := CInt(GetObjProperty(who, PROP_STATSUM));
  var StCap    := CInt(who.statcap * 10);

  if(!who.cmdlevel)
    if(StSum + Diff > StCap)

      if(Diff > 0)
        if(!GetObjProperty(who,PROP_STATCAP))
          SetObjProperty(who,PROP_STATCAP,1);
        endif

        Diff := (StCap - StSum);
        Val  := CurVal + Diff;

        if(!Diff)
          return 0;
        endif
      endif

    elseif(StSum == StCap)

      if(Diff < 0)
        EraseObjProperty(who,PROP_STATCAP);
      else
        if(!GetObjProperty(who,PROP_STATCAP))
          SetObjProperty(who,PROP_STATCAP,1);
        endif

        return 0;
      endif

    elseif(GetObjProperty(who,PROP_STATCAP))
      EraseObjProperty(who,PROP_STATCAP);
    endif

  endif

  _SetRawAttributeValue(who, AttrId, RwVal);

  StSum += Diff;
  SetAttributeBaseValue(who, AttrId, Val);
  SetObjProperty(who, PROP_STATSUM, StSum);

  if(Diff != 0)
    RecalcVitals(who);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function SendAttrRefresh(who)

  if(who.isa(POLCLASS_NPC))
    return 0;
  endif

  var pid := CInt(GetObjProperty(who, PROP_ATTRCORE_PID));
  if(pid)
    pid := GetProcess(Pid);
  endif

  if(!pid)
    return 0;
  endif
  
  var sev         := array;
  sev.+type       := EVID_AT_REC;

  pid.sendevent(sev);
  return 1;

endfunction

//--------------------------------------------------------------------------------

function SendAttrEvent(who, EvID, AttrId, Val, NoStatBoost := 0)

  if(who.isa(POLCLASS_NPC))
    return 0;
  endif

  var pid := CInt(GetObjProperty(who, PROP_ATTRCORE_PID));
  if(pid)
    pid := GetProcess(Pid);
  endif

  if(!pid)
    case(EvID)
      EVID_SK_SET: return SetOffLineSkillValue(who, AttrId, Val);
      EVID_ST_SET: return SetOffLineStatValue(who, AttrId, Val);
      default:     return 0;
    endcase
  endif

  if(!pid)
    LogError("SendAttrEvent: Osoba "+who.name+"/"+who.serial+" nie ma rdzenia atrybutow!");
    return 0;
  endif
  
  var sev         := array;
  sev.+attid      := AttrId;
  sev.+type       := EvID;
  sev.+Val        := Val;
  sev.+NoStatBoost := NoStatBoost;

  pid.sendevent(sev);
  return 1;

endfunction

//--------------------------------------------------------------------------------

function BuildRawSkillArray(who)

 var i, AttrId;
  var maxSkId := GetMaxSkillId();

 for(i:=0;i<=maxSkId;i:=i+1)
   AttrId := GetAttributeIdBySkillId(i);
   _SetRawAttributeValue(who, AttrId, BaseToRawSkill(GetAttributeBaseValue(who, AttrId)));
 endfor

endfunction

//--------------------------------------------------------------------------------

function BuildRawStatArray(who)

 SetRawAttributeValue(who, ATTRIBUTEID_STRENGTH,     BaseToRawSkill(GetBaseStrengthBaseValue(who)));
 SetRawAttributeValue(who, ATTRIBUTEID_INTELLIGENCE, BaseToRawSkill(GetBaseIntelligenceBaseValue(who)));
 SetRawAttributeValue(who, ATTRIBUTEID_DEXTERITY,    BaseToRawSkill(GetBaseDexterityBaseValue(who)));

endfunction

//--------------------------------------------------------------------------------

function GetRawAttributeValue(who, AttrId)

  if(!AttrId)
    return error;
  endif

  var SkVal := GetObjProperty(who, PROP_ATTRIBUTE + AttrId);

  if(!SkVal or !SkVal.size() or SkVal[1] == error or SkVal[2] == error)
    if(!who.isa(POLCLASS_MOBILE))
      return error;
    endif
    SkVal := array(BaseToRawSkill(GetAttributeBaseValue(who, AttrId)), CInt(SkVal[2]));
    SetObjProperty(who, PROP_ATTRIBUTE + AttrId, SkVal);
  endif

  return SkVal[1];

endfunction

//--------------------------------------------------------------------------------

function SetRawAttributeValue(who, AttrId, RwVal)

  if(!who.isa(POLCLASS_MOBILE) or !AttrId or RwVal == error)
    return;
  endif

  _SetRawAttributeValue(who, AttrId, RwVal);
  SetAttributeBaseValue(who, AttrId, RawToBaseSkill(RwVal));

endfunction

//--------------------------------------------------------------------------------

function _SetRawAttributeValue(who, AttrId, RwVal)

  if(!AttrId or !who or RwVal == error)
    return;
  endif

  var CurSk := GetObjProperty(who, PROP_ATTRIBUTE + AttrId);

  if(!CurSk)
    CurSk := array(RwVal, 0);
  else
    CurSk[1] := RwVal;
  endif

  SetObjProperty(who, PROP_ATTRIBUTE + AttrId, CurSk);

endfunction

//--------------------------------------------------------------------------------

function SetSkillsByRawProp(who)

  var SkId, AttrId, Val;
  var maxSkId := GetMaxSkillId();
    
  for(SkId:=0;SkId <= maxSkId; SkId:=SkId+1)
    AttrId := GetAttributeIdBySkillId(SkId);
    if(AttrId != error)
      Val := GetRawAttributeValue(who, AttrId);
      SetAttributeBaseValue(who, AttrId, RawToBaseSkill(Val));
    endif
  endfor

endfunction

//--------------------------------------------------------------------------------
