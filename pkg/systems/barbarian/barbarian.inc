//-----------------------------------------------------------------------------

function CheckBarbEquip(who, Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(Item.isa(POLCLASS_WEAPON))

    if(ItemCfg[Item.objtype].Attribute != ATTRIBUTEID_AXESMANSHIP and ItemCfg[Item.objtype].Attribute != ATTRIBUTEID_MACEFIGHTING and !ItemCfg[Item.objtype].NoCombatSkillGain)
      SendSysMessage(who, "Nie mozesz zalozyc tej broni.", FONT_NORMAL, COLOR_RED);
      return 0;
    endif

  elseif(Item.isa(POLCLASS_ARMOR))

    if(ItemCfg[Item.objtype].Shield)
      if(Item.objtype != 0x1B78 and Item.objtype != 0x1B7A)
        SendSysMessage(who, "Nie mozesz zalozyc tej tarczy.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    elseif((!ItemCfg[Item.objtype].Cloth and !ItemCfg[Item.objtype].Wild and !ItemCfg[Item.objtype].Leather and !ItemCfg[Item.objtype].Studded and !ItemCfg[Item.objtype].BoneArmor) or ItemCfg[Item.objtype].Kensai)
      if((Item.objtype < 0x9010 or Item.objtype > 0x9019) and Item.objtype != 0x204E) // daemon & balron armors, deathshroud
        SendSysMessage(who, "Nie mozesz zalozyc tej zbroi.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
