//-----------------------------------------------------------------------------

include "../potion";

//-----------------------------------------------------------------------------

const MAX_KEG_POTIONS                  := 100;
const KEG_LOCK_DUR                     := 345600;
const KEG_LOCK_VAX_AMT                 := 5;
const KEG_NORMAL_COLOR                 := 0;
const KEG_LOCK_COLOR                   := 991;
const KEG_MATURE_COLOR                 := 1132;
const KEG_ROTTEN_COLOR                 := 1118;

//-----------------------------------------------------------------------------

function GetPotionsInKeg(keg)

  return CInt(GetObjProperty(keg,"PotionsAmount"));

endfunction

//-----------------------------------------------------------------------------

function GetKegType(keg)

  return CInt(GetObjProperty(Keg,"PotionType"));

endfunction

//-----------------------------------------------------------------------------

function ReducePotionsInKeg(keg,amt)

  amt := CInt(amt);
  if(!amt or amt < 0)
    return 0;
  endif

  if(CInt(GetObjProperty(keg,"PotionsAmount")) - amt < 0)
    return 0;
  endif

  SetObjProperty(keg,"PotionsAmount",CInt(GetObjProperty(keg,"PotionsAmount")) - amt);
  RenameKeg(keg);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function AddPotionsToKeg(keg,amt)

  amt := CInt(amt);
  
  if(!amt or amt < 0)
    return 0;
  endif

  if(CInt(GetObjProperty(keg,"PotionsAmount")) + amt > MAX_KEG_POTIONS)
    return 0;
  endif

  SetObjProperty(keg,"PotionsAmount",CInt(GetObjProperty(keg,"PotionsAmount")) + amt);
  RenameKeg(keg);
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function AddPotionToKeg(who,keg,potion,NoEmpty := 0)

  var Obj  := CInt(potion.objtype);

  if(NoEmpty == ALCH_ADD)
    Obj := potion;
  endif

  if(GetExpPotionMod(Potion))
    if(NoEmpty != NO_MSG)
      SendSysMessage(who,"Nie mozesz przelac wyjatkowej mikstury do kega.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  elseif(GetItemPoisonLvl(Potion))
    if(NoEmpty != NO_MSG)
      SendSysMessage(who,"Nie mozesz przelac tej mikstury do kega.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  elseif(!IsPotion(Obj))
    if(NoEmpty != NO_MSG)
      SendSysMessage(who, "Nie mozesz tego wlac do kega.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  var AlchemyCfg := ReadConfigFile(":alchemy:alchemy");
  var EmptyObj   := CInt(AlchemyCfg[Obj].EmptyBottle);

  if(!EmptyObj)
    EmptyObj := UOBJ_EMPTY_BOTTLE;
  endif

  var type := GetObjProperty(keg, "PotionType");
  var amt  := GetPotionsInKeg(keg);
  var aAmt := 1;

  if(Potion.isa(POLCLASS_ITEM))
    aAmt := Potion.amount;
  endif

  if(!amt)
    SetObjProperty(keg, "PotionType", CInt(Obj));
    type := CInt(Obj);
    SetObjProperty(keg, "OwnerSerial", who.serial);
  endif
  
  if(amt >= MAX_KEG_POTIONS)
    if(NoEmpty != NO_MSG)
      SendSysMessage(who, "Keg nie utrzyma juz wiecej.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif
  
  if(aAmt + amt > MAX_KEG_POTIONS)
    aAmt := MAX_KEG_POTIONS - amt;
  endif

  if(type != Obj)
    if(NoEmpty != NO_MSG)
      SendSysMessage(who, "Chyba nie myslisz ze dobrym pomyslem byloby mieszac te dwie mikstury.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  elseif(AddPotionsToKeg(keg,aAmt))
    if(SubPotion(potion,aAmt,NoEmpty))
      if(NoEmpty != NO_EMPTY and NoEmpty != ALCH_ADD)
        CreateItemInPack(who,  EmptyObj, aAmt);
      endif
      PlaySoundEffect(who, SOUND_EFFECT_POUR);
      if(NoEmpty != NO_MSG and NoEmpty != ALCH_ADD)
        SendSysMessage(who, "Oproznil"+ggm(who,3)+" zawartosc butli do kega.",FONT_NORMAL,COLOR_GREEN);
      endif
    else
      ReducePotionsInKeg(keg,aAmt);
      return 0;
    endif
  else
    if(NoEmpty != NO_MSG)
      SendSysMessage(who, "Keg nie utrzyma juz wiecej.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetFullness( pkeg )
  var amt := GetPotionsInKeg(pkeg);
  var str;
  if (amt == 0)
    str := "keg jest pusty";
  elseif ((amt >= 1) && (amt <= 4))
    str := "keg jest prawie pusty";
  elseif ((amt >= 5) && (amt <= 19))
    str := "keg jest troszke zapelniony";
  elseif ((amt >= 20) && (amt <= 29))
    str := "keg jest troche zapelniony";
  elseif ((amt >= 30) && (amt <= 39))
    str := "keg jest juz prawie zapelniony do polowy";
  elseif ((amt >= 40) && (amt <= 46))
    str := "keg jest zapelniony prawie do polowy";
  elseif ((amt >= 47) && (amt <= 53))
    str := "keg jest zapelniony do polowy";
  elseif ((amt >= 54) && (amt <= 69))
    str := "keg jest juz prawie zapelniony";
  elseif ((amt >= 70) && (amt <= 79))
    str := "keg jest prawie zapelniony";
  elseif ((amt >= 80) && (amt <= 95))
    str := "keg jest zapelniony";
  elseif ((amt >= 96) && (amt <= 99))
    str := "w kegu juz prawie sie nic nie zmiesci";
  else
    str := "w kegu nie zmiesci sie juz nic wiecej";
  endif
  return str;
endfunction

//-----------------------------------------------------------------------------

function RenameKeg(keg)

  if(!keg)
    return 0;
  endif

  var Obj  := GetKegType(keg);
  var amt  := GetPotionsInKeg(keg);
  var desc := GetDescForItem(obj);
  var str  := "keg";
  var Qual := "";
  
  case(GetPotionQuality(Keg))
    POTQUAL_MATURE: Qual := "Dojrzaly ";
    POTQUAL_ROTTEN: Qual := "Zgnily ";
  endcase

  if(Keg.locked)
    Qual := "Zalakowany ";
  endif

  if(!amt or !obj)
    EraseObjProperty(keg,"PotionType");
  else
    str    := Qual + "keg - " + desc + " , " + GetFullness(keg);
    SetObjProperty(keg, "PotionName", desc);
  endif

  SetName(keg, str);
  
  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function LockPotionKeg(Keg, who)

  if(!Keg.locked and !GetPotionQuality(Keg))
    var Chance := GetEffectiveSkill(who, SKILLID_ALCHEMY);
    Chance := CInt(Chance / 10) + ToMin(Chance - 90, 0) * 4;

    SetObjProperty(Keg, "LockStartTime", ReadGameClock());
    SetObjProperty(Keg, "LockDuration", KEG_LOCK_DUR + RandomInt(CInt(KEG_LOCK_DUR / 3)));
    SetObjProperty(Keg, "LockChance", Chance);
    Keg.color  := KEG_LOCK_COLOR;
    Keg.locked := 1;
    RenameKeg(Keg);
    start_script(":potionkeg:controllockkeg", Keg);
  endif

endfunction

//-----------------------------------------------------------------------------
