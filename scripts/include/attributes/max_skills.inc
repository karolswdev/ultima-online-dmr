//--------------------------------------------------------------------------------

include ":npcguilds:common";

//--------------------------------------------------------------------------------

const MAX_SKILLS_COUNT       := 20;

const MSKT_CLASS             := 1;
const MSKT_NPCGUILD          := 2;

const MSK_CLASS_COUNT        := 15;
const MSK_NPCGUILD_COUNT     := 15;

const MSK_DELAYED_INTERVAL   := 43200;
const MSK_DELAYED_AMOUNT     := 5;

//================================================================================
//--------------------------------------------------------------------------------
// Max Skill Values
//--------------------------------------------------------------------------------
//================================================================================

function GetSkillBaseMaxValue(who, SkID)

  if(IsBlockedSkillForClass(who, SkID))
    return 0;
  endif

  var AttrId := GetAttributeIdBySkillId(SkID);
  var SkVal := GetObjProperty(who, PROP_MAXSK + "_" + AttrId);

  if(SkVal)
    SkVal := CInt(SkVal);

    if(SkVal < 0 or SkVal > MAX_SK_MAX_VALUE)
      SkVal := MinMax(SkVal, 0, MAX_SK_MAX_VALUE);
      SetObjProperty(who, PROP_MAXSK + "_" + AttrId, SkVal);
    endif

    return SkVal;
  else
    return DEF_SK_MAX_VALUE;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetSkillMaxValue(who, SkID)

  return CDbl(GetSkillBaseMaxValue(who, SkID)) / 10.0;

endfunction

//--------------------------------------------------------------------------------

function GetMaxSkills(who)

  var Data := GetObjProperty(who, PROP_MAXSK);
  
  if(!Data or !IsVarType(Data, VAR_DICTIONARY))
    return dictionary;
  endif

  return Data;

endfunction

//--------------------------------------------------------------------------------

function EraseNpcGuildSkill(who, byref MaxSk, SkID)

  var AttrId := GetAttributeIdBySkillId(SkID);

  if(MaxSk.exists(AttrId))
    if(MaxSk[AttrId][1] == MSKT_NPCGUILD)
      var PlayerData := GetObjProperty(who, PROP_NGDDATA);
      
      if(PlayerData.MaxSkID == SkID)
        PlayerData.MaxSkID := error;
        PlayerData.SkChTime := 0;

        foreach Guild in (PlayerData.Guilds)
          __ParamUnused(Guild);
          SetBitOff(PlayerData.Guilds[_Guild_iter].Flags, NGPF_MAXSK_CHOOSEN);
        endforeach

        SetObjProperty(who, PROP_NGDDATA, PlayerData);
      endif
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function CheckDelayedMaxSkill(who)

  var MaxSk := GetMaxSkills(who);

  foreach Data in MaxSk
    if(Data.size() == 3)
      if(Data[3] + MSK_DELAYED_INTERVAL < ReadGameClock())

        var SkId := GetSkillIdByAttributeId(_Data_iter);
        var whoSk := GetBaseSkillBaseValue(who, SkID);
        var CurSkMax := CInt(GetObjProperty(who, PROP_MAXSK+"_"+_Data_iter));
        var SkElem := GetSkillConfigElem(SkId);

        CurSkMax := Max(CurSkMax - MSK_DELAYED_AMOUNT, DEF_SK_MAX_VALUE);

        if(whoSk > CurSkMax)
          SetBaseSkillBaseValue(who, SkID, CurSkMax);
        endif

        if(CurSkMax == DEF_SK_MAX_VALUE)
          MaxSk.erase(_Data_iter);
          SetObjProperty(who, PROP_MAXSK, MaxSk);
          EraseObjProperty(who, PROP_MAXSK+"_"+_Data_iter);
          return;
        endif
        
        Data[3] := ReadGameClock() + MSK_DELAYED_INTERVAL;
        SetObjProperty(who, PROP_MAXSK, MaxSk);
        SetObjProperty(who, PROP_MAXSK+"_"+_Data_iter, CurSkMax);
        SendSysMessage(who, "Twoja udoskonalona umiejetnosc "+SkElem.Name+" zanika...", FONT_NORMAL, COLOR_ORANGE);
        SetObjProperty(who, "#LastMaxSkillDelay", ReadGameClock() + 1800);
      else
        var Delay := CInt(GetObjProperty(who, "#LastMaxSkillDelay"));

        if(!Delay or Delay < ReadGameClock())
        
          var SkId := GetSkillIdByAttributeId(_Data_iter);
          var SkElem := GetSkillConfigElem(SkId);

          SendSysMessage(who, "Twoja udoskonalona umiejetnosc "+SkElem.Name+" niedlugo zaniknie...", FONT_NORMAL, COLOR_RED_LIGHT);
          Delay := ReadGameClock() + 900;
          SetObjProperty(who, "#LastMaxSkillDelay", Delay);
        endif
      endif
    endif
  endforeach
  
endfunction

//--------------------------------------------------------------------------------

function EraseMaxSkillDelayed(who, SkID)

  SkID  := CInt(SkID);

  var MaxSk := GetMaxSkills(who);
  var AttrId := GetAttributeIdBySkillId(SkID);

  if(!MaxSk.exists(AttrId))
    return error;
  endif
  
  EraseNpcGuildSkill(who, MaxSk, SkID);

  if(MaxSk[AttrId].size() < 3)
    MaxSk[AttrId][3] := ReadGameClock();
    SetObjProperty(who, PROP_MAXSK, MaxSk);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function EraseMaxSkill(who, SkID)

  SkID  := CInt(SkID);

  var MaxSk := GetMaxSkills(who);
  var AttrId := GetAttributeIdBySkillId(SkID);

  if(!MaxSk.exists(AttrId))
    return error;
  endif

  EraseNpcGuildSkill(who, MaxSk, SkID);

  var whoSk := GetBaseSkillBaseValue(who, SkID);

  if(whoSk > DEF_SK_MAX_VALUE)
    SetBaseSkillBaseValue(who, SkID, DEF_SK_MAX_VALUE);
  endif

  MaxSk.erase(AttrId);
  EraseObjProperty(who, PROP_MAXSK+"_"+AttrId);
  SetObjProperty(who, PROP_MAXSK, MaxSk);

  SendAttrRefresh(who);

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetMaxSkillsForType(who, Type)

  var MaxSk := GetMaxSkills(who);
  var Skills := array;

  foreach SkData in MaxSk
    if(Type == SkData[1])
      Skills.append(GetSkillIdByAttributeId(_SkData_iter));
    endif
  endforeach

  return Skills;

endfunction

//--------------------------------------------------------------------------------

function GetMaxSkillTypeName(Type)

  case(Type)
    MSKT_CLASS:     
      return "Klasa";
    MSKT_NPCGUILD:
      return "Gildia";
  endcase

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetMaxSkillMaxCountForType(Type)

  case(Type)
    MSKT_CLASS:     
      return MSK_CLASS_COUNT;
    MSKT_NPCGUILD:
      return MSK_NPCGUILD_COUNT;
  endcase

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CanAddMaxSkill(who, Type)

  var CurType  := GetMaxSkillsForType(who, Type);
  var MaxCount := GetMaxSkillMaxCountForType(Type);

  return (MaxCount and MaxCount > CurType.size());

endfunction

//--------------------------------------------------------------------------------

function RecheckGuildSkill(who, SkID, Type)

  if(Type == MSKT_NPCGUILD)
    var PlayerData := GetObjProperty(who, PROP_NGDDATA);
    PlayerData.MaxSkID  := SkID;
    PlayerData.SkChTime := ReadGameClock();
    SetObjProperty(who, PROP_NGDDATA, PlayerData);
  endif

endfunction

//--------------------------------------------------------------------------------

function SetMaxSkillValue(who, SkID, SkVal, Type, SkillTime := 0)

  SkID  := CInt(SkID);
  SkVal := CInt(SkVal);

  var AttrId := GetAttributeIdBySkillId(SkID);

  var MaxSk := GetMaxSkills(who);
  var whoSk := GetBaseSkillBaseValue(who, SkID);

  if(Type == MSKT_NPCGUILD)
    var PlayerData := GetObjProperty(who, PROP_NGDDATA);
    var GuildMax := 0;

    foreach GdData in (PlayerData.Guilds)
      if(GdData.Flags & NGPF_MAXSK_CHOOSEN)
        GuildMax := 1;
        break;
      endif
    endforeach
    
    if(!GuildMax)
      return error; // gracz musi sam wybrac najpierw jakis skill w gildii, aby byla gildia przypisana.
    endif
  endif

  if(MaxSk.exists(AttrId))
    if(GetSkillBaseMaxValue(who, SkID) == SkVal and MaxSk[AttrId][1] == Type and MaxSk[AttrId].size() == 2)
      RecheckGuildSkill(who, SkID, Type);
      return 1;
    endif
  endif

  if(whoSk > SkVal)
    SetBaseSkillBaseValue(who, SkID, SkVal);
  endif

  var CurType  := GetMaxSkillsForType(who, Type);
  var MaxCount := GetMaxSkillMaxCountForType(Type);

  if(!MaxCount)
    return 0;
  endif

  if(MaxSk.exists(AttrId) and MaxSk[AttrId][1] != Type and MaxSk[AttrId][1] == MSKT_NPCGUILD)
    EraseNpcGuildSkill(who, MaxSk, SkID);
  endif

  if(!MaxSk.exists(AttrId) or MaxSk[AttrId][1] != Type)
    while(CurType.size() >= MaxCount)
      EraseMaxSkill(who, CurType[1]);
      MaxSk := GetMaxSkills(who);
      CurType.erase(1);
    endwhile
  endif

  RecheckGuildSkill(who, SkID, Type);

  if(SkillTime)
    MaxSk[AttrId] := array(Type, SkillTime);
  else
    MaxSk[AttrId] := array(Type, ReadGameClock());
  endif

  SetObjProperty(who, PROP_MAXSK, MaxSk);
  SetObjProperty(who, PROP_MAXSK+"_"+AttrId, SkVal);

  SendAttrRefresh(who);
  return 1;

endfunction

//--------------------------------------------------------------------------------
