//-----------------------------------------------------------------------------

const UOBJ_CRYSTAL_MMP_FIREDMG                := 0xA310;
const UOBJ_CRYSTAL_MMP_COLDDMG                := 0xA311;
const UOBJ_CRYSTAL_MMP_LIGHTDMG               := 0xA312;
const UOBJ_CRYSTAL_MMP_DUR                    := 0xA313;
const UOBJ_CRYSTAL_MMP_LIFE                   := 0xA314;
const UOBJ_CRYSTAL_MMP_MANA                   := 0xA315;
const UOBJ_CRYSTAL_MMP_STAMINA                := 0xA316;
const UOBJ_CRYSTAL_MMP_INVIS                  := 0xA317;
const UOBJ_CRYSTAL_MMP_LIGHT                  := 0xA318;

const UOBJ_CRYSTAL_MMS_FIRERESIST             := 0xA331;
const UOBJ_CRYSTAL_MMS_COLDRESIST             := 0xA332;
const UOBJ_CRYSTAL_MMS_LIGHTRESIST            := 0xA333;
const UOBJ_CRYSTAL_MMS_ARMOR                  := 0xA334;
const UOBJ_CRYSTAL_MMS_DMG                    := 0xA335;
const UOBJ_CRYSTAL_MMS_STRENGTH               := 0xA336;
const UOBJ_CRYSTAL_MMS_DEXTERITY              := 0xA337;
const UOBJ_CRYSTAL_MMS_INTELLIGENCE           := 0xA338;

//-----------------------------------------------------------------------------
// Scrolls

//-----------------------------------------------------------------------------

const UOBJ_CRYSTALSCROLL_MMP_FIREDMG          := 0xA350;
const UOBJ_CRYSTALSCROLL_MMP_COLDDMG          := 0xA351;
const UOBJ_CRYSTALSCROLL_MMP_LIGHTDMG         := 0xA352;
const UOBJ_CRYSTALSCROLL_MMP_DUR              := 0xA353;
const UOBJ_CRYSTALSCROLL_MMP_LIFE             := 0xA354;
const UOBJ_CRYSTALSCROLL_MMP_MANA             := 0xA355;
const UOBJ_CRYSTALSCROLL_MMP_STAMINA          := 0xA356;
const UOBJ_CRYSTALSCROLL_MMP_INVIS            := 0xA357;
const UOBJ_CRYSTALSCROLL_MMP_LIGHT            := 0xA358;

const UOBJ_CRYSTALSCROLL_MMS_FIRERESIST       := 0xA371;
const UOBJ_CRYSTALSCROLL_MMS_COLDRESIST       := 0xA372;
const UOBJ_CRYSTALSCROLL_MMS_LIGHTRESIST      := 0xA373;
const UOBJ_CRYSTALSCROLL_MMS_ARMOR            := 0xA374;
const UOBJ_CRYSTALSCROLL_MMS_DMG              := 0xA375;
const UOBJ_CRYSTALSCROLL_MMS_STRENGTH         := 0xA376;
const UOBJ_CRYSTALSCROLL_MMS_DEXTERITY        := 0xA377;
const UOBJ_CRYSTALSCROLL_MMS_INTELLIGENCE     := 0xA378;

//-----------------------------------------------------------------------------

const UOBJ_MAGIC_CRYSTALS_BOOK                := 0xA302;

//-----------------------------------------------------------------------------

function GetPrefixCrystals()

  return {
    UOBJ_CRYSTAL_MMP_FIREDMG,
    UOBJ_CRYSTAL_MMP_COLDDMG,
    UOBJ_CRYSTAL_MMP_LIGHTDMG,
    UOBJ_CRYSTAL_MMP_DUR,
    UOBJ_CRYSTAL_MMP_LIFE,
    UOBJ_CRYSTAL_MMP_MANA,
    UOBJ_CRYSTAL_MMP_STAMINA,
    UOBJ_CRYSTAL_MMP_INVIS,
    UOBJ_CRYSTAL_MMP_LIGHT
  };
  
endfunction

//-----------------------------------------------------------------------------

function GetSuffixCrystals()
    
  return {
    UOBJ_CRYSTAL_MMS_FIRERESIST,
    UOBJ_CRYSTAL_MMS_COLDRESIST,
    UOBJ_CRYSTAL_MMS_LIGHTRESIST,
    UOBJ_CRYSTAL_MMS_ARMOR,
    UOBJ_CRYSTAL_MMS_DMG,
    UOBJ_CRYSTAL_MMS_STRENGTH,
    UOBJ_CRYSTAL_MMS_DEXTERITY,
    UOBJ_CRYSTAL_MMS_INTELLIGENCE
  };
  
endfunction

//-----------------------------------------------------------------------------

function GetPrefixCrystalsScrolls()

  return {
    UOBJ_CRYSTALSCROLL_MMP_FIREDMG,
    UOBJ_CRYSTALSCROLL_MMP_COLDDMG,
    UOBJ_CRYSTALSCROLL_MMP_LIGHTDMG,
    UOBJ_CRYSTALSCROLL_MMP_DUR,
    UOBJ_CRYSTALSCROLL_MMP_LIFE,
    UOBJ_CRYSTALSCROLL_MMP_MANA,
    UOBJ_CRYSTALSCROLL_MMP_STAMINA,
    UOBJ_CRYSTALSCROLL_MMP_INVIS,
    UOBJ_CRYSTALSCROLL_MMP_LIGHT
  };

endfunction

//-----------------------------------------------------------------------------

function GetSuffixCrystalsScrolls()

  return {
    UOBJ_CRYSTALSCROLL_MMS_FIRERESIST,
    UOBJ_CRYSTALSCROLL_MMS_COLDRESIST,
    UOBJ_CRYSTALSCROLL_MMS_LIGHTRESIST,
    UOBJ_CRYSTALSCROLL_MMS_ARMOR,
    UOBJ_CRYSTALSCROLL_MMS_DMG,
    UOBJ_CRYSTALSCROLL_MMS_STRENGTH,
    UOBJ_CRYSTALSCROLL_MMS_DEXTERITY,
    UOBJ_CRYSTALSCROLL_MMS_INTELLIGENCE
  };

endfunction

//-----------------------------------------------------------------------------

function GetMagicModByCrystal(cObjType)

  case(cObjType)
    UOBJ_CRYSTAL_MMP_FIREDMG:           return MAGICMOD_FIREDMG;
    UOBJ_CRYSTAL_MMP_COLDDMG:           return MAGICMOD_COLDDMG;
    UOBJ_CRYSTAL_MMP_LIGHTDMG:          return MAGICMOD_LIGHTDMG;
    UOBJ_CRYSTAL_MMP_DUR:               return MAGICMOD_DUR;
    UOBJ_CRYSTAL_MMP_LIFE:              return MAGICMOD_LIFE;
    UOBJ_CRYSTAL_MMP_MANA:              return MAGICMOD_MANA;
    UOBJ_CRYSTAL_MMP_STAMINA:           return MAGICMOD_STAMINA;
    UOBJ_CRYSTAL_MMP_INVIS:             return MAGICMOD_INVIS;
    UOBJ_CRYSTAL_MMP_LIGHT:             return MAGICMOD_LIGHT;

    UOBJ_CRYSTAL_MMS_FIRERESIST:        return MAGICMOD_FIRERESIST;
    UOBJ_CRYSTAL_MMS_COLDRESIST:        return MAGICMOD_COLDRESIST;
    UOBJ_CRYSTAL_MMS_LIGHTRESIST:       return MAGICMOD_LIGHTRESIST;
    UOBJ_CRYSTAL_MMS_ARMOR:             return MAGICMOD_ARMOR;
    UOBJ_CRYSTAL_MMS_DMG:               return MAGICMOD_DMG;
    UOBJ_CRYSTAL_MMS_STRENGTH:          return MAGICMOD_STRENGTH;
    UOBJ_CRYSTAL_MMS_DEXTERITY:         return MAGICMOD_DEXTERITY;
    UOBJ_CRYSTAL_MMS_INTELLIGENCE:      return MAGICMOD_INTELLIGENCE;
  endcase
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function GetAllCrystalsScrolls()

  var MagicCrystalsScrolls := {};

  foreach Scroll in GetPrefixCrystalsScrolls()
    MagicCrystalsScrolls.append(Scroll);
  endforeach
  foreach Scroll in GetSuffixCrystalsScrolls()
    MagicCrystalsScrolls.append(Scroll);
  endforeach

  return MagicCrystalsScrolls;
  
endfunction

//-----------------------------------------------------------------------------

function GetOtherMagicMixes()

  return {
    UOBJ_ROYAL_ORE,
    UOBJ_FIREHIDE,
    UOBJ_OHIILOG,
    UOBJ_WAND
  };

endfunction

//-----------------------------------------------------------------------------

function GetMagicCrystalByMCScroll(cObjType)

  case(cObjType)
    UOBJ_CRYSTALSCROLL_MMP_FIREDMG:           return UOBJ_CRYSTAL_MMP_FIREDMG;
    UOBJ_CRYSTALSCROLL_MMP_COLDDMG:           return UOBJ_CRYSTAL_MMP_COLDDMG;
    UOBJ_CRYSTALSCROLL_MMP_LIGHTDMG:          return UOBJ_CRYSTAL_MMP_LIGHTDMG;
    UOBJ_CRYSTALSCROLL_MMP_DUR:               return UOBJ_CRYSTAL_MMP_DUR;
    UOBJ_CRYSTALSCROLL_MMP_LIFE:              return UOBJ_CRYSTAL_MMP_LIFE;
    UOBJ_CRYSTALSCROLL_MMP_MANA:              return UOBJ_CRYSTAL_MMP_MANA;
    UOBJ_CRYSTALSCROLL_MMP_STAMINA:           return UOBJ_CRYSTAL_MMP_STAMINA;
    UOBJ_CRYSTALSCROLL_MMP_INVIS:             return UOBJ_CRYSTAL_MMP_INVIS;
    UOBJ_CRYSTALSCROLL_MMP_LIGHT:             return UOBJ_CRYSTAL_MMP_LIGHT;

    UOBJ_CRYSTALSCROLL_MMS_FIRERESIST:        return UOBJ_CRYSTAL_MMS_FIRERESIST;
    UOBJ_CRYSTALSCROLL_MMS_COLDRESIST:        return UOBJ_CRYSTAL_MMS_COLDRESIST;
    UOBJ_CRYSTALSCROLL_MMS_LIGHTRESIST:       return UOBJ_CRYSTAL_MMS_LIGHTRESIST;
    UOBJ_CRYSTALSCROLL_MMS_ARMOR:             return UOBJ_CRYSTAL_MMS_ARMOR;
    UOBJ_CRYSTALSCROLL_MMS_DMG:               return UOBJ_CRYSTAL_MMS_DMG;
    UOBJ_CRYSTALSCROLL_MMS_STRENGTH:          return UOBJ_CRYSTAL_MMS_STRENGTH;
    UOBJ_CRYSTALSCROLL_MMS_DEXTERITY:         return UOBJ_CRYSTAL_MMS_DEXTERITY;
    UOBJ_CRYSTALSCROLL_MMS_INTELLIGENCE:      return UOBJ_CRYSTAL_MMS_INTELLIGENCE;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInCrBook(CrBook,ScrollObj)

  ScrollObj := Cint(ScrollObj);

  if(!ScrollObj)
    return 0;
  endif

  var PrefixCr := GetObjProperty(CrBook,"PrefixCrystals");
  var SuffixCr := GetObjProperty(CrBook,"SuffixCrystals");
  if((ScrollObj in PrefixCr) or (ScrollObj in SuffixCr))
    return 1;
  endif
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function AddScrollToCrBook(CrBook,ScrollObj)

  ScrollObj := Cint(ScrollObj);

  if(!ScrollObj)
    return 0;
  endif

  if(IsInCrBook(CrBook,ScrollObj))
    return 0;
  endif

  var PrefixCr := GetObjProperty(CrBook,"PrefixCrystals");
  var SuffixCr := GetObjProperty(CrBook,"SuffixCrystals");
  
  if(!PrefixCr)
    PrefixCr := {};
  endif
  
  if(!SuffixCr)
    SuffixCr := {};
  endif

  if(ScrollObj in GetPrefixCrystalsScrolls())
    PrefixCr.append(ScrollObj);
  elseif(ScrollObj in GetSuffixCrystalsScrolls())
    SuffixCr.append(ScrollObj);
  else
    return 0;
  endif
  
  SetObjProperty(CrBook,"PrefixCrystals",PrefixCr);
  SetObjProperty(CrBook,"SuffixCrystals",SuffixCr);
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetMagicScrollObjType(PreRegObj,SufRegObj)

  foreach Scroll in GetAllCrystalsScrolls()
    if(PreRegObj == Cint(MainCfg[Scroll].firstreg) and SufRegObj == Cint(MainCfg[Scroll].secreg))
      return Scroll;
    endif
  endforeach

  return 0;
  
endfunction

//-----------------------------------------------------------------------------
