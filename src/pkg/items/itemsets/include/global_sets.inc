//-----------------------------------------------------------------------------

include ":itemsets:shared";
include ":itemsets:definition";

//-----------------------------------------------------------------------------

const ISGLOBAL_DATA          := ":itemsets:global";
const ISGTIT_GROUP           := "Gr_";
const ISGTIT_SET             := "Is_";

//-----------------------------------------------------------------------------

function GetIsgGroup(gName)

  if(!gName)
    return error{"errortext" := "Nie przekazano nazwy grupy."};
  endif

  var Data := GetIsgData();

  return Data.FindElement(ISGTIT_GROUP + gName);

endfunction

//-----------------------------------------------------------------------------

function GetIsgGroups()

  var Data := GetIsgData();

  if(!Data)
    return Data;
  endif

  var Keys := Data.keys();
  var Groups := array;

  foreach Key in Keys
    if(Key[1,3] == ISGTIT_GROUP)
      Key[1,3] := "";
      Groups.append(Key);
    endif
  endforeach

  return Groups;

endfunction

//-----------------------------------------------------------------------------

function GetIsgs(gName)

  var Data := GetIsgData();

  if(!Data)
    return Data;
  endif

  var Keys := Data.keys();
  var Sets := array;

  foreach Key in Keys
    if(Key[1,3] == ISGTIT_SET)
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

function GetIsg(isName)

  if(!isName)
    return error{"errortext" := "Nie przekazano nazwy seta."};
  endif

  var Data := GetIsgData();

  return Data.FindElement(ISGTIT_SET + isName);

endfunction

//-----------------------------------------------------------------------------

function GetIsgData()

  var Data := OpenDataFile(ISGLOBAL_DATA);

  if(!Data)
    Data := CreateDataFile(ISGLOBAL_DATA);
  endif

  if(!Data)
    LogError("GetWmsData: Nie udalo sie stworzyc ISGLOBAL_DATA ["+ISGLOBAL_DATA+"]: "+Data.errortext);
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function DelIsgGroup(gName)

  if(!gName)
    return RetError("Nie przekazano nazwy grupy!");
  endif

  var Data := GetIsgData();

  if(!Data)
    return Data;
  endif

  if(!GetIsgGroup(gName))
    return RetError("Grupa o nazwie "+gName+" nie istnieje!");
  endif

  var IsgSets := GetIsgs(gName);

  if(IsgSets.size())
    return RetError("Nie mozna usunac grupy "+gName+" poniewaz istnieje "+IsgSets.size()+" setow w niej!");
  endif

  return Data.DeleteElement(ISGTIT_GROUP + gName);

endfunction

//-----------------------------------------------------------------------------

function AddIsgGroup(who, gName)

  if(!gName)
    return RetError("Nie przekazano nazwy grupy!");
  endif

  var Data := GetIsgData();

  if(!Data)
    return Data;
  endif

  if(GetIsgGroup(gName))
    return RetError("Grupa o nazwie "+gName+" juz istnieje.");
  endif

  var Group := Data.CreateElement(ISGTIT_GROUP + gName);

  if(Group)
    Group.SetProp("Creator", who.acctname);
    Group.SetProp("AddTime", PolCore().systime);
    Group.SetProp("Name", gName);
  endif

  return Group;

endfunction

//-----------------------------------------------------------------------------

function DelGlobalSet(Name)

  var Data := GetIsgData();

  if(!Data)
    return Data;
  endif

  if(!GetIsg(Name))
    return RetError("Set o nazwie "+Name+" nie istnieje.");
  endif

  var Res := Data.DeleteElement(ISGTIT_SET + Name);

  return Res;

endfunction

//-----------------------------------------------------------------------------

function AddGlobalSet(Name, IsgData, gName)

  var Elem := GetIsg(Name);

  if(!Elem)
  
    var Data := GetIsgData();

    if(!Data)
      return Data;
    endif

    Elem := Data.CreateElement(ISGTIT_SET + Name);
  endif

  if(!Elem)
    return Elem;
  endif
  
  Elem.SetProp("Group", gName);
  Elem.SetProp("Name", IsgData.Name);
  Elem.SetProp("Type", IsgData.type);
  Elem.SetProp("AddTime", IsgData.addTime);
  Elem.SetProp("Creator", IsgData.Creator);
  Elem.SetProp("ItemCount", IsgData.Count);
  Elem.SetProp("Items", IsgData.Items);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetIsgDataStruct(Name)

  var IsgElem := GetIsg(Name);

  if(!IsgElem)
    return IsgElem;
  endif

  var IsgData := ItemSetStruct();

  IsgData.+Group  := IsgElem.GetProp("Group");
  IsgData.Name    := IsgElem.GetProp("Name");
  IsgData.type    := IsgElem.GetProp("Type");
  IsgData.Creator := IsgElem.GetProp("Creator");
  IsgData.addTime := IsgElem.GetProp("AddTime");
  IsgData.Count   := IsgElem.GetProp("ItemCount");
  IsgData.Items   := IsgElem.GetProp("Items");

  return IsgData;
  
endfunction

//-----------------------------------------------------------------------------
