//-----------------------------------------------------------------------------
// Modified: 2007-11-24
//-----------------------------------------------------------------------------

function BuildMailPostGump(byref mailProps)

  GInit(30, 30);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GSetUnicode();
  GResPic(0,0,BKGND_BLACKSLAB,500,420);
  GResPic(25, 30, BKGND_PAPER, 450, 58);
  GResPic(45, 100, BKGND_PAPER, 430, 210);
  GGumpPic(29, 34, 2002);
  GTextLine(55, 36, 999, "Tytul: "); 
  GTextEntry(95, 36, 370, 20, 0, 5, mailProps.title); 
  GTextLine(55, 59, 999, "Do: "); 
  GTextEntry(95, 59, 370, 20, 0, 6, mailProps.senderName); 
  
  var i;

  for(i:=1; i<=10; i:=i+1)
    GTextEntry(55, 85+i*20, 410, 20, 0, 10+i, mailProps.text[i]);
  endfor
  
  GResPic(25, 340, BKGND_PAPER, 60, 60);
  
  if(mailProps.attachedItem)
    GButton(90, 383, 2117, 2118, 14);
    GTextLine(110, 380, 999, "Odlacz przedmiot");
  else
    GButton(90, 383, 2117, 2118, 12);
    GTextLine(110, 380, 999, "Dolacz przedmiot");
  endif
  
  if(mailProps.attachedItem)
    var Item := FindItemBySerial(mailProps.attachedItem);
    GTextCrop(30, 315, 340, 20, 1152, Item.desc);
    GTilePicHue(35, 360, Item.graphic,Item.color);

    GTextLine(100, 340, 999, "Zaplata:");
    GResPic(155, 337, BKGND_PAPER, 100, 24);
    GTextEntry(165, 340, 200, 20, 0, 7, mailProps.requiredGold); 
  endif

  GTextLine(360, 310, 999, "Koszt:");
  GTextLine(410, 310, 1121, FormatSize(CInt(mailProps.mailCost), ".")+" gp"); 

  GTextLine(290, 340, 999, "Wyslij zloto:"); 
  GResPic(375, 337, BKGND_PAPER, 100, 24);
  GTextEntry(385, 340, 200, 20, 0, 8, mailProps.attachedGold); 

  GButton(340, 383, 2117, 2118, 10);
  GTextLine(360, 380, 999, "Wyslij");

  GButton(410, 383, 2117, 2118, 11);
  GTextLine(430, 380, 999, "Anuluj");

endfunction

//-----------------------------------------------------------------------------

function FillMailPost(byref Res, byref mailProps)

  if(!Res.unitext[5] or !Res.unitext[6])
    return;
  endif

  mailProps.title := Res.unitext[5];
  mailProps.senderName := CChrZ(Res.unitext[6]);
  if(Res.unitext.exists(7))
    mailProps.requiredGold := CInt(CChrZ(Res.unitext[7]));
  endif
  mailProps.attachedGold := CInt(CChrZ(Res.unitext[8]));

  var i;

  for(i:=1; i<=10; i:=i+1)
    mailProps.text[i] := Res.unitext[10+i];
  endfor

endfunction

//-----------------------------------------------------------------------------

function VerifyPostMail(who, byref mailProps)

  var Err := error;

  if(!mailProps.title.size())
    Err.errortext := "Nie wpisano tytulu wiadomosci.";
    return Err;
  endif

  if(!mailProps.senderName)
    Err.errortext := "Nie wpisano imienia odbiorcy.";
    return Err;
  endif

  var Sender := FindCharacterByName(mailProps.senderName, 1);

  if(!Sender)
    Err.errortext := "Nie znaleziono osoby o imieniu "+mailProps.senderName+".";
    return Err;
  elseif(Sender.cmdlevel and !who.cmdlevel)
    Err.errortext := "Nie znaleziono osoby o imieniu "+mailProps.senderName+".";
    return Err;
  endif

  if(Sender == who)
    Err.errortext := "Chcesz wyslac poczte do siebie?";
    return Err;
  endif

  mailProps.senderSerial := Sender.serial;
  mailProps.senderName   := GetRealName(Sender);

  if(mailProps.requiredGold and !mailProps.attachedItem)
    mailProps.requiredGold := 0;
  endif

  if(mailProps.attachedGold and mailProps.requiredGold)
    Err.errortext := "Mozesz albo wyslac zloto, albo zadac zaplaty za przedmiot.";
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SendMailPost(who, mailBox := 0, Title := "", senderName := "")

  var mailProps;
  FormatMailProps(mailProps);
  mailProps.title := ToUnicode(Title);
  mailProps.senderName := senderName;

  var Res;

  while(who)
    BuildMailPostGump(mailProps);
    Res := GSend(who);
    FillMailPost(Res, mailProps);

    if(mailBox)
      if(Distance(who, mailBox) > 2)
        SendSysMessage(who, "Stoisz za daleko.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif

    if(Res.butid == 10)

      var vRes := VerifyPostMail(who, mailProps);

      if(!vRes)
        SendSysMessage(who, vRes.errortext, FONT_NORMAL, COLOR_RED);
        continue;
      endif

      if(!SpendGold(who, mailProps.mailCost))
        SendSysMessage(who, "Potrzebujesz "+FormatSize(mailProps.mailCost, ".")+" gp, aby moc wyslac ta wiadomosc.", FONT_NORMAL, COLOR_RED);
        continue;
      endif

      var nProps := mailProps;
      var Receiver := FindPlayerBySerial(mailProps.senderSerial);
      nProps.senderName := GetRealName(who);
      nProps.senderSerial := who.serial;
   
      vRes := PostMail(Receiver, nProps);

      if(!vRes)
        CreateGold(who.backpack, mailProps.mailCost);
        SendSysMessage(who, vRes.errortext, FONT_NORMAL, COLOR_RED);
        continue;
      else
        SendSysMessage(who, "Wyslal"+ggm(who,3)+" wiadomosc do osoby "+GetRealName(Receiver)+".", FONT_NORMAL, COLOR_WHITE);
        break;
      endif

    elseif(Res.butid == 12)
      AttachItem(who, mailProps);
    elseif(Res.butid == 14)
      CancelAttachItem(who, mailProps);
    else
      break;
    endif

  endwhile

  if(mailProps.attachedItem)
    var Item := FindItemBySerial(mailProps.attachedItem);
    ReleaseItem(Item);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------

function BuildGlobalMailPostGump(byref mailProps)

  GInit(60, 60);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GSetUnicode();
  GResPic(0,0,BKGND_BLACKSLAB,500,360);
  GResPic(25, 30, BKGND_PAPER, 450, 58);
  GResPic(45, 100, BKGND_PAPER, 430, 210);
  GGumpPic(29, 34, 2002);
  GTextLine(55, 36, 999, "Tytul: "); 
  GTextEntry(95, 36, 370, 20, 0, 5, mailProps.title); 
  
  var i;

  for(i:=1; i<=10; i:=i+1)
    GTextEntry(55, 85+i*20, 410, 20, 0, 10+i, mailProps.text[i]);
  endfor
  
  GTextLine(40, 320, 999, "Koszt:");
  GTextLine(90, 320, 1121, FormatSize(CInt(mailProps.mailCost), ".")+" gp"); 

  GButton(340, 323, 2117, 2118, 10);
  GTextLine(360, 320, 999, "Wyslij");

  GButton(410, 323, 2117, 2118, 11);
  GTextLine(430, 320, 999, "Anuluj");

endfunction

//-----------------------------------------------------------------------------

function FillGlobalMailPost(byref Res, byref mailProps)

  mailProps.title := Res.unitext[5];

  var i;

  for(i:=1; i<=10; i:=i+1)
    mailProps.text[i] := Res.unitext[10+i];
  endfor

endfunction

//-----------------------------------------------------------------------------

function VerifyGlobalPostMail(byref mailProps)

  var Err := error;

  if(!mailProps.title.size())
    Err.errortext := "Nie wpisano tytulu wiadomosci.";
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SendGlobalMail(who, mobArr, mailBox := 0)

  if(!mobArr or !mobArr.size() or !who)
    return 0;
  endif

  var mailProps;
  var Res;

  FormatMailProps(mailProps);
  mailProps.mailCost := MAIL_NORMAL_FEE * mobArr.size();

  while(who)
    BuildGlobalMailPostGump(mailProps);
    Res := GSend(who);
    FillGlobalMailPost(Res, mailProps);

    if(mailBox)
      if(Distance(who, mailBox) > 2)
        SendSysMessage(who, "Stoisz za daleko.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif

    if(Res.butid == 10)

      var vRes := VerifyGlobalPostMail(mailProps);

      if(!vRes)
        SendSysMessage(who, vRes.errortext, FONT_NORMAL, COLOR_RED);
        continue;
      endif

      if(!SpendGold(who, mailProps.mailCost))
        SendSysMessage(who, "Potrzebujesz "+FormatSize(mailProps.mailCost, ".")+" gp, aby moc wyslac ta wiadomosc.", FONT_NORMAL, COLOR_RED);
        continue;
      endif

      var nProps := mailProps;
      var vCnt   := 0;
      var i;

      nProps.senderName := GetRealName(who);
      nProps.senderSerial := who.serial;

      for(i:=1; i<=mobArr.size(); i:=i+1)
        var Receiver := FindPlayerBySerial(mobArr[i]);
        vRes := PostMail(Receiver, nProps);
        if(vRes)
          vCnt := vCnt + 1;
        endif
      endfor

      SendSysMessage(who, "Wyslal"+ggm(who,3)+" wiadomosc do "+vCnt+" osob.", FONT_NORMAL, COLOR_WHITE);
      return 1;

    else
      break;
    endif

  endwhile

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AttachItem(who, byref mailProps)

  SendSysMessage(who, "Jaki przedmiot chcesz dolaczyc?", FONT_NORMAL, COLOR_GREEN);

  var aItem := Target(who);

  if(!StandardItemCheck(who, aItem))
    return;
  elseif(GetTopContOwner(aItem) != who)
    SendSysMessage(who, "Przedmiot nie znajduje sie w twoim plecaku.", FONT_NORMAL, COLOR_RED);
    ReleaseItem(aItem);
    return;
  elseif(aItem.objtype == UOBJ_GOLDCOIN)
    SendSysMessage(who, "Zloto dolacza sie w inny sposob.", FONT_NORMAL, COLOR_RED);
    ReleaseItem(aItem);
    return;
  endif

  mailProps.attachedItem := aItem.serial;
  mailProps.mailCost := MAIL_ITEM_FEE;
  SendSysMessage(who, "Dolaczyl"+ggm(who,3)+" przedmiot '"+aItem.desc+"' do wiadomosci.", FONT_NORMAL, COLOR_GREEN);

endfunction

//-----------------------------------------------------------------------------

function CancelAttachItem(who, byref mailProps)

  var aItem := FindItemBySerial(mailProps.attachedItem);

  if(aItem)
    mailProps.attachedItem := 0;
    mailProps.mailCost := MAIL_NORMAL_FEE;
    ReleaseItem(aItem);
    SendSysMessage(who, "Anulowal"+ggm(who,3)+" dolaczanie przedmiotu '"+aItem.desc+"' do wiadomosci.", FONT_NORMAL, COLOR_GREEN);
  endif

endfunction

//-----------------------------------------------------------------------------
