//--------------------------------------------------------------------------------

function GetMaxMaterialMod(MatObj)

  if(!MatObj)
    return 0;
  endif

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);

  // jak ma MatMod to jest skora.
  if(ItemCfg[MatObj].MatMod)
    return 7; // mistyk
  endif

  var SmithCfg  := ReadConfigFile(":blacksmithy:materials");

  if(SmithCfg[MatObj].ArMod)
    return 16; // mistyk
  endif

  var ObjTypes := GetLogObjTypes();

  if(MatObj in ObjTypes)
    return ObjTypes.size();
  endif

  ObjTypes := GetBoardObjTypes();

  if(MatObj in ObjTypes)
    return ObjTypes.size();
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetMaterialModPercent(MatObj)

  var CurMod := GetMaterialMod(MatObj);
  var MaxMod := GetMaxMaterialMod(MatObj);

  if(CurMod and MaxMod)
    return CDbl(CurMod) / CDbl(MaxMod);
  endif

  return 0.0;

endfunction

//--------------------------------------------------------------------------------

function GetMaterialMod(MatObj)

  if(!MatObj)
    return 0;
  endif

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[MatObj].MatMod)
    return CInt(ItemCfg[MatObj].MatMod);
  endif

  var SmithCfg  := ReadConfigFile(":blacksmithy:materials");

  if(SmithCfg[MatObj].ArMod)
    return CInt(SmithCfg[MatObj].ArMod);
  endif

  var ObjTypes, Idx;

  ObjTypes := GetLogObjtypes();
  Idx      := RetIndex(ObjTypes, MatObj);

  if(Idx)
    return Idx;
  endif

  ObjTypes := GetBoardObjtypes();
  Idx      := RetIndex(ObjTypes, MatObj);
  if(Idx)
    return Idx;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetCraftConfigForItem(SkillID, ObjType)

  var Configs := GetCraftConfigs(SkillID);
  var cfgFile;

  foreach cfgName in Configs
    cfgFile := ReadConfigFile(cfgName);

    if(cfgFile[ObjType])
      return cfgFile[ObjType];
    endif

  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetObjTypesForCraft(SkID)

  var SkElem := GetSkillConfigElem(SkID);
  var List := GetConfigStringArray(SkElem, "CraftConfig");
  var ObjTypes := array;
  var cfgFile;

  foreach cfgName in List
    cfgFile := ReadConfigFile(cfgName);

    if(SkElem.CraftPropProduct)
      foreach Id in GetConfigStringKeys(cfgFile)
        var Product := GetConfigString(cfgFile[Id], SkElem.CraftPropProduct);
        Product := SplitWords(Product);
        ObjTypes.append( GetItemObjType(Product[1]) );
      endforeach
    else
      foreach Obj in GetConfigStringKeys(cfgFile)
        ObjTypes.append(CInt(Obj));
      endforeach
    endif
    sleepms(1);
  endforeach

  return ObjTypes;

endfunction

//-----------------------------------------------------------------------------

function FindSkillIdForCraftItem(Item)

  var cfgFile, SkElem;
  var SkID;
  var cfgFile;

  for(SkID:=0; SkID<=SKILLID__HIGHEST; SkID+=1)
    SkElem := GetSkillConfigElem(SkID);

    if(SkElem.AttrGroup != "crafting")
      continue;
    endif

    var List := GetConfigStringArray(SkElem, "CraftConfig");

    foreach cfgName in List
      cfgFile := ReadConfigFile(cfgName);

      if(SkElem.CraftPropProduct)
        foreach Id in GetConfigStringKeys(cfgFile)
          var Product := GetConfigString(cfgFile[Id], SkElem.CraftPropProduct);
          Product := SplitWords(Product);
          if(GetItemObjType(Product[1]) == ObjType)
            return SkID;
          endif
        endforeach
      elseif(cfgFile[ObjType])
        return SkID;
      endif
      sleepms(1);
    endforeach
  endfor

  return error;

endfunction

//-----------------------------------------------------------------------------

function GetCraftDataForItem(ObjType, SkillId := -1)

  ObjType := CInt(ObjType);

  var maxId := __IfElse(SkillId == -1, GetMaxSkillId(), SkillId);
  var cfgFile, SkElem;
  var SkID;
  var Data := struct;

  for(SkID:=__IfElse(SkillId == -1, 0, SkillId); SkID<=maxId; SkID+=1)
    SkElem := GetSkillConfigElem(SkID);

    if(SkElem.AttrGroup != "crafting")
      continue;
    endif

    var List := GetConfigStringArray(SkElem, "CraftConfig");

    foreach cfgName in List
      cfgFile := ReadConfigFile(cfgName);

      if(SkElem.CraftPropProduct)
        foreach Id in GetConfigStringKeys(cfgFile)
          var Product := GetConfigString(cfgFile[Id], SkElem.CraftPropProduct);
          Product := SplitWords(Product);
          if(GetItemObjType(Product[1]) == ObjType)
            Data.+SkillID := SkId;
            Data.+SkElem := SkElem;
            Data.+cfgElem := cfgFile[Id];
            Data.+cfgFileName := cfgName;
            Data.+cfgFile := cfgFile;
            break;
          endif
        endforeach
      elseif(cfgFile[ObjType])
        Data.+SkillID := SkId;
        Data.+SkElem := SkElem;
        Data.+cfgElem := cfgFile[ObjType];
        Data.+cfgFileName := cfgName;
        Data.+cfgFile := cfgFile;
        break;
      endif
      sleepms(1);
    endforeach
  endfor

  if(Data.cfgElem)
    if(Data.SkElem.CraftGroupProp)
      var GroupCfg := ReadConfigFile(Data.SkElem.CraftGroups);
      var Group := GroupCfg[GetConfigString(Data.cfgElem, Data.SkElem.CraftGroupProp)];
      Data.+Diff := GetConfigInt(Group, Data.SkElem.CraftPropDiff);
    else
      Data.+Diff := GetConfigInt(Data.cfgElem, Data.SkElem.CraftPropDiff);
    endif
    Data.+Gain := GetCraftPoints(Data.SkillID, 100);
    return Data;
  endif

  return error;

endfunction 

//-----------------------------------------------------------------------------

function GetCraftConfigs(SkillID)

  var SkElem := GetSkillConfigElem(SkillID);

  return GetConfigStringArray(SkElem, "CraftConfig");

endfunction

//--------------------------------------------------------------------------------

function GetCraftObjTypes(SkillID, skMin, skMax)

  var Configs := GetCraftConfigs(SkillID);
  var SkElem := GetSkillConfigElem(SkillID);

  var cfgFile, Items;
  var List := array;
  var Diff, ObjType;

  foreach cfgName in Configs
    cfgFile := ReadConfigFile(cfgName);
    Items := GetConfigStringKeys(cfgFile);

    foreach ItemObj in Items
      Diff := GetConfigInt(cfgFile[ItemObj], SkElem.CraftPropDiff);

      if(Diff >= skMin and Diff <= skMax)
        if(SkElem.CraftPropProduct)
          ObjType := SplitWords(GetConfigString(cfgFile[ItemObj], SkElem.CraftPropProduct));
          ObjType := GetItemObjType(ObjType[1]);
        else
          ObjType := CInt(ItemObj);
        endif
        List.append(ObjType);
      endif

    endforeach
  endforeach

  return List;

endfunction

//-----------------------------------------------------------------------------

function GetCraftPoints(SkillID, Diff)

  var Points := GetDefaultPointsValue(SkillID);
  var Perc := 1.0 + CDbl(Diff) / 100.0;

  Points := CInt(Min(CDbl(Points) * Perc, Points*1.5));

  return Points;

endfunction

//--------------------------------------------------------------------------------
