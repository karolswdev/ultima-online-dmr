//-----------------------------------------------------------------------------

include "../pkg/systems/combat/combat";

//-----------------------------------------------------------------------------
// Poison delay types
//-----------------------------------------------------------------------------

const PDTYPE_NORMAL             := 1;
const PDTYPE_MAGIC              := 2;

const MAGIC_NPC_POISON_DELAY    := 30;
const MAGIC_PLAYER_POISON_DELAY := 1;

const POTION_CURE_POISON_DELAY  := 5;

//-----------------------------------------------------------------------------

function SetPoison(who, plvl, Poisoner := 0, msg := "")

  plvl := CInt(plvl);
  var cplvl := GetPoisonLevel(who);
  
  if(IsInvulToAttack(who))
    return 0;
  endif

  if(cplvl and cplvl < plvl)
    IncreasePoison(who,plvl,Poisoner);
    return 1;
  elseif(cplvl)
    return 0;
  endif
  var parms := {};
  parms[1] := who;
  parms[2] := plvl;
  parms[3] := msg;
  parms[4] := CInt(Poisoner.serial);
  start_script (":magiceffects:poisondamage", parms);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CurePoison (who, sound := 0, msg := "")

  EraseObjProperty (who, "#poisoninfo");
  who.SetPoisoned(0);
  if (sound)
    PlaySoundEffect (who, sound);
  endif

  if (msg)
    PrintTextAbovePrivate (who, msg, who);
  endif

endfunction

//-----------------------------------------------------------------------------

function DecreasePoison (who, nplvl, sound := 0, msg := "")

  var poisoned := GetObjProperty(who, "#poisoninfo");

  if(!poisoned or poisoned[1] <= nplvl)
    return 0;
  endif

  if(nplvl < 1)
    CurePoison (who, sound, msg);
    return 1;
  else
    SetPoisonLevel(who, nplvl);
  endif

  if(sound)
    PlaySoundEffect (who, sound);
  endif

  if (msg)
    PrintTextAbovePrivate (who, msg, who);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IncreasePoison (who, nplvl, Poisoner, sound := 0, msg := "")

  var poisoned := GetObjProperty(who, "#poisoninfo");

  if(!poisoned or nplvl <= poisoned[1])
    return 0;
  endif

  if(nplvl > 10)
    nplvl := 10;
  endif

  SetPoisonLevel(who, nplvl, Poisoner);

  if(sound)
    PlaySoundEffect (who, sound);
  endif

  if (msg)
    PrintTextAbovePrivate (who, msg, who);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetPoisonLevel(who)

  var poisoned := GetObjProperty (who, "#poisoninfo");
  if (!poisoned)
    return 0;
  endif

  return Cint(poisoned[1]);

endfunction

//-----------------------------------------------------------------------------

function SetItemProtPoison(item, plvl, charges, who)

  if(GetItemProtPoisonLvl(Item))
    return 0;
  endif

  SetName(item,item.desc + " ochrony przed trucizna");
  var ProtPoison := {};
  ProtPoison[1] := plvl;
  ProtPoison[2] := charges;
  ProtPoison[3] := charges;
  ProtPoison[4] := who.serial;
  SetObjProperty(item,"protpoison",ProtPoison);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetItemProtPoisonLvl(Item)

  return CInt(GetObjProperty(item,"protpoison")[1]);

endfunction

//-----------------------------------------------------------------------------

function GetItemProtPoisonCharges(Item)

  return CInt(GetObjProperty(item,"protpoison")[2]);

endfunction

//-----------------------------------------------------------------------------

function GetItemProtPoisonMaxCharges(Item)

  return CInt(GetObjProperty(item,"protpoison")[3]);

endfunction

//-----------------------------------------------------------------------------

function GetItemProtPoisoner(Item)

  return FindPlayerBySerial(GetObjProperty(item,"protpoison")[4]);

endfunction

//-----------------------------------------------------------------------------

function SetItemPoisoned(who,item,plvl,charges)

  SetObjProperty(item,"PoisonLvl",plvl);
  SetObjProperty(item,"PoisonCharges",charges);
  SetObjProperty(item,"PoisonMaxCharges",charges);
  SetObjProperty(item,"Poisoner",who.serial);

  if(item.isa(POLCLASS_WEAPON))
    if(AddNewOnHitScript(item,POISON_HIT_SCRIPT))
      return 1;
    else
      return 0;
    endif
  else
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetItemPoisonLvl(item)

  return Cint(GetObjProperty(item,"PoisonLvl"));

endfunction

//-----------------------------------------------------------------------------

function GetItemPoisonCharges(item)

  return Cint(GetObjProperty(item,"PoisonCharges"));

endfunction

//-----------------------------------------------------------------------------

function GetItemPoisonMaxCharges(item)

  return Cint(GetObjProperty(item,"PoisonMaxCharges"));

endfunction

//-----------------------------------------------------------------------------

function GetItemPoisoner(Item)

  return FindPlayerBySerial(GetObjProperty(item,"Poisoner"));

endfunction

//-----------------------------------------------------------------------------

function CheckItemPoisonCharges(item)

  var Charges := GetItemPoisonCharges(item);
  Charges := Charges - 1;
  if(!Charges)
	var PoisonLvl := GetItemPoisonLvl(item);
	if(PoisonLvl > 2)
		if(RandomDiceRoll("1d12") == 1)
			SetObjProperty(item,"PoisonLvl", PoisonLvl-2);
		else
			SetObjProperty(item,"PoisonLvl", PoisonLvl-1);		
		endif
		SetObjProperty(item,"PoisonCharges", GetItemPoisonMaxCharges(item));
		return 1;
	else
		EraseItemPoison(item);
		return 0;
	endif
  endif
  SetObjProperty(item,"PoisonCharges",Charges);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetPoisonChargesDesc(item)
  
  var Ch       := GetItemPoisonCharges(item);
  var MCh      := GetItemPoisonMaxCharges(item);
  var Perc     := CInt((Ch * 10) / MCh);

  case(perc)
    1:
    2:    return "posiada sladowe ilosci trucizny";
    3:    
    4:    return "w niewielkiej czesci pokryty jest trucizna";
    5:
    6:    return "w polowie pokryty jest trucizna";
    7:
    8:    return "w wiekszosci pokryty jest trucizna";
    9:
    10:   return "w calosci pokryty jest trucizna";
    default: return "";
  endcase

endfunction

//-----------------------------------------------------------------------------

function EraseItemPoison(item)

  EraseObjProperty(item,"PoisonLvl");
  EraseObjProperty(item,"PoisonCharges");
  EraseObjProperty(item,"PoisonMaxCharges");
  EraseObjProperty(item,"Poisoner");

  if(item.isa(POLCLASS_WEAPON))
    if(EraseOnHitScript(item,POISON_HIT_SCRIPT))
      return 1;
    else
      return 0;
    endif
  else
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetPoisonPotionDesc(PLvl)

  var PObj     := 0;
  var Name     := "trucizna";

  foreach Pot in GetPoisonPotions()
    if(GetPotionStrength(Pot) == PLvl)
      Name := "";
      PObj := Pot;
      break;
    elseif(GetPotionStrength(Pot) == PLvl - 1)
      Name := "wyjatkowa ";
      PObj := Pot;
      break;
    endif
  endforeach

  if(PObj)
    Name := Name + GetPotionName(PObj);
  endif

  return Name;

endfunction

//-----------------------------------------------------------------------------

function PotionDecreasePoison(who, nplvl)

  if(DecreasePoison(who, nplvl))
    SetPoisonDelay(who, PDTYPE_NORMAL, POTION_CURE_POISON_DELAY);
    return 1;
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function SetPoisonLevel(who, nplvl, Poisoner := 0)

  var poisoned := GetObjProperty(who, "#poisoninfo");

  if(!poisoned)
    return 0;
  endif

  var endtime := poisoned[2];
  if(nplvl > poisoned[1])
    endtime := ReadGameClock() + 10 + (nplvl * 3);
    poisoned[2] := endtime;
  endif

  poisoned[1] := nplvl;

  if(Poisoner)
    poisoned[4] := Poisoner.serial;
  endif

  var poison_damage := GetPoisonDamagePerTick(poisoned[1]);
  SetObjProperty(who,"#poisoninfo",poisoned);
  SendDelBuff(who, BFICON_POISON);
  SendAddBuff(who, BFICON_POISON, endtime - ReadGameClock(), 1017383, 1075633, array(poison_damage, 7));

endfunction

//-----------------------------------------------------------------------------

function GetPoisonDamagePerTick (plvl)

  plvl := MinMax(plvl, 1, 10);

  return CInt(plvl * 1.5);

endfunction

//-----------------------------------------------------------------------------

function CalcPoisonLevel(who, skMageId := SKILLID_MAGERY, skEvalId := SKILLID_EVALINT)

  var CasterMagery    := GetEffectiveSkill(who, skMageId);
  var CasterEi        := GetEffectiveSkill(who, skEvalId);

  return CInt(CasterMagery / 33 + CalcSkillExponential(who, SKILLID_POISONING, 3) + CasterEi / 100);

endfunction 

//-----------------------------------------------------------------------------

function SetPoisonDelay(who, Type, Seconds, bywho := 0)

  if(Type == PDTYPE_NORMAL)
    
    if(CInt(GetObjProperty(who, "#PoisonDelay")) > ReadGameClock())
      return 0;
    endif

    SetObjProperty(who, "#PoisonDelay", ReadGameClock() + Seconds);

  elseif(Type == PDTYPE_MAGIC)

    // normalny obowiazuje kazdego
    if(CInt(GetObjProperty(who, "#PoisonDelay")) > ReadGameClock())
      return 0;
    endif

    // magiczny delay obowiazuje tylko osoby, ktore maja < 90 poisona
    if(CInt(GetObjProperty(who, "#MagicPoisonDelay")) > ReadGameClock())
      if(GetBaseSkill(bywho, SKILLID_POISONING) < 90) // jak bywho bedzie 0, to zwroci 0, czyli ok.
        return 0;
      endif
    endif

    SetObjProperty(who, "#MagicPoisonDelay", ReadGameClock() + Seconds);

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function IsItemPoisoned(item)
  
  if (GetItemPoisonLvl(item) > 0 )
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------
