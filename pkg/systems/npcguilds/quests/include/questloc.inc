//-----------------------------------------------------------------------------

include ":quests:questdata";

//-----------------------------------------------------------------------------

const QLPROP_REALM           := "Realm";
const QLPROP_LOCS            := "Locs";

//-----------------------------------------------------------------------------

const QUEST_MAX_LOCTRIES    := 10;

//-----------------------------------------------------------------------------

function InitQCrTargLoc(byref Targ)

  Targ := struct;

  Targ.+x     := 0;
  Targ.+y     := 0;
  Targ.+z     := 0;
  Targ.+Realm := 0;

endfunction

//-----------------------------------------------------------------------------

function GetQuestLocGroup(GroupName)

  if(!GroupName)
    return 0;
  endif

  var LocData   := GetQuestData(QGROUP_LOCDATA);
  var Group     := LocData.FindElement(QLOC_TITLE+GroupName);
  var Res       := error;

  if(!Group)
    Res.errortext := "Nie znaleziono Grupy quest-lokacji ["+GroupName+"]";
    return Res;
  endif

  var Locs := Group.GetProp(QLPROP_LOCS);

  if(!Locs or !Locs.size())
    Res.errortext := "Grupa quest-lokacji jest pusta! ["+GroupName+"]";
    return Res;
  endif

  return Group;

endfunction

//-----------------------------------------------------------------------------

function GenQuestLocFromGroup(Group)

  var Locs  := Group.GetProp(QLPROP_LOCS);

  if(!Locs)
    if(Group.x != error)
      return array(Group.x - 10, Group.y - 10, Group.x + 10, Group.y + 10, GetRealmId(Group.realm));
    endif

    return 0;
  endif

  var Loc   := 0;
  var Tries := 0;

  while(!Loc and Tries < QUEST_MAX_LOCTRIES)
    Loc := Locs[RandomInt(Locs.size())+1];

    if(Loc)
      return Loc;
    endif

    Tries := Tries + 1;
  endwhile

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Location Addition & Verification
//-----------------------------------------------------------------------------
//=============================================================================

function AddQuestLocGroup(byref QGLocData, who := 0)

  var Err := VerifyQuestLocGroup(QGLocData);
   
  if(Err == error)
    return Err;
  endif

  Err := error;

  var LocData   := GetQuestData(QGROUP_LOCDATA);
  var NewGroup  := LocData.CreateElement(QLOC_TITLE+QGLocData.GroupName);
   
  if(!NewGroup)
    Err.errortext := "Wystapil blad podczas tworzenia nowej grupy.";
  else
    NewGroup.SetProp(QLPROP_LOCS, array);
    NewGroup.SetProp(QAPROP_FLAGS, 0);

    SetQuestElemCrInfo(QGROUP_LOCDATA, NewGroup, QGPROP_LCRINFO, who, QGLocData.GroupName);
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestLocGroup(byref QGLocData)

  var Err := error;

  if(!QGLocData.GroupName)
    Err.errortext := "Brak nazwy grupy lokacji.";
  elseif(len(QGLocData.GroupName) < 4)
    Err.errortext := "Nazwa za krotka (4 znaki).";
  else
    var LocData := GetQuestData(QGROUP_LOCDATA);
    if(LocData.FindElement(QLOC_TITLE+QGLocData.GroupName))
      Err.errortext := "Taka grupa juz istnieje.";
    endif
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetQuestLocGroup(byref QLocData, who := 0)

  var LocData := GetQuestData(QGROUP_LOCDATA);
  var Group := LocData.FindElement(QLOC_TITLE+QLocData.GroupName);

  Group.SetProp(QLPROP_FLAGS, QLocData.Flags);
  SetQuestElemUpInfo(QGROUP_LOCDATA, Group, QGPROP_LUPINFO, who, QLocData.GroupName);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetQuestLocs(byref QLocData, who := 0)

  var Err := VerifyQuestLocs(QLocData);
  
  if(Err == error)
    return Err;
  endif

  Err       := error;
  var LocData := GetQuestData(QGROUP_LOCDATA);
  var Group := LocData.FindElement(QLOC_TITLE+QLocData.GroupName);

  Group.SetProp(QLPROP_LOCS, QLocData.Locs);
  SetQuestElemUpInfo(QGROUP_LOCDATA, Group, QGPROP_LUPINFO, who, QLocData.GroupName);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestLocs(byref QLocData)

  var Err   := error;

  if(!QLocData.GroupName)
    Err.errortext := "Brak nazwy grupy lokacji.";
    return Err;
  endif

  var LocData  := GetQuestData(QGROUP_LOCDATA);
  var Group    := LocData.FindElement(QLOC_TITLE+QLocData.GroupName);
  var Rlms     := Realms();

  if(!Group)
    Err.errortext := "Bledna nazwa grupy lokacji.";
  else
    var i      := 1;
    var Width, Height, Realm;

    foreach Loc in (QLocData.Locs)
      if(Loc.size() != 5)
        Err.errortext := "Blad, liczba parametrow ["+Loc.size()+"] sie nie zgadza, wymagane [5]";
        break;
      endif

      Loc[1] := CInt(Loc[1]);
      Loc[2] := CInt(Loc[2]);
      Loc[3] := CInt(Loc[3]);
      Loc[4] := CInt(Loc[4]);
      Loc[5] := CInt(Loc[5]);

      Realm  := GetRealmById(Loc[5]);

      if(!Realm)
        Err.errortext := "Bledny ID Swiata ["+Loc[5]+"], Nr: "+i;
        break;
      endif

      Width  := Rlms[Realm].width;
      Height := Rlms[Realm].height;

      if(!Loc[3] or !Loc[4])
        Err.errortext := "Brak wspolrzednych koncowych ["+Loc[3]+","+Loc[4]+"], Nr: "+i;
        break;
      elseif(Loc[1] >= Loc[3])
        Err.errortext := "Wsporzedna 1 >= 3(x): ["+Loc[1]+" >= "+Loc[3]+"], Nr: "+i;
        break;
      elseif(Loc[2] >= Loc[4])
        Err.errortext := "Wsporzedna 2 >= 4(y): ["+Loc[2]+" >= "+Loc[4]+"], Nr: "+i;
        break;
      elseif(Loc[3] > Width)
        Err.errortext := "Wsporzedna 3(x) jest za duza! ["+Loc[3]+", "+Realm+" M: "+Width+"], Nr: "+i;
        break;
      elseif(Loc[4] > Height)
        Err.errortext := "Wsporzedna 4(y) jest za duza! ["+Loc[4]+", "+Realm+" M: "+Height+"], Nr: "+i;
        break;
      endif
      i := i + 1;
    endforeach
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteQuestLocGroup(LocGroup)

  var LocData  := GetQuestData(QGROUP_LOCDATA);

  if(LocData)
    if(!LocData.FindElement(QLOC_TITLE+LocGroup))
      var Err := error;
      Err.errortext := "Nie znaleziono grupy "+LocGroup;
      return Err;
    endif

    var Stages := GetDataKeys(LocData, QSTAGE_TITLE);
    var SElem;

    foreach Stage in Stages
      SElem := LocData.FindElement(QSTAGE_TITLE+Stage);
      if(SElem.GetProp(QSTPROP_LOC) == LocGroup)
        var Err := error;
        Err.errortext := "Etap ["+Stage+"] zawiera grupe lokacji ["+LocGroup+"] !";
        return Err;
      endif
    endforeach

    var Res := LocData.DeleteElement(QLOC_TITLE+LocGroup);
    if(!Res)
      return Res;
    else
      return 1;
    endif
  else
    return LocData;
  endif

endfunction

//-----------------------------------------------------------------------------
