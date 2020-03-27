//-----------------------------------------------------------------------------

include ":npcsets:shared";
include ":npcsets:definition";

//-----------------------------------------------------------------------------

const NSGLOBAL_DATA          := ":npcsets:global";
const NSGTIT_GROUP           := "Gr_";
const NSGTIT_SET             := "Ns_";

//-----------------------------------------------------------------------------

function GetNsgGroup(gName)

  if(!gName)
    return error{"errortext" := "Nie przekazano nazwy grupy."};
  endif

  var Data := GetNsgData();

  return Data.FindElement(NSGTIT_GROUP + gName);

endfunction

//-----------------------------------------------------------------------------

function GetNsgGroups()

  var Data := GetNsgData();

  if(!Data)
    return Data;
  endif

  var Keys := Data.keys();
  var Groups := array;

  foreach Key in Keys
    if(Key[1,3] == NSGTIT_GROUP)
      Key[1,3] := "";
      Groups.append(Key);
    endif
  endforeach

  return Groups;

endfunction

//-----------------------------------------------------------------------------

function GetNsgs(gName)

  var Data := GetNsgData();

  if(!Data)
    return Data;
  endif

  var Keys := Data.keys();
  var Sets := array;

  foreach Key in Keys
    if(Key[1,3] == NSGTIT_SET)
      var Elem := Data.FindElement(Key);

      if(Elem.GetProp("Group") == gName)
        Key[1,3] := "";
        Sets.append(Key);
      endif
    endif
  endforeach

  return Sets;

endfunction

//-----------------------------------------------------------------------------

function GetNsg(isName)

  if(!isName)
    return error{"errortext" := "Nie przekazano nazwy seta."};
  endif

  var Data := GetNsgData();

  return Data.FindElement(NSGTIT_SET + isName);

endfunction

//-----------------------------------------------------------------------------

function GetNsgData()

  var Data := OpenDataFile(NSGLOBAL_DATA);

  if(!Data)
    Data := CreateDataFile(NSGLOBAL_DATA);
  endif

  if(!Data)
    LogError("GetWmsData: Nie udalo sie stworzyc NSGLOBAL_DATA ["+NSGLOBAL_DATA+"]: "+Data.errortext);
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function DelNsgGroup(gName)

  if(!gName)
    return RetError("Nie przekazano nazwy grupy!");
  endif

  var Data := GetNsgData();

  if(!Data)
    return Data;
  endif

  if(!GetNsgGroup(gName))
    return RetError("Grupa o nazwie "+gName+" nie istnieje!");
  endif

  var NsgSets := GetNsgs(gName);

  if(NsgSets.size())
    return RetError("Nie mozna usunac grupy "+gName+" poniewaz istnieje "+NsgSets.size()+" setow w niej!");
  endif

  return Data.DeleteElement(NSGTIT_GROUP + gName);

endfunction

//-----------------------------------------------------------------------------

function AddNsgGroup(who, gName)

  if(!gName)
    return RetError("Nie przekazano nazwy grupy!");
  endif

  var Data := GetNsgData();

  if(!Data)
    return Data;
  endif

  if(GetNsgGroup(gName))
    return RetError("Grupa o nazwie "+gName+" juz istnieje.");
  endif

  var Group := Data.CreateElement(NSGTIT_GROUP + gName);

  if(Group)
    Group.SetProp("Creator", who.acctname);
    Group.SetProp("AddTime", PolCore().systime);
    Group.SetProp("Name", gName);
  endif

  return Group;

endfunction

//-----------------------------------------------------------------------------

function DelGlobalSet(Name)

  var Data := GetNsgData();

  if(!Data)
    return Data;
  endif

  if(!GetNsg(Name))
    return RetError("Set o nazwie "+Name+" nie istnieje.");
  endif

  var Res := Data.DeleteElement(NSGTIT_SET + Name);

  return Res;

endfunction

//-----------------------------------------------------------------------------

function AddGlobalSet(Name, NsgData, gName)

  var Elem := GetNsg(Name);

  if(!Elem)
  
    var Data := GetNsgData();

    if(!Data)
      return Data;
    endif

    Elem := Data.CreateElement(NSGTIT_SET + Name);
  endif

  if(!Elem)
    return Elem;
  endif
  
  Elem.SetProp("Group", gName);
  Elem.SetProp("Name", NsgData.Name);
  Elem.SetProp("Type", NsgData.type);
  Elem.SetProp("AddTime", NsgData.addTime);
  Elem.SetProp("Creator", NsgData.Creator);
  Elem.SetProp("NpcCount", NsgData.Count);
  Elem.SetProp("Npcs", NsgData.Npcs);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetNsgDataStruct(Name)

  var NsgElem := GetNsg(Name);

  if(!NsgElem)
    return NsgElem;
  endif

  var NsgData := NpcSetStruct();

  NsgData.+Group  := NsgElem.GetProp("Group");
  NsgData.Name    := NsgElem.GetProp("Name");
  NsgData.type    := NsgElem.GetProp("Type");
  NsgData.Creator := NsgElem.GetProp("Creator");
  NsgData.addTime := NsgElem.GetProp("AddTime");
  NsgData.Count   := NsgElem.GetProp("NpcCount");
  NsgData.Npcs   := NsgElem.GetProp("Npcs");

  return NsgData;
  
endfunction

//-----------------------------------------------------------------------------
