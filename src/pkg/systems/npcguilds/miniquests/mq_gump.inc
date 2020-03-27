//-----------------------------------------------------------------------------

include "include/mobile/npcs/utils";

//-----------------------------------------------------------------------------

function SelectMQuestType(who)

  BQGInit("Typy MiniQuestow:", 280, 280);

  var i;

  for(i:=1;i<=MQTYPE_MAXID;i:=i+1)
    GButton(30, 43+i*20, 2117, 2118, i);
    GTextLine(50, 40+i*20, 560, GetMQuestTypeName(i));
  endfor

  return CInt(GSend(who).butid);

endfunction

//-----------------------------------------------------------------------------

function SelectCityNpcTemplate(who)

  var i;
  var Keys   := array;

  foreach Temp in GetAllNpcTemplates()
    if(GetNpcConfig(Temp).CityNpc)
      Keys.append(Temp);
    endif
  endforeach

  GInit(20, 20);
  GMultiHeaderInit();
  GResPic(0, 0, 3600, 320, 500);
  GTextLine(30, 20, 999, "Miastowi Npece: ("+Keys.size()+"): ");
  GButton(210, 20, 2714, 2715, -1);
  GTextLine(230, 20, 560, "Wroc");
  GAddPageData(285,18,2650,2651,285,467,2648,2647);
  GMultiPage();
    
  for(i:=1;i<=Keys.size();i:=i+1)
    GCheckNextPage(22);
    GButton(30, 48+GNPCount()*20, 2117, 2118, i);
    GTextLine(50, 45+GNPCount()*20, 1419, i);
    GTextLine(80, 45+GNPCount()*20, 560, Keys[i]);
  endfor

  var Res := CInt(GSend(who)[0]);

  if(Res)
    return Keys[Res];
  else
    return 0;
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// MiniQuest Selection Gump
//-----------------------------------------------------------------------------
//=============================================================================

function SelectMQuest(who, QSet := 0, QEnd := 0)

  var Data := GetMQuestData();
  var Keys := GetDataKeys(Data, MQUEST_TITLE, BQG_ZEROFILL, QSet, QEnd);
  var Res;

  Keys.sort();

  BuildMQuests(Keys, Data);

  while(IsLoggedIn(who))
    Res := CInt(GSend(who).butid);

    if(Res != 100000)
      if(Res > 10000)
        BuildMQuestModInfo(Data, CInt(UnZeroFill(Keys[Res-10000])));
        continue;
      elseif(Res > 0)
        return CInt(UnZeroFill(Keys[Res]));
      else
        return 0;
      endif
    endif

    BuildMQuests(Keys, Data);
  endwhile

  return 0;

endfunction

//-----------------------------------------------------------------------------

function BuildMQuests(byref Keys, byref Data)

  BQGInit("MiniQuesty: "+Keys.size(), 600, 500, 0, 1);
  GAddPageData(565,18,2650,2651,565,467,2648,2647);

  GTextLine(30,  40, 999, "Wybierz");
  GTextLine(100, 40, 999, "Tytul");
  GTextLine(350, 40, 999, "Lvl");
  GTextLine(380, 40, 999, "Typ");
  GTextLine(460, 40, 999, "PodTyp");
  GTextLine(558, 40, 999, "Info");
  GMultiPage();

  var Quest, i := 1;

  foreach Key in Keys
    GCheckNextPage(20);
    Quest := Data.FindElement(MQUEST_TITLE+UnZeroFill(Key));
    GTextLine(30, 60+GNPCount()*20, 1419, Key);
    GButton(70, 63+GNPCount()*20, 2117, 2118, i);
    GButton(560, 63+GNPCount()*20, 2117, 2118, 10000+i);
    GTextLine(100,60+GNPCount()*20, 550, Quest.GetProp(MQPROP_TITLE));
    GTextLine(380,60+GNPCount()*20, 550, GetMQuestTypeName(Quest.GetProp(MQPROP_TYPE)));
    i := i + 1;
  endforeach

endfunction

//-----------------------------------------------------------------------------

function BuildMQuestModInfo(byref Data, QId)

  var Quest := Data.FindElement(MQUEST_TITLE+QId);

  BQGInit("Info: MiniQuest ID: "+QID, 440, 330, 100000);
  GTextLine(30, 40, 999, "Nazwa: ");
  GTextLine(80, 40, 550, Quest.GetProp(MQPROP_TITLE));
  

  BuildMQuestElemModInfo(Quest, 30, 80);

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Build Quest Element Info (Created, Modified, Usage)
//--------------------------------------------------------------------------------
//================================================================================

function BuildMQuestElemModInfo(Elem, sX, sY)

  var CrInfo  := Elem.GetProp(MQPROP_CRINFO);
  var UpInfo  := Elem.GetProp(MQPROP_UPINFO);
  var UseInfo := Elem.GetProp(MQPROP_USEINFO);

  BuildQuestElemModSegInfo(CrInfo,  "Stworzone:",    sX, sY);
  BuildQuestElemModSegInfo(UpInfo,  "Modyfikowane:", sX, sY+50);
  BuildQuestElemModSegInfo(UseInfo, "Uzywane:",      sX, sY+100);

endfunction

//-----------------------------------------------------------------------------
