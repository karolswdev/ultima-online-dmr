//-----------------------------------------------------------------------------

const DCC_EMOTE_FOUND        := 0x01;
const DCC_BWORD_FOUND        := 0x02;
const DCC_CWORD_FOUND        := 0x03;
const DCC_CLEAR              := 0x00;

//-----------------------------------------------------------------------------

const DEF_SP_INWORD        := 2;

//-----------------------------------------------------------------------------

function CenCheck(text)

  var WordsCfg := ReadConfigFile(CENWORDS_CFG);
  var NrMax    := GetConfigMaxIntKey(WordsCfg);

  for i := 1 to NrMax
    if(text[WordsCfg[i].Word])
      return 0;
    endif
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeepCenCheck(text)

  if(text.size() == error)
    text := CAscZ(text);
  endif

  var Test   := array;
  var mWords := array(array);
  var mLow   := array;
  var Unics  := ReadConfigFile(UNICS_CFG);
  var bWords := ReadConfigFile(BWORDS_CFG);
  var cWords := ReadConfigFile(CWORDS_CFG);
  var Emotes := ReadConfigFile(EMOTES_CFG);
  var bMaxId := GetConfigMaxIntKey(bWords);
  var cMaxId := GetConfigMaxIntKey(cWords);
  var eMaxId := GetConfigMaxIntKey(Emotes);
  var Chr,i,Pos;

  foreach mChr in text
    Chr := mChr;

    if(Unics[Chr])
      Chr := Unics[Chr].Chr;
    endif

    if((Chr >= 0x41 and Chr <= 0x5A) or (Chr >= 0x61 and Chr <= 0x7A))
      if(Chr < 0x61)
        Chr := Chr + 0x20;
      endif

      if(Test[Test.size()] != Chr)
        Test.append(Chr);
      endif

      mWords[mWords.size()].append(Chr);
    endif

    if(Chr == 0x20)
      if(mWords[mWords.size()].size())
        mWords[mWords.size()] := CChrZ(mWords[mWords.size()]);
        mWords.append(array);
      endif
    endif

    mLow.Append(Chr);
  endforeach

  Test := CChrZ(Test);

  if(!mWords[mWords.size()].size())
    mWords.erase(mWords.size());
  else
    mWords[mWords.size()] := CChrZ(mWords[mWords.size()]);
  endif

  foreach Word in mWords
    for(i:=1;i<=cMaxId;i:=i+1)
      if(cWords[i].Equal)
        if(Word == cWords[i].Word)
          return DCC_CWORD_FOUND;
        endif
      else
        Pos := CheckWord(Word,cWords[i].Word,DEF_SP_INWORD,cWords[i].Direct);
        if(Pos)
          if(cWords[i].MinPos)
            if(Pos > cWords[i].MinPos and (len(Word) - Pos) > cWords[i].MaxPos)
              continue;
            endif
          endif
          return DCC_CWORD_FOUND;
        endif
      endif
    endfor
  endforeach

  for(i:=1;i<=bMaxId;i:=i+1)
    if(CheckWord(Test,bWords[i].Word,bWords[i].Sp))
      return DCC_BWORD_FOUND;
    endif
    sleepms(1);
  endfor

  Test := CChrZ(mLow);

  if(Test[":"] or Test[";"] or Text["x"])
    var Ex;

    for(i:=1;i<=eMaxId;i:=i+1)
      Ex := 0;
      TestEmote(mLow,text,Test,Ex,Emotes[i].Em);
      if(Ex)
        return DCC_EMOTE_FOUND;
      endif
    endfor
  endif

  return DCC_CLEAR;

endfunction

//-----------------------------------------------------------------------------

function CheckWord(byref Sent,byref Word,SpMax,Direct := 1)

  if(Direct)
    if(Sent[Word])
      return 1;
    endif
  endif

  var i,wk := 1,sp := 0,f := 0;

  for(i:=1;i<=len(Sent);i:=i+1)
    if(Sent[i] == Word[wk])
      if(wk == 1)
        f := i;
      endif
      wk := wk + 1;
      sp := 0;
    else
      sp := sp + 1;
    endif

    if(sp >= SpMax)
      wk := 1;
      f  := 0;
      sp := 0;
    endif

    if(wk-1 == len(Word))
      return f;
    endif

    sleepms(2);
  endfor

  return 0;

endfunction

//-----------------------------------------------------------------------------

function TestEmote(byref mLow, byref Msg, byref Test, byref Exist, byref Emot)

  var Idx := 1;
  var Cnt := 0;
  var EmotArr := CAscZ(Emot);

  while(Idx and Cnt < 3)
    Idx := SearchEmoteIndex(mLow, EmotArr, Idx);

    if(Idx)
      var eLen := EmotArr.size();
      DeleteWord(Msg, Idx, eLen);
      DeleteWord(mLow, Idx, eLen);
      Exist := 1;
    endif
    Cnt += 1;
  endwhile

  if(Exist)
    Test  := CChrZ(mLow);
  endif

endfunction

//-----------------------------------------------------------------------------

function SearchEmoteIndex(byref Sent, byref Word, StartPos)

  if(Sent.size() < StartPos+Word.size()-1)
    return 0;
  endif

  var i,wk := 0;

  for(i:=StartPos; i<=Sent.size()-Word.size()+1; i+=1)

    wk := 0;
    
    if(Sent[i] == Word[1])
      for(wk:=2; wk<=Word.Size(); wk+=1)
        if(Sent[i+wk-1] != Word[wk])
          wk := 0;
          break;
        endif
      endfor
    endif
    
    if(wk)
      // sprawdzamyu czy koniec zdania lub byla chociaz spacja na koncu
      if(Sent.size() > i+Word.size()-1)
        if(Sent[i+Word.size()] != 0x20)
          continue;
        endif
      endif

      return i;
    endif
  endfor

  return 0;

endfunction

//-----------------------------------------------------------------------------

function DeleteWord(byref Sent, sPos, sLen)

  var i;
  for(i:=1;i<=sLen;i:=i+1)
    Sent.erase(sPos);
  endfor

endfunction

//-----------------------------------------------------------------------------

function CenCheckLine(who, byref Line)

  var Res := DeepCenCheck(Line);

  case(Res)
    DCC_EMOTE_FOUND:    SendSysMessage(who, "W twojej wiadomosci sa niedozwolone znaki.", FONT_NORMAL, COLOR_RED);
    DCC_BWORD_FOUND:    
    DCC_CWORD_FOUND:    SendSysMessage(who, "W twojej wiadomosci sa niedozwolone slowa.", FONT_NORMAL, COLOR_RED);
    DCC_CLEAR:          return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CenCheckLines(who, byref Lines)

  var i := 1;
  foreach Line in Lines
    if(!CenCheckLine(who, Line))
      SendSysMessage(who, "Linia numer "+i+".", FONT_NORMAL, COLOR_RED);
      return 0;
    endif
    i := i + 1;
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------
