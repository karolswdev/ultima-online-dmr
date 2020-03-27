//-----------------------------------------------------------------------------

include "include/objtypes/magic";

//-----------------------------------------------------------------------------

const PALADIN_MURD_TIME      := 604800;

//-----------------------------------------------------------------------------

function LosePaladinFaith(who)

  if(GetBaseSkill(who, SKILLID_PALADINISM) > 0)
    SetBaseSkill(who, SKILLID_PALADINISM, 0);
    SetBaseSkill(who, SKILLID_ZEAL, 0);

    var Shield := GetEquipmentByLayer(who, LAYER_HAND1);

    if(Shield.objtype != UOBJ_PD_SHIELD)
      Shield := GetEquipmentByLayer(who, LAYER_HAND2);
    endif

    if(Shield.objtype == UOBJ_PD_SHIELD)
      MoveToPack(who, Shield);
    endif

    EraseObjProperty(who, "PaladinMurdTimer");
    SendSysMessage(who, "Zbezczescil"+ggm(who,3)+" swe imie! Nie jestes warty godla paladyna!", FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckPaladinMurderer(who)

  if(GetBaseSkill(who, SKILLID_PALADINISM) > 0)
    if(who.murderer and CanMod(who, MOD_STATUS) == 1)
      var Time := CInt(GetObjProperty(who, "PaladinMurdTimer"));

      if(Time + PALADIN_MURD_TIME < ReadGameClock())
        LosePaladinFaith(who);
      endif
    else
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function ErasePaladinMurderer(who)

  if(GetBaseSkill(who, SKILLID_PALADINISM) > 0)
    EraseObjProperty(who, "PaladinMurdTimer");
    SendSysMessage(who, "Odpokutowal"+ggm(who,3)+" swe grzechy. Jestes godzien bycia paladynem.", FONT_NORMAL, COLOR_YELLOW);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetPaladinMurderer(who)

  if(GetBaseSkill(who, SKILLID_PALADINISM) > 0)
    if(!GetObjProperty(who, "PaladinMurdTimer"))
      ReduceSkill(who, SKILLID_ZEAL, 0.80); 
      SetObjProperty(who, "PaladinMurdTimer", ReadGameClock());
      SendSysMessage(who, "Zgrzeszyl"+ggm(who,3)+"! Zostal"+ggm(who,3)+" morderca!", FONT_NORMAL, COLOR_RED);
      SendSysMessage(who, "Masz 3 miesiace czasu na odpokutowanie, jesli nie uda Ci sie pozbyc statusu mordercy, nie bedziesz godzien bycia paladynem!", FONT_NORMAL, COLOR_RED_LIGHT);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function LoseDarkKnightFaith(who)

  if(GetBaseSkill(who, SKILLID_FANATICISM) > 0)
    SetBaseSkill(who, SKILLID_FANATICISM, 0);
    SetBaseSkill(who, SKILLID_ZEAL, 0);

    var Shield := GetEquipmentByLayer(who, LAYER_HAND1);
    if(Shield.objtype != UOBJ_DK_SHIELD)
      Shield := GetEquipmentByLayer(who, LAYER_HAND2);
    endif

    if(Shield.objtype == UOBJ_DK_SHIELD)
      MoveToPack(who, Shield);
    endif
    SendSysMessage(who, "Zbezczescil"+ggm(who,3)+" swe imie! Nie jestes warty godla mrocznego rycerza!", FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckFaithEquip(who, Item)

  var SkPal  := GetBaseSkill(who, SKILLID_PALADINISM);
  var SkFan  := GetBaseSkill(who, SKILLID_FANATICISM);

  if(!SkPal and !SkFan)
    return 1;
  endif

  if(Item.isa(POLCLASS_ARMOR))
    var ItemCfg := ReadConfigFile(ITEMS_CFG);

    if(ItemCfg[Item.objtype].Cloth or ItemCfg[Item.objtype].blockcircle == 8)
      return 1;
    endif

    if(ItemCfg[Item.objtype].Leather or ItemCfg[Item.objtype].Studded or ItemCfg[Item.objtype].Ringmail)
      SendSysMessage(who, "Zalozyc mozesz jedynie zbroje kolcze i plytowe.", FONT_NORMAL, COLOR_RED);
      return 0;
    endif
  endif

  if(GetItemMakeMod(Item))
    case(GetIngObjByColor(Item.color))
      default:                    SendSysMessage(who, "Nie mozesz zalozyc przedmiotu wykonanego z tego materialu!", FONT_NORMAL, COLOR_RED);
                                  return 0;

      UOBJ_IRON_INGOT:
      UOBJ_SILVER_INGOT:
      UOBJ_ROYAL_INGOT:
      UOBJ_MYSTIC_INGOT:          break;

      UOBJ_GOLDM_INGOT:
      UOBJ_AGAPITE_INGOT:
      UOBJ_VALORITE_INGOT:
      UOBJ_MYTHERIL_INGOT:
      UOBJ_AZURITE_INGOT:         if(!SkPal)
                                    SendSysMessage(who, "Nie mozesz zalozyc przedmiotu wykonanego z tego materialu!", FONT_NORMAL, COLOR_RED);
                                    return 0;
                                  endif
        
      UOBJ_BLACKROCK_INGOT:
      UOBJ_VERITE_INGOT:
      UOBJ_BLOODROCK_INGOT:       if(!SkFan)
                                    SendSysMessage(who, "Nie mozesz zalozyc przedmiotu wykonanego z tego materialu!", FONT_NORMAL, COLOR_RED);
                                    return 0;
                                  endif
    endcase
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetPaladinShield(who)

  var Shield := GetEquipmentByLayer(who, LAYER_HAND2);
  if(Shield.objtype != UOBJ_PD_SHIELD or !Shield)
    Shield := GetEquipmentByLayer(who, LAYER_HAND1);
  endif

  if(Shield and Shield.objtype == UOBJ_PD_SHIELD)
    return Shield;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetDarkKnightShield(who)

  var Shield := GetEquipmentByLayer(who, LAYER_HAND2);
  if(Shield.objtype != UOBJ_DK_SHIELD or !Shield)
    Shield := GetEquipmentByLayer(who, LAYER_HAND1);
  endif

  if(Shield and Shield.objtype == UOBJ_DK_SHIELD)
    return Shield;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
