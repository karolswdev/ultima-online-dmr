//-----------------------------------------------------------------------------

include "include/finding";
include "include/move";

//-----------------------------------------------------------------------------

function MakeSubGump(who, type, Filter := 0)

  Elems := array;

  foreach Elem in ReadFile(LogPkg+Type)
    Elem := UnPack(Elem);
    if(Elem)
      Elems.append(Elem);
    endif
  endforeach
  
  Elems.reverse();

  var ShowNum := 5000;

  if(Elems.size() < ShowNum)
    ShowNum := Elems.size();
  else
    if(!Filter.exists("from"))
      SendSysMessage(who, "Logi ograniczone do ilosci 5k. Jesli chcesz wyswietlic poprzednie logi, uzyj komendy z opcja from, np. .cmd from 10-07-10.", FONT_NORMAL, COLOR_ORANGE);
    endif
  endif

  if(Filter)
    if(Filter.exists("from"))
      Filter["__fromunixtime__"] := MakeUnixTimeByDate(Filter["from"]);

      if(Filter["__fromunixtime__"] == error)
        SendSysMessage(who, "Data ["+Filter["from"]+"] jaka wpisal"+ggm(who,3)+" jest bledna.", FONT_NORMAL, COLOR_RED);
        return 0;
      else
        SendSysMessage(who, "Wyswietlanie logow od daty: "+Filter["from"]+" Unix: "+Filter["__fromunixtime__"], FONT_NORMAL, COLOR_ORANGE);
      endif
    endif
  endif

  GInit();
  GMultiHeaderInit();
  GResPic( 0, 0, 3600, 900, 480 );
  GTextLine(140, 20, 1420, "Log "+Type+" [All: "+Elems.size()+"] [Show: "+ShowNum+"]");

  GMultiPage();
  GAddPageData(865,20,2650,2651,865,448,2648,2647);

  var eIdx := 1;

  foreach Elem in Elems
    if(FilterPassed(Elem, Type, Filter))
      GCheckNextPage(20);
      BuildMsg(Elem, Type);
      eIdx += 1;

      if(eIdx > ShowNum)
        break;
      endif
    endif
  endforeach

  var Res := GSend(who)[0];
  
  case(Type)
    SHPS_LOG: if(Res)
                SendShpsGump(who, Res);
                MakeSubGump(who, type);
              endif

     SM_LOG:  if(Res)
                var Char := FindCharacterByName(Elems[Res][2]);
                if(Char)
                  SendSysMessage(who, "Przenoszenie do "+Char.name+".", FONT_NORMAL, COLOR_GREEN);
                  MoveObject(who, Char.x, Char.y, Char.z, Char.realm, MOVECHAR_FORCELOCATION);
                else
                  SendSysMessage(who, "Nie znaleziono osoby.", FONT_NORMAL, COLOR_RED);
                endif
              endif
  endcase

endfunction

//-----------------------------------------------------------------------------

function FilterPassed(byref Elem, byref LogKind, byref Filter)

  if(!Filter or !Filter.size())
    return 1;
  endif

  if(Filter.exists("from"))
    if(!Filter.exists("__fromunixtime__"))
      return 0;
    endif

    if(CInt(Elem[1]) > Filter["__fromunixtime__"])
      return 0;
    endif
  endif

  if(Filter.exists("source"))
    if(!Elem[2][Filter["source"]] and !Elem[3][Filter["source"]])
      return 0;
    endif
  endif

  case(LogKind)
    INF_LOG:      if(Filter.exists("name"))
                    if(!Elem[4][Filter["name"]] and !Elem[5][Filter["name"]])
                      return 0;
                    endif
                  endif

                  if(Filter.exists("serial"))
                    if(CInt(Elem[6]) != Filter["serial"])
                      return 0;
                    endif
                  endif

    GOTO_LOG:
    MOVE_LOG:     if(Filter.exists("name"))
                    if(!Elem[6][Filter["name"]])
                      return 0;
                    endif
                  endif

                  if(Filter.exists("serial"))
                    if(CInt(Elem[7]) != Filter["serial"])
                      return 0;
                    endif
                  endif

    QSITEMS_LOG:
    FBITEMS_LOG:
    CREATE_LOG:   if(Filter.exists("name"))
                    if(!Elem[5][Filter["name"]])
                      return 0;
                    endif
                  endif

                  if(Filter.exists("serial"))
                    if(CInt(Elem[6]) != Filter["serial"])
                      return 0;
                    endif
                  endif
                  
                  if(Filter.exists("contserial"))
                    if(CInt(Elem[7]) != Filter["contserial"])
                      return 0;
                    endif
                  endif
                  
                  if(Filter.exists("objtype"))
                    if(CInt(Elem[8]) != Filter["objtype"])
                      return 0;
                    endif
                  endif
                  
    KILL_LOG:     if(Filter.exists("name"))
                    if(!Elem[5][Filter["name"]] and !Elem[6][Filter["name"]])
                      return 0;
                    endif
                  endif

    DESTROY_LOG:  if(Filter.exists("name"))
                    if(!Elem[4][Filter["name"]] and !Elem[5][Filter["name"]])
                      return 0;
                    endif
                  endif

                  if(Filter.exists("serial"))
                    if(CInt(Elem[6]) != Filter["serial"])
                      return 0;
                    endif
                  endif
                  
                  if(Filter.exists("contserial"))
                    if(CInt(Elem[7]) != Filter["contserial"])
                      return 0;
                    endif
                  endif

    INFO_LOG:     if(Filter.exists("name"))
                    if(!Elem[4][Filter["name"]] and !Elem[5][Filter["name"]])
                      return 0;
                    endif
                  endif

                  if(Filter.exists("serial"))
                    if(CInt(Elem[6]) != Filter["serial"])
                      return 0;
                    endif
                  endif
                  
                  if(Filter.exists("prop"))
                    if(!Elem[7][Filter["prop"]])
                      return 0;
                    endif
                  endif
                  
    POSSESS_LOG:  if(Filter.exists("name"))
                    if(!Elem[5][Filter["name"]] and !Elem[6][Filter["name"]])
                      return 0;
                    endif
                  endif
                  
    SPAWN_LOG:    if(Filter.exists("nr"))
                    if(CInt(Elem[4]) != Filter["nr"])
                      return 0;
                    endif
                  endif

                  if(Filter.exists("city"))
                    if(Elem[5] != Filter["city"])
                      return 0;
                    endif
                  endif
                  
    WRONGIP_LOG:  if(Filter.exists("name"))
                    if(!Elem[4][Filter["name"]])
                      return 0;
                    endif
                  endif
    
    PASS_LOG:     if(Filter.exists("name"))
                    if(!Elem[4][Filter["name"]])
                      return 0;
                    endif
                  endif
    
    SHPS_LOG:     if(Filter.exists("name"))
                    if(!Elem[4][Filter["name"]])
                      return 0;
                    endif
                  endif
                  
    SM_LOG:       if(Filter.exists("name"))
                    if(!Elem[4][Filter["name"]])
                      return 0;
                    endif
                  endif
  endcase

  return 1;

endfunction

//-----------------------------------------------------------------------------

function BuildMsg(byref Elem, byref LogKind)

  GTextLine(20, GNPCount()*20+40, 1419, CUnixTime(Elem[1]));
  GTextLine(150,GNPCount()*20+40, 550,  CutWord(Elem[2],12)+"/"+Elem[3]);

  case(LogKind)
    INF_LOG:      GTextLine(330, GNPCount()*20+40, 560, Elem[4]+"/"+Elem[5]);
                  GTextLine(600, GNPCount()*20+40, 560, Elem[6]);
                  GTextLine(700, GNPCount()*20+40, 560, Elem[7]+","+Elem[8]+","+Elem[9]+" "+Elem[10]);

    GOTO_LOG:
    MOVE_LOG:     GTextLine(330, GNPCount()*20+40, 560, Elem[6]+"/"+Elem[7]);
                  if(CInt(Elem[8]))
                    GTextLine(575, GNPCount()*20+40, 560, Elem[8]);
                  endif
                  GTextLine(660, GNPCount()*20+40, 560, Elem[4][1]+","+Elem[4][2]+","+Elem[4][3]+" "+ClStr(Elem[4][4][1,4]));
                  GTextLine(775, GNPCount()*20+40, 560, Elem[5][1]+","+Elem[5][2]+","+Elem[5][3]+" "+ClStr(Elem[5][4][1,4]));

    QSITEMS_LOG:
    FBITEMS_LOG:
    CREATE_LOG:   GTextCrop(320, GNPCount()*20+40, 150, 30, 560, Elem[5]);
                  if(Elem[7])
                    GTextLine(530, GNPCount()*20+40, 560, Elem[7]);
                  endif
                  GTextLine(620, GNPCount()*20+40, 570, Elem[6]);
                  GTextLine(720, GNPCount()*20+40, 560, Elem[4][1]+","+Elem[4][2]+","+Elem[4][3]+" "+ClStr(Elem[4][4]));

    KILL_LOG:     GTextLine(330, GNPCount()*20+40, 560, Elem[5]+"/"+Elem[6]);
                  GTextLine(640, GNPCount()*20+40, 560, Elem[4][1]+","+Elem[4][2]+","+Elem[4][3]+" "+ClStr(Elem[4][4]));

    DESTROY_LOG:  GTextLine(330, GNPCount()*20+40, 560, Elem[4]);
                  GTextLine(370, GNPCount()*20+40, 560, Elem[5]);
                  GTextLine(500, GNPCount()*20+40, 560, Elem[6]);
                  GTextLine(600, GNPCount()*20+40, 560, Elem[7]);

    INFO_LOG:     GTextLine(310, GNPCount()*20+40, 560, CutWord(Elem[4],12)+"/"+Elem[5]);
                  GTextLine(470, GNPCount()*20+40, 560, Elem[6]);
                  GTextLine(560, GNPCount()*20+40, 560, CutWord(Elem[7],12));
                  GTextLine(670, GNPCount()*20+40, 560, Elem[8]);
                  GTextLine(780, GNPCount()*20+40, 560, Elem[9]);

    POSSESS_LOG:  GTextLine(330, GNPCount()*20+40, 560, Elem[5]+"/"+Elem[6]);
                  GTextLine(640, GNPCount()*20+40, 560, Elem[4][1]+","+Elem[4][2]+","+Elem[4][3]+" "+ClStr(Elem[4][4]));

    SPAWN_LOG:    GTextLine(330, GNPCount()*20+40, 560, "Nr: "+Elem[4]);
                  GTextLine(400, GNPCount()*20+40, 560, "City: "+Elem[5]);
                  GTextLine(640, GNPCount()*20+40, 560, Elem[6][1]+","+Elem[6][2]+","+Elem[6][3]+" "+ClStr(Elem[6][4]));

    WRONGIP_LOG:  GTextLine(330, GNPCount()*20+40, 560, Elem[4]);
    
    PASS_LOG:     GTextLine(330, GNPCount()*20+40, 560, Elem[4]);
    
    SHPS_LOG:     GTextLine(360, GNPCount()*20+40, 560, Elem[4]+","+Elem[5]);

    SM_LOG:       GTextLine(360, GNPCount()*20+40, 560, Elem[4]+","+Elem[5]);
                  GTextLine(460, GNPCount()*20+40, 560, Elem[6]+","+Elem[7]);
  endcase

endfunction

//-----------------------------------------------------------------------------

function SplitLogEntries(byref Logs)

  foreach Log in Logs
    Log := array(Log[2,14], Log[18, len(Log)-16]);
    sleepms(2);
  endforeach

  Logs.reverse();

endfunction

//-----------------------------------------------------------------------------

function ShowLogs(who, LogFile, Filter)
  
  while(IsLoggedIn(who))
    var AllLogs := ReadFile(LogFile);
    var i       := 1;
    var Color   := 550;
    SplitLogEntries(AllLogs);

    GInit(30, 30);
    GMultiHeaderInit();
    GResPic(0, 0, 3600, 1200,475);
    GButton(20, 20, 2714, 2715, 1);
    GTextLine(40, 20, 999, LogFile+" ["+AllLogs.size()+"]");
    GMultiPage();
    GAddPageData(1165,20,2650,2651,1165,438,2648,2647);

    foreach Log in AllLogs
      if(Filter)
        if(!Log[2][Filter])
          continue;
        endif
      endif

      if(Log[2]["CheckStuck"])
        Color := 1419;
      elseif(Log[2]["apisywan"])
        Color := 999;
      else
        Color := 550;
      endif

      GCheckNextPage(20);
      GTextLine(28,  GNPCount()*20+45, 1212, i);
      GTextLine(65,  GNPCount()*20+45, 670, Log[1]);
      GTextLine(195, GNPCount()*20+45, Color, Log[2]);
      i := i + 1;
      sleepms(2);
    endforeach

    SendSysMessage(who, (i-1)+"/"+AllLogs.size()+" wpisow.", FONT_NORMAL, COLOR_BLUE);
    var Res := GSend(who);

    if(!Res[0])
      break;
    endif
  endwhile
endfunction

//-----------------------------------------------------------------------------

function ShowSortedLogs(who, LogFile, Filter)

  var AllLogs := ReadFile(LogFile);
  var i       := 1;
  var Color   := 550;
  var Dict    := dictionary;
  var dtDict  := dictionary;
  SplitLogEntries(AllLogs);

  GInit(30, 30);
  GMultiHeaderInit();
  GResPic(0, 0, 3600, 1200,450);
  GMultiPage();
  GAddPageData(1165,20,2650,2651,1165,413,2648,2647);

  foreach Log in AllLogs
    if(Filter)
      if(!Log[2][Filter])
        continue;
      endif
    endif

    Dict[Log[2]]   := CInt(Dict[Log[2]]) + 1;
    dtDict[Log[2]] := Log[1];
  endforeach

  var sDict := dictionary;
  var aCnt  := 0;
  
  foreach Cnt in Dict
    sDict[ZeroFill(Cnt, 4)+"_"+_Cnt_iter] := _Cnt_iter;
  endforeach

  var Keys := sDict.keys();
  Keys.reverse();

  foreach Key in Keys
    var Log  := sDict[Key];
    var cCnt := CInt(UnZeroFill(Key));

    if(Log["CheckStuck"])
      Color := 1419;
    elseif(Log["apisywan"])
      Color := 999;
    else
      Color := 550;
    endif

    GCheckNextPage(20);
    GTextLine(28,  GNPCount()*20+20, 1212, i);
    GTextLine(65,  GNPCount()*20+20, 1419, cCnt);
    GTextLine(100, GNPCount()*20+20, 670, dtDict[Log]);
    GTextLine(235, GNPCount()*20+20, Color, Log);
    i := i + 1;
    aCnt := aCnt + cCnt;
  endforeach

  SendSysMessage(who, (i-1)+"/"+aCnt+"/"+AllLogs.size()+" wpisow.", FONT_NORMAL, COLOR_BLUE);
  GSend(who);

endfunction

//-----------------------------------------------------------------------------
