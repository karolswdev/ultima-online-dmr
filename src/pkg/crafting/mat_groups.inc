//--------------------------------------------------------------------------------

const SCMGF_FORCE_CHOOSE := 0x01;

//--------------------------------------------------------------------------------

function GetCraftMatGroupConfig(Name)

  var Cfg := ReadConfigFile(":crafting:mat_groups");

  return Cfg[Name];

endfunction

//--------------------------------------------------------------------------------

function GetCraftMatGroupObjTypes(Name)

  var cfgElem := GetCraftMatGroupConfig(Name);
  var List := GetConfigStringArray(cfgElem, "ObjType");
  var ObjType;
  
  if(!List)
    LogError("Probowano odczytac niezdefiniowana grupe materialow '"+Name+"' configu :crafting:mat_groups!");
    return error;
  endif

  foreach ObjName in List
    ObjType := GetItemObjType(ObjName);

    if(!ObjType)
      LogError("Nie zdefiniowany objtype '"+ObjName+"' w grupie '"+Name+"' configu :crafting:mat_groups!");
      return error;
    else
      ObjName := ObjType;
    endif

  endforeach

  if(!List.size())
    LogError("Brak zdefiniowanych objtypow w grupie '"+Name+"' configu :crafting:mat_groups!");
    return error;
  endif

  return List;

endfunction

//--------------------------------------------------------------------------------

function SelectCraftMatFromGroup(who, GroupName, Flags := 0)

  var ObjType := GetItemObjType(GroupName);
  var ObjTypes, Message;

  if(ObjType)
    ObjTypes := array(ObjType);
    Message := GetDescForItem(ObjType)+", ktore";
  else
    ObjTypes := GetCraftMatGroupObjTypes(GroupName);
    var cfgElem  := GetCraftMatGroupConfig(GroupName);
    Message := ClStr(cfgElem.Message);
  endif

  if(!ObjTypes)
    return ObjTypes;
  endif

  if(!Message)
    Message := "material, ktory";
  endif

  if(ObjTypes.size() > 1 or IsBitOn(Flags, SCMGF_FORCE_CHOOSE))

    SendSysMessage(who,"Wybierz "+ClStr(Message)+" chcesz uzyc.",FONT_NORMAL, COLOR_GREEN);
    var TgtMat   := ObjTarget(who);

    if(!StandardItemCheck(who, TgtMat))
      return 0;
    endif

    if(!(TgtMat.objtype in ObjTypes))
      SendSysMessage(who,"To zly material.", FONT_NORMAL, COLOR_RED);
      return 0;
    endif
    
    return TgtMat;

  else
    return ObjTypes[1];
  endif
  
  return 0;

endfunction

//--------------------------------------------------------------------------------
// Predefiniowane funkcje do pobierania grup objtypow
//--------------------------------------------------------------------------------

function GetGemsObjTypes()

  return GetCraftMatGroupObjTypes("gems");

endfunction

//--------------------------------------------------------------------------------

function GetIngotsObjTypes()

  return GetCraftMatGroupObjTypes("ingots");

endfunction

//--------------------------------------------------------------------------------

function GetOresObjTypes()

  return GetCraftMatGroupObjTypes("ores");

endfunction

//--------------------------------------------------------------------------------
