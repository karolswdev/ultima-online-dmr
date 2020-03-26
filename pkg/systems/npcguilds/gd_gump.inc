//-----------------------------------------------------------------------------
// Modified: 2005-05-28
//-----------------------------------------------------------------------------

include "include/gumpboxes";
include "include/clock";

include ":quests:bq_gump";
include ":npcguilds:guildsdata";

//-----------------------------------------------------------------------------

function BGDInit(Title, Width, Height, bId := 0, Multi := 0)

  GInit(20, 20);
  if(Multi)
    GMode(GMODE_MULTIHD, GSMODE_NORMAL);
  else
    GMode(GMODE_NORMAL, GSMODE_NORMAL);
  endif

  GResPic(0, 0, 3600, Width, Height);
  GTextLine(30, 20, 999, Title);
  GButton(Width-300, 20, 2714, 2715, bId);
  GTextLine(Width-280, 20, 560, "Wroc");

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// SubGumps
//-----------------------------------------------------------------------------
//=============================================================================

function ChooseNGdStatusGump(who)

  GInit(20, 20);
  GResPic(0, 0, 3600, 280, 280);
  GTextLine(30, 20, 999, "Statusy Gildii:");
  GButton(160, 20, 2714, 2715, 0);
  GTextLine(180, 20, 560, "Wroc");

  var i;

  for(i:=1;i<=NGDSTATUS_MAXID;i:=i+1)
    GButton(30, 43+i*20, 2117, 2118, i);
    GTextLine(50, 40+i*20, 560, GetNGdStatusName(i));
  endfor

  return CInt(GSend(who)[0]);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Npc Guild Selection Gump
//-----------------------------------------------------------------------------
//=============================================================================

function SelectNpcGuild(who)

  var Data := GetNGdData();
  var Keys := GetDataKeys(Data, NGUILD_TITLE, BQG_ZEROFILL);
  var Res;

  Keys.sort();

  BuildNpcGuild(Keys, Data);

  while(IsLoggedIn(who))
    Res := CInt(GSend(who)[0]);

    if(Res != 100000)
      if(Res > 10000)
        BuildNpcGuildModInfo(Data, CInt(UnZeroFill(Keys[Res-10000])));
        continue;
      elseif(Res > 0)
        return CInt(UnZeroFill(Keys[Res]));
      else
        return 0;
      endif
    endif

    BuildNpcGuild(Keys, Data);
  endwhile

  return 0;

endfunction

//-----------------------------------------------------------------------------

function BuildNpcGuild(byref Keys, byref Data)

  BGDInit("Gildie Npc: "+Keys.size(), 600, 500, 0, 1);
  GAddPageData(565,18,2650,2651,565,467,2648,2647);

  GTextLine(30,  40, 999, "Wybierz");
  GTextLine(100, 40, 999, "Nazwa");
  GTextLine(380, 40, 999, "Typ");
  GTextLine(558, 40, 999, "Info");
  GMultiPage();

  var Guild, i := 1;

  foreach Key in Keys
    GCheckNextPage(20);
    Guild := Data.FindElement(NGUILD_TITLE+UnZeroFill(Key));
    GTextLine(30, 60+GNPCount()*20, 1419, Key);
    GButton(70, 63+GNPCount()*20, 2117, 2118, i);
    GButton(560, 63+GNPCount()*20, 2117, 2118, 10000+i);
    GTextLine(100,60+GNPCount()*20, 550, Guild.GetProp(GDPROP_NAME));
    GTextLine(380,60+GNPCount()*20, 550, GetNGdTypeName(Guild.GetProp(GDPROP_TYPE)));
    i := i + 1;
  endforeach

endfunction

//-----------------------------------------------------------------------------

function BuildNpcGuildModInfo(byref Data, GdId)

  var Guild := Data.FindElement(NGUILD_TITLE+GdId);

  BGDInit("Info: NpcGuild ID: "+GdId, 440, 330, 100000);
  GTextLine(30, 40, 999, "Nazwa: "+Guild.GetProp(GDPROP_NAME));

  var CrInfo := Guild.GetProp(GDAPROP_CRINFO);
  var UpInfo := Guild.GetProp(GDAPROP_UPINFO);

  BuildQuestElemModSegInfo(CrInfo,  "Stworzona:",    30, 80);
  BuildQuestElemModSegInfo(UpInfo,  "Modyfikowana:", 30, 130);

endfunction

//-----------------------------------------------------------------------------

function BuildGuildNpcsGump(who, GdId, byref Guild, Flags := 0)

  var Npcs, Res;

  FindGuildNpcs(GdId, Guild, Npcs);
  BuildGuildNpcsShowGump(Guild, Npcs, Flags);

  while(IsLoggedIn(who))
    Res := GSend(who);
    if(Res[0] > 100)
      MoveObject(who, Npcs[Res[0]-100].x, Npcs[Res[0]-100].y, Npcs[Res[0]-100].z, Npcs[Res[0]-100].realm, MOVECHAR_FORCELOCATION);
    else
      return;
    endif

    FindGuildNpcs(GdId, Guild, Npcs);
    BuildGuildNpcsShowGump(Guild, Npcs, Flags);
  endwhile

endfunction

//-----------------------------------------------------------------------------

function BuildGuildNpcsShowGump(byref Guild, byref Npcs, Flags := 0)

  __ParamUnused(Flags);

  BGDInit("Npece Gildii "+Guild.GetProp(GDPROP_NAME), 460, 500, 0, 1);

  GAddPageData(425,18,2650,2651,425,465,2648,2647);

  var n := 1, NpcData;

  GTextLine(50, 40, 999, "Nr");
  GTextLine(80, 40, 999, "Imie");
  GTextLine(300, 40, 999, "Serial");
  GTextLine(390, 40, 999, "Lvl");

  GMultiPage();

  foreach Npc in Npcs
    GCheckNextPage(20);
    NpcData := GetObjProperty(Npc, PROP_NGDDATA);
    GButton(30, 63+GNPCount()*20, 2117, 2118, 100+n);
    GTextLine(50, 60+GNPCount()*20, 999, n);
    GTextLine(80, 60+GNPCount()*20, 670, Npc.name);
    GTextLine(300, 60+GNPCount()*20, 550, Npc.serial);
    GTextLine(390, 60+GNPCount()*20, 550, NpcData.Level);
    n := n + 1;
  endforeach

endfunction

//-----------------------------------------------------------------------------

function BuildGuildMembersGump(who, GdId, byref Guild, Flags := 0)

  var Members, Res;

  GetGuildMembersListSorted(GdId, Guild, Members);
  BuildGuildMembersShowGump(who, GdId, Guild, Members, Flags);

  while(IsLoggedIn(who))
    Res := GSend(who);
    if(Res[0] > 1000)
      if(IsAdmin(who))
        if(YesNo(who,"Wyrzucic?"))
          var rRes := ResignPlayerFromGuild(Members[Res[0]-1000], GdId);
          if(!rRes)
            SendSysMessage(who, CStr(rRes.errortext), FONT_NORMAL, COLOR_RED);
          else
            SendSysMessage(who, "Wyrzucono osobe "+GetRealName(Members[Res[0]-1000])+" z Gildii ID: "+GdId, FONT_NORMAL, COLOR_GREEN);
          endif
        endif
      endif
    elseif(Res[0] > 100)
      if(IsGm(who))
        MoveObject(who, Members[Res[0]-100].x, Members[Res[0]-100].y, Members[Res[0]-100].z, Members[Res[0]-100].realm, MOVECHAR_FORCELOCATION);
      endif
    else
      return;
    endif

    GetGuildMembersListSorted(GdId, Guild, Members);
    BuildGuildMembersShowGump(who, GdId, Guild, Members, Flags);
  endwhile

endfunction

//-----------------------------------------------------------------------------

function GetGuildMembersList(byref Guild, byref Members)

  Members := array;
  var Member;

  foreach MembSer in (Guild.GetProp(GDPROP_MEMBERS))
    GCheckNextPage(15);
    Member := FindPlayerBySerial(MembSer);
    if(Member)
      Members.append(Member);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function GetGuildMembersListSorted(GdId, byref Guild, byref Members)

  var dSort := dictionary;
  Members := array;
  var Member;

  foreach MembSer in (Guild.GetProp(GDPROP_MEMBERS))
    GCheckNextPage(15);
    Member := FindPlayerBySerial(MembSer);
    if(Member)
      var PlayerData := GetObjProperty(Member, PROP_NGDDATA);
      var Exp := CInt(PlayerData.Guilds[GdId].Exp);

      dSort[ZeroFill(Exp, 10)+"_"+Member.serial] := Member;
    endif
  endforeach

  foreach Memb in dSort
    Members.append(Memb);
  endforeach

  Members.reverse();

  return Members;

endfunction

//-----------------------------------------------------------------------------

function BuildGuildMembersShowGump(who, GdId, byref Guild, byref Members, Flags := 0)

  if(Flags & BQG_SHOWEXTRA)
    BGDInit("Czlonkowie Gildii "+Guild.GetProp(GDPROP_NAME), 750, 500, 0, 1);
    GTextLine(650, 40, 999, "Serial");
    GAddPageData(715,18,2650,2651,715,465,2648,2647);
  else
    BGDInit("Czlonkowie Gildii "+Guild.GetProp(GDPROP_NAME), 650, 500, 0, 1);
    GAddPageData(615,18,2650,2651,615,465,2648,2647);
  endif

  GTextLine(50, 40, 999, "Nr");
  GTextLine(80, 40, 999, "Imie");
  GTextLine(270, 40, 999, "Tytul");
  GTextLine(400, 40, 999, "Ost. wizyta");

  GMultiPage();

  var n := 1, Data, LastSeenTime;

  foreach Member in Members
    Data := GetObjProperty(Member, PROP_NGDDATA);

    LastSeenTime := DescGameTime(ReadGameClock() - Data.Guilds[GdId].LastVisit);
    if(LastSeenTime["Brak"])
      LastSeenTime := "Nie dawno";
    else
      LastSeenTime := LastSeenTime + " temu";
    endif

    GCheckNextPage(20);
    if(Flags & BQG_EDITABLE)
      GButton(30, 63+GNPCount()*20, 2117, 2118, 100+n);
    endif
    GTextLine(50, 60+GNPCount()*20, 999, n);
    GTextLine(80, 60+GNPCount()*20, 670, GetRealName(Member));
    GTextLine(270, 60+GNPCount()*20, 550, GetNpcGuildTitle(Guild.GetProp(GDPROP_TYPE), Data.Guilds[GdId].Level));
    if(Data.Guilds[GdId].LastVisit)
      GTextLine(400, 60+GNPCount()*20, 550, LastSeenTime);
    endif
    if(Flags & BQG_SHOWEXTRA)
      GTextLine(650, 60+GNPCount()*20, 550, Member.serial);
      if(IsAdmin(who))
        GButton(630, 63+GNPCount()*20, 2117, 2118, 1000+n);
      endif
    endif
    n := n + 1;
  endforeach

endfunction

//-----------------------------------------------------------------------------
