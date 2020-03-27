//================================================================================
//--------------------------------------------------------------------------------
// Vitals
//--------------------------------------------------------------------------------
//================================================================================

function GetVitalIds()

  return { VITALID_LIFE, VITALID_MANA, VITALID_STAMINA };

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Hit Points
//--------------------------------------------------------------------------------
//================================================================================

function GetVitalById(who, VitalId)

  if(who.isa(POLCLASS_MOBILE))
    return Cint(GetVital(who, VitalId)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetMaxVitalById(who, VitalId)

  if(who.isa(POLCLASS_MOBILE))
    return Cint(GetVitalMaximumValue(who, VitalId)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetVitalPercent(who, VitalId)

  return CDbl(GetVitalById(who, VitalId)) / CDbl(GetMaxVitalById(who, VitalId));

endfunction

//--------------------------------------------------------------------------------

function SetVitalById(who, VitalId, Amt)

  if(who.isa(POLCLASS_MOBILE))
    return SetVital(who, VitalId, Cint(Amt*100));
  endif

endfunction

//--------------------------------------------------------------------------------

function SetMaxVitalById(who, VitalId, Amt)

  if(who.isa(POLCLASS_MOBILE))
    if(CInt(Amt))
      SetObjProperty(who,VitalId,CInt(Amt));
    endif

    return RecalcVitals(who);
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function HealVitalById(who, VitalId, Amt)

  if(who.isa(POLCLASS_MOBILE))
    case(VitalId)
      VITALID_LIFE:
        return HealDamage(who, Amt);
      
      default:
        return SetVitalById(who, VitalId, GetVitalById(who, VitalId) + Amt);

    endcase
  endif

  return 0;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Hit Points
//--------------------------------------------------------------------------------
//================================================================================

function GetHp(who)

  if(who.isa(POLCLASS_MOBILE))
    return Cint(GetVital(who, VITALID_LIFE)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetMaxHp(who)

  if(who.isa(POLCLASS_MOBILE))
    return Cint(GetVitalMaximumValue(who, VITALID_LIFE)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetHpPercent(who)

  return CDbl(GetHp(who)) / CDbl(GetMaxHp(who));

endfunction

//--------------------------------------------------------------------------------

function SetHp(who, hp)

  if(who.isa(POLCLASS_MOBILE))
    return SetVital(who, VITALID_LIFE, Cint(hp*100));
  endif

endfunction

//--------------------------------------------------------------------------------

function SetMaxHp(who,Amt)

  if(who.isa(POLCLASS_MOBILE))
    if(CInt(Amt))
      SetObjProperty(who,VITALID_LIFE,CInt(Amt));
    endif

    return RecalcVitals(who);
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetHpRegenRate(who)

  if(who.isa(POLCLASS_MOBILE))
    return GetVitalRegenRate(who, VITALID_LIFE);
  else
    return 0;
  endif

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Mana
//--------------------------------------------------------------------------------
//================================================================================

function GetMana(who)

  if(who.isa(POLCLASS_MOBILE))
    return CInt(GetVital(who, VITALID_MANA)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetManaPercent(who)

  return CDbl(GetMana(who)) / CDbl(GetMaxMana(who));

endfunction

//--------------------------------------------------------------------------------

function GetMaxMana(who)

  if(who.isa(POLCLASS_MOBILE))
    return Cint(GetVitalMaximumValue(who, VITALID_MANA)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function SetMana(who, amount)

  if(who.isa(POLCLASS_MOBILE))
    SetVital(who, VITALID_MANA, cint(amount*100.0));
  endif

endfunction

//--------------------------------------------------------------------------------

function SetMaxMana(who,Amt)

  if(who.isa(POLCLASS_MOBILE))
    if(CInt(Amt))
      SetObjProperty(who,VITALID_MANA,CInt(Amt));
    endif
    return RecalcVitals(who);
  endif

  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function ConsumeManaAmount(Caster, Amt)

  if(GetMana(Caster) < Amt)
    return 0;
  else
    SetMana(Caster, GetMana(caster) - Amt);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetManaRegenRate(who)

  if(who.isa(POLCLASS_MOBILE))
    return GetVitalRegenRate(who, VITALID_MANA);
  else
    return 0;
  endif

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Stamina
//--------------------------------------------------------------------------------
//================================================================================

function GetStamina(who)

  if(who.isa(POLCLASS_MOBILE))
    return Cint(GetVital(who, VITALID_STAMINA)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetMaxStamina(who)

  if(who.isa(POLCLASS_MOBILE))
    return Cint(GetVitalMaximumValue(who, VITALID_STAMINA)/ 100);
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function SetStamina(who, stamina)

  if(who.isa(POLCLASS_MOBILE))
    return SetVital(who, VITALID_STAMINA, Cint(stamina*100));
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function SetMaxStamina(who,Amt)

  if(who.isa(POLCLASS_MOBILE))
    if(CInt(Amt))
      SetObjProperty(who,VITALID_STAMINA,CInt(Amt));
    endif
    return RecalcVitals(who);
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function ConsumeStamina(Caster, Amt)

  if(GetStamina(Caster) < Amt)
    return 0;
  else
    SetStamina(Caster, GetStamina(caster) - Amt);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------
