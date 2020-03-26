//--------------------------------------------------------------------------------

include "include/items";
include "include/objtypes/magic";
include "../pkg/systems/combat/combat";

include ":magic:magicatt";
include ":magicitems:common";

//--------------------------------------------------------------------------------
//Prefix Mods
//--------------------------------------------------------------------------------

const MAGIC_PREFIX_MOD        := 1;

const MAGICMOD_FIREDMG        := 10;
const MAGICMOD_COLDDMG        := 11;
const MAGICMOD_LIGHTDMG       := 12;
const MAGICMOD_DUR            := 13;
const MAGICMOD_LIFE           := 14;
const MAGICMOD_MANA           := 15;
const MAGICMOD_STAMINA        := 16;
const MAGICMOD_INVIS          := 17;
const MAGICMOD_LIGHT          := 18;
const MAGICMOD_POISONDMG      := 19;

//--------------------------------------------------------------------------------
//Suffix Mods
//--------------------------------------------------------------------------------

const MAGIC_SUFFIX_MOD        := 100;

const MAGICMOD_FIRERESIST     := 101;
const MAGICMOD_COLDRESIST     := 102;
const MAGICMOD_LIGHTRESIST    := 103;
const MAGICMOD_ARMOR          := 104;
const MAGICMOD_DMG            := 105;
const MAGICMOD_STRENGTH       := 106;
const MAGICMOD_DEXTERITY      := 107;
const MAGICMOD_INTELLIGENCE   := 108;

const MAGICMOD_SKILLMOD       := 200;
const MAGICMOD_SK_RAND_ALL    := 400;

// 2015-05-01 - zmniejszone z 5 do 4, z racji zwiekszenia szansy na drop magikow, to beda sie szybciej zuzywac.

const CHARGE_TIME             := 5; // __ECONOMY__ - tutaj trzeba zmniejszac w razie jakby bylo za duzo magikow.
const MAX_MAGIC_LVL           := 20;

//--------------------------------------------------------------------------------

function TryToMakeItemMagic(item, chance, magiclvl);

  if(CInt(chance) > Cint(RandomInt(100)+1))

    var MagicMods     := GetMagicModsType(item);
    MagicLvl := CInt(MagicLvl);

    if(MagicLvl <= 0)
      MagicLvl := 1;
    endif

    if(MagicMods)
      return MakeItemMagic(item,magiclvl,MagicMods);
    endif

  endif
  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function MakeItemMagic(item,magiclvl,MagicMods)

  if(!MagicMods.prefix or !MagicMods.suffix)
    return 0;
  endif
  
  if(GetMagicMods(item))
    return 0;
  endif
  
  var MagicPrefixMod := MagicMods.prefix;
  if(MagicPrefixMod.size())
    MagicPrefixMod     := MagicPrefixMod[(RandomInt(len(MagicPrefixMod))+1)];
  endif

  var MagicSuffixMod := MagicMods.suffix;
  if(MagicSuffixMod.size())
    MagicSuffixMod     := MagicSuffixMod[(RandomInt(len(MagicSuffixMod))+1)];
  endif
  
  if(MagicSuffixMod == MAGICMOD_SK_RAND_ALL)
    var skMaxId := GetMaxSkillId();
    repeat
      MagicSuffixMod := MAGICMOD_SKILLMOD + RandomInt(skMaxId+1);
    until(!GetSkillConfigElem(MagicSuffixMod - MAGICMOD_SKILLMOD).NotVisible and MagicSuffixMod <= MAGICMOD_SKILLMOD + skMaxId);
  endif

  var MagicItemArray := {};
  MagicItemArray.append(MagicPrefixMod);
  MagicItemArray.append(MagicSuffixMod);
  MagicItemArray.append(MagicLvl);

  SetMagicMods(item,MagicItemArray);

  AddMagicName(item);
  
  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetMagicModsType(item)

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);
  var MagicMods := 0;
  
  if(ItemCfg[item.objtype].CanBeMagicWeapon)
    MagicMods := GetMagicWeaponMods(item);
  elseif(ItemCfg[item.objtype].CanBeMagicArmor)
    MagicMods := GetMagicArmorMods(item);
  elseif(ItemCfg[item.objtype].CanBeMagicCloth or ItemCfg[item.objtype].CanBeMagicJews)
    MagicMods := GetMagicClothMods(item);
  endif

  return MagicMods;

endfunction

//--------------------------------------------------------------------------------

function GetMagicWeaponMods(item)

  var Itemcfg := ReadConfigFile(ITEMS_CFG);
  var SkillID := FindSkillidByIdentifier(ItemCfg[item.objtype].Attribute);

  var MagicMods := {};
  var MagicPrefixMods := {};
  var MagicSuffixMods := {};

  MagicPrefixMods.append(MAGICMOD_FIREDMG);
  MagicPrefixMods.append(MAGICMOD_COLDDMG);
  MagicPrefixMods.append(MAGICMOD_LIGHTDMG);
  MagicPrefixMods.append(MAGICMOD_POISONDMG);
  if(Item.hp and item.maxhp)
    MagicPrefixMods.append(MAGICMOD_DUR);
  endif

  MagicSuffixMods.append(MAGICMOD_DMG);
  MagicSuffixMods.append(MAGICMOD_STRENGTH);
  MagicSuffixMods.append(MAGICMOD_DEXTERITY);

  if(SkillID and RandomInt(100) < 50)
    MagicSuffixMods.append(MAGICMOD_SKILLMOD + SkillID);
  endif

  MagicMods.+prefix := MagicPrefixMods;
  MagicMods.+suffix := MagicSuffixMods;
  
  return MagicMods;

endfunction

//--------------------------------------------------------------------------------

function GetMagicArmorMods(item)

  var Itemcfg         := ReadConfigFile(ITEMS_CFG);
  var MagicMods       := {};
  var MagicPrefixMods := {};
  var MagicSuffixMods := {};

  if(Item.hp and item.maxhp)
    MagicPrefixMods.append(MAGICMOD_DUR);
  endif
  MagicPrefixMods.append(MAGICMOD_LIFE);

  if(CInt(ItemCfg[item.objtype].blockcircle) != 1)
    MagicPrefixMods.append(MAGICMOD_MANA);
  endif

  MagicPrefixMods.append(MAGICMOD_STAMINA);

  MagicSuffixMods.append(MAGICMOD_ARMOR);
  MagicSuffixMods.append(MAGICMOD_FIRERESIST);
  MagicSuffixMods.append(MAGICMOD_COLDRESIST);
  MagicSuffixMods.append(MAGICMOD_LIGHTRESIST);
  
  if(RandomInt(100) < 50)
    MagicSuffixMods.append(MAGICMOD_SKILLMOD + SKILLID_PARRY);
  endif

  MagicMods.+prefix := MagicPrefixMods;
  MagicMods.+suffix := MagicSuffixMods;

  return MagicMods;

endfunction

//--------------------------------------------------------------------------------

function GetMagicClothMods(item)

  var MagicMods := {};
  var MagicPrefixMods := {};
  var MagicSuffixMods := {};

  if(Item.hp and item.maxhp)
    MagicPrefixMods.append(MAGICMOD_DUR);
  endif
  MagicPrefixMods.append(MAGICMOD_INVIS);
  MagicPrefixMods.append(MAGICMOD_LIGHT);
  MagicPrefixMods.append(MAGICMOD_LIFE);
  MagicPrefixMods.append(MAGICMOD_MANA);
  MagicPrefixMods.append(MAGICMOD_STAMINA);

  MagicSuffixMods.append(MAGICMOD_FIRERESIST);
  MagicSuffixMods.append(MAGICMOD_COLDRESIST);
  MagicSuffixMods.append(MAGICMOD_LIGHTRESIST);
  MagicSuffixMods.append(MAGICMOD_STRENGTH);
  MagicSuffixMods.append(MAGICMOD_DEXTERITY);
  MagicSuffixMods.append(MAGICMOD_INTELLIGENCE);
  
  if(RandomInt(100) < 50)
    MagicSuffixMods.append(MAGICMOD_SK_RAND_ALL);
  endif

  MagicMods.+prefix := MagicPrefixMods;
  MagicMods.+suffix := MagicSuffixMods;

  return MagicMods;

endfunction

//--------------------------------------------------------------------------------

function CanBeMagicObjType(ObjType)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[ObjType].CanBeMagicWeapon)
    return 1;
  elseif(ItemCfg[ObjType].CanBeMagicArmor)
    return 1;
  elseif(ItemCfg[ObjType].CanBeMagicCloth or ItemCfg[ObjType].CanBeMagicJews)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CanBeMagicItem(item)

  return CanBeMagicObjType(item.objtype);
  
endfunction

//--------------------------------------------------------------------------------

function AddMagicName(item)

  SetName(item,"magiczn"+GewForItem(item.objtype)+" "+GetDescForItem(item.objtype));
  return item;

endfunction

//--------------------------------------------------------------------------------

function EraseMagicName(item)

  var itemdesc := item.desc;
  itemdesc["magiczny "] := "";
  itemdesc["magiczna "] := "";
  itemdesc["magiczne "] := "";
  SetName(item,itemdesc);
  return item;
  
endfunction

//--------------------------------------------------------------------------------

function IdentifyMagicItem(who,item, ItemLevel := 0)
  
  if(IsMagicWand(item))

    if(!IsIdentified(item))
      var ItemCfg   := ReadConfigFIle(ITEMS_CFG);
      var Charges   := CInt((20 + (ItemLevel * 60)) / GetSpellValue(ItemCfg[item.objtype].SpellID,"Circle"));
      var SpellName := GetSpellValue(ItemCfg[item.objtype].SpellID,"Name",STRING_ID);
      if(!SpellName)
        SpellName   := "";
      endif
      SetName(item,GetDescForItem(item.objtype) + " " + SpellName);
      if(!Charges)
        Charges := 1;
      endif
      SetObjProperty(item,"charges",Charges);
      SetObjProperty(item,"Identified",who.serial);
      Item.quality := EXP_QUALITY_MULTIPLIER + 0.1;
    endif

    if(CInt(GetObjProperty(item,"charges")) <= 0)
      SendSysMessage(who,"Ten przedmiot nie ma juz ladunkow.",FONT_NORMAL,COLOR_RED);
    else
      SendSysMessage(who,"Ten przedmiot ma "+GetObjProperty(item,"charges")+" ladunkow.",FONT_NORMAL,COLOR_BLUE);
    endif

    return item;
  endif

  if(!IsMagicItem(item))
    return item;
  endif

  item := EraseMagicName(item);

  var MagicMods         := GetMagicMods(item);
  if(!IsIdentified(item))

    var MagicPrefixMod  := Cint(MagicMods[1]);
    var MagicSuffixMod  := Cint(MagicMods[2]);
    var MagicLvl        := Cint(MagicMods[3]);
    var SkVal           := ItemLevel*20;
    MagicLvl            := MagicLvl + ItemLevel;
    MagicMods[3]        := MagicLvl;
    MagicMods[4]        := Cint(SkVal * 90) + RandomInt(2000) + (Cint(MAX_MAGIC_LVL * 200) - Cint(MagicLvl * 150));
    MagicMods[5]        := Cint(SkVal * 90) + RandomInt(2000) + (Cint(MAX_MAGIC_LVL * 200) - Cint(MagicLvl * 150));
    MagicMods[6]        := MagicMods[4];
    MagicMods[7]        := MagicMods[5];

    SetMagicMods(item,MagicMods);
    item                := ImplicateMagicMods(item,MagicMods);

    var PrefixName      := GetPrefixName(item,MagicPrefixMod);
    var SuffixName      := GetSuffixName(MagicSuffixMod);
    var NewName         := PrefixName + " " + GetDescForItem(item.objtype) + " " + SuffixName;
    var ItemColor       := GetMagicItemModColor(MagicPrefixMod);
  
    SetName(item,NewName);
    item.color := ItemColor;
    SetObjProperty(item,"Identified",who.serial);
  else
    AddQualityDesc(item);
  endif

  if(who.cmdlevel)
    SendChargesMessage(who,MagicMods);
  endif

  return item;
  
endfunction

//--------------------------------------------------------------------------------

function ResetMagicItemName(Item)

  var MagicMods         := GetMagicMods(item);

  if(IsIdentified(item))

    var MagicPrefixMod  := Cint(MagicMods[1]);
    var MagicSuffixMod  := Cint(MagicMods[2]);
    var PrefixName      := GetPrefixName(item,MagicPrefixMod);
    var SuffixName      := GetSuffixName(MagicSuffixMod);
    var NewName         := __IfElse(MagicPrefixMod, PrefixName + " ", "") + GetDescForItem(item.objtype) + __IfElse(MagicSuffixMod, " " + SuffixName, "");
    var ItemColor       := GetMagicItemModColor(MagicPrefixMod);

    SetName(item,NewName);

    if(MagicPrefixMod)
      item.color := ItemColor;
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function AddQualityDesc(item)

  var AddOn := GetQualDesc(GetMagicItemLevel(item),item);
  var ItemD := item.desc;
  if(!ItemD[AddOn])
    SetName(item,AddOn + " " + ItemD);
    return 1;
  endif
  return 0;

endfunction

//--------------------------------------------------------------------------------

function SendDurabilityMessage(who,item,MagicMods)

  if(!Cint(MagicMods[6]))
    MagicMods[6] := MagicMods[4];
    SetMagicMods(item,MagicMods);
  endif

  if(!Cint(MagicMods[7]))
    MagicMods[7] := MagicMods[5];
    SetMagicMods(item,MagicMods);
  endif

  var Percent := CInt(CInt((CInt(MagicMods[4]) * 100) / CInt(MagicMods[6])) / 2) + CInt(CInt((CInt(MagicMods[5]) * 100) / CInt(MagicMods[7])) / 2);
  SendSysMessage(who,GetChargesDesc(Percent),FONT_NORMAL,COLOR_BLUE);
  SendSysMessage(who,"Ten magiczny przedmiot jest "+GetQualDesc(MagicMods[3])+".",FONT_NORMAL,COLOR_BLUE);

endfunction

//--------------------------------------------------------------------------------

function SendMagicItemDesc(who, Item)

  var MagicMods := GetMagicMods(Item);

  SendSysMessage(who,"Ten magiczny przedmiot jest "+GetQualDesc(MagicMods[3])+".",FONT_NORMAL,COLOR_BLUE);

endfunction

//--------------------------------------------------------------------------------

function GetChargesDesc(Perc)

  case(CInt(Perc / 10))
    0:
    1:  return "Ten przedmiot posiada niewielka ilosc mocy.";
    2:  return "Ten przedmiot posiada niewielka ilosc mocy.";
    3:  return "Ten przedmiot stracil wieksza czesc swej mocy.";
    4:  return "Ten przedmiot ma jeszcze troche mocy.";
    5:  return "Ten przedmiot stracil polowe swej mocy.";
    6:  return "Ten przedmiot stracil niemal polowe swej mocy.";
    7:  return "Ten przedmiot posiada jeszcze spora ilosc mocy.";
    8:  return "Ten przedmiot jest lekko wyczerpany z mocy.";
    9:  return "Ten przedmiot jest niemal calkowicie wypelniony moca.";
    10: return "Ten przedmiot jest calkowicie wypelniony moca.";
  endcase

  return "";

endfunction

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------

function GetQualDesc(MLvl,Item := 0)

  var Gew := "y";
  if(Item)
    Gew := GewForItem(item.objtype);
  endif
  
  var msg := "przecietn"+Gew;
  case(MLvl)
    1:
    2:   msg := "slab"+Gew;
    3:
    4:   msg := "przecietn"+Gew;
    5:
    6:   msg := "zwykl"+Gew;
    7:
    8:   msg := "dobr"+Gew;
    9:
    10:  msg := "doskonal"+Gew;
    11:
    12:  msg := "wspanial"+Gew;
    13:
    14:  msg := "wyjatkow"+Gew;
    15:
    16:  msg := "niezwykl"+Gew;
    17:
    18:  msg := "cudown"+Gew;
    19:
    20:   if(Gew == "y")
           Gew := "i";
         elseif(Gew == "e")
           Gew := "ie";
         endif
         msg := "bosk"+Gew;
    21:  msg := "legendarn"+Gew;
    default: msg := "nieznan"+Gew;
  endcase
  return msg;

endfunction

//--------------------------------------------------------------------------------

function SendChargesMessage(who,MagicMods)

  SendSysMessage(who,"P: "+Cint(MagicMods[4])+" PM: "+Cint(MagicMods[6]),FONT_NORMAL,COLOR_GREEN);
  SendSysMessage(who,"S: "+Cint(MagicMods[5])+" SM: "+Cint(MagicMods[7]),FONT_NORMAL,COLOR_GREEN);

endfunction

//--------------------------------------------------------------------------------

function ImplicateMagicMods(item,MagicMods)

  var MagicPrefixMod := MagicMods[1];
  var MagicSuffixMod := MagicMods[2];
  var MagicLvl       := MagicMods[3];
  var iCfg           := ReadConfigFile(ITEMS_CFG);
                         
  case(MagicPrefixMod)
    MAGICMOD_DUR:        SetItemMaxHpMod(item,Cint(MagicLvl * 2));
                         SetItemHp(item,GetItemMaxHp(item));
    MAGICMOD_FIREDMG:
    MAGICMOD_COLDDMG:
    MAGICMOD_POISONDMG:
    MAGICMOD_LIGHTDMG:   if(item.isa(POLCLASS_WEAPON) or iCfg[item.objtype].Gloves)
                           AddNewOnHitScript(item,MAGIC_HIT_SCRIPT);
                         endif
  endcase

  case(MagicSuffixMod)
    MAGICMOD_ARMOR:      item.ar_mod  := Cint(MagicLvl);

    MAGICMOD_DMG:        if(iCfg[item.objtype].Gloves)
                           item.ar_mod  := Cint(MagicLvl / 2);
                         else
                           item.dmg_mod := Cint(MagicLvl / 2);
                         endif
  endcase

  return item;

endfunction

//--------------------------------------------------------------------------------

function GetPrefixName(item,MagicMod)

  var Name := "";
  case(MagicMod)
    MAGICMOD_FIREDMG:    Name := "ognist";
    MAGICMOD_COLDDMG:    Name := "lodow";
    MAGICMOD_LIGHTDMG:   Name := "piorunujac";
    MAGICMOD_POISONDMG:  Name := "trujac";
    MAGICMOD_DUR:        Name := "wytrzymal";
    MAGICMOD_LIFE:       Name := "witaln";
    MAGICMOD_MANA:       Name := "madr";
    MAGICMOD_STAMINA:    Name := "stabiln";
    MAGICMOD_INVIS:      Name := "niewidzialn";
    MAGICMOD_LIGHT:      Name := "jasn";
  endcase

  return Name + GewForItem(item.objtype);
  
endfunction

//--------------------------------------------------------------------------------

function GetSuffixName(MagicMod)

  var Name := "";
  case(MagicMod)
    MAGICMOD_FIRERESIST:    Name := "ochrony przed ogniem";
    MAGICMOD_COLDRESIST:    Name := "ochrony przed zimnem";
    MAGICMOD_LIGHTRESIST:   Name := "ochrony przed piorunami";
    MAGICMOD_ARMOR:         Name := "obrony";
    MAGICMOD_DMG:           Name := "mocy";
    MAGICMOD_STRENGTH:      Name := "sily";
    MAGICMOD_DEXTERITY:     Name := "zrecznosci";
    MAGICMOD_INTELLIGENCE:  Name := "inteligencji";
    MAGICMOD_SK_RAND_ALL:   Name := "losowej umiejetnosci";
  endcase
  
  if(MagicMod >= MAGICMOD_SKILLMOD and MagicMod <= (MAGICMOD_SKILLMOD + GetMaxSkillId()))
    var SkElem := GetSkillConfigElem(Cint(MagicMod - MAGICMOD_SKILLMOD));
    Name := "umiejetnosci "+SkElem.name;
  endif
  
  return Name;
  
endfunction

//--------------------------------------------------------------------------------

function GetMagicItemModColor(MagicMod)

  var color := 0;
  case(MagicMod)
    MAGICMOD_FIREDMG:    Color := 237;
    MAGICMOD_COLDDMG:    Color := 601;
    MAGICMOD_LIGHTDMG:   Color := 296;
    MAGICMOD_POISONDMG:  Color := 65;
    MAGICMOD_DUR:        Color := 1146;
    MAGICMOD_LIFE:       Color := 133;
    MAGICMOD_MANA:       Color := 600;
    MAGICMOD_STAMINA:    Color := 999;
    MAGICMOD_INVIS:      Color := 1109;
    MAGICMOD_LIGHT:      Color := 1154;
  endcase

  return Color;
  
endfunction

//--------------------------------------------------------------------------------

function SetMagicMods(item,MagicMods)

  return SetObjProperty(item,"MagicItem",MagicMods);

endfunction

//--------------------------------------------------------------------------------

function GetMagicItemPrefixMod(item)

  return Cint(GetMagicMods(item)[1]);

endfunction

//--------------------------------------------------------------------------------

function GetMagicItemSuffixMod(item)

  return Cint(GetMagicMods(item)[2]);

endfunction

//--------------------------------------------------------------------------------

function GetMagicItemPrefixCharges(item)

  return Cint(GetMagicMods(item)[4]);

endfunction

//--------------------------------------------------------------------------------

function SetMagicItemPrefixCharges(item,Charges)

  var MagicMods := GetMagicMods(item);
  MagicMods[4]  := Charges;
  SetMagicMods(item,MagicMods);
  
endfunction

//--------------------------------------------------------------------------------

function GetMagicItemSuffixCharges(item)

  return Cint(GetMagicMods(item)[5]);

endfunction

//--------------------------------------------------------------------------------

function SetMagicItemSuffixCharges(item,Charges)

  var MagicMods := GetMagicMods(item);
  MagicMods[5]  := Charges;
  SetMagicMods(item,MagicMods);

endfunction

//--------------------------------------------------------------------------------

function CheckMagicPrefixItemCharges(item,who := 0)

  if(GetMagicItemPrefixCharges(item) <= 0 and GetMagicItemPrefixMod(item))
    if(who)
      SendSysMessage(who,"Przedmiot "+item.desc+" stracil swa moc.",FONT_NORMAL,COLOR_RED);
    endif
    ErasePrefixMagicMod(item);
    return 0;
  elseif(GetMagicItemPrefixCharges(item) <= 0)
    return 0;
  endif
  
  return 1;
  
endfunction

//--------------------------------------------------------------------------------

function CheckMagicSuffixItemCharges(item,who := 0)

  if(!IsIdentified(item) or !IsMagicItem(item))
    return 0;
  endif

  if(GetMagicItemSuffixCharges(item) <= 0 and GetMagicItemSuffixMod(item))
    if(who)
      SendSysMessage(who,"Przedmiot "+item.desc+" stracil swa moc.",FONT_NORMAL,COLOR_RED);
    endif
    EraseSuffixMagicMod(item);
    return 0;
  elseif(GetMagicItemSuffixCharges(item) <= 0)
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function IdentifyMagicMod(MagicMod)

  var TimeCharges    := 0;
  var MagicModType   := 0;

  case(MagicMod)
    MAGICMOD_FIREDMG:              MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 0;
    MAGICMOD_COLDDMG:              MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 0;
    MAGICMOD_LIGHTDMG:             MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 0;
    MAGICMOD_POISONDMG:            MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 0;
    MAGICMOD_DUR:                  MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 1;
    MAGICMOD_LIFE:                 MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 1;
    MAGICMOD_MANA:                 MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 1;
    MAGICMOD_STAMINA:              MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 1;
    MAGICMOD_INVIS:                MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 1;
    MAGICMOD_LIGHT:                MagicModType := MAGIC_PREFIX_MOD; TimeCharges := 1;
    
    MAGICMOD_FIRERESIST:           MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 0;
    MAGICMOD_COLDRESIST:           MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 0;
    MAGICMOD_LIGHTRESIST:          MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 0;
    MAGICMOD_ARMOR:                MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 1;
    MAGICMOD_DMG:                  MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 1;
    MAGICMOD_STRENGTH:             MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 1;
    MAGICMOD_DEXTERITY:            MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 1;
    MAGICMOD_INTELLIGENCE:         MagicModType := MAGIC_SUFFIX_MOD; TimeCharges := 1;
  endcase

  if(MagicMod >= MAGICMOD_SKILLMOD and MagicMod <= MAGICMOD_SKILLMOD + GetMaxSkillId())
    MagicModType := MAGIC_SUFFIX_MOD;
    TimeCharges  := 1;
  endif

  var Ret := {};
  Ret[1] := MagicModType;
  Ret[2] := TimeCharges;
  
  return Ret;

endfunction

//--------------------------------------------------------------------------------

function ErasePrefixMagicMod(item)

  if(!IsMagicItem(item))
    return;
  endif
  
  var MagicPrefixMod := GetMagicItemPrefixMod(item);
  var MagicSuffixMod := GetMagicItemSuffixMod(item);
  var MagicLvl       := GetMagicItemLevel(item);
  var SuffixName     := GetSuffixName(MagicSuffixMod);

  var MagicMods      := GetMagicMods(item);
  MagicMods[1]       := 0;
  SetMagicMods(item,MagicMods);

  case(MagicPrefixMod)
    MAGICMOD_DUR:        SetItemMaxHpMod(item, GetItemMaxHpMod(item) - Cint(MagicLvl * 2));
    MAGICMOD_FIREDMG:
    MAGICMOD_COLDDMG:
    MAGICMOD_FIREDMG:    if(item.isa(POLCLASS_WEAPON))
                           EraseOnHitScript(item,MAGIC_HIT_SCRIPT);
                         endif
  endcase

  if(!MagicMods[1] and !MagicMods[2])
    EraseItemMagic(item);
    return;
  endif

  SetName(item,GetDescForItem(item.objtype) + " " + SuffixName);
  
endfunction

//--------------------------------------------------------------------------------

function EraseSuffixMagicMod(item)

  if(!IsMagicItem(item))
    return;
  endif

  var MagicPrefixMod := GetMagicItemPrefixMod(item);
  var MagicSuffixMod := GetMagicItemSuffixMod(item);
  var MagicLvl       := GetMagicItemLevel(item);
  var PrefixName     := GetPrefixName(item,MagicPrefixMod);

  var MagicMods      := GetMagicMods(item);
  MagicMods[2]       := 0;
  SetMagicMods(item,MagicMods);

  case(MagicSuffixMod)
    MAGICMOD_ARMOR:      item.ar_mod  := item.ar_mod - Cint(MagicLvl * 2);
    MAGICMOD_DMG:        item.dmg_mod := item.ar_mod - Cint(MagicLvl * 2);
  endcase

  if(!MagicMods[1] and !MagicMods[2])
    EraseItemMagic(item);
    return;
  endif

  SetName(item,PrefixName + " " + GetDescForItem(item.objtype));

endfunction

//--------------------------------------------------------------------------------

function EraseItemMagic(item)

  SetName(item,GetDescForItem(item));
  EraseObjProperty(item,"MagicItem");
  EraseObjProperty(item,"Identified");
  item.color := 0;
  
endfunction

//-----------------------------------------------------------------------------

function UnEquipMagicItems(who, Mods)

  foreach Item in ListEquippedItems(who)
    if(IsMagicItem(Item))
      if(IsIdentified(Item))
        var MagicMods := GetMagicMods(item);

        var MagicPrefixMod := Cint(MagicMods[1]);
        var MagicSuffixMod := Cint(MagicMods[2]);

        if(MagicPrefixMod in Mods)
          MoveToPack(Item, who);
        elseif(MagicSuffixMod in Mods)
          MoveToPack(Item, who);
        endif
      endif
    endif
  endforeach

endfunction

//--------------------------------------------------------------------------------
 
function GetMagicModsChargesDesc(item)

  var MagicMods := GetMagicMods(item);
  var ret := error;
  if(!MagicMods)
    ret.errortext := "No Magic Mods";
    return ret;
  endif

  var Percent := CInt(CInt((CInt(MagicMods[4]) * 100) / CInt(MagicMods[6])) / 2) + CInt(CInt((CInt(MagicMods[5]) * 100) / CInt(MagicMods[7])) / 2);

  case(CInt(Percent / 10))
    0:
    1:  return "prawie brak";
    2:  return "niewiele";
    3:  return "czesc";
    4:  return "troche";
    5:  return "polowa";
    6:  return "troche ponad polowa";
    7:  return "spora czesc";
    8:  return "lekko wyczerpany";
    9:  return "niemal calkowicie wypelniony";
    10: return "calkowicie wypelniony";
  endcase

  ret.errortext := "Perc Error";
  return ret;
endfunction

//--------------------------------------------------------------------------------
