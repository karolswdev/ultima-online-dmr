//-----------------------------------------------------------------------------

function ShowItemDefinitionDetails(who, defName, Items, ParentName := 0)

  while(IsLoggedIn(who))

    BQGInit("Szczegoly definicji:", 600, 500, 0, 1);

    GTextLine(150, 20, 550, defName);

    if(ParentName)
      GTextLine(220, 20, 999, "Pojemnik:");
      GTextLine(275, 20, COLOR_BLUE, ParentName);
    endif

    GTextLine(430, 20, 999, "Ilosc:");
    GTextLine(465, 20, COLOR_YELLOW, Items.size());

    GAddPageData(565,18,2650,2651,565,467,2648,2647);

    BuildItemDefinitionContents(Items);

    var Res := GSend(who);

    if(Res.butid)
      ShowItemDefinitionDetails(who, defName, Items[Res.butid].Items, Items[Res.butid].Name);
    else
      break;
    endif

  endwhile

endfunction

//-----------------------------------------------------------------------------

function BuildItemDefinitionContents(byref Items)

  GTextLine(30,  40, 999, "Wewnatrz");
  GTextLine(100, 40, 999, "Nazwa");
  GTextLine(240, 40, 999, "ObjType");
  GTextLine(300, 40, 999, "Graphic");
  GTextLine(360, 40, 999, "Kolor");
  GTextLine(420, 40, 999, "Ilosc");
  GTextLine(460, 40, 999, "x");
  GTextLine(500, 40, 999, "y");
  GTextLine(540, 40, 999, "z");
  GMultiPage();

  var i := 1;

  foreach Item in Items

    GCheckNextPage(20);

    if(Item.Items.size())
      GButton(70, 63+GNPCount()*20, 2117, 2118, i);
    endif

    GTextLine(100,60+GNPCount()*20, 1321, Item.Name);
    GTextLine(240,60+GNPCount()*20, 560, hex(Item.objtype));
    GTextLine(300,60+GNPCount()*20, 540, __ifelse(Item.graphic != Item.objtype, hex(Item.graphic), ""));
    GTextLine(360,60+GNPCount()*20, __ifelse(Item.color, Item.color, 550), Item.color);
    GTextLine(420,60+GNPCount()*20, COLOR_YELLOW, Item.amount);
    
    if(Item.x != error)
      GTextLine(460, 60+GNPCount()*20, COLOR_GREEN, Item.x);
      GTextLine(500, 60+GNPCount()*20, COLOR_GREEN, Item.y);
      GTextLine(540, 60+GNPCount()*20, COLOR_GREEN, Item.z);
    endif

    i := i + 1;

  endforeach

endfunction

//-----------------------------------------------------------------------------
