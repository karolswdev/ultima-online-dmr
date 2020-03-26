//-----------------------------------------------------------------------------

const CRAFT_RECP_MIN_LEVEL   := 3;
const CRAFT_RECP_BASE_COST   := 5000;
const CRAFT_RECP_DIFF_COST   := 20000;

const PROP_CRAFT_RECEPTURE   := "CraftRecep";

//-----------------------------------------------------------------------------

function GetCraftReceptures(who, SkillID)

  var List := GetObjProperty(who, "CraftRecp_"+SkillID);

  if(!List or !IsVarType(List, VAR_ARRAY))
    List := array;
  endif

  return List;

endfunction

//-----------------------------------------------------------------------------

function HaveSkillReceptures(SkillID)

  var SkElem := GetSkillConfigElem(SkillID);

  if(SkElem.CraftReceptures)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetCraftReceptures(who, SkillID, byref List)

  SetObjProperty(who, "CraftRecp_"+SkillID, List);

endfunction

//-----------------------------------------------------------------------------

function AddAllCraftReceptures(who, SkillID)

  var List := GetCraftReceptures(who, SkillID);
  var AvailReceps := GetAvailCraftReceptures(SkillID);
  var n := 0;

  foreach Recep in AvailReceps

    if(!(Recep.objtype in List))
      n := n + 1;
      List.append(Recep.objtype);
    endif

  endforeach

  if(n > 0)
    SendSysMessage(who, "Poznal"+ggm(who,GGM_AS)+" "+n+" receptur na przedmioty umiejetnosci "+GetSkillConfigElem(SkillId).Name+".", FONT_NORMAL, COLOR_WHITE);
    SetCraftReceptures(who, SkillID, List);
  endif

  return n;

endfunction

//-----------------------------------------------------------------------------

function DelAllCraftReceptures(who, SkillID)

  var List := GetCraftReceptures(who, SkillID);
  var n := List.size();

  if(n > 0)
    SendSysMessage(who, "Nie znasz juz "+n+" receptur na przedmioty umiejetnosci "+GetSkillConfigElem(SkillId).Name+".", FONT_NORMAL, COLOR_ORANGE);
    List := array;
    SetCraftReceptures(who, SkillID, List);
  endif

  return n;

endfunction

//-----------------------------------------------------------------------------

function AddCraftRecepture(who, SKillID, ItemObj)

  var List := GetCraftReceptures(who, SkillID);

  if(!(ItemObj in List))
    var RecpData := GetCraftReceptureData(SkillID, ItemObj);
    SendSysMessage(who, "Poznal"+ggm(who,GGM_AS)+" recepture na przedmiot "+ClStr(RecpData.desc)+" umiejetnosci "+GetSkillConfigElem(SkillId).Name+".", FONT_NORMAL, COLOR_WHITE);
    List.append(ItemObj);
  else
    return 0;
  endif

  SetCraftReceptures(who, SkillID, List);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function DelCraftRecepture(who, SKillID, ItemObj)

  var List := GetCraftReceptures(who, SkillID);
  var Idx := RetIndex(List, ItemObj);

  if(Idx)
    var RecpData := GetCraftReceptureData(SkillID, ItemObj);
    SendSysMessage(who, "Nie znasz juz receptury na przedmiot "+ClStr(RecpData.desc)+" umiejetnosci "+GetSkillConfigElem(SkillId).Name+".", FONT_NORMAL, COLOR_ORANGE);
    List.erase(Idx);
  else
    return 0;
  endif

  SetCraftReceptures(who, SkillID, List);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetCraftReceptureCost(cfgElem, SkElem)

  var Cost := CRAFT_RECP_BASE_COST;
  var Diff := GetConfigInt(cfgElem, SkElem.CraftPropDiff) - 50;
  Diff := MinMax(Diff, 0, 50) * 2;

  Cost += CInt(CRAFT_RECP_DIFF_COST * Diff / 100);

  if(SkElem.CraftRecepMod)
    Cost := CInt(CDbl(SkElem.CraftRecepMod) * Cost);
  endif
  
  return Cost;

endfunction

//-----------------------------------------------------------------------------

function DefineReceptureFromElem(byref rItem, SkElem, Elem, ItemObj)
  
  rItem := struct;
  rItem.+objtype := CInt(ItemObj);

  if(SkElem.CraftPropDesc)
    rItem.+desc    := GetConfigString(Elem, SkElem.CraftPropDesc);
  else
    rItem.+desc    := GetDescForItem(ItemObj);
  endif

  var ObjType;

  if(SkElem.CraftPropProduct)
    ObjType := SplitWords(GetConfigString(Elem, SkElem.CraftPropProduct));
    ObjType := GetItemObjType(ObjType[1]);
    rItem.+graphic := GetGraphicForItem(ObjType);
  else
    ObjType := ItemObj;
    rItem.+graphic := GetGraphicForItem(ItemObj);
  endif

  rItem.+color   := GetColorForItem(ObjType);
  rItem.+level   := CInt(Elem.CraftLevel);
  rItem.+cost    := GetCraftReceptureCost(Elem, SkElem);
  rItem.+gX      := CInt(Elem.GumpX);
  rItem.+gY      := CInt(Elem.GumpY);

endfunction

//-----------------------------------------------------------------------------

function GetCraftItemScrollsList(SkillId)

  var Configs := GetCraftConfigs(SkillID);

  var cfgFile, Items;
  var List := array;

  foreach cfgName in Configs
    cfgFile := ReadConfigFile(cfgName);
    Items := GetConfigStringKeys(cfgFile);

    foreach ItemObj in Items
      if(cfgFile[ItemObj].ItemScroll)
        var rItem := struct;
        rItem.+SkillID := SkillID;
        rItem.+ObjType := ItemObj;
        rItem.+SkillReq := CInt(cfgFile[ItemObj].ScrollSkillReq);

        List.append(rItem);
      endif
    endforeach
  endforeach

  return List;

endfunction

//-----------------------------------------------------------------------------

function GetAvailCraftReceptures(SkillID, pShowItemScroll := 0)

  var Configs := GetCraftConfigs(SkillID);
  var SkElem := GetSkillConfigElem(SkillID);

  var cfgFile, Items;
  var List := array;

  foreach cfgName in Configs
    cfgFile := ReadConfigFile(cfgName);
    Items := GetConfigStringKeys(cfgFile);

    foreach ItemObj in Items
      if((!cfgFile[ItemObj].ItemScroll or pShowItemScroll) and cfgFile[ItemObj].CraftLevel and CInt(cfgFile[ItemObj].CraftLevel) >= CRAFT_RECP_MIN_LEVEL)
        var rItem;
        DefineReceptureFromElem(rItem, SkElem, cfgFile[ItemObj], ItemObj);
        List.append(rItem);
      endif
    endforeach
  endforeach

  return List;

endfunction

//-----------------------------------------------------------------------------

function GetCraftReceptureData(SkillID, ItemObj)

  var Configs := GetCraftConfigs(SkillID);
  var SkElem := GetSkillConfigElem(SkillID);

  var cfgFile;

  foreach cfgName in Configs
    cfgFile := ReadConfigFile(cfgName);

    if(cfgFile[ItemObj])
      if(cfgFile[ItemObj].CraftLevel and CInt(cfgFile[ItemObj].CraftLevel) >= CRAFT_RECP_MIN_LEVEL)
        var rItem;
        DefineReceptureFromElem(rItem, SkElem, cfgFile[ItemObj], ItemObj);
        return rItem;
      endif
    endif

  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
