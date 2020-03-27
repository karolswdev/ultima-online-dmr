//-----------------------------------------------------------------------------
// Modified: 2006-11-22
//-----------------------------------------------------------------------------

include "include/gumps";
include "include/gump_utils";
include "include/clock";

//================================================================================
//--------------------------------------------------------------------------------
// Build Quest Gump Utility
//--------------------------------------------------------------------------------
//================================================================================

function BuildEventElemModSegInfo(byref Info, Title, sX, sY)

  if(Info)
    var Person := FindPlayerBySerial(Info[3]);
    GTextLine(sX, sY, 999, Title);
    if(Info[5])
      GTextLine(sX+250, sY, 670, Info[5]);
    endif
    GTextLine(sX, sY+20, 1419, CUnixTime(Info[1]));
    if(Person)
      GTextLine(sX+140, sY+20, 670, Person.name);
    endif
    if(Info[2])
      GTextLine(sX+290, sY+20, 670, Info[2]);
    endif
    if(Info[4])
      GTextLine(sX+360, sY+20, 670, Info[4]);
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function BuildEventElemModInfo(Elem, sX, sY)

  var CrInfo  := Elem.GetProp(EVAPROP_CRINFO);
  var UpInfo  := Elem.GetProp(EVAPROP_UPINFO);
  var UseInfo := Elem.GetProp(EVAPROP_USEINFO);

  BuildEventElemModSegInfo(CrInfo,  "Stworzone:",    sX, sY);
  BuildEventElemModSegInfo(UpInfo,  "Modyfikowane:", sX, sY+50);
  BuildEventElemModSegInfo(UseInfo, "Uzywane:",      sX, sY+100);

endfunction

//-----------------------------------------------------------------------------

function BuildEventPlayers(who, EvId, DataElem, Flags := 0)

  var Members, Res;

  GetEventPlayersListSorted(EvId, DataElem, Members);
  BuildEventPlayersShowGump(who, EvId, DataElem, Members, Flags);

  while(IsLoggedIn(who))
    Res := GSend(who).butid;

    if(Res > 1000)
      if(IsAdmin(who))
        if(YesNo(who,"Wyrzucic?"))
          var rRes := DeleteEventPlayer(Members[Res-1000], EvId, DataElem);
          if(!rRes)
            SendSysMessage(who, CStr(rRes.errortext), FONT_NORMAL, COLOR_RED);
          else
            SendSysMessage(who, "Skasowano osobe "+GetRealName(Members[Res-1000])+" z listy eventu: "+ToAscii(DataElem.GetProp("Name")), FONT_NORMAL, COLOR_GREEN);
          endif
        endif
      endif
    elseif(Res > 100)
      if(IsGm(who))
        MoveObject(who, Members[Res-100].x, Members[Res-100].y, Members[Res-100].z, Members[Res-100].realm, MOVECHAR_FORCELOCATION);
      endif
    else
      return;
    endif

    GetEventPlayersListSorted(EvId, DataElem, Members);
    BuildEventPlayersShowGump(who, EvId, DataElem, Members, Flags);
  endwhile

endfunction

//-----------------------------------------------------------------------------

function GetEventPlayersListSorted(EvId, DataElem, byref Members)

  var dSort := dictionary;
  Members := array;
  var Member;

  foreach MembSer in (DataElem.GetProp("Players"))
    GCheckNextPage(15);
    Member := FindPlayerBySerial(MembSer);
    if(Member)
      var PlayerData := GetObjProperty(Member, PROP_EVENTS_DATA);
      var Points := CInt(PlayerData.Events[EvId].Points);

      dSort[ZeroFill(Points, 10)+"_"+Member.serial] := Member;
    endif
  endforeach

  foreach Memb in dSort
    Members.append(Memb);
  endforeach

  Members.reverse();

  return Members;

endfunction

//-----------------------------------------------------------------------------

const BEPSG_SHOW_TITLE_GUILD   := 0x800000;

//-----------------------------------------------------------------------------

function BuildEventPlayersShowGump(who, EvId, Event, byref Members, Flags := 0)

  if(Flags & BQG_SHOWEXTRA)
    BQGInit(ToUnicode("Event: ")+Event.GetProp("Name"), 650, 500, 0, 1);
    GTextLine(530, 40, 999, "Serial");
    GAddPageData(615,18,2650,2651,615,465,2648,2647);
  else
    BQGInit(ToUnicode("Event: ")+Event.GetProp("Name"), 560, 500, 0, 1);
    GAddPageData(525,18,2650,2651,525,465,2648,2647);
  endif

  GTextLine(50, 40, 999, "Nr");
  GTextLine(80, 40, 999, "Imie");
  GTextLine(270, 40, 999, "Tytul");

  if(Flags & BEPSG_SHOW_TITLE_GUILD)
    GTextLine(380, 40, 999, "Druzyna");
  else
    GTextLine(380, 40, 999, "Ost. obecnosc");
  endif

  GMultiPage();

  var n := 1, Data, LastSeenTime;

  foreach Member in Members
    Data := GetObjProperty(Member, PROP_EVENTS_DATA);
    LastSeenTime := 0;

    if(Flags & BEPSG_SHOW_TITLE_GUILD)
      LastSeenTime := Member.title_guild;
    elseif(Data.Events[EvId].LastTime)
      LastSeenTime := DescGameTime(ReadGameClock() - CInt(Data.Events[EvId].LastTime));
      if(LastSeenTime["Brak"])
        LastSeenTime := "Nie dawno";
      else
        LastSeenTime := LastSeenTime + " temu";
      endif
    endif

    GCheckNextPage(20);
    if(Flags & BQG_EDITABLE)
      GButton(30, 63+GNPCount()*20, 2117, 2118, 100+n);
    endif
    GTextLine(50, 60+GNPCount()*20, 999, n);
    GTextLine(80, 60+GNPCount()*20, 670, GetRealName(Member));
    GTextLine(270, 60+GNPCount()*20, 550, GetEventTitle(EvId, Data.Events[EvId].Points)+" ("+CInt(Data.Events[EvId].Points)+")");
    if(LastSeenTime)
      GTextLine(380, 60+GNPCount()*20, 550, LastSeenTime);
    endif
    if(Flags & BQG_SHOWEXTRA)
      GTextLine(570, 60+GNPCount()*20, 550, Member.serial);
      if(IsAdmin(who))
        GButton(550, 63+GNPCount()*20, 2117, 2118, 1000+n);
      endif
    endif
    n := n + 1;
  endforeach

endfunction

//--------------------------------------------------------------------------------
