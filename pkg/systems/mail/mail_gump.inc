//-----------------------------------------------------------------------------

function BuildMailTitleGump(i, Id, mailProps)

  var timeLeft := (mailProps.sendTime - ReadGameClock()) + mailProps.existTime;

  GResPic(25, 70+i*85, BKGND_PAPER, 450, 58);
  GGumpPic(29, 74+i*85, 2002);
  GButton(450, 109+i*85, 2224, 2224, 100+Id); // 2153
  GTextLine(55, 76+i*85, 999, "Tytul: "); 
  GTextCrop(95, 76+i*85, 350, 20, 0, mailProps.title); 
  GTextLine(55, 99+i*85, 999, "Od: "); 
  GTextCrop(95, 99+i*85, 350, 20, 0, mailProps.senderName); 
  GTextLine(280, 105+i*85, 1000, DescGameTime(timeLeft));

  if(mailProps.requiredGold)
    GGumpPic(407, 127+i*85, 4002);
  elseif(mailProps.attachedItem)
    GGumpPic(407, 127+i*85, 4008);
  endif

  if(mailProps.attachedGold)
    GGumpPic(442, 127+i*85, 4028);
  endif
 
  if(mailProps.flags & MAILF_READ)
    GGumpPic(450, 75+i*85, 5411);
  endif

endfunction

//-----------------------------------------------------------------------------

function ShowMailCont(who, mailProps)

  var AItem := FindItemBySerial(mailProps.attachedItem);
  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Desc;

  GInit(30,30);
  GPage();
  GResPic(0,0,BKGND_BLACKSLAB,500,400);
  GCheckerTrans(15,15,470,370);
  GTextLine(50,20,1152,AItem.desc);
  GButton(458,14,0xFAE,0xFB0,999);
  GAddPageData(10,370,0xFAE,0xFB0,462,370,0xFA5,0xFA7);

  GGumpPic(10, 45,BKGND_BLACKSLAB+7);
  GGumpPic(130,45,BKGND_BLACKSLAB+7);
  GGumpPic(250,45,BKGND_BLACKSLAB+7);
  GGumpPic(363,45,BKGND_BLACKSLAB+7);

  foreach Item in EnumerateItemsInContainer(AItem)
    GCheckNextPage(10);
    Desc := Item.desc;
    if(ItemCfg[Item.objtype].ItemTypeName)
      Desc := ItemCfg[Item.objtype].ItemTypeName+" "+Desc;
    endif
    
    GTextLine(20,78+(GNPCount()*30),1419,_Item_iter+".");
    GTilePicHue(50,78+(GNPCount()*30),Item.graphic,Item.color);
    GTextCrop(105,78+(GNPCount()*30),410,20,1152,Desc);
  endforeach

  GSend(who);

endfunction

//-----------------------------------------------------------------------------
