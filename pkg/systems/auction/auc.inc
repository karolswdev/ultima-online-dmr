//-----------------------------------------------------------------------------

include ":mail:mail";

//-----------------------------------------------------------------------------

const AUC_PRICE_PERC       := 0.05;
const AUC_MIN_PRICE        := 100;
const MAX_AUC_ITEMS        := 64;
const MAX_DESC_LEN         := 50;
const MAX_AUC_PERSON_PUTS  := 8;
const MIN_BET_PERC         := 0.04;
const MIN_BET              := 100;
const AUC_BET_TIME         := 120;
const AUC_CANCEL_TIME      := 1800;

const AUC_TIME             := 86400;
const AUC_GM_TIME          := 345600;
const AUC_END_DELAY_TIME   := 600;
const AUC_BETTIME_INC      := 180;

//=============================================================================
//-----------------------------------------------------------------------------
// Auction
//-----------------------------------------------------------------------------
//=============================================================================

function GetAucItems(byref Arr,me,Empty := 1)

  if(!me)
    return;
  endif

  if(Empty)
    Arr := {};
  endif
    
  foreach Item In EnumerateItemsInContainer(me.backpack)
    if(GetObjProperty(Item,"SellPrice") and GetObjProperty(Item,"SellOwner"))
      if(!FindPlayerBySerial(GetObjProperty(Item,"SellOwner")))
        DestroyItem(Item);
      elseif(!(Item in Arr))
        Arr.append(Item);
      endif
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function GetAllAucItems(byref Arr,Aucs)

  Arr := {};
  foreach Auc in Aucs
    GetAucItems(Arr,Auc,0);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function CountAucItems(Aucs)

  var Items := array;
  GetAllAucItems(Items,Aucs);
  return Items.size();

endfunction

//-----------------------------------------------------------------------------

function GetItemAuctionPrice(Item)

  if(!CInt(GetObjProperty(Item,"ActualPrice")))
    SetObjProperty(Item,"ActualPrice",CInt(GetObjProperty(Item,"SellPrice")));
  endif
  
  return CInt(GetObjProperty(Item,"ActualPrice"));

endfunction

//-----------------------------------------------------------------------------

function GetItemBuyoutPrice(Item)

  return CInt(GetObjProperty(Item,"SellBuyout"));

endfunction

//-----------------------------------------------------------------------------

function SetItemPrice(Item,Price)

  Price := CInt(Price);
  
  if(!Price)
    return 0;
  endif
  
  var APrice := GetItemAuctionPrice(Item);

  if(Price < APrice + MIN_BET)
    return 0;
  endif
  
  SetObjProperty(Item,"ActualPrice",Price);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetAucItemTimeLeft(Item)

  var Duration := GetAutDuration(Item);

  return Max(0, GetAucStartTime(Item) + Duration - ReadGameClock());

endfunction

//-----------------------------------------------------------------------------

function GetAucStartTime(Item)

  if(!CInt(GetObjProperty(Item,"SellStartTime")))
    SetObjProperty(Item,"SellStartTime",ReadGameClock());
  endif

  return CInt(GetObjProperty(Item,"SellStartTime"));

endfunction

//-----------------------------------------------------------------------------

function GetAutDuration(Item)

  var Duration := CInt(GetObjProperty(Item, "SellDuration"));

  if(!Duration)
    Duration := AUC_TIME;
    SetObjProperty(Item, "SellDuration", AUC_TIME);
  endif

  return Duration;

endfunction

//-----------------------------------------------------------------------------

function SetAucStartTime(Item, Time)

  SetObjProperty(Item,"SellStartTime", Time);

endfunction

//-----------------------------------------------------------------------------

function GAucEdit(x, y, Width, Title, Id, eText := "")

  GTextLine(x, y, 1153, Title);
  GResPic(x+90, y-3, 0xBB8, Width+20, 26);
  GTextEntry(x+100, y, Width, 20, 999, Id, eText);

endfunction

//-----------------------------------------------------------------------------

function GAucChoose(x, y, Width, Title, Id, TIme)

  GTextLine(x, y, 1153, Title);
  GResPic(x+90, y-3, 0xBB8, Width+20, 26);
  GTextLine(x+100, y, 999, Time);
  GButton(x+90+Width+20, y, 2714, 2715, Id);

endfunction

//-----------------------------------------------------------------------------

function GAucText(x, y, Width, Title, Text)

  GTextLine(x, y, 1153, Title);
  GTextCrop(x+100, y, Width, 20, 1152, Text);

endfunction

//-----------------------------------------------------------------------------

function AuctionGump(byref Event)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  var who     := Event.source;
  var npc     := Event.npc;
  var bDone   := 0;

  var Res;
  var Desc, Buyout;
  var Price := CInt(ItemCfg[Event.item.objtype].VendorBuysFor);
  var Duration := 14*3600;

  while(!bDone)

    Res := SendAucGump(who, Event.item, Desc, Price, Buyout, Duration);

    if(Res[0] == 1 and Res[1])
    
      Desc   := CroppedId(Res[10]);
      Price  := CInt(CroppedId(Res[11]));
      Buyout := CInt(CroppedId(Res[12]));

    elseif(Res[0] == 10)

      Desc   := CroppedId(Res[10]);
      Price  := CInt(CroppedId(Res[11]));
      Buyout := CInt(CroppedId(Res[12]));

      var Times := array(2, 4, 8, 14, 28, 56);
      var Dict := dictionary;

      foreach Sec in Times
        Dict[Sec*3600] := DescGameTime(Sec*3600);
      endforeach

      Duration := SelectListGump(who, Dict, "Wybierz czas trwania aukcji");
      continue;

    else
      break;
    endif

    bDone := 1;

    if(!Desc)
      SendSysMessage(who, "Nie wpisal"+ggm(who,3)+" opisu.", FONT_NORMAL, COLOR_RED);
      bDone := 0;
    endif

    if(!Price)
      SendSysMessage(who, "Nie wpisal"+ggm(who,3)+" ceny.", FONT_NORMAL, COLOR_RED);
      bDone := 0;
    endif

    if(Buyout and Buyout < Price + AUC_MIN_PRICE and Buyout > Price)
      SendSysMessage(who, "Cena wykupu jest zbyt zblizona do poczatkowej ceny!", FONT_NORMAL, COLOR_RED);
      bDone := 0;
    elseif(Buyout and Buyout <= Price)
      SendSysMessage(who, "Cena wykupu jest mniejsza od poczatkowej ceny!", FONT_NORMAL, COLOR_RED);
      bDone := 0;
    endif

    if(len(Desc) > MAX_DESC_LEN)
      SendSysMessage(who, "Podany przez ciebie opis jest za dlugi, maksimum "+MAX_DESC_LEN+" znakow.", FONT_NORMAL, COLOR_RED);
      bDone := 0;
    endif
    
  endwhile

  if(!bDone)
    SendSysMessage(who, "Anulowano wystawianie przedmiotu.", FONT_NORMAL, COLOR_RED);
    Event.+cancel := 1;
    SendEvent(npc, Event);
    return;
  endif

  Event.+price  := Price;
  Event.+buyout := Buyout;
  Event.+desc   := Desc;
  Event.+duration := Duration;

  SendEvent(npc, Event);

endfunction

//-----------------------------------------------------------------------------

function SendAucGump(who, Item, Desc, Price, Buyout, Time)

  GInit(100,50);
  GPage();
  GResPic(0,0,BKGND_BLACKSLAB,480,250);
  GTextLine(150,20,560,"Wystawianie przedmiotu");
  
  GAucText(30, 50, 250, "Przedmiot:", Item.desc);
  GAucEdit(30, 90, 250, "Opis:", 10, Desc);
  GAucEdit(30, 120, 100, "Cena:", 11, Price);
  GAucEdit(30, 150, 100, "Wykup:", 12, Buyout);
  GAucChoose(30, 180, 100, "Czas trwania:", 10, DescGameTime(Time));
  
  GButton(140, 210, 2714, 2715, 1);
  GTextLine(160, 210, 560, "Zatwierdz");
  
  GButton(280, 210, 2714, 2715, 2);
  GTextLine(300, 210, 560, "Anuluj");

  return GSend(who);

endfunction

//-----------------------------------------------------------------------------

function EndAucForItem(Item, Buyer := 0)

  var Owner  := FindPlayerBySerial(GetObjProperty(Item,"SellOwner"));
  var LGold  := GetItemAuctionPrice(Item);
  var fromMob := GetAuctioneerFromItem(Item);

  if(!IsPlayer(Buyer))
    Buyer := FindPlayerBySerial(GetObjProperty(Item,"SellLastBet"));
  endif

  EraseObjProperty(Item,"SellOwner");
  EraseObjProperty(Item,"SellDesc");
  EraseObjProperty(Item,"SellBuyout");
  EraseObjProperty(Item,"SellStartTime");
  EraseObjProperty(Item,"SellDuration");
  EraseObjProperty(Item,"SellPrice");
  EraseObjProperty(Item,"ActualPrice");
  EraseObjProperty(Item,"SellLastBet");

  if(Buyer)
    SendSysMessage(Buyer, "Wygral"+ggm(Buyer,3)+" aukcje na przedmiot '"+Item.desc+"'!", FONT_NORMAL, COLOR_ORANGE);
    if(!PostItemMail(Buyer, fromMob, "Wygral"+ggm(Buyer,3)+" aukcje!", array("Wygral"+ggm(Buyer,3)+" aukcje na przedmiot:", "'"+Item.desc+"'", "Ostateczna stawka byla rowna: "+FormatSize(LGold, ".")+" gp."), Item, MAILF_RETURNED, MAIL_TEXT_TIME))
      var bBox := FindBankBox(Buyer);
      if(!MoveItemToContainer(Item, bBox))
        DestroyItem(Item);
      endif
    endif
    
    SendSysMessage(Owner, "Aukcja na twoj przedmiot '"+Item.desc+"' zostala sprzedana pomyslnie.", FONT_NORMAL, COLOR_WHITE);
    if(!PostGoldMail(Owner, fromMob, "Aukcja sprzedana!", array("Aukcja na twoj przedmiot, zostala sprzedana pomyslnie:", "'"+Item.desc+"'", "Ostateczna stawka byla rowna: "+FormatSize(LGold, ".")+" gp."), LGold))
      var oBox := FindBankBox(Owner);
      if(oBox)
        CreateGold(oBox, LGold);
      endif
    endif
  else
    SendSysMessage(Owner, "Aukcja na twoj przedmiot '"+Item.desc+"' dobiegla konca.", FONT_NORMAL, COLOR_ORANGE);
    if(!PostItemMail(Owner, fromMob, "Czas aukcji dobiegl konca.", array("Nikt nie podbil ceny twojej aukcji na przedmiot:", "'"+Item.desc+"'", "", "Przedmiot zostal dolaczony nizej.", "Zycze wiecej szczescia nastepnym razem!."), Item, MAILF_RETURNED, MAIL_TEXT_TIME))
      var oBox := FindBankBox(Owner);
      if(!MoveItemToContainer(Item, oBox))
        DestroyItem(Item);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GetAuctioneerFromItem(Item)

  var fromMob := Item.container.container;

  if(fromMob.objtype == UOBJ_NPCPACK)
    fromMob := fromMob.container;
  endif

  if(!fromMob.isa(POLCLASS_MOBILE))
    fromMob := "Aukcja";
  endif
  
  return fromMob;

endfunction

//-----------------------------------------------------------------------------

function CancelAucForItem(Item, who := 0)

  var Owner  := FindPlayerBySerial(GetObjProperty(Item,"SellOwner"));
  var WhoBet := FindPlayerBySerial(GetObjProperty(Item,"SellLastBet"));
  var fromMob := GetAuctioneerFromItem(Item);

  if(WhoBet)
    var Price   := GetItemAuctionPrice(Item);

    SendSysMessage(WhoBet, "Aukcja na przedmiot '"+Item.desc+"', ktory probowal"+ggm(WhoBet,3)+" kupic zostala anulowana.", FONT_NORMAL, COLOR_ORANGE);
    if(!PostGoldMail(WhoBet, fromMob, "Przedmiot zostal anulowany.", array("Aukcja na przedmiot, ktory probowal"+ggm(WhoBet,3)+" kupic:", "'"+Item.desc+"'","zostala anulowana.","Ponizej zostala dolaczona twoja ostatnia stawka,","w wysokosci "+FormatSize(Price, ".")+" gp"), Price))
      var wbBox := FindBankBox(WhoBet);
      if(wbBox)
        CreateGold(wbBox, Price);
      endif
    endif
  endif

  EraseObjProperty(Item,"SellOwner");
  EraseObjProperty(Item,"SellDesc");
  EraseObjProperty(Item,"SellBuyout");
  EraseObjProperty(Item,"SellStartTime");;
  EraseObjProperty(Item,"SellDuration");
  EraseObjProperty(Item,"SellPrice");
  EraseObjProperty(Item,"ActualPrice");
  EraseObjProperty(Item,"SellLastBet");

  if(who)
    SendSysMessage(who, "Anulowal"+ggm(who,3)+" aukcje na przedmiot '"+Item.desc+"'!", FONT_NORMAL, COLOR_ORANGE);
  endif
  
  if(Owner and Owner != who)
    SendSysMessage(Owner, "Aukcja na twoj przedmiot '"+Item.desc+"' zostala anulowana.", FONT_NORMAL, COLOR_ORANGE);
  endif

  if(!PostItemMail(Owner, fromMob, "Aukcja na przedmiot zostala anulowana.", array("Aukcja na przedmiot, ktory wystawil"+ggm(Owner,3)+":", "'"+Item.desc+"'", "zostala anulowana."), Item, MAILF_RETURNED, MAIL_TEXT_TIME))
    var oBox := FindBankBox(Owner);
    if(!MoveItemToContainer(Item, oBox))
      DestroyItem(Item);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
