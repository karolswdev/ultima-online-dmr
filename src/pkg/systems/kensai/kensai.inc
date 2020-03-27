//-----------------------------------------------------------------------------

function CheckKensaiEquip(who, Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(Item.isa(POLCLASS_WEAPON))

    if(!ItemCfg[Item.objtype].Kensai and !ItemCfg[Item.objtype].NoCombatSkillGain)
      SendSysMessage(who, "Nie mozesz zalozyc tej broni.", FONT_NORMAL, COLOR_RED);
      return 0;
    endif

  elseif(Item.isa(POLCLASS_ARMOR))

    if(ItemCfg[Item.objtype].Shield)
      SendSysMessage(who, "Nie mozesz zalozyc tarczy.", FONT_NORMAL, COLOR_RED);
      return 0;
    elseif(!ItemCfg[Item.objtype].Cloth and !ItemCfg[Item.objtype].Leather and !ItemCfg[Item.objtype].Studded and !ItemCfg[Item.objtype].Kensai)
      if(Item.objtype != 0x204E) // deathshroud
        SendSysMessage(who, "Nie mozesz zalozyc tej zbroi.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
