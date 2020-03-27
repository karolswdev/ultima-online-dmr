//--------------------------------------------------------------------------------

include "include/tempmods";

//================================================================================
//--------------------------------------------------------------------------------
// Names & Descriptions
//--------------------------------------------------------------------------------
//================================================================================

const NAME_FULL           := 0x00;
const NAME_CLEAN_AMOUNT   := 0x01;

//--------------------------------------------------------------------------------

function GetName(Object , Param := NAME_FULL)
  
  var Name := "";
  if(Object.isa(POLCLASS_ITEM))
    Name := Object.desc;  
  elseif(Object.name)
    Name := Object.name;
  endif

  if(Object.amount and Param & NAME_CLEAN_AMOUNT)
    Name :=  CutDesc(Name);
  endif
 
  return name;
endfunction

//-----------------------------------------------------------------------------

function GetRealName(who)

  if(CanMod(who, MOD_INCO) == 0)
    return ClStr(GetModOldAmount(who, MOD_INCO));
  else
    return GetName(who);
  endif

endfunction

//--------------------------------------------------------------------------------

function GetPlayerDesc(who)

  if(!who)
    return "Brak";
  endif

  return GetRealName(who)+" ["+who.serial+"/"+who.acctname+"]";

endfunction

//--------------------------------------------------------------------------------
// ucina nazwe z ang. przedrostka a lub an
//--------------------------------------------------------------------------------

function CutDesc(Desc)

  if(!Desc)
    return "";
  endif

  if(Desc[1,2] == "a ")
    Desc[1,2] := "";
  elseif(Desc[1,3] == "an ")
    Desc[1,3] := "";
  endif

  if(CInt(Desc))
    Desc[CInt(Desc) + " "] := "";
  endif

  return Desc;

endfunction

//--------------------------------------------------------------------------------
// zwraca pelna nazwe przedmiotu, obcieta z przedrostkow z dodanym przyrostkiem odpowiednim dla ilosci
//--------------------------------------------------------------------------------

function CutItemDesc(Item)

  var Desc := CutDesc(Item.desc);

  if(Desc)
    if(Item.amount > 1)
      var ItemCfg := ReadConfigFile(ITEMS_CFG);
      var Suffix  := GetDescSuffix(ItemCfg[Item.objtype].desc);
      if(Suffix)
        var Pos := find(ItemCfg[Item.objtype].desc, "%", 1);
        if(Desc[Pos, len(Suffix)] == Suffix)
          Desc[Pos, len(Suffix)] := GetDescSuffix(ItemCfg[Item.objtype].desc, GDFIF_ONE_AMT);
        endif
      endif
    endif
  endif

  return Desc;

endfunction

//--------------------------------------------------------------------------------
// Description For Item
//--------------------------------------------------------------------------------

const GDFIF_DONT_CUT         := 0x01;
const GDFIF_ADD_TYPE         := 0x02;
const GDFIF_ONE_AMT          := 0x04;

//--------------------------------------------------------------------------------

function GetDescForItem(Obj, Flags := 0)

  if(!Obj)
    return "";
  endif

  if(Obj.isa(POLCLASS_ITEM))
    Obj := Obj.graphic;
  elseif(!CInt(Obj))
    Obj := GetObjTypeByName(Obj);
  else
    Obj := CInt(Obj);
  endif

  if(!Obj)
    return "";
  endif

  var mItemCfg := ReadConfigFile(ITEMS_CFG);
  var Desc     := mItemCfg[obj].desc;

  if(Desc == error)
    return "";
  endif

  if(Flags & GDFIF_DONT_CUT)
    return Desc;
  endif

  ClearItemDescSuffix(Desc, Flags);

  if(Flags & GDFIF_ADD_TYPE)
    if(mItemCfg[Obj].ItemTypeName)
      Desc := Desc + " ("+mItemCfg[Obj].ItemTypeName+")";
    endif
  endif

  return Desc;

endfunction

//--------------------------------------------------------------------------------

function ClearItemDescSuffix(byref Desc, Flags := 0)

  Desc["%%"] := "";

  var Pos := find(Desc, "%", 1);
  if(Pos)
    var mPos := find(Desc, "/", Pos+1);
    var ePos := find(Desc, "%", Pos+1);

    if(!ePos)
      Desc["%"] := "";
    elseif(mPos)
      if(Flags & GDFIF_ONE_AMT)
        Desc[Pos, ePos-Pos+1] := Desc[mPos+1, ePos-mPos-1];
      else
        Desc[Pos, ePos-Pos+1] := Desc[Pos+1, mPos-Pos-1];
      endif
    else
      Desc[Pos, ePos-Pos+1] := Desc[Pos+1, ePos-Pos-1];
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function GetDescSuffix(byref Desc, Flags := 0)

  var Pos := find(Desc, "%", 1);

  if(Pos)
    var mPos := find(Desc, "/", Pos+1);
    var ePos := find(Desc, "%", Pos+1);

    if(Pos == ePos-1 or !ePos)
      return "";
    elseif(mPos)
      if(Flags & GDFIF_ONE_AMT)
        return Desc[mPos+1, ePos-mPos-1];
      else
        return "";
      endif
    elseif(!(Flags & GDFIF_ONE_AMT))
      return Desc[Pos+1, ePos-Pos-1];
    endif
  endif

  return "";

endfunction

//--------------------------------------------------------------------------------
//Get End Prefix Word For Item
//--------------------------------------------------------------------------------

function GewForItem(itemobj)

  if(!itemobj)
    return "e";
  endif

  var mItemCfg := ReadConfigFile(ITEMS_CFG);
  var ChName   := mItemCfg[itemobj].ChName;

  if(!ChName)
    ChName := "e";
  endif

  return ChName;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Item Names
//--------------------------------------------------------------------------------
//================================================================================

//--------------------------------------------------------------------------------
// Desc Prefix
//--------------------------------------------------------------------------------

function GetDpsType(Obj)

  var mItemCfg := ReadConfigFile(ITEMS_CFG);
  var DpsType  := mItemCfg[Obj].PreNameType;

  return ClStr(DpsType);

endfunction

//--------------------------------------------------------------------------------

function GetDps(UObj,WObj := 0)

  var mItemCfg := ReadConfigFile(ITEMS_CFG);
  var PreName  := mItemCfg[UObj].PreName;

  var PreType := "";

  if(!PreName)
    return "";
  endif

  if(mItemCfg[UObj].AddPNType and WObj)
    PreType := GewForItem(WObj);
  endif

  return ClStr(PreName) + PreType;

endfunction

//--------------------------------------------------------------------------------

function GetMatDescForItem(Obj, MatObj)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Desc    := GetDescForItem(Obj);
  var iType   := ItemCfg[Obj].ItemTypeName;

  if(iType)
    Desc := iType+" "+Desc;
  endif

  if(MatObj)
    var Dps     := GetDps(MatObj, Obj);
    var DpsType := GetDpsType(MatObj);

    if(Dps and DpsType)
      Desc := Desc+" "+DpsType+" "+Dps;
    elseif(Dps)
      Desc := Dps+" "+Desc;
    endif
  endif

  return Desc;

endfunction

//--------------------------------------------------------------------------------

const TT_STRING := " [Wlasciwosci:";
const PROP_TT_DICT := "ToolTipDict";

//--------------------------------------------------------------------------------

function GetToolTipProp(Object, PropName)
  
  PropName := CStr(PropName);
  
  var dict := GetObjProperty(Object, PROP_TT_DICT);
  
  if(dict[PropName])
    return dict[PropName];  
  endif

  var ret := error;
  ret.errortext := "Prop dont exist";
  return ret;
  
endfunction


function GetToolTipPropOld(Object, PropName)
  
  PropName := CStr(PropName);
  var ret := error;

  if(!object or !PropName or PropName == "")
    ret.+text := "Blad w parametrach";
    return ret;
  endif
    
  var Name := GetName(Object);
  var StartIndex := find(Name,TT_STRING,1);
  var Length;
  if(!StartIndex)
    ret.+text := "Nie ma w ogole ToolTipProps";
    return ret;
  endif
 
  StartIndex := find(Name, PropName, StartIndex); 
 
  if(!StartIndex)
    ret.+text := "Nie ma ToolTipProp: " + PropName;
    return ret;   
  endif
   
  StartIndex := find(Name, ":", StartIndex);
  
  if(!StartIndex)
    ret.+text := "Blad wewnatrz ToolTipProp: " + PropName;
    return ret;   
  endif
  
  StartIndex := StartIndex + 1;
  Length := find(Name, " ", StartIndex);
  
  if(!Length)
    Length := find(Name, "]", StartIndex); 
    if(!Length)
      ret.+text := "Brak konca ToolTipProp: " + PropName;
      return ret;
    endif
  endif

  Length := Length - StartIndex;
  
  return Name[StartIndex,Length];
    
endfunction

//--------------------------------------------------------------------------------

function SetToolTipProp(Object, Name, Value)
  
  Name := CStr(Name);

  var dict := GetObjProperty(Object, PROP_TT_DICT);
  
  if(!dict)
    dict := dictionary;
  endif
  
  dict[Name] := Value;

  var ret := SetObjProperty(Object, PROP_TT_DICT, dict);
  IncRevision(Object);
  return ret;

endfunction


function SetToolTipPropOld(Object, PropName, PropValue)
  
  PropName := CStr(PropName);
  PropValue := StrReplace(CStr(PropValue), " ", "_");
  if(!object or !PropName or !PropValue or PropName == "")
    return 0;
  endif
    
  var Name := GetName(Object,NAME_CLEAN_AMOUNT);
  var StartIndex := find(Name,TT_STRING,1);
  if(!StartIndex)
    Name := Name + TT_STRING + "]";
    StartIndex := find(Name,TT_STRING,1);
  endif
  StartIndex := StartIndex + len(TT_STRING);
  var Length := find(Name,"]",StartIndex) - StartIndex;
  var Props := Name[StartIndex,Length];
  
  var StartPropIndex := Find(Props,PropName, 1);
  var StartPropValueIndex;
  var LengthPropValue;
  if(StartPropIndex)
    StartPropValueIndex := Find(Props,":",StartPropIndex) +1;
    Length := Find(Props," ", StartPropValueIndex);
    if(!Length)
      Length := len(Props);
    endif
    LengthPropValue := Length - StartPropValueIndex + 1;
    Props[StartPropValueIndex,LengthPropValue] := PropValue;
  else
    Props := Props + " " + PropName + ":" + PropValue;
  endif
 
  Name[StartIndex,Length] := Props; 

  SetName(Object,Name + "%%");
  return 1;
endfunction

//--------------------------------------------------------------------------------

function EraseToolTipProp(Object, Name)
  
  Name := CStr(Name);
  
  var ret := error;
  var dict := GetObjProperty(Object, PROP_TT_DICT);
  
  if(!dict[Name])
    ret.errortext := "Prop does not exist";
    return ret;
  endif
  
  dict.erase(Name);

  if(dict.size())
    ret := SetObjProperty(Object, PROP_TT_DICT, dict);
  else
    ret := EraseObjProperty(Object, PROP_TT_DICT);
  endif
    
  IncRevision(Object);
  return ret;

endfunction

function EraseToolTipPropOld(Object, PropName)
  
  PropName := CStr(PropName);
 
  if(!object or !PropName)
    return 0;
  endif
    
  var Name := GetName(Object,NAME_CLEAN_AMOUNT);
  var StartIndex := find(Name,TT_STRING,1);
  if(!StartIndex)
    return 0;
  endif
  StartIndex := StartIndex + len(TT_STRING);
  var Length := find(Name,"]",StartIndex) - StartIndex;
  var Props := Name[StartIndex,Length];
  var StartPropIndex := Find(Props,PropName, 1);
  var LengthProp;
  if(StartPropIndex)
    LengthProp := Find(Props," ", StartPropIndex);
    StartPropIndex := StartPropIndex - 1;
    if(!LengthProp)
      LengthProp := len(Props) + 1;
    endif
    LengthProp := LengthProp - StartPropIndex;
    
    Props[StartPropIndex,LengthProp] := "";
  else
    return 0;
  endif

  if(Len(Props))
    Name[StartIndex,Length] := Props; 
    SetName(Object,Name + "%%");
  else
    EraseToolTipOld(Object);
  endif
  return 1;
endfunction

//--------------------------------------------------------------------------------

function EraseToolTipOld(object)
  
  var Name := GetName(Object, NAME_CLEAN_AMOUNT);
  var StartIndex := find(Name, TT_STRING,1);
  if(!StartIndex)
    return 0;
  endif
  var Length := Find(Name,"]", StartIndex) - StartIndex + 1;
  Name[StartIndex,Length] := "";
  SetName(Object,Name + "%%");
  return 1;
endfunction

//--------------------------------------------------------------------------------

function ExtractToolTipProp(Object, byref Dict)
  
  Dict := GetObjProperty(Object, PROP_TT_DICT);

endfunction

function ExtractToolTipPropOld(Object, byref PropDict)
  
  var Name        := GetName(Object);
  var StartIndex  := find(Name,TT_STRING,1);

  if(!StartIndex)
    return 0;
  endif

  StartIndex := StartIndex + len(TT_STRING);
  var Length := find(Name,"]",StartIndex) - StartIndex;
  var Props := Name[StartIndex,Length];

  var SplitedProp;
  foreach Prop in SplitWords(Props)
    SplitedProp := SplitWords(Prop ,":");
    PropDict[SplitedProp[1]] := StrReplace(SplitedProp[2], "_", " ");
  endforeach
 
  return 1;
endfunction

//--------------------------------------------------------------------------------

function ImpactToolTipProp(Object, byref Dict)
    
    SetObjProperty(Object, PROP_TT_DICT, Dict);
    IncRevision(Object);

endfunction

function ImpactToolTipPropOld(Object, byref PropDict)
 
  if(!object)
    return 0;
  endif
    
  var Name := GetName(Object, NAME_CLEAN_AMOUNT);
  var StartIndex := find(Name,TT_STRING,1);
  if(!StartIndex)
    Name := Name + TT_STRING + "]";
    StartIndex := find(Name,TT_STRING,1);
  endif
  StartIndex := StartIndex + len(TT_STRING);
  var Length := find(Name,"]",StartIndex) - StartIndex;
  
  var Props := "";
  foreach Prop in PropDict
    Props := Props + _Prop_iter + ":" + CStr(Prop) + " ";
  endforeach
  Props := Props[1, len(Props)-1];
  Name[StartIndex,Length] := Props; 

  SetName(Object,Name + "%%");
  return 1;

endfunction

//--------------------------------------------------------------------------------

function CleanToolTipOld(name)
  
  var StartIndex := Find(Name, TT_STRING, 1);

  if(StartIndex)
    var Length := Find(Name, "]", StartIndex) - StartIndex + 1;
    Name[StartIndex,Length] := "";
  endif

  return name;

endfunction

//--------------------------------------------------------------------------------
