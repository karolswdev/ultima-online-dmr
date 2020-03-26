//-----------------------------------------------------------------------------

include "include/tempmods";
include ":magic:magicatt";
include "include/poison";
include "include/checks/objchecks";
include "include/checks/locchecks";
include "include/objtypes/skills";
include "include/reputation/murderer";
include "include/reputation/pvparea";
include "include/player/young";
include "include/attack";

//-----------------------------------------------------------------------------

const YELLOW_POTION_EFFECT_PID         := "#YellowPotionEffectPid";
const RED_POTION_EFFECT_PID            := "#RedPotionEffectPid";
const STANDARD_POTION_MOD_DUR          := 900;
const NO_SLEEP                         := 1;
const NO_EMPTY                         := 1;
const NO_MSG                           := 2;
const ALCH_ADD                         := 3;
const DRINK_TIME                       := 15;
const KENSAI_DRINK_TIME                := 5;
const NO_EMPTY_BOTTLE_SOUND            := 1;
const NOPARA_TIME                      := 5;

const UOBJ_THRBOTTLE                   := 0xDCFF;

const POTQUAL_MATURE                   := 1;
const POTQUAL_ROTTEN                   := 2;

//-----------------------------------------------------------------------------

function ThrowPotion(who,Mob,potion, NoSleeping := 0)

  if(!StandardItemCheck(who,potion,NO_COMBAT_CHECK))
    return 0;
  endif
  
  if(ChNocZone(who))
    SendSysMessage(who,"Cos Ci przeszkadza...",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  
  var Graphic := potion.graphic;
  var Name    := potion.name;
  
  if(!SubtractAmount(potion,1))
    return 0;
  endif
  
  PlayMovEffXYZ( who.x, who.y, who.z+15, Mob.x, Mob.y, Mob.z, Graphic, 8, 0, 0, who.realm );
  
  if(IsPlayer(who))
    Detach();
    NoLongerBusy(who);
  endif

  var ThrPotion := CreateItemAtLoc(Mob.x,Mob.y,Mob.z,UOBJ_THRBOTTLE,1,Mob.realm);
  if(!ThrPotion)
    return 0;
  endif

  ThrPotion.graphic   := Graphic;
  ThrPotion.name      := Name;
  ThrPotion.invisible := 0;
  
  if(!NoSleeping)
    PrintTextAbove(ThrPotion,"*3*");
    sleep(1);
    PrintTextAbove(ThrPotion,"*2*");
    sleep(1);
    PrintTextAbove(ThrPotion,"*1*");
    sleep(1);
  elseif(NoSleeping < 0)
    PrintTextAbove(ThrPotion,"*3*");
    sleepms(500);
    PrintTextAbove(ThrPotion,"*2*");
    sleepms(500);
    PrintTextAbove(ThrPotion,"*1*");
    sleepms(500);
  endif

  sleepms(200);
  DestroyItem(ThrPotion);

  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function CanDrink(who,potion, pDelay := 0)

  if(GetObjProperty(who,"#Drinking"+potion.graphic) and GetObjProperty(who,"#Drinking"+potion.graphic) > ReadGameClock())
    SendSysMessage(who,"Musisz chwile poczekac zanim bedziesz mogl"+ggm(who,4)+" wypic nastepna miksture.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(GetObjProperty(who,"#DrinkingOverall") and GetObjProperty(who,"#DrinkingOverall") > ReadGameClock())
    SendSysMessage(who,"Musisz chwile poczekac zanim bedziesz mogl"+ggm(who,4)+" wypic nastepna miksture.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  
  if(!pDelay)
    if(GetBaseSkill(who, SKILLID_KENSAI) > 24)
      pDelay    := KENSAI_DRINK_TIME;
    else
      pDelay := CInt(DRINK_TIME);
    endif
  endif
  
  var dAllDelay := CInt(DRINK_TIME / 2);

  if(GetBaseSkill(who, SKILLID_KENSAI) > 24)
    dAllDelay := KENSAI_DRINK_TIME;
  endif

  SetObjProperty(who,"#Drinking"+potion.graphic,ReadGameClock() + pDelay);
  SetObjProperty(who,"#DrinkingOverall",ReadGameClock() + dAllDelay);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function EmptyBottle(who,potion, NoSound := 0)

  var AlchemyCfg := ReadConfigFile(":alchemy:alchemy");
  var EmptyObj   := CInt(AlchemyCfg[Potion.objtype].EmptyBottle);
  var PLvl       := GetItemPoisonLvl(Potion);

  if(!EmptyObj)
    EmptyObj := UOBJ_EMPTY_BOTTLE;
  endif

  if(!SubtractAmount(Potion,1))
    return 0;
  endif

  if(PLvl)
    SetPoison(who,PLvl,0,"Ta mikstura byla zatruta!");
  endif

  if(!NoSound)
    PlaySoundEffect(who,SOUND_EFFECT_DRINK);
  endif

  CreateItemInPack(who,EmptyObj,1);
  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function GetPotionStrength(PotionObj)

  var AlchemyCfg := ReadConfigFile(":alchemy:alchemy");
  return Cint(AlchemyCfg[PotionObj].Strength);

endfunction

//-----------------------------------------------------------------------------

function GetPotionDiff(PotionObj)

  var AlchemyCfg := ReadConfigFile(":alchemy:alchemy");
  return Cint(AlchemyCfg[PotionObj].skill);

endfunction

//-----------------------------------------------------------------------------

function GetPotionName(PotionObj)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  return ClStr(ItemCfg[PotionObj].sName);

endfunction

//-----------------------------------------------------------------------------

function IsPotion(ObjType)

  var AlchemyCfg := ReadConfigFile(":alchemy:alchemy");
  if(CInt(AlchemyCfg[ObjType].skill))
    return 1;
  endif
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function GetEmptyBottleObjType(ObjType)

  var AlchemyCfg := ReadConfigFile(":alchemy:alchemy");
  var EmptyObj   := CInt(AlchemyCfg[ObjType].EmptyBottle);

  if(!EmptyObj)
    EmptyObj := UOBJ_EMPTY_BOTTLE;
  endif

  return EmptyObj;

endfunction

//-----------------------------------------------------------------------------

function FindEmptyBottle(who,ObjType)

  var EmptyObj := GetEmptyBottleObjType(ObjType);

  foreach item in EnumerateItemsInContainer(who.backpack)
    if(item.container.objtype != UOBJ_POTION_KEG and item.objtype == EmptyObj)
      return item;
    endif
  endforeach
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function ThrowExplosionPotion(who,Targ,Potion)

  var Strength := GetPotionStrength(potion.objtype);
  var ExpMod   := GetExpPotionMod(Potion);
  var QualMod  := GetPotionQualMod(Potion, 0.2);
  var Mx       := Targ.x;
  var My       := Targ.y;
  var Mz       := Targ.z;
  var MRlm     := who.realm;
  var Range    := CInt(3 * GetPotionQualMod(Potion));
  var PotObj   := potion.objtype;

  if(!ThrowPotion(who,Targ,Potion))
    return;
  endif

  Strength := CInt(Strength * QualMod);
  Strength := Strength + RandomInt(Cint(Strength / 2)) + CInt(CInt(ToMax(ExpMod,10) * 0.60) * (CInt(Strength / 2) * 0.1)) + RandomInt(10);
  if(ExpMod)
    Strength := Strength + CInt( ( ExpMod - 10 ) / 2 ) + 2;
  endif

  var Circle := 1 + (Strength / 3);
  if(Circle > 10)
    Circle := 10;
  endif

  if(IsHolyPotion(PotObj))
    PlayPcStEffect(Mx, My, Mz, MRlm, FX_EXPLODE_3, 1283, 7, 0x10);
  else
    PlayStationaryEff(Mx, My, Mz, FX_EXPLODE_3, 7, 0x10, 0, MRlm);
  endif

  var DispStrength := CInt(Strength*3.5);

  foreach item in ListItemsNearLocation(mx, my, mz, 1, mRlm)
    if((item.objtype == UOBJ_STONEFIELD_EW or item.objtype == UOBJ_STONEFIELD_NS) and GetObjProperty(item, PROP_SUMMONED_FIELD) and !CInt(GetObjProperty(item,"FireMoon")))
      PlayPcStEffect(Item.x, Item.y, Item.z+1, Item.realm, FX_EXPLODE_3, 0, 0x10, 0x10);
      var Hp := GetDispelHp(item);
      if(Hp <= DispStrength)
        DestroyItem(Item);
      else
        SetDispelHp(Item, Hp-DispStrength);
      endif
    endif
  endforeach

  foreach Mob in ListMobilesNearLocationEx(Mx, My, Mz, Range, LISTEX_FLAG_NORMAL + LISTEX_FLAG_HIDDEN, MRlm)

    if(who.isa(POLCLASS_NPC))
      if(IsTamed(Mob))
        if(IsNpcSummoned(Mob))
          continue;
        endif
      elseif(Mob.isa(POLCLASS_NPC) and Mob != Targ)
        continue;
      endif
    elseif(Mob.isa(POLCLASS_NPC) and Mob.frozen and Mob.hidden)
      continue;
    endif

    if(IsOrder(Mob) and IsHolyPotion(PotObj) and Mob != who)
      continue;
    endif

    if(!CanVsAttack(who, Mob))
      continue;
    endif

    if(!CheckLosAt(Mob, Mx, My, Mz))
      continue;
    endif

    if(IsHolyPotion(PotObj))
      PlayPcEffect(Mob, FX_EXPLODE_3, 1283, 7, 0x10);
    else
      PlayObjectCenteredEffect(Mob, FX_EXPLODE_3, 7, 0x10);
    endif

    PlaySoundEffect(Mob, SOUND_EFFECT_EXPLOSION);

    if(Mob.paralyzed)
      SetModDelay(Mob, MOD_PARA, NOPARA_TIME);
    endif

    AddMurdReport(Mob, who);
    DoMagicDamage(Mob, who, Strength, Circle, DMGTYPE_FIRE, SPTYPE_NONE, NO_RESIST_GAIN);

  endforeach
  
endfunction

//-----------------------------------------------------------------------------

function SubPotion(potion,Amt,Opt)

  if(Opt == ALCH_ADD)
    return 1;
  elseif(potion.isa(POLCLASS_ITEM))
    if(SubtractAmount(potion,Amt))
      return 1;
    endif
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetExpPotionMod(Potion)

  return CInt(GetObjProperty(Potion,"ExpMod"));

endfunction

//-----------------------------------------------------------------------------

function GetPotionQuality(Potion)

  return CInt(GetObjProperty(Potion,"Quality"));

endfunction

//-----------------------------------------------------------------------------

function CalcPotStrength(Strength,ExpMod, rChance := 0)

  var Amt := CInt(Strength);

  if(rChance)
    var Mod := CInt( CInt(Strength / 2) * (rChance * 0.01) );
    Amt := Amt + Mod + RandomInt(CInt(Strength / 2) - Mod);
  else
    Amt := Amt + CInt(RandomInt(Strength) / 2);
  endif
  
  Amt := Amt + CInt( CInt(ExpMod * 0.60) * (CInt(Strength / 2) * 0.1) );

  return Amt;

endfunction

//-----------------------------------------------------------------------------

function GetPotionQualMod(Potion, Mod := 1.0)

  var Qual := GetPotionQuality(Potion);
  var QualMod := 1.0;

  case(Qual)
    POTQUAL_MATURE:   QualMod := 1.5;
    POTQUAL_ROTTEN:   QualMod := 0.5;
  endcase

  if(Mod != 1.0 and QualMod != 1.0)
    QualMod := 1.0 + (QualMod - 1.0) * Mod;
  endif

  return QualMod;

endfunction

//-----------------------------------------------------------------------------

function IsHolyPotion(PotObj)

  if(PotObj == UOBJ_LESSER_HOLY_POTION or PotObj == UOBJ_HOLY_POTION or PotObj == UOBJ_GREATER_HOLY_POTION)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetStatRandomStrengthMod(who)

  var rMax := ToMax(CInt(((GetBaseStrength(who) + GetBaseDexterity(who)) - 80) / 20), 6);

  return rMax + RandomInt(7 - rMax);

endfunction

//-----------------------------------------------------------------------------

function GetDispelHp(Item)

  var Hp := CInt(GetObjProperty(item, "DispelHp"));

  if(Hp)
    return Hp;
  else
    return 100;
  endif

endfunction

//-----------------------------------------------------------------------------

function SetDispelHp(Item, Hp)

  SetObjProperty(item, "DispelHp", Hp);

endfunction

//-----------------------------------------------------------------------------

function IsOiled(Item)

  if(GetObjProperty(item,"Oiled"))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
