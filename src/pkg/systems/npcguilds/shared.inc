//-----------------------------------------------------------------------------
// Created: 2005-07-08
// Modified: 2011-04-18
//-----------------------------------------------------------------------------

include ":quests:questloc";

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Common Gumps
//-----------------------------------------------------------------------------
//=============================================================================

function SelectQuestLocGroupGump(who, byref LocData, Flags := 0)

  var Data := GetQuestData(QGROUP_LOCDATA);
  var Keys := GetDataKeys(Data, QLOC_TITLE);
  var i := 1, Group, LocNum;

  if(!Keys.size())
    SendSysMessage(who, "Brak Grup lokacji.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  while(IsLoggedIn(who))
    i := 1;

    GInit(20, 20);
    GMode(GMODE_MULTIHD, GSMODE_NORMAL);
    GResPic(0, 0, 3600, 320, 500);
    GTextLine(30, 20, 999, "Grupy Lokacji ("+Keys.size()+"): ");
    GButton(210, 20, 2714, 2715, -1);
    GTextLine(230, 20, 560, "Wroc");
    GAddPageData(285,18,2650,2651,285,467,2648,2647);

    GTextLine(272, 40, 999, "Info");
    GTextLine(200, 40, 999, "Ilosc");
    GTextLine(20, 40, 999, "Wybierz");

    GMultiPage();

    foreach GLocName in Keys
      Group  := Data.FindElement(QLOC_TITLE+GLocName);
      LocNum := Group.GetProp(QLPROP_LOCS);
      if(LocNum)
        LocNum := LocNum.size();
      else
        LocNum := 0;
      endif

      GCheckNextPage(20);

      if(LocNum)
        GButton(20, 63+GNPCount()*20, 2117, 2118, i);
        GButton(275, 63+GNPCount()*20, 2117, 2118, 10000+i);
        GTextLine(40, 60+GNPCount()*20, 550, GLocName);
      else
        if(Flags & BQG_EDITABLE)
          GButton(20, 63+GNPCount()*20, 2117, 2118, i);
          GButton(275, 63+GNPCount()*20, 2117, 2118, 10000+i);
        endif
        GTextLine(40, 60+GNPCount()*20, 1203, GLocName);
      endif
      GTextLine(200, 60+GNPCount()*20, 540, LocNum);
      i := i + 1;
    endforeach

    var Res := GSend(who)[0];

    if(Res > 10000)
      LocData.GroupName := Keys[Res-10000];
      ShowQuestLocGroupInfo(LocData);
      InitQLocData(LocData);
      Keys := GetDataKeys(Data, QLOC_TITLE);
    elseif(Res > 0)
      if(Flags & BQG_RETVAL)
        return Keys[Res];
      else
        LocData.GroupName := Keys[Res];
      endif
      return 0;
    else
      LocData.GroupName := 0;
      return 0;
    endif

  endwhile

  LocData.GroupName := 0;
  return 0;

endfunction

//-----------------------------------------------------------------------------

function ShowQuestLocGroupInfo(byref QLocData)

  var Data  := GetQuestData(QGROUP_LOCDATA);
  var Group := Data.FindElement(QLOC_TITLE+QLocData.GroupName);
  var Flags := CInt(Group.GetProp(QAPROP_FLAGS));
  var Res;

  while(IsLoggedIn(who))

    BuildQuestLocGroupInfo(QLocData, Group);
    Res := GSend(who).butid;

    if(Res >= 10 and Res < 42)
      var Bit := CInt(Pow(2, CInt(Res - 10)));
      if(Flags & Bit)
        SetBitOff(Flags, Bit);
      else
        SetBitOn(Flags, Bit);
      endif
      Group.SetProp(QAPROP_FLAGS, Flags);
      SetQuestElemUpInfo(Group, 0, who);
    else
      return;
    endif

  endwhile

endfunction

//-----------------------------------------------------------------------------

function BuildQuestLocGroupInfo(byref QLocData, byref Group)

  var Flags := Group.GetProp(QAPROP_FLAGS);
  BQGInit("Info: Grupa lokacji "+QLocData.GroupName, 440, 330);

  BQGAddTextBox(100, 65, 125, 20, FCLR_TEBOX, "Nie sprawdzaj Z", BQG_EDITABLE | BQG_SHOWTITLES, 1, 0, "Flagi:");
  BQGCheckBox(225, 62, BQG_SHOWTITLES | BQG_EDITABLE, Flags & QLF_NOCHECK_Z, 10);
  BQGAddTextBox(100, 95, 125, 20, FCLR_TEBOX, "Pobieraj Z swiata", BQG_EDITABLE | BQG_SHOWTITLES);
  BQGCheckBox(225, 92, BQG_SHOWTITLES | BQG_EDITABLE, Flags & QLF_WORLD_Z, 11);
  BQGAddTextBox(100, 125, 125, 20, FCLR_TEBOX, "Akceptuj multi", BQG_EDITABLE | BQG_SHOWTITLES);
  BQGCheckBox(225, 122, BQG_SHOWTITLES | BQG_EDITABLE, Flags & QLF_ACP_MULTI, 12);

  BuildQuestElemModInfo(Group, 30, 200);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Npc Guild Common Gumps
//-----------------------------------------------------------------------------
//=============================================================================

function ChooseNGdTypeGump(who, Flags := 0)

  GInit(20, 20);
  GResPic(0, 0, 3600, 280, 280);
  GTextLine(30, 20, 999, "Typy Gildii:");
  GButton(160, 20, 2714, 2715, 0);
  if(Flags & BQG_ADDABLE)
    GTextLine(180, 20, 560, "Wszystkie");
  else
    GTextLine(180, 20, 560, "Wroc");
  endif

  var i;

  for(i:=1;i<=NGDTYPE_MAXID;i:=i+1)
    GButton(30, 43+i*20, 2117, 2118, i);
    GTextLine(50, 40+i*20, 560, GetNGdTypeName(i));
  endfor

  return CInt(GSend(who)[0]);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Other Common Gumps
//-----------------------------------------------------------------------------
//=============================================================================

function ChooseSkillIDGump(who)

  GInit(20, 20);
  GPage();
  GResPic(0, 0, 3600, 280, 460);
  GTextLine(30, 20, 999, "Umiejetnosci:");
  GButton(160, 20, 2714, 2715, 0);
  GTextLine(180, 20, 560, "Wroc");
  GAddPageData(245,18,2650,2651,245,427,2648,2647);

  var i;

  for(i:=0;i<=SKILLID__HIGHEST;i:=i+1)
    GCheckNextPage(20);
    GButton(30, 43+GNPCount()*20, 2117, 2118, i+1);
    GTextLine(50, 40+GNPCount()*20, 560, GetSkillConfigElem(i).Name);
  endfor

  return CInt(GSend(who)[0]);

endfunction

//-----------------------------------------------------------------------------

function ChooseCityGump(who)

  GInit(20, 20);
  GPage();
  GResPic(0, 0, 3600, 280, 400);
  GTextLine(30, 20, 999, "Miasta:");
  GButton(160, 20, 2714, 2715, 0);
  GTextLine(180, 20, 560, "Wroc");
  GAddPageData(245,18,2650,2651,245,365,2648,2647);

  var i;
  var Cities := GetCities();

  for(i:=1;i<=Cities.size();i:=i+1)
    GCheckNextPage(17);
    GButton(30, 43+GNPCount()*20, 2117, 2118, i);
    GTextLine(50, 40+GNPCount()*20, 560, Cities[i]);
  endfor

  var CityId := CInt(GSend(who)[0]);

  if(CItyId)
    return Cities[CityId];
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
