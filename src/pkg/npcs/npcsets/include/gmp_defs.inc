//-----------------------------------------------------------------------------

function ShowNpcDefinitionDetails(who, defName, Npcs)

  while(IsLoggedIn(who))

    BQGInit("Szczegoly definicji:", 600, 500, 0, 1);

    GTextLine(150, 20, 550, defName);

    GTextLine(430, 20, 999, "Ilosc:");
    GTextLine(465, 20, COLOR_YELLOW, Npcs.size());

    GAddPageData(565,18,2650,2651,565,467,2648,2647);

    BuildNpcDefinitionContents(Npcs);

    var Res := GSend(who);

    if(Res.butid)
      ShowItemDefinitionDetails(who, defName, Npcs[Res.butid].Equips, Npcs[Res.butid].Name);
    else
      break;
    endif

  endwhile

endfunction

//-----------------------------------------------------------------------------

function BuildNpcDefinitionContents(byref Npcs)

  GTextLine(30,  40, 999, "Equipy");
  GTextLine(100, 40, 999, "Nazwa");
  GTextLine(210, 40, 999, "Template");
  GTextLine(300, 40, 999, "Graphic");
  GTextLine(360, 40, 999, "Kolor");
  GTextLine(460, 40, 999, "x");
  GTextLine(500, 40, 999, "y");
  GTextLine(540, 40, 999, "z");
  GMultiPage();

  var i := 1;

  foreach Npc in Npcs

    GCheckNextPage(20);

    if(Npc.Equips.size())
      GButton(70, 63+GNPCount()*20, 2117, 2118, i);
    endif

    GTextLine(100,60+GNPCount()*20, 1321, Npc.Name);
    GTextLine(210,60+GNPCount()*20, 560, Npc.template);
    GTextLine(300,60+GNPCount()*20, 540, hex(Npc.graphic));
    GTextLine(360,60+GNPCount()*20, __ifelse(Npc.color, Npc.color, 550), Npc.color);
    
    if(Npc.x != error)
      GTextLine(460, 60+GNPCount()*20, COLOR_GREEN, Npc.x);
      GTextLine(500, 60+GNPCount()*20, COLOR_GREEN, Npc.y);
      GTextLine(540, 60+GNPCount()*20, COLOR_GREEN, Npc.z);
    endif

    i := i + 1;

  endforeach

endfunction

//-----------------------------------------------------------------------------
