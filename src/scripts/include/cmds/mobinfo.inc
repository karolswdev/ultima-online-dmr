//-----------------------------------------------------------------------------

function ShowAggroTable(who, whom)

  __ParamUnused(who);

  StandMultiSkLayout(16);
  GButton(  325,410,2453,2454,1);
  GAddPageData(365,55,2650,2651,365,383,2648,2647);

  var AggroTable := GetObjProperty(whom, "AggroTable");

  GTextLine(60,30,610,"Tabela aggro: "+whom.name);
  GButton(40, 33, 2117, 2118, 9114);
  
  GTextLine(50, 70, 999, "Gracz");
  GTextLine(200,70, 999, "Poziom");
  GTextLine(250,70, 999, "Ostatnio");

  GMultiPage();
  
  if(AggroTable.size())

    var Player;

    foreach Elem in (AggroTable)
      Player := FindPlayerBySerial(_Elem_iter);

      if(!Player)
        continue;
      endif
      
      GCheckNextPage(15);
      GTextLine(50, 90+GNPCount()*20, __IfElse(whom.opponent == Player, 1209, 550), GetRealName(Player));
      GTextLine(200,90+GNPCount()*20, 1319, Elem[2] );
      GTextLine(250,90+GNPCount()*20, 570, RealTime(ReadGameClock() - Elem[1]) );
    endforeach
  else

    GTextLine(70, 77, 550, "Brak");

  endif

endfunction

//-----------------------------------------------------------------------------