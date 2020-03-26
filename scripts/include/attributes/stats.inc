//================================================================================
//--------------------------------------------------------------------------------
// Get Funcs
//--------------------------------------------------------------------------------
//================================================================================


//-----------------------------------------------------------------------------
function GetStrength(who)
  return GetAttribute(who, ATTRIBUTEID_STRENGTH);
endfunction

//-----------------------------------------------------------------------------

function GetDexterity(who)
  return GetAttribute(who, ATTRIBUTEID_DEXTERITY);
endfunction

//-----------------------------------------------------------------------------

function GetIntelligence(who)
  return GetAttribute(who, ATTRIBUTEID_INTELLIGENCE);
endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Base Funcs
//--------------------------------------------------------------------------------
//================================================================================

//-----------------------------------------------------------------------------
function GetBaseStrength(who)
  return Cint(GetAttributeBaseValue(who, ATTRIBUTEID_STRENGTH)/ 10);
endfunction

//-----------------------------------------------------------------------------

function GetBaseDexterity(who)
  return Cint(GetAttributeBaseValue(who, ATTRIBUTEID_DEXTERITY)/ 10);
endfunction

//-----------------------------------------------------------------------------

function GetBaseIntelligence(who)
  return Cint(GetAttributeBaseValue(who, ATTRIBUTEID_INTELLIGENCE)/ 10);
endfunction

//-----------------------------------------------------------------------------

function GetEffectiveStrengthBaseValue(who)
  return ToMin(GetAttributeBaseValue(who, ATTRIBUTEID_STRENGTH) + GetAttributeTemporaryMod(who, ATTRIBUTEID_STRENGTH), 0);
endfunction

//-----------------------------------------------------------------------------

function GetEffectiveDexterityBaseValue(who)
  return ToMin(GetAttributeBaseValue(who, ATTRIBUTEID_DEXTERITY) + GetAttributeTemporaryMod(who, ATTRIBUTEID_DEXTERITY), 0);
endfunction

//-----------------------------------------------------------------------------

function GetEffectiveIntelligenceBaseValue(who)
  return ToMin(GetAttributeBaseValue(who, ATTRIBUTEID_INTELLIGENCE) + GetAttributeTemporaryMod(who, ATTRIBUTEID_INTELLIGENCE), 0);
endfunction

//-----------------------------------------------------------------------------

function GetBaseStrengthBaseValue(who)
  return GetAttributeBaseValue(who, ATTRIBUTEID_STRENGTH);
endfunction

//-----------------------------------------------------------------------------

function GetBaseDexterityBaseValue(who)
  return GetAttributeBaseValue(who, ATTRIBUTEID_DEXTERITY);
endfunction

//-----------------------------------------------------------------------------

function GetBaseIntelligenceBaseValue(who)
  return GetAttributeBaseValue(who, ATTRIBUTEID_INTELLIGENCE);
endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Mods Funcs
//--------------------------------------------------------------------------------
//================================================================================

function GetStrengthMod(who)
  return Cint(GetAttributeTemporaryMod(who, ATTRIBUTEID_STRENGTH)/ 10);
endfunction

//-----------------------------------------------------------------------------

function GetDexterityMod(who)
  return Cint(GetAttributeTemporaryMod(who, ATTRIBUTEID_DEXTERITY)/ 10);
endfunction

//-----------------------------------------------------------------------------

function GetIntelligenceMod(who)
  return Cint(GetAttributeTemporaryMod(who, ATTRIBUTEID_INTELLIGENCE)/ 10);
endfunction

//-----------------------------------------------------------------------------

function ModifyStrengthMod(who, moda)
  if(ModifyAttributeTemporaryModBaseValue(who, ATTRIBUTEID_STRENGTH, Cint(moda*10)))
    RecalcVitals(who);
    return 1;
  else
    return 0;
  endif
endfunction

//-----------------------------------------------------------------------------

function ModifyDexterityMod(who, moda)
  if(ModifyAttributeTemporaryModBaseValue(who, ATTRIBUTEID_DEXTERITY, Cint(moda*10)))
    RecalcVitals(who);
    return 1;
  else
    return 0;
  endif
endfunction

//-----------------------------------------------------------------------------

function ModifyIntelligenceMod(who, moda)
  if(ModifyAttributeTemporaryModBaseValue(who, ATTRIBUTEID_INTELLIGENCE, Cint(moda*10)))
    RecalcVitals(who);
    return 1;
  else
    return 0;
  endif
endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Set Mods Funcs
//--------------------------------------------------------------------------------
//================================================================================

function SetStrengthMod(who, Amt)

  return SetStrengthModBaseValue(who, Amt * 10);

endfunction

//-----------------------------------------------------------------------------

function SetStrengthModBaseValue(who, Amt)

  if(SetAttributeTemporaryMod(who, ATTRIBUTEID_STRENGTH, Amt))
    RecalcVitals(who);
    return 1;
  endif
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function SetDexterityMod(who, Amt)

  return SetDexterityModBaseValue(who, Amt * 10);

endfunction

//-----------------------------------------------------------------------------

function SetDexterityModBaseValue(who, Amt)

  if(SetAttributeTemporaryMod(who, ATTRIBUTEID_DEXTERITY, Amt))
    RecalcVitals(who);
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetIntelligenceMod(who, Amt)

  return SetIntelligenceModBaseValue(who, Amt * 10);
  
endfunction

//-----------------------------------------------------------------------------

function SetIntelligenceModBaseValue(who, Amt)

  if(SetAttributeTemporaryMod(who, ATTRIBUTEID_INTELLIGENCE, Amt))
    RecalcVitals(who);
    return 1;
  endif
  return 0;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Base Set Funcs
//--------------------------------------------------------------------------------
//================================================================================

function SetBaseStrength(who, Str)

  return SetBaseStrengthBaseValue(who,Str * 10);
  
endfunction

//-----------------------------------------------------------------------------

function SetBaseStrengthBaseValue(who, Str)

  if(Str < 0)
    Str := 0;
  endif

  if(who.isa(POLCLASS_NPC))
    return SetAttributeBaseValue(who,ATTRIBUTEID_STRENGTH,Str);
  elseif(who.isa(POLCLASS_MOBILE))
    return SendAttrEvent(who,EVID_ST_SET,ATTRIBUTEID_STRENGTH,Str);
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetBaseDexterity(who, Dex)

  return SetBaseDexterityBaseValue(who, Dex * 10);

endfunction

//-----------------------------------------------------------------------------

function SetBaseDexterityBaseValue(who, Dex)

  if(Dex < 0)
    Dex := 0;
  endif

  if(who.isa(POLCLASS_NPC))
    return SetAttributeBaseValue(who,ATTRIBUTEID_DEXTERITY,Dex);
  elseif(who.isa(POLCLASS_MOBILE))
    return SendAttrEvent(who,EVID_ST_SET,ATTRIBUTEID_DEXTERITY,Dex);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetBaseIntelligence(who, Int)

  return SetBaseIntelligenceBaseValue(who, Int * 10);

endfunction

//-----------------------------------------------------------------------------

function SetBaseIntelligenceBaseValue(who, Int)

  if(Int < 0)
    Int := 0;
  endif

  if(who.isa(POLCLASS_NPC))
    return SetAttributeBaseValue(who,ATTRIBUTEID_INTELLIGENCE,Int);
  elseif(who.isa(POLCLASS_MOBILE))
    return SendAttrEvent(who,EVID_ST_SET,ATTRIBUTEID_INTELLIGENCE,Int);
  endif
  
  return 0;
  
endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Setne w statystykach 

//--------------------------------------------------------------------------------
//================================================================================

function GetBaseStrengthHundrethValue(who)

  return GetAttributeHundrethValue(who, ATTRIBUTEID_STRENGTH);

endfunction

//-----------------------------------------------------------------------------

function GetBaseDexterityHundrethValue(who)

  return GetAttributeHundrethValue(who, ATTRIBUTEID_DEXTERITY);

endfunction

//-----------------------------------------------------------------------------

function GetBaseIntelligenceHundrethValue(who)

  return GetAttributeHundrethValue(who, ATTRIBUTEID_INTELLIGENCE);

endfunction

//-----------------------------------------------------------------------------
