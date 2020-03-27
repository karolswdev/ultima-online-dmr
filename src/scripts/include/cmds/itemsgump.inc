//-----------------------------------------------------------------------------

function ShowItemsInGump(who, byref Items, AddInf := 0)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  GInit(30, 30);
  GMultiHeaderInit();
  GResPic(0, 0, 3600, 750, 460);
  GAddPageData(718,16,2650,2651,718,428,2648,2647);
  GMultiPage();

  var i := 1;

  foreach Item in Items
    GCheckNextPage(20);
    GTextLine(20, 30+GNPCount()*20, 550, i);
    GButton  (50, 30+GNPCount()*20, 2714, 2715, i);
    GTextLine(80, 30+GNPCount()*20, 550, hex(Item.objtype));
    GTextLine(140, 30+GNPCount()*20, 670, CInt(Item.objtype));

    if(ItemCfg[Item.objtype])
      GTextLine(200, 30+GNPCount()*20, 999, CLStr(GetConfigString(ItemCfg[Item.objtype],"Name")));
      if(Item.desc)
        GTextLine(350, 30+GNPCount()*20, 995, CLStr(Item.desc));
      else
        GTextLine(350, 30+GNPCount()*20, 995, CLStr(GetConfigString(ItemCfg[Item.objtype],"Desc")));
      endif
    elseif(Item.desc)
      GTextLine(350, 30+GNPCount()*20, 995, CLStr(Item.desc));
    endif

    if(Item.serial)
      GTextLine(500, 30+GNPCount()*20, 670, Item.serial);
    endif

    GTextLine(595, 30+GNPCount()*20, 995, Item.x+","+Item.y+","+Item.z);

    if(AddInf == "BI")
      GTextLine(690, 30+GNPCount()*20, 670, "BI "+GetItemBINr(Item));
    elseif(AddInf == "WMS")
      GTextLine(690, 30+GNPCount()*20, 670, "WMS "+IsWms(Item));
    endif

    i := i + 1;
  endforeach
  
  var Res := GSend(who);
  return CInt(Res[0]);

endfunction

//-----------------------------------------------------------------------------
