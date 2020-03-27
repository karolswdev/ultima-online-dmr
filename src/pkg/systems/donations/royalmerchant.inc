//-----------------------------------------------------------------------------

function SendRewardGump(byref Event)

  var items := Event.items;
  var SelItem := error;
  var who := Event.source;
  var npc := Event.npc;
  var Res := BuildDonationGump(who,items);
  SelItem := CInt(Res);

  if(!SelItem)
    Event.+cancel := 1;
    SendEvent(npc, Event);
  else
    Event.+selitem := SelItem;
    SendEvent(npc, Event);
  endif

endfunction

//-----------------------------------------------------------------------------

function BuildDonationGump(who, Items)

  var accName := who.acctname;
  var Acc     := FindAccount(accName);
  var coins := CInt(GetObjProperty(who, "HonorPoints"));

  GInit(80,50);
  GResPic(0,0,9200,450,380);
  GPage();
  GAddPageData(415,16,2650,2651,415,345,2648,2647);
  GTextLine(250, 22, 999, "Posiadane punkty:");
  GTextLine(360, 22, 1424, FormatSize(coins, "."));

  foreach Item in Items

    var Name := GetDescForItem(Item[1]);
    var Graphic := GetGraphicForItem(Item[1]);
    var Color := GetColorForItem(Item[1]);
    var Price := Cint(Item[2]);
    GCheckNextPage(3);
    GResPic(23, (55+(GNPCount()*100)), BKGND_STONE, 86, 82);
    GTilePicHue(26, (64+(GNPCount()*100)), Graphic,Color);
    
    var tColor := 600;
    var pColor := 1425;

    if(Price > coins)
      tColor := 998;
    else
      var ObjType;
      if(Item[3])
        ObjType := Item[3];
      else
        ObjType := Item[1];
      endif
      GButton(130, (95+(GNPCount()*100)), 2714, 2715, ObjType);
      GTextLine(160,(95+(GNPCount()*100)), 64, "Kup");
    endif

    if(Price > coins)
      pColor := 1214;
    endif

    GTextLine(135,(65+(GNPCount()*100)),tColor, Name);
    GTextLine(320,(65+(GNPCount()*100)),999, "Koszt:");
    GTextLine(365,(65+(GNPCount()*100)),pColor, FormatSize(Price, ".")+" monet");

  endforeach

  return GSend( who )[0];

endfunction

//-----------------------------------------------------------------------------

function GetDonationItems(byref Items)

  Items := array;
  var itemsCfg := ReadConfigFile(":donations:donationitems");
  var aItems := GetConfigStringArray(itemsCfg["Rewards"],"Item");

  foreach aItem in aItems
    Items.append(SplitWords(aItem));
  endforeach
  
endfunction

//-----------------------------------------------------------------------------
