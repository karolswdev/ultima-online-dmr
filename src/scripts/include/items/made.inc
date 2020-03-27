//-----------------------------------------------------------------------------
// Made Types (Merchant Skills)
//-----------------------------------------------------------------------------

const MTYPE_INGOT            := 0x00000001;
const MTYPE_HIDE             := 0x00000002;
const MTYPE_LOG              := 0x00000004;
const MTYPE_MAGIC            := 0x00000008;
const MTYPE_REGS             := 0x00000010;
const MTYPE_COOK             := 0x00000020;
const MTYPE_GLASS            := 0x00000040;

//=============================================================================
//-----------------------------------------------------------------------------
// Made Props
//-----------------------------------------------------------------------------
//=============================================================================

function GenMade(who, Type := 0, Mod := 0)

  if(IsPlayer(who))
    return array(GetRealName(who), who.serial, Type, Mod);
  else
    return array("", 0, Type, Mod);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMaxMaterialModByMadeType(Type)

  case(Type)
    MTYPE_INGOT: 
      return 16;
    MTYPE_HIDE: 
      return 7;
    MTYPE_LOG: 
      return 9;
    default: 
      return 1;
  endcase

endfunction

//-----------------------------------------------------------------------------

function GetHarvesterSerial(Item)

  var Data := GetObjProperty(Item, PROP_HARVESTED);
  return CInt(Data[2]);

endfunction

//-----------------------------------------------------------------------------

function GetHarvested(Item)

  return GetObjProperty(Item, PROP_HARVESTED);

endfunction

//-----------------------------------------------------------------------------

function GetHarvestDescriptorForData(matObj, harvData)

  var itmDesc := GetItemDescriptor(matObj);

  if(harvData)
    itmDesc.CProps.insert(PROP_HARVESTED, harvData);
  endif

  return itmDesc;
  
endfunction

//-----------------------------------------------------------------------------

function GetHarvestDescriptor(matObj, who)

  var itmDesc := GetItemDescriptor(matObj);

  if(!GetObjProperty(who, PROP_SIGN_HARVOFF))
    itmDesc.CProps.insert(PROP_HARVESTED, array(GetRealName(who), who.serial));
  endif

  return itmDesc;
  
endfunction

//-----------------------------------------------------------------------------

function SetMade(who, Item, Type := 0, Mod := 0)

  if(Item.isa(POLCLASS_ITEM))
    SetObjProperty(Item, PROP_MADE, GenMade(who, Type, Mod) );
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMade(Item)

  return GetObjProperty(Item, PROP_MADE);

endfunction

//-----------------------------------------------------------------------------

function GetMadeDescriptor(ObjType, who, Type, Mod)

  var itmDesc := GetItemDescriptor(ObjType);
  var propMade := GenMade(who, Type, Mod);
  itmDesc.CProps.insert(PROP_MADE, propMade);

  return itmDesc;

endfunction

//-----------------------------------------------------------------------------

function IsItemMade(Item)

  if(GetObjProperty(Item, PROP_MADE))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetItemMakerName(Item)

  return ClStr(GetObjProperty(Item, PROP_MADE)[1]);

endfunction

//-----------------------------------------------------------------------------

function GetItemMakerSerial(Item)

  return CInt(GetObjProperty(Item, PROP_MADE)[2]);

endfunction

//-----------------------------------------------------------------------------

function GetItemType(Item)

  return CInt(GetObjProperty(Item, PROP_MADE)[3]);

endfunction

//-----------------------------------------------------------------------------
// Ingots Mod: 1-16

function GetItemMakeMod(item)

  return CInt(GetObjProperty(Item, PROP_MADE)[4]);

endfunction

//-----------------------------------------------------------------------------

function GetItemMakeModPerc(Item)
  
  var Type := GetItemType(Item);
  var Mod := GetItemMakeMod(item);

  if(!Mod or !Type)
    return 0;
  endif

  var MaxMod := GetMaxMaterialModByMadeType(Type);

  if(Mod > 1 and MaxMod > 1)
    return CDbl(Mod-1) / (MaxMod-1); // -1 aby zwykly nie dawal procenta zadnego (czyli mod=1)
  endif

  return 0.0;

endfunction

//-----------------------------------------------------------------------------

function GetMatItemObjType(Item)

  var ItemCfg;
  var Mod := GetItemMakeMod(Item);
  var PropName, ObjTypes;

  case(GetItemType(Item))
    MTYPE_LOG:    ObjTypes := GetBoardObjTypes();
                  ItemCfg  := ReadConfigFile(ITEMS_CFG);
                  PropName := "LogMod";
    MTYPE_HIDE:   ObjTypes := GetLeatherObjTypes();
                  ItemCfg  := ReadConfigFile(ITEMS_CFG);
                  PropName := "MatMod";
    MTYPE_INGOT:  ObjTypes := GetIngotsObjTypes();
                  ItemCfg  := ReadConfigFile(":blacksmithy:materials");
                  PropName := "ArMod";
    default:
      return error;
  endcase

  foreach ObjType in ObjTypes

    if(GetConfigInt(ItemCfg[ObjType], PropName) == Mod)
      return ObjType;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetMatItemCfgFromMade(Item, byref BaseElem)

  var ObjCfg, ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Mod := GetItemMakeMod(Item);
  var PropName, ObjTypes;

  case(GetItemType(Item))
    MTYPE_LOG:    ObjTypes := GetBoardObjTypes();
                  ObjCfg   := ItemCfg;
                  PropName := "LogMod";
    MTYPE_HIDE:   ObjTypes := GetLeatherObjTypes();
                  ObjCfg   := ItemCfg;
                  PropName := "MatMod";
    MTYPE_INGOT:  ObjTypes := GetIngotsObjTypes();
                  ObjCfg   := ReadConfigFile(":blacksmithy:materials");
                  PropName := "ArMod";
    default:
      return error;
  endcase

  BaseElem := 0;

  foreach ObjType in ObjTypes
    if(!BaseElem)
      BaseElem := ItemCfg[ObjType];
    endif

    if(GetConfigInt(ObjCfg[ObjType], PropName) == Mod)
      return ItemCfg[ObjType];
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
// Ingots: 1-24 (1-16 * 0.7 - 1.5)

function CheckItemQuality(item)

  return ToMin(CInt(GetItemMakeMod(item) * GetItemQuality(Item)), 1);

endfunction

//-------------------------------------------------------------------------------
// Quality: 0.7 - 1.5 w craftach
//-------------------------------------------------------------------------------

function HasItemQuality(Item)

  return (Item.quality != error) or (GetObjProperty(Item, "Quality") != error);

endfunction

//-------------------------------------------------------------------------------

function GetItemQuality(Item)

  if(Item.quality != error)
    return Item.quality;
  elseif(GetObjProperty(Item, "Quality") != error)
    return CDbl(GetObjProperty(Item, "Quality"));
  else
    return 1;
  endif

endfunction

//-------------------------------------------------------------------------------

function SetItemQuality(Item, Qual)

  if(Item.quality != error)
    Item.quality := Qual;
  else
    SetObjProperty(Item, "Quality", CDbl(Qual));
  endif

endfunction

//-----------------------------------------------------------------------------
