//-----------------------------------------------------------------------------

include "include/const/props";
include "include/attributes/main";

//-----------------------------------------------------------------------------

const NPC_ABLID_HEALING        := 1;
const NPC_ABLID_PATHFIND       := 2;
const NPC_ABLID_ENDURANCE      := 3;
const NPC_ABL_MAXID            := 3;
const NPC_ABL_MIN_MASTER_TAM   := 80;

//=============================================================================
//-----------------------------------------------------------------------------
// Special Abilities
//-----------------------------------------------------------------------------
//=============================================================================

function GetSpecialAbilities(Npc)

  var Abilits := GetRawSpecialAbilities(Npc);

  foreach AblVal in Abilits
    AblVal := RawToBaseSkill(AblVal);
  endforeach

  return Abilits;

endfunction

//-----------------------------------------------------------------------------

function SetSpecialRawAbilities(Npc, byref RawAbilits)

  var aEv := struct;
  aEv.+type       := EVID_SETSPECABS;
  aEv.+RawAbilits := RawAbilits;

  SendEvent(Npc, aEv);

endfunction

//-----------------------------------------------------------------------------

function GetRawSpecialAbilities(Npc)

  var RawAbilits := GetObjProperty(Npc, PROP_NPC_SPECABL);

  if(!RawAbilits or !RawAbilits.size())
    RawAbilits := array;
    var i;
    for(i:=1;i<=NPC_ABL_MAXID;i:=i+1)
      RawAbilits[i] := 0;
    endfor
    SetObjProperty(Npc, PROP_NPC_SPECABL, RawAbilits);
  endif

  if(RawAbilits.size() < NPC_ABL_MAXID)
    var i;
    for(i:=RawAbilits.size()+1;i<=NPC_ABL_MAXID;i:=i+1)
      RawAbilits[i] := 0;
    endfor
    SetObjProperty(Npc, PROP_NPC_SPECABL, RawAbilits);
  endif

  return RawAbilits;

endfunction

//-----------------------------------------------------------------------------

function GetBaseNpcAbilities(Npc)

  var Raw := GetRawSpecialAbilities(Npc);
  var i;
  var Abl := array;

  for(i:=1; i<=NPC_ABL_MAXID; i+=1)
    Abl[i] := RawToBaseSkill(Raw[i]);
  endfor

  return Abl;

endfunction

//-----------------------------------------------------------------------------

function GetBaseSpecialAbility(Npc, AblID)

  if(AblID <= NPC_ABL_MAXID)
    return RawToBaseSkill(GetRawSpecialAbilities(Npc)[AblID]);
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function SetBaseSpecialAbility(Npc, AblId, NewValue)

  var aEv := struct;
  aEv.+type   := EVID_SETSPECABS;
  aEv.+AblId  := AblId;
  aEv.+RawVal := BaseToRawSkill(NewValue);

  SendEvent(Npc, aEv);

endfunction

//-----------------------------------------------------------------------------

function __SetSpecialRawAbilities(Npc, byref RawAbs)

  if(RawAbs.size() < NPC_ABL_MAXID)
    var i;
    for(i:=RawAbs.size();i<=NPC_ABL_MAXID;i:=i+1)
      RawAbs[i] := 0;
    endfor
  endif

  SetObjProperty(Npc, PROP_NPC_SPECABL, RawAbs);
  SetEnduranceBonus(Npc, RawToBaseSkill(RawAbs[NPC_ABLID_ENDURANCE]));

endfunction

//-----------------------------------------------------------------------------

function GainSpecialAbility(Npc, AblID, Gain)

  if(Abilities[AblID] >= DEF_SK_MAX_VALUE)
    return;
  endif
  
  /*
  __NONPVP__ DELETED
  if (IsInNonPvPArea(Npc.x,Npc.y,Npc.realm))
    Gain := CInt (Gain * 0.5);
  endif
  */

  RawAbilities[AblID] := RawAbilities[AblID] + CInt(Gain);

  var BaseVal := RawToBaseSkill(RawAbilities[AblID]);

  if(AblID == NPC_ABLID_ENDURANCE)
    if(Abilities[AblID] != BaseVal)
      SetEnduranceBonus(Npc, BaseVal);
    endif
  endif

  Abilities[AblID] := BaseVal;

  SetObjProperty(Npc, PROP_NPC_SPECABL, RawAbilities);

endfunction

//-----------------------------------------------------------------------------

function GetAbilityName(AblID)

  case(AblID)
    NPC_ABLID_HEALING:     return "Uzdrawianie";
    NPC_ABLID_PATHFIND:    return "Znajdowanie sciezki";
    NPC_ABLID_ENDURANCE:   return "Wytrzymalosc";
  endcase

  return ""; 

endfunction

//-----------------------------------------------------------------------------

function SetEnduranceBonus(Npc, SkVal)

  var cfgElem  := GetNpcConfig(Npc);
  var NewHp    := CInt(cfgElem.HITS);
  var NewMana  := CInt(cfgElem.MANA);
  var NewRs    := CInt(cfgElem.RunSpeed);

  if(!NewRs)
    NewRs := CInt(cfgElem.DEX);
  endif

  NewHp   := NewHp + (NewHp * (SkVal * 0.00025));
  NewMana := NewMana + (NewMana * (SkVal * 0.00025));

  if(NewRs < 100)
    NewRs := NewRs + CInt(NewRs * (SkVal * 0.0008));
  else
    NewRs := NewRs + CInt(SkVal/100);
  endif

  SetMaxHp(Npc, NewHp);
  SetMaxMana(Npc, NewMana);
  Npc.run_speed := NewRs;

endfunction

//-----------------------------------------------------------------------------
