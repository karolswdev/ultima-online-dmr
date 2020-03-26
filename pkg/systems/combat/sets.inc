//-----------------------------------------------------------------------------

const PROP_AR_SET            := "#ArSet";
const SETS_CFG               := ":combat:sets";

const SETCFG_ARMORS          := "Armors";
const SETCFG_GLOBAL          := "Global";

//-----------------------------------------------------------------------------

function CheckSetEquip(who, item)

  if(who.isa(POLCLASS_NPC))
    return;
  endif

  if(Item.isa(POLCLASS_ARMOR))
    if(!IsArmorSetEquipped(who))
      CheckArmorSetEquip(who, Item);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckArmorSetEquip(who, Item)

  var SetCfg    := ReadConfigFile(SETS_CFG);
  var ArSetElem := SetCfg[SETCFG_ARMORS];

  if(!ArSetElem)
    LogError("CheckSetEquip: Nie znaleziono elementu ["+SETCFG_ARMORS+"] w ["+SETS_CFG+"]"); 
  else
    var ArNames  := ListConfigElemProps(ArSetElem);
    var SetItems;

    foreach Name in ArNames
      SetItems := SplitWords(GetConfigString(ArSetElem, Name));
      foreach ItemObj in SetItems
        ItemObj := CInt(ItemObj);
      endforeach

      if(Item.objtype in SetItems)
        if(CheckArmorSetEquipped(who, Item, SetItems))
          return;
        endif
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function IsArmorSetEquipped(who)

  if(GetObjProperty(who, PROP_AR_SET))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckArmorSetEquipped(who, Item, byref SetItems)

  var Items   := array(Item);
  var ArSum   := CInt(Item.ar / 2);
  var QualMod := GetSetItemQualMod(Item);
  var MadeMod := GetItemMakeMod(Item);
  var ItArMod := Item.ar_mod;

  foreach eItem in ListEquippedItems(who)
    if(QualMod == GetSetItemQualMod(eItem) and ItArMod == eItem.ar_mod)
      if(GetItemMakeMod(eItem) == MadeMod)
        if(eItem.objtype in SetItems)
          Items.append(eItem);
          ArSum := ArSum + CInt(eItem.ar / 2);
        endif
      endif
    endif
  endforeach

  if(Items.size() == SetItems.size())
    var SetCfg := ReadConfigFile(SETS_CFG);
    var Perc   := SetCfg[SETCFG_GLOBAL].ArPercMod;
    if(!Perc)
      LogError("CheckArmorSetEquipped: Nie znaleziono procentu ArMod! ["+SETS_CFG+"::"+SETCFG_GLOBAL+"]->ArPercMod]");
      return 1;
    endif

    foreach sItem in Items
      sItem := sItem.serial;
    endforeach

    var ArMod    := CInt(ArSum * (Perc * 0.01));
    var MaxArMod := CInt(SetCfg[SETCFG_GLOBAL].MaxArMod);

    if(ArMod > MaxArMod)
      ArMod := MaxArMod;
    endif

    var Props := array(ReadGameClock(), ArMod, Items);
    SetObjProperty(who, PROP_AR_SET, Props);
    who.ar_mod := who.ar_mod + ArMod;
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetSetItemQualMod(Item)

  if(Item.quality < 1.0)
    return 1;
  elseif(Item.quality > 1.0)
    return 3;
  else
    return 2;
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckSetUnEquip(who, item)

  if(who.isa(POLCLASS_NPC))
    return;
  endif

  var ArProps := GetObjProperty(who, PROP_AR_SET);

  if(ArProps)
    if(Item.serial in (ArProps[3]))
      who.ar_mod := who.ar_mod - ArProps[2];
      EraseObjProperty(who, PROP_AR_SET);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
