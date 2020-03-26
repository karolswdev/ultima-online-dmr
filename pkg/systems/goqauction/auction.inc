function CreateCategoriesContainers()

    var cats := array { "Zwierzeta",
			"Zbroje",
			"Bronie",
			"Mikstury",
			"Narzedzia",
			"Deedy",
			"Magiczne",
			"Inne" };
			
    
    var storageRoom := OpenAuctionStorage();
    
    var theBag;
    
    foreach cat in cats
	theBag := CreateRootItemInStorageArea(storageRoom, cat, UOBJ_BANKBOX);
	    if(!theBag)
		AlertAdmins("Nie mozna bylo zrobic pojemnikow kategorii " + cat);
	    endif
    endforeach
    
    AlertAdmins("Wszystko sie powiodlo.");
    
endfunction

function FindAndOpenCategoryContainer(category)

    var storageRoom := FindRootItemInStorageArea(OpenAuctionStorage(), category);
    
    if(!storageRoom)
	AlertAdmins("Nie mozna odnalezc kategorii Aukcji - " + category);
    endif
    
    return storageRoom;
    
endfunction

function OpenAuctionStorage()

    var theStorage := FindStorageArea("NewAuction");
    
    if(!theStorage)
	theStorage := CreateStorageArea("NewAuction");
    endif
    
    return theStorage;
    
endfunction

function PostAuctionItem(who, theItem, category, duration, price)

    if(GetPayment(who, theItem, duration))
	
	if(MoveItemToContainer(theItem, FindAndOpenCategoryContainer(category), 1))
	    SetupAuctionItem(theItem, duration, price, who);
	else
	    SendSysMessage(who, "Blad systemu aukcyjnego: kod 1", FONT_NORMAL, RED_COLOR);
	endif
	
    endif
    
endfunction

function GetPayment(who, theItem, duration)

    var theGold;
    
    theGold := theItem.weight;
    
    theGold := theGold + CInt((duration/110));
    
    if(who.spendgold(theGold))
	return 1;
    else
	SendSysMessage(who, "Nie masz odpowiedniej ilosci zlota: " + theGold);
	return 0;
    endif
    
endfunction

function SetupActionItem(theItem, duration, price, seller)

    SetObjProperty(theItem, "ItemDuration", ReadGameClock()+duration);
    SetObjProperty(theItem, "ItemPrice", price);
    SetObjProperty(theItem, "SellerSer", seller.serial);
    
    SendSysMessage(who, "Twoja aukcja zostala dodana!", FONT_NORMAL, GREEN_COLOR);
    
endfunction

function PerformDecay(category)

    foreach item in ListRootItemsInContainer(FindAndOpenCategoryContainer(category))
	if(ReadGameClock() > CInt(GetObjProperty(item, "ItemDuration")))
        if(!GetObjProperty(item, "HighBidder"))
	    ReturnItemToOwner(item);
        else
    	    SellItemToBidder(item);
        endif
	endif
    endforeach
    
endfunction

function SellITemToBidder(theItem)

    var theBidder := SystemFindObjectBySerial(CInt(GetObjProperty(theItem, "HighBidder")));
    var theOwner := SystemFindObjectBySerial(CInt(GetObjProperty(theItem, "ItemSeller")));
    
    
    if(!theBidder)
	syslog("Osoba ktora bidowala w aukcji juz nie istnieje, zwrot do wlasciciela");
	ReturnItemToOwner(theItem);
	return;
    endif
    
    syslog("Wysylka " + theItem.desc + "/" + theItem.serial + " do " + theBidder.serial + " - wygral aukcje");
    
    SendPostItem(theOwner, theBidder.name, "Wygrana aukcji - " + theItem.desc, "Gratulacje. Twoja oferta byla najwyzsza", CInt(GetObjProperty(theItem, "CurrPrice")), theItem);
   
    
    ClearAuctionItem(theItem);
    
endfunction

function ReturnItemToOwner(theItem)

    var theOwner := SystemFindObjectBySerial(CInt(GetObjProperty(theItem, "ItemSeller")), 1);
	if(theOwner)
        syslog("Zwrot " + theItem.desc + "/" + theItem.serial + " do wlasciciela " + theOwner.name);
	SendPostItem(theOwner, theOwner.name, "Zwrot " + theItem.desc, "Zwrot aukcji - termin minal", 0, theItem);
	ClearAuctionItem(theItem);
	else
	    syslog("Niszczenie " + theItem.desc + " - wlasciciel nie istnieje na VF");
	    DestroyItem(theItem);
	endif
	
endfunction

function ClearAuctionItem(theItem)

    EraseObjProperty(theItem, "ItemSeller");
    EraseObjProperty(theItem, "HighBidder");
    EraseObjProperty(theItem, "ItemDuration");
    EraseObjProperty(theItem, "CurrPrice");
    EraseObjProperty(theItem, "ItemPrice");
    EraseObjProperty(theItem, "SellerComment");

endfunction

// gump functions


function ChooseCategory(who, theAuctionStone, TYPE_AUCTION := "NORMAL")

	// For smooth gump display
	Malloc(who);

    GInit();
    GResPic(106, 50, 3500, 527, 381);
    GTextLine(274, 64, 0, "A U K C J E  S W I A T A");
    GGumpPic(123, 70, 113, 0); 
    GGumpPic(578, 67, 113, 0); 
    GGumpPic(124, 378, 113, 0); 
    GGumpPic(575, 383, 113, 0); 
    GTextLine(247, 106, 0, "Zwierzeta");
    GTextLine(330, 106, 33, ItemsCount(who, theAuctionStone, "Zwierzeta", TYPE_AUCTION));
    GTextLine(247, 136, 0, "Zbroje");
    GTextLine(330, 136, 33, ItemsCount(who, theAuctionStone, "Zbroje", TYPE_AUCTION));
    GTextLine(247, 166, 0, "Bronie");
    GTextLine(330, 166, 33, ItemsCount(who, theAuctionStone, "Bronie", TYPE_AUCTION));
    GTextLine(247, 196, 0, "Mikstury");
    GTextLine(330, 196, 33, ItemsCount(who, theAuctionStone, "Mikstury", TYPE_AUCTION));
    GTextLine(247, 226, 0, "Narzedzia");
    GTextLine(330, 226, 33, ItemsCount(who, theAuctionStone, "Narzedzia", TYPE_AUCTION));
    GTextLine(247, 256, 0, "Deedy");
    GTextLine(330, 256, 33, ItemsCount(who, theAuctionStone, "Deedy", TYPE_AUCTION));
    GTextLine(247, 286, 0, "Magiczne");
    GTextLine(330, 286, 33, ItemsCount(who, theAuctionStone, "Magiczne", TYPE_AUCTION));
    GTextLine(247, 316, 0, "Inne");
    GTextLine(330, 316, 33, ItemsCount(who, theAuctionStone, "Inne", TYPE_AUCTION));
    GButton(210, 108, 5605, 5601, 1);
    GButton(210, 138, 5605, 5601, 2);
    GButton(210, 168, 5605, 5601, 3);
    GButton(210, 198, 5605, 5601, 4);
    GButton(210, 228, 5605, 5601, 5);
    GButton(210, 258, 5605, 5601, 6);
    GButton(210, 288, 5605, 5601, 7);
    GButton(210, 318, 5605, 5601, 8);
    
    GButton(169, 384, 5538, 5538, 777);
    GTextLine(196, 386, 0, "Wroc");
    
    var res := GSend(who)[0];
    
        case(res)
            1:  return "Zwierzeta";
            2:  return "Zbroje";
            3:  return "Bronie";
            4:  return "Mikstury";
            5:  return "Narzedzia";
            6:  return "Deedy";
            7:  return "Magiczne";
            8:  return "Inne";
            777: return "BACK";
            default: return "error";
        endcase
        
        return "error";
        
endfunction

function AuctionDecay()

    var cats := array { "Zwierzeta",
			"Zbroje",
			"Bronie",
			"Mikstury",
			"Narzedzia",
			"Deedy",
			"Magiczne",
			"Inne" };
            
    foreach cat in cats
        PerformDecay(cat);
    endforeach
    
endfunction

function ItemsCount(who, theAuctionStone, auctionCategory, TYPE_AUCTION := "NORMAL")


    var theCont := FindAndOpenCategoryContainer(auctionCategory);
    
        if(!theCont)
            SendSysMessage(who, "Blad systemu aukcyjnego: kod 2", FONT_NORMAL, RED_COLOR);
            EraseObjProperty(who, "#AucWindow");
            return;
        endif
        
    var theItems := array();
        
    foreach item in ListRootItemsInContainer(theCont)
        if(TYPE_AUCTION == "INSTANT")
            if(GetObjProperty(item, "ItemPrice"))
                theItems.append(item);
            endif
        endif
        if(TYPE_AUCTION == "AUCTIONS")
            if(GetObjProperty(item, "CurrPrice"))
                theItems.append(item);
            endif
        endif
        
        if(TYPE_AUCTION == "NORMAL")
            theItems.append(item);
        endif
    endforeach
    
    return theItems.size();
    
endfunction
