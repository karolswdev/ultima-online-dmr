function ParsePost(who)

    var retArr := array();

    foreach item in ListRootItemsInContainer(OpenPostBox(who))
        retArr.append(item);
    endforeach
    
    return retArr;
    
endfunction

function OpenPostBox(who)

    var worldPost := OpenWorldPost();
    var postBoxName := "Postbox of " + who.serial;
    var thePostBox := FindRootItemInStorageArea(worldPost, postBoxName);
    
    if(!thePostBox)
        thePostBox := CreateRootItemInStorageArea(worldPost, postBoxName, UOBJ_BANKBOX);
        syslog(":: poczta :: tworzenie skrzynki pocztowej dla " + who.name);
    endif
    
    return thePostBox;
    
endfunction

function ParsePostItem(postItem)

    var retArr := array();
    
    retArr.append(CInt(GetObjProperty(postItem, "postSender"))); // 1 - sender's serial
    retArr.append(CInt(GetObjProperty(postItem, "postSentTime"))); // 2 - sent time
    retArr.append(postItem);
    
    return retArr;
    
 endfunction
 
function ClearPostItem(postItem)
    
    EraseObjProperty(postItem, "postSender");
    EraseObjProperty(postItem, "postSentTime");
    EraseObjProperty(postItem, "postTopic");
    EraseObjProperty(postItem, "postMessage");
    
endfunction

function PayPrePayment(who, givenPostItem)
    
    if(SpendGoldFromBank(who, CInt(GetObjProperty(givenPostItem, "PrePayment")))
	SendSysMessage(who, "Zaplacil" + GGm(who, 3) + " za przedmiot.", FONT_NORMAL, RED_COLOR);
	EraseObjPRoperty(postItem, "PrePayment");
	return 1;
    else
	SendSysMessage(who, "Nie masz odpowiedniej ilosci zlota w banku.", FONT_NORMAL, RED_COLOR);
	return 0;
    endif

endfunction
    
 function OpenWorldPost()
 
 var worldPost := FindStorageArea("VF POST");
 if(!worldPost)
    worldPost := CreateStorageArea("VF POST");
 endif
 
 return worldPost;
 
 endfunction
 
 function ParseSenderName(postItem)
 
    if(CInt(GetObjProperty(postItem, "postSender")) == 666)
        return "Aukcje DM-R";
	elseif(CInt(GetObjProperty(postItem, "postSender"))  == 777)
		return "Ring Smierci";
    else
        return SystemFindObjectBySerial(CInt(GetObjProperty(postItem, "postSender"))).name;
    endif
 endfunction
 
 function ParseTopic(postItem)
 
  // topic of the message
    
    var theTopic;
    
        theTopic := CStr(GetObjProperty(postItem, "postTopic"));
        
        if(!theTopic || theTopic["error"])
            theTopic := "Bez tematu";
        endif
        
    return theTopic;
    
 endfunction
 
 function ParseMessage(postItem)
 
    var theMessage := GetObjProperty(postItem, "postMessage");
    
    return theMessage;
    
endfunction

function GetPostPackage(who)

    var thePackage := SystemFindObjectBySerial(CInt(GetObjProperty(who, "#postItem")));
    
    if(thePackage)    
        SendSysMessage(who, "Znaleziono " + thePackage.desc);
    else
        SendSysMessage(who, "Nie znaleziono paczki!");
        return;
    endif
    
    return thePackage;

    
endfunction

function GetPostItem(who, theItem)

    if(MoveItemToContainer(theItem, who.backpack, 1))
        SendSysMessage(who, "Wlozyl" + GGm(who, 3) + theItem.desc + "do plecaka", FONT_NORMAL, GREEN_COLOR);
        ClearPostItem(theItem);
        EraseObjProperty(who, "#postItem");
    else
        SendSysMessage(who, "Nie masz miejsca w plecaku!", FONT_NORMAL, RED_COLOR);
    endif
    
endfunction

function PayPostItem(who, theItem)

    if(who.spendgold(CInt(GetObjProperty(theItem, "postPrePayment"))))
    
        var theCheque := CreatePrePaymentChequeForPostage(who, CInt(GetObjProperty(theItem, "postPrePayment")));
        MoveItemToContainer(theCheque, who.backpack);
        theCheque.invisible := 1;
        
        if(who.cmdlevel)
            SendSysMessage(who, "Zaplace " + SystemFindObjectBySerial(CInt(GetObjProperty(theItem, "postSender"))).name + " czekiem. Czek sie nazywa " + theCheque.desc);
        endif
        SetObjProperty(who, "#PostAttachment", theCheque.serial);
        
        SendPostItem(who, SystemFindObjectBySerial(CInt(GetObjProperty(theItem, "postSender"))).name, "Zaplata za przedmiot", "Zaplata za " + theItem.desc, 0);
        EraseObjProperty(theItem, "postPrePayment");
        EraseObjProperty(who, "#postItem");
        
        
        SendSysMessage(who, "Zaplacil" + GGM(who, 3) + " za przedmiot.", FONT_NORMAL, GREEN_COLOR);
    endif
    
endfunction

function SendPostItem(who, theName, theTopic, theMessage, prePayment, postAttachment := 0)


    var theAttachment;

    if(who.cmdlevel)
        SendSysMessage(who, "Pobranie: " + prePayment);
    endif
    
    prePayment := CInt(prePayment);
    
    if(!postAttachment)

    if(!GetObjProperty(who, "#PostAttachment"))
        SendSysMessage(who, "Musisz wyslac jakis zalacznik.", FONT_NORMAL, RED_COLOR);
        return;
    endif
    
    endif
    
    if(!postAttachment)
    
    theAttachment := SystemFindObjectBySerial(CInt(GetObjProperty(who, "#PostAttachment")));
    
    if(theAttachment.container != who.backpack)
        SendSysMessage(who, "Twoj zalacznik nie znajduje sie w plecaku.", FONT_NORMAL, RED_COLOR);
        return;
    endif
    
    else
    
    theAttachment := postAttachment;
    
    endif
    
    var thePerson := FindCharacterByName(theName);
    
    if(!thePerson)
        SendPostItem(who, who.name, "Zwrot " + theTopic, "Nie znaleziono adresata", prePayment);
        EraseObjProperty(who, "#PostAttachment");
    else
        MoveItemToContainer(theAttachment, OpenPostBox(thePerson));
        SetObjProperty(theAttachment, "postSender", who.serial);
        SetObjProperty(theAttachment, "postMessage", theMessage);
        SetObjProperty(theAttachment, "postTopic", theTopic);
        SetObjProperty(theAttachment, "postSentTime", ReadGameClock()+604800);
        
        EraseObjProperty(who, "#PostAttachment");
        
        if(prePayment > 0)
        
        SetObjProperty(theAttachment, "postPrePayment", prePayment);
        
        endif
        
    endif
    
endfunction

function CreatePrePaymentChequeForPostage(who, price)

    var theDeed := CreateItemAtLocation(5849, 1169, 0, 5359);
    
        if(theDeed)
        
            theDeed.color := 53;
            SetName(theDeed, "Deed na " + price + " zlota");
            
            SetObjProperty(theDeed, "Value", price);
            
            return theDeed;
        endif
        
endfunction

    
        
