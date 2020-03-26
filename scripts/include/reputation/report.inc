//-----------------------------------------------------------------------------

include "include/const/props";
include "include/objtypes/magic";
include "include/checks/stdchecks";
include "include/mobile/tamed";
include "include/faith";
include "include/reputation/murderer";

//-----------------------------------------------------------------------------

function SendReportGump(who)

  if(!IsPlayer(who))
    return 0;
  endif

  if(GetObjProperty(who, PROP_NOREPORT))
    EraseObjProperty(who, PROP_REPORT_MURDER);
    EraseObjProperty(who, PROP_LOGIN_REPORT_MURDER);
    EraseObjProperty(who, PROP_NOREPORT);
    return 0;
  endif

  var Reports := GetObjProperty(who, PROP_LOGIN_REPORT_MURDER);

  if(!Reports)
    if(!who.dead)
      return 0;
    endif

    var Killer := GetLastHitter(who);
    Reports    := GetMurdReports(who);

    if(!Reports.size())
      return 0;
    endif

    if(IsTamed(Killer))
      Killer := GetMaster(Killer);
    endif

    if(Killer == who or !Reports[Killer.serial] or !IsPlayer(Killer))
      EraseObjProperty(who, PROP_REPORT_MURDER);
      EraseObjProperty(who, PROP_LOGIN_REPORT_MURDER);
      return 0;
    endif
  endif

  EraseObjProperty(who, PROP_REPORT_MURDER);

  if(!IsLoggedIn(who))
    SetObjProperty(who, PROP_LOGIN_REPORT_MURDER, Reports);
    return 0;
  endif

  var Killers := GetMurdReportKillers(Reports);
  
  SetObjProperty(who, PROP_LOGIN_REPORT_MURDER, Reports);

  foreach Killer in Killers
    if(!CanReport(who, Killer))
      Killer := 0;
    endif
  endforeach
  
  GInit(50,50);
  GNoDispose();
  GNoMove();
  GNoClose();
  
  GResPic(0,20,BKGND_BLACKSLAB,400,400);
  GGumpPic(20 ,40,0x71);
  GGumpPic(350,40,0x71);

  GResPic(170,10,BKGND_GOLDTRIM,62,40);
  GGumpPic(175,16,0x46);
  
  if(Reports.size() > 1)
    GTextLine(100,60 ,500,"Zostal"+ggm(who,3)+" zamordowan"+ggm(who,6)+" przez osoby:");
  else
    GTextLine(100,60 ,500,"Zostal"+ggm(who,3)+" zamordowan"+ggm(who,6)+" przez osobe:");
  endif
  
  var time := ReadGameClock() + 120;
  var i := 0;

  foreach Killer in Killers
    if(Killer)
      GCheckBox(45, i*30+95, 0x868, 0x86a, i+5);
      GGumpPic(80, i*30+97, 0x475);
      GTextLine(95, i*30+100, 560, ClStr(Killer.name));
      i := i + 1;
    endif
  endforeach

  if(i == 0)
    EraseObjProperty(who, PROP_REPORT_MURDER);
    EraseObjProperty(who, PROP_LOGIN_REPORT_MURDER);
    return 0;
  endif

  GTextLine(50, 330,500,"Zaznacz osoby, ktore chcesz oglosic mordercami.");

  GButton( 100, 365, 0x47b, 0x47c, 2 );
  GButton( 240, 365, 0x478, 0x479, 3 );

  var Res := GSend(who);

  i := 5;
  
  if(time > ReadGameClock())
    if(CInt(Res[0]) == 2)
      foreach Killer in Killers
        if(Res[i] and Killer)
          SendSysMessage(who,   "Oglosil"+ggm(who,3)+" osobe "+ClStr(Killer.name)+" jako morderce.",FONT_NORMAL,COLOR_GREEN);
          SendSysMessage(Killer,"Twoj niecny wystepek zostal zgloszony przez "+ClStr(who.name)+".",FONT_NORMAL,COLOR_RED_LIGHT);
          AdjustMurderCounts(Killer, who);
        endif
        i := i + 1;
      endforeach
      EraseObjProperty(who, PROP_LOGIN_REPORT_MURDER);
    elseif(CInt(Res[0]) == 3)
      EraseObjProperty(who, PROP_LOGIN_REPORT_MURDER);
    endif
  else
    SendSysMessage(who,"Za pozno by oglosil te osobe.",FONT_NORMAL,COLOR_RED);
  endif
  
  EraseObjProperty(who, PROP_REPORT_MURDER);

endfunction

//-----------------------------------------------------------------------------
