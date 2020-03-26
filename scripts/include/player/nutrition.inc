//--------------------------------------------------------------------------------
// Podstawy odzywiania sa w include/player/basic
//--------------------------------------------------------------------------------

include "include/itemuses";
include "include/player/young";
include ":tips:tips";

//-----------------------------------------------------------------------------
// Nutrition player basics
//-----------------------------------------------------------------------------

const NUTR_FULL_FACTOR       := 86;   // Nutr GainMod: 5-0% (86-100) do 0-17% (0-85)
const NUTR_MIN_VAL           := 0;
const NUTR_MAX_VAL           := 100;
//--------------------------------------------------------------------------------

const PROP_NUTR_VITAMIN      := "nVitamin";
const PROP_NUTR_PROTEIN      := "nProtein";
const PROP_NUTR_HUNGER       := "nHunger";
const PROP_NUTR_SUGAR        := "nSugar";

//--------------------------------------------------------------------------------

const NVI_VITAMIN            := 1;
const NVI_PROTEIN            := 2;
const NVI_HUNGER             := 3;
const NVI_SUGAR              := 4;

const NVIID_VITAMIN          := "Vitamin";
const NVIID_PROTEIN          := "Protein";
const NVIID_HUNGER           := "Hunger";
const NVIID_SUGAR            := "Sugar";

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

const EAT_SCRIPT             := ":eatitems:eat";

const PROP_FOODQUALITY       := "Qual";
const PROP_LASTHUNGER_TIME   := "LastHungerTime";
const PROP_HUNGERDELAY       := "HungerDelay";

//--------------------------------------------------------------------------------

const HGDELAY_MIN_SEC        := 3600;
const HGDELAY_MAX_SEC        := 6400;

const HUNGER_DEC_VAL         := 3;   // Nutrition Decrease Value by Hunger Time
const CRITICAL_HUNGER_VAL    := 25;
//================================================================================
//--------------------------------------------------------------------------------
// EatItem Nutrition Values
//--------------------------------------------------------------------------------
//================================================================================

function IsFullNutrVals(who, ItemObj)

  var Item := 0;

  if(ItemObj.isa(POLCLASS_ITEM))
    Item    := ItemObj;
    ItemObj := Item.objtype;
  endif

  var iCfg  := ReadConfigFile(ITEMS_CFG);

  var Vits  := CInt(iCfg[ItemObj].Vitamin);
  var Prot  := CInt(iCfg[ItemObj].Protein);
  var Hung  := CInt(iCfg[ItemObj].Hunger);
  var Sugar := CInt(iCfg[ItemObj].Sugar);
  var Res   := 0;

  if(!Vits and !Prot and !Sugar and !Hung)
    return error;
  endif

  if(Item)
    var Qual := MinMax(ToMin(GetFoodQual(Item) - 3, 0) * 0.50, 1.00, 2.00);
    Vits  := CInt(Vits  * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_VITAMIN));
    Prot  := CInt(Prot  * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_PROTEIN));
    Hung  := CInt(Hung  * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_HUNGER));
    Sugar := CInt(Sugar * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_SUGAR));
  endif

  if(Vits)
    if(GetNutrValue(who, NVI_VITAMIN) < NUTR_MAX_VAL)
      Res := Res + 1;
    endif
  endif

  if(Prot)
    if(GetNutrValue(who, NVI_PROTEIN) < NUTR_MAX_VAL)
      Res := Res + 1;
    endif
  endif

  if(Hung)
    if(GetNutrValue(who, NVI_HUNGER) < NUTR_MAX_VAL)
      Res := Res + 1;
    endif
  endif

  if(Sugar)
    if(GetNutrValue(who, NVI_SUGAR) < NUTR_MAX_VAL)
      Res := Res + 1;
    endif
  endif

  return !Res;

endfunction

//--------------------------------------------------------------------------------

function UpdNutrVals(who, ItemObj, Mod := 1)

  var Item := 0;

  if(ItemObj.isa(POLCLASS_ITEM))
    Item    := ItemObj;
    ItemObj := Item.objtype;
  endif

  var iCfg := ReadConfigFile(ITEMS_CFG);

  var Vits  := CInt(iCfg[ItemObj].Vitamin);
  var Prot  := CInt(iCfg[ItemObj].Protein);
  var Hung  := CInt(iCfg[ItemObj].Hunger);
  var Sugar := CInt(iCfg[ItemObj].Sugar);
  var Res   := 0;

  if(!Vits and !Prot and !Sugar and !Hung)
    return error;
  endif

  if(Item)
    var Qual := MinMax(ToMin(GetFoodQual(Item) - 3, 0) * 0.50, 1.00, 2.00);
    Vits  := CInt(Vits  * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_VITAMIN));
    Prot  := CInt(Prot  * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_PROTEIN));
    Hung  := CInt(Hung  * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_HUNGER));
    Sugar := CInt(Sugar * Qual) + CInt(GetObjProperty(Item, PROP_NUTR_SUGAR));
  endif

  Vits  := Vits  * Mod;
  Prot  := Prot  * Mod;
  Hung  := Hung  * Mod;
  Sugar := Sugar * Mod;

  Res := Res + SetNutrValue(who, NVI_VITAMIN, GetNutrValue(who, NVI_VITAMIN) + Vits);
  Res := Res + SetNutrValue(who, NVI_PROTEIN, GetNutrValue(who, NVI_PROTEIN) + Prot);
  Res := Res + SetNutrValue(who, NVI_HUNGER,  GetNutrValue(who, NVI_HUNGER)  + Hung);
  Res := Res + SetNutrValue(who, NVI_SUGAR,   GetNutrValue(who, NVI_SUGAR)   + Sugar);

  return Res;

endfunction

//--------------------------------------------------------------------------------

function SetNutrValue(who, nvIdx, Val)

  Val := MinMax(Val, NUTR_MIN_VAL, NUTR_MAX_VAL);

  var pVal := GetNutrValue(who, nvIdx);

  if(Val == pVal)
    return 0;
  endif

  if(nvIdx == NVI_HUNGER)
    if(GetNutrFactor(pVal) != GetNutrFactor(Val))
      SendNutrMsg(who, Val);
    endif
  endif

  case(nvIdx)
    NVI_VITAMIN:  SetObjProperty(who, PROP_NUTR_VITAMIN, Val);
    NVI_PROTEIN:  SetObjProperty(who, PROP_NUTR_PROTEIN, Val);
    NVI_HUNGER:   SetObjProperty(who, PROP_NUTR_HUNGER,  Val);
    NVI_SUGAR:    SetObjProperty(who, PROP_NUTR_SUGAR,   Val);
  endcase

  return 1;

endfunction

//--------------------------------------------------------------------------------

const SNMF_DONT_HURT         := 0x01;

//--------------------------------------------------------------------------------

function SendNutrMsg(who, Val, Flags := 0)
  var v := GetNutrFactor(Val);
  case(v)
    0:      if(Val == 0)
              DealDamage(who, GetHp(who)+1, who);
              SendSysMessage(who, "Umierasz z glodu!", FONT_NORMAL, NutrMsgColor(v));
            else
              if(!(Flags & SNMF_DONT_HURT))
                DealDamage(who, CInt(GetMaxHp(who)/4) + RandomInt(CInt(GetMaxHp(who)/4)), who);
                DealStaminaDamage(who, CInt(GetStamina(who) / 2));
                DealManaDamage(who, CInt(GetMana(who) / 2));
              endif
              SendSysMessage(who, "Twoje zle odzywianie przyprawie cie o chorobe.", FONT_NORMAL, NutrMsgColor(v));
            endif

    1:      if(!(Flags & SNMF_DONT_HURT))
              DealStaminaDamage(who, CInt(GetStamina(who) / 2));
              DealManaDamage(who, CInt(GetMana(who) / 2));
            endif
            SendSysMessage(who, "Czujesz ze opadaja ci sily, nie mozesz sie na niczym skupic.", FONT_NORMAL, NutrMsgColor(v));
            
    2:      if(!(Flags & SNMF_DONT_HURT))
              DealStaminaDamage(who, 5 + RandomInt(10));
            endif
            SendSysMessage(who, "Czujesz sie ospal"+ggm(who,6)+" i zmeczon"+ggm(who,6)+".", FONT_NORMAL, NutrMsgColor(v));

    3:      SendSysMessage(who, "Mogl"+ggm(who,1)+" czuc sie lepiej.", FONT_NORMAL, NutrMsgColor(v));
    4:      SendSysMessage(who, "Czujesz sie dobrze.", FONT_NORMAL, NutrMsgColor(v));
    5:      SendSysMessage(who, "Czujesz sie w pelni sil.", FONT_NORMAL, NutrMsgColor(v));
    6:      SendSysMessage(who, "Czujesz sie ciezk"+ggm(who,23)+" i zmeczon"+ggm(who,6)+".", FONT_NORMAL, NutrMsgColor(v));
    7:      SendSysMessage(who, "Masz ochote zwymiotowac.", FONT_NORMAL, NutrMsgColor(v));

    default: return;
  endcase

  TriggerTipEvent(who, TEID_HUNGER);

endfunction

//--------------------------------------------------------------------------------

function GetNutrValState(Val)

  case(GetNutrFactor(Val))
    0:    return "Bardzo duzy niedobor";
    1:    return "Duzy niedobor";
    2:    return "Lekki niedobor";
    3:    return "Niewiele ponizej normy";
    4:    return "Norma";
    5:    return "Optymalne zaspokojenie";
    6:    return "Przejedzenie";
    7:    return "Bardzo duze przejedzenie";
  endcase

endfunction

//--------------------------------------------------------------------------------

function GetNutrFactor(Val)

  if(Val < 10)
    return 0;
  elseif(Val < 25)
    return 1;
  elseif(Val < 38)
    return 2;
  elseif(Val < 50)
    return 3;
  elseif(Val < 74)
    return 4;
  elseif(Val < NUTR_FULL_FACTOR)
    return 5;
  elseif(Val < NUTR_FULL_FACTOR+8)
    return 6;
  else
    return 7;
  endif

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------
//================================================================================

function GetFoodQual(Item)

  var Qual := GetObjProperty(Item, PROP_FOODQUALITY);

  if(Qual == error)
    return 3;
  else
    return CInt(Qual);
  endif

endfunction

//--------------------------------------------------------------------------------

function UpdHungerTime(who, Perc, special := 0)

  var HungerDelay := GetObjProperty(who, PROP_HUNGERDELAY);

  if(special)
    SetObjProperty(who, PROP_HUNGERDELAY,GetObjProperty(who, PROP_ONLINETIMER) - GetObjProperty(who, PROP_LASTHUNGER_TIME) + HGDELAY_MAX_SEC);
  elseif(HungerDelay)
    HungerDelay := HungerDelay + (CInt(HungerDelay/4) * (Perc * 0.01));
    SetObjProperty(who, PROP_HUNGERDELAY, MinMax(HungerDelay, HGDELAY_MIN_SEC, HGDELAY_MAX_SEC));
  endif


endfunction

//--------------------------------------------------------------------------------

function NutrMsgColor(val)
  case(val)  
    0:        return COLOR_RED;
    1:   
    2: 
    3:        return COLOR_RED_LIGHT;       
    4:        return COLOR_GREEN;
    5:        return COLOR_BLUE;
    6:        return COLOR_YELLOW_LIGHT;
    7:        return COLOR_YELLOW_DARK;
    default:  return 0;
  endcase
endfunction

//--------------------------------------------------------------------------------

function GetNutrValue(who, nvIdx)

  var Val := 0;

  case(nvIdx)
    NVI_VITAMIN:  Val := GetObjProperty(who, PROP_NUTR_VITAMIN);
    NVI_PROTEIN:  Val := GetObjProperty(who, PROP_NUTR_PROTEIN);
    NVI_HUNGER:   Val := GetObjProperty(who, PROP_NUTR_HUNGER);
    NVI_SUGAR:    Val := GetObjProperty(who, PROP_NUTR_SUGAR);
  endcase

  if(Val == error)
    Val := CInt((NUTR_MAX_VAL - NUTR_MIN_VAL) * 0.75);

    case(nvIdx)
      NVI_VITAMIN:  SetObjProperty(who, PROP_NUTR_VITAMIN, Val);
      NVI_PROTEIN:  SetObjProperty(who, PROP_NUTR_PROTEIN, Val);
      NVI_HUNGER:   SetObjProperty(who, PROP_NUTR_HUNGER,  Val);
      NVI_SUGAR:    SetObjProperty(who, PROP_NUTR_SUGAR,   Val);
    endcase
  endif

  return Val;

endfunction

//-----------------------------------------------------------------------------