//-----------------------------------------------------------------------------
// Modified: 2007-11-23
//-----------------------------------------------------------------------------

use uo;
use storage;

//-----------------------------------------------------------------------------

include "include/objtypes/misc";
include "include/const/misc";
include "include/move";
include "include/packets";
include "include/escrow";

//--------------------------------------------------------------------------------

const PROP_MAILS             := "Mails";
const MAILS_HISTORY          := "MailHistory";

//-----------------------------------------------------------------------------

const MAILF_READ             := 0x01;
const MAILF_GOLD_SPEND       := 0x02;
const MAILF_RETURNED         := 0x04;

const MAIL_TEXT_TIME         := 604800;
const MAIL_ITEM_TIME         := 86400;
const MAIL_READ_TIME         := 86400;

const MAIL_NORMAL_FEE        := 150;
const MAIL_ITEM_FEE          := 400;

//================================================================================
//--------------------------------------------------------------------------------
// Mail
//--------------------------------------------------------------------------------
//================================================================================

function FindMailBox( character )

  if(!character or !character.isa(POLCLASS_MOBILE) or !character.serial)
    return 0;
  endif

  var worldMail     := OpenWorldMail();
  var mail_obj_name := MAILBOX_PREFIX + character.serial;
  var Mailbox       := FindRootItemInStorageArea( worldMail, mail_obj_name );

  if(!Mailbox)
    Mailbox := CreateRootItemInStorageArea( worldMail, mail_obj_name, UOBJ_STORE_MAILBOX );
    LogNotice("[FindMailBox] "+GetRealName(character)+"/"+character.serial+" Tworzenie nowej skrzynki pocztowej.", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
  endif
  
  if(!Mailbox)
    LogError("[FindMailBox] "+GetRealName(character)+"/"+character.serial+" Nie udalo sie stworzyc skrzyni pocztowej: "+Mailbox, LOGF_SCRIPTS);
  endif
  
  return Mailbox;
  
endfunction

//--------------------------------------------------------------------------------

function WipeMailBox( character )

  var worldmail := OpenWorldMail();
  var mail_obj_name := MAILBOX_PREFIX + character.serial;
  return DestroyRootItemInStorageArea( worldmail, mail_obj_name );

endfunction

//-----------------------------------------------------------------------------

function OpenWorldMail()

  var Mail := FindStorageArea( MAIL_STORAGE );
  if (!Mail)
    Mail := CreateStorageArea( MAIL_STORAGE );
  endif

  if (!Mail)
    LogError("[OpenWorldMail] Nie udalo sie stworzyc globalnej poczty!", LOGF_SCRIPTS);
  endif

  return Mail;

endfunction

//-----------------------------------------------------------------------------

function FormatMailProps(byref mailProps)

  mailProps := struct;
  mailProps.+senderName   := "";
  mailProps.+senderSerial := 0;
  mailProps.+title        := "";
  mailProps.+text         := array;
  mailProps.+attachedItem := 0;
  mailProps.+attachedGold := 0;
  mailProps.+requiredGold := 0;
  mailProps.+sendTime     := 0;
  mailProps.+existTime    := 0;
  mailProps.+flags        := 0;
  mailProps.+mailCost     := MAIL_NORMAL_FEE;

endfunction

//-----------------------------------------------------------------------------

function VerifyMail(who, mailProps)

  var wmBox := FindMailBox(who);
  var Err := error;

  if(!wmBox)
    Err.+errortext := who.name+" nie posiada skrzynki pocztowej.";
    return Err;
  endif

  if(!mailProps.title.size() or !mailProps.title)
    Err.+errortext := "Wiadomosc nie posiada tytulu.";
    return Err;
  endif

  if(!mailProps.senderName)
    Err.+errortext := "Wiadomosc nie posiada nadawcy.";
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetMails(who, byref Mails)

  var wmBox := FindMailBox(who);

  Mails := GetObjProperty(wmBox, PROP_MAILS);

  if(!Mails or !Mails.size())
    Mails := dictionary;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMailNextId(who)

  var wmBox := FindMailBox(who);

  var nextId := CInt(GetObjProperty(wmBox, "NextMailId"))+1;
  SetObjProperty(wmBox, "NextMailId", nextId);

  return nextId;

endfunction

//-----------------------------------------------------------------------------

function PostMail(who, mailProps)

  var Err := VerifyMail(who, mailProps);

  if(!Err)
    return Err;
  endif

  Err := error;

  var wmBox := FindMailBox(who);

  if(!wmBox)
    Err.errortext := who.name+" nie posiada skrzynki pocztowej.";
    return Err;
  endif

  var Mails;
  GetMails(who, Mails);

  var Sender := FindPlayerBySerial(mailProps.senderSerial);

  if((!Sender and mailProps.senderSerial) or !mailProps.senderName)
    Err.errortext := "Nie odnaleziono nadawcy!";
    return Err;
  endif

  var aItem := 0;

  if(mailProps.attachedItem)
    aItem := FindItemBySerial(mailProps.attachedItem);

    if(!aItem)
      Err.errortext := "Nie odnaleziono dolaczonego przedmiotu.";
      return Err;
    endif

    if(!MoveItemToContainer(aItem, wmBox))
      Err.errortext := "Nie udalo sie dolaczyc przedmiotu.";
      return Err;
    endif

  endif

  if(mailProps.attachedGold and !(mailProps.flags & MAILF_GOLD_SPEND))
    if(!SpendGold(Sender, mailProps.attachedGold))
      Err.errortext := "Nie masz wystarczajacej ilosci zlota przy sobie, jaka chcial"+ggm(who,3)+" dolaczyc do wiadomosci.";
      if(aItem)
        MoveToPack(aItem, Sender);
      endif
      return Err;
    endif
  endif

  mailProps.sendTime := ReadGameClock();

  if(aItem)
    ReleaseItem(aItem);
  endif

  if(!mailProps.existTime)
    if(aItem)
      mailProps.existTime := MAIL_ITEM_TIME;
    else
      mailProps.existTime := MAIL_TEXT_TIME;
    endif
  endif

  
  var historyProps := mailProps;
  var smBox := FindMailBox(Sender);
  var mailHistory := GetMailsHistory(smBox);
  historyProps.+hAttachedGold := historyProps.attachedGold;
  historyProps.+hAttachedItem := FindItemBySerial(historyProps.attachedItem).desc;
  historyProps.+type := 1; //1-wyslane 2-odebrane
  historyProps.+receiver := who.name;
  mailHistory.append(historyProps);

  var nextId := GetMailNextId(who);
  Mails[nextId] := mailProps;

  if(IsFullMailHistory(smBox))
    mailHistory.erase(1);
    SetObjProperty(smBox, MAILS_HISTORY, mailHistory);
  else
    SetObjProperty(smBox, MAILS_HISTORY, mailHistory);
  endif

  SetObjProperty(smBox, MAILS_HISTORY, mailHistory);
  SetObjProperty(wmBox, PROP_MAILS, Mails);
  SendSysMessage(who, "Otrzymales nowa poczte.", FONT_NORMAL, COLOR_WHITE);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetMailReadFlag(who, Id)

  var wmBox := FindMailBox(who);
  var Mails;

  GetMails(who, Mails);

  if(Mails.exists(Id))
    var mailProps := Mails[Id];

    if(!(mailProps.flags & MAILF_READ))
      mailProps.flags := mailProps.flags | MAILF_READ;
      //mailProps.sendTime := ReadGameClock();  - to chyba bez sensu, po co tutaj zmieniac sendTime?
     
      if(MAIL_READ_TIME < (mailProps.sendTime - ReadGameClock()) + mailProps.existTime)
        mailProps.existTime := MAIL_READ_TIME;
      endif

      Mails[Id] := mailProps;
      SetObjProperty(wmBox, PROP_MAILS, Mails);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function PostTextMail(who, fromMob, Title, TextArr)

  var mailProps;
  FormatMailProps(mailProps);

  if(fromMob.isa(POLCLASS_MOBILE))
    mailProps.senderName := GetRealName(fromMob);
    mailProps.senderSerial := fromMob.serial;
  else
    mailProps.senderName := fromMob;
    mailProps.senderSerial := 0;
  endif

  mailProps.title := ToUnicode(Title);
  mailProps.text := array;

  var i;

  for(i:=1; i<=10; i:=i+1)
    if(i <= TextArr.size())
      mailProps.text[i] := ToUnicode(TextArr[i]);
    else
      mailProps.text[i] := "";
    endif
  endfor

  return PostMail(who, mailProps);

endfunction

//-----------------------------------------------------------------------------

function PostGoldMail(who, fromMob, Title, TextArr, GoldAmt)

  var mailProps;
  FormatMailProps(mailProps);

  if(fromMob.isa(POLCLASS_MOBILE))
    mailProps.senderName := GetRealName(fromMob);
    mailProps.senderSerial := fromMob.serial;
  else
    mailProps.senderName := fromMob;
    mailProps.senderSerial := 0;
  endif

  mailProps.title        := ToUnicode(Title);
  mailProps.text         := array;
  mailProps.attachedGold := GoldAmt;
  mailProps.flags        := MAILF_GOLD_SPEND;

  var i;

  for(i:=1; i<=10; i:=i+1)
    if(i <= TextArr.size())
      mailProps.text[i] := ToUnicode(TextArr[i]);
    else
      mailProps.text[i] := array;
    endif
  endfor

  var PostRes := PostMail(who, mailProps);

  if(!PostRes)
    var Msg := "PostGoldMail: R: "+GetRealName(who)+"["+who.serial+"] S: "+mailProps.senderName+"["+mailProps.senderSerial+"] GA: "+FormatSize(GoldAmt, ".")+" Err: "+PostRes.errortext;
    LogNotice(Msg);
  endif

  return PostRes;

endfunction

//-----------------------------------------------------------------------------

function PostItemMail(who, fromMob, Title, TextArr, aItem, oFlags := 0, eTime := 0)

  var mailProps;
  FormatMailProps(mailProps);

  if(fromMob.isa(POLCLASS_MOBILE))
    mailProps.senderName := GetRealName(fromMob);
    mailProps.senderSerial := fromMob.serial;
  else
    mailProps.senderName := fromMob;
    mailProps.senderSerial := 0;
  endif

  mailProps.title        := ToUnicode(Title);
  mailProps.text         := array;
  mailProps.attachedItem := aItem.serial;
  mailProps.existTime    := eTime;
  mailProps.flags        := oFlags;

  var i;

  for(i:=1; i<=10; i:=i+1)
    if(i <= TextArr.size())
      mailProps.text[i] := ToUnicode(TextArr[i]);
    else
      mailProps.text[i] := array;
    endif
  endfor

  var PostRes := PostMail(who, mailProps);
  
  if(!PostRes)
    var Msg := "PostItemMail: R: "+GetRealName(who)+"["+who.serial+"] S: "+mailProps.senderName+"["+mailProps.senderSerial+"] aI: "+CutWord(aItem.desc, 30)+"["+aItem.serial+"] Err: "+PostRes.errortext;
    LogNotice(Msg);
  endif

  return PostRes;

endfunction

//-----------------------------------------------------------------------------

function DeleteMail(who, Id)

  var wmBox := FindMailBox(who);
  var Mails;
  GetMails(who, Mails);

  if(!Mails.exists(Id))
    return 0;
  endif

  var mailProps := Mails[Id];
  var hMailProps := Mails[Id];

  hMailProps.+type := 2;
  hMailProps.+receiver := FindPlayerBySerial(hmailProps.senderSerial).name;
  var mailHistory := GetMailsHistory(wmBox);
  mailHistory.append(hMailProps);
  print("zloto :"+hMailProps.hAttachedGold);
  print("item :"+hMailProps.hAttachedItem);

  if(mailProps.attachedItem)
    var aItem := FindItemBySerial(mailProps.attachedItem);

    if(aItem and aItem.container == wmBox)
      var Sender := FindPlayerBySerial(mailProps.senderSerial);
      if(!Sender or (mailProps.flags & MAILF_RETURNED))
        if(!MoveItemToPlayerEscrow(Sender, aItem))
          LogError("[DeleteMail] Nie udalo sie przeniesc przedmiotu z maila do Escrow, nadawca: '"+GetRealName(Sender)+"'. Item: "+aItem.serial+"/"+aItem.objtype+"/"+GetName(aItem));
          DestroyItem(aItem);
        endif
      else
        var Err := PostItemMail(Sender, who, "Przedmiot zostal zwrocony.", 
        array(who.name+" odmowil"+ggm(who,4)+" przyjecia wyslanego przez Ciebie przedmiotu."), aItem, MAILF_RETURNED, MAIL_TEXT_TIME);

        if(!Err)
          return Err;
        endif
      endif
    endif
  endif

  GetMails(who, Mails);
  Mails.erase(Id);

  if(IsFullMailHistory(wmBox))
    mailHistory.erase(1);
    SetObjProperty(wmBox, MAILS_HISTORY, mailHistory);
  else
    SetObjProperty(wmBox, MAILS_HISTORY, mailHistory);
  endif

  SetObjProperty(wmBox, PROP_MAILS, Mails);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function HasUnreadMails(who)

  var Mails;
  GetMails(who, Mails);

  foreach mailProps in Mails
    if(!(mailProps.flags & MAILF_READ))
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------


function GetMailsHistory(wmBox)

  var history := GetObjProperty(wmbox,MAILS_HISTORY);

  if(!history or !history.size()) 
    history := array;
  endif

  return history;

endfunction

//-----------------------------------------------------------------------------

function IsFullMailHistory(wmBox)

  var mailsHistory := GetMailsHistory(wmBox);

  if(!mailsHistory or !mailsHistory.size())
    return;
  endif

  if(mailsHistory.size() >= 20)
    return 1;
  endif

  return 0;
  

endfunction

//-----------------------------------------------------------------------------

