//-----------------------------------------------------------------------------

function GamblerGump(byref Event)

  var who     := Event.source;
  var npc     := Event.npc;
  var items   := Event.items;

  var Res;
  var SelItem := error;

  BuildMainGambleGump(Items);
  Res := GSend(who);

  if(Res[0] > 100)
    SelItem := CInt(Res[0]) - 100;
  endif

  if(!SelItem)
    Event.+cancel := 1;
    SendEvent(npc, Event);
  else
    Event.+selitem := SelItem;
    SendEvent(npc, Event);
  endif

endfunction

//-----------------------------------------------------------------------------

function BuildMainGambleGump(Items)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  GInit(30,30);
  GPage();
  GResPic(0,0,0x2422,650,400);
  GCheckerTrans(10,10,630,380);
  GTextLine(320,30,1150,"Hazard");
  
  GGumpPic(10,60,0x2423);
  GGumpPic(215,60,0x2423);
  
  var w := 0;
  var h := 0;
  var i := 1;
  
  GPage();
  
  foreach Item in Items

    if(h > 1)
      GButtonPage(612,370,0xFA5,0xFA7,GetNextPage());
      GPage();
      GButtonPage(12,370,0xFAE,0xFB0,GetPrevPage());
      w := 0;
      h := 0;
    endif

    GResPic(50+(w*150),100+(h*160),0x2422,100,100);
    GTilePic(65+(w*150),115+(h*160), GetGraphicForItem(Item.objtype));

    GTextLine(62+(w*150),152+(h*160),560,FormatSize(Item.price, ".") + " gp");
    
    var Desc := Item.desc;

    if(ItemCfg[Item.objtype].ItemTypeName)
      Desc := ItemCfg[Item.objtype].ItemTypeName+" "+Desc;
    endif
    
    GTextCrop(50+(w*150),80+(h*160),100,20,100,Desc);
    GButton(122+(w*150),200+(h*160),0xFA9,0xFAA,100+i);
    
    w := w + 1;
    i := i + 1;
    if(w > 3)
      h := h + 1;
      w := 0;
    endif
    
  endforeach

endfunction

//-----------------------------------------------------------------------------
