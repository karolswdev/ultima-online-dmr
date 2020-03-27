//-----------------------------------------------------------------------------

include "include/client/fonts";

//-----------------------------------------------------------------------------

use unicode;

const UNILANG_CODE := "POL";

//-----------------------------------------------------------------------------

function StrLower(text)

  if(text.size() == error)
    return lower(text);
  else
    var Unics := ReadConfigFile(UNICS_CFG);

    foreach Chr in text
      if(Chr >= 0x41 and Chr <= 0x5A)
        Chr := Chr + 0x20;
      elseif(Unics[Chr].Low)
        Chr := Unics[Chr].Low;
      endif
    endforeach
  endif

  return text;

endfunction

//-----------------------------------------------------------------------------

function StrUpper(text)

  if(text.size() == error)
    return upper(text);
  else
    var Unics := ReadConfigFile(UNICS_CFG);

    foreach Chr in text
      if(Chr >= 0x61 and Chr <= 0x7A)
        Chr := Chr - 0x20;
      elseif(Unics[Chr].Up)
        Chr := Unics[Chr].Up;
      endif
    endforeach
  endif

  return text;

endfunction

//-----------------------------------------------------------------------------

function GetEventUniText(byref ev, Low := 0)

  var txt := ev.uc_text;
  if(txt == error)
    txt := ev.text;
  endif

  if(Low)
    return StrLower(txt);
  else
    return txt;
  endif

endfunction

//-----------------------------------------------------------------------------

function UniStrFind(txt, ftxt)

  if(txt.size() == error)
    txt := CAscZ(txt);
  endif

  if(ftxt.size() == error)
    ftxt := CAscZ(ftxt);
  endif

  var i,f;
  for(i:=1;i<=txt.size();i:=i+1)
    if(i+ftxt.size()-1 > txt.size())
      break;
    endif

    for(f:=1;f<=ftxt.size();f:=f+1)
      if(txt[i+f-1] != ftxt[f])
        break;
      endif
    endfor

    if(f-1 == ftxt.size())
      return i;
    endif
  endfor

  return 0;

endfunction

//-----------------------------------------------------------------------------

function UniSendSysMessage(who, Msg, Font := _DEFAULT_UCFONT, Color := _DEFAULT_UCCOLOR)

  if(Msg.size() == error)
    Msg := ToUnicode(Msg);
  endif

  SendSysMessageUC(who, Msg, UNILANG_CODE, Font, Color);

endfunction

//-----------------------------------------------------------------------------

function UniSendGood(who, Msg, Color := COLOR_GREEN)

  return UniSendSysMessage(who, Msg, _DEFAULT_UCFONT, Color);

endfunction

//-----------------------------------------------------------------------------

function UniSay(Msg)

  SayUC(ToUnicode(Msg));

endfunction

//-----------------------------------------------------------------------------

function UniPrintTextAbove(who, Msg, Font := _DEFAULT_UCFONT, Color := _DEFAULT_UCCOLOR)

  if(Msg.size() == error)
    Msg := ToUnicode(Msg);
  endif

  PrintTextAboveUC(who, Msg, UNILANG_CODE, Font, Color);

endfunction

//-----------------------------------------------------------------------------

function UniPrintTextAbovePrivate(who, Msg, Source, Font := _DEFAULT_UCFONT, Color := _DEFAULT_UCCOLOR)

  if(Msg.size() == error)
    Msg := ToUnicode(Msg);
  endif

  PrintTextAbovePrivateUC(who, Msg, UNILANG_CODE, Source, Font, Color);

endfunction

//-----------------------------------------------------------------------------

function ConvToUnicode(byref text)

  if(text.size() == error or !text)
    text := CAscZ(text);
    ConvIsoToUni(text);
  endif

endfunction

//-----------------------------------------------------------------------------

function ConvToAscii(byref text)

  if(text.size() != error and text)
    text := ConvPlLetters(text);
    text := CChrZ(text);
  endif

endfunction

//-----------------------------------------------------------------------------

function ToUnicode(text)

  ConvToUnicode(text);
  return text;

endfunction

//-----------------------------------------------------------------------------

function ToAscii(text)

  ConvToAscii(text);
  return text;

endfunction

//-----------------------------------------------------------------------------

function ConvPlLetters(txt)

  if(txt.size() == error or !txt)
    return txt;
  endif

  var Unics := ReadConfigFile(UNICS_CFG);

  foreach Let in txt
    if(Unics[Let].Chr)
      Let := Unics[Let].Chr;
    endif
  endforeach

  return txt;

endfunction

//-----------------------------------------------------------------------------

function ConvIsoToUni(byref text)

  if(text.size() == error or !text)
    return text;
  endif

  var Unics := ReadConfigFile(UNICS_CFG);
  var IsoChars := dictionary;

  foreach Elem in GetConfigIntKeys(Unics)
    IsoChars[CInt(Unics[Elem].Iso)] := CInt(Elem);
  endforeach
  

  foreach Let in text
    if(IsoChars.exists(Let))
      Let := IsoChars[Let];
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function ConvUniToIso(byref text)

  if(text.size() == error or !text)
    return text;
  endif

  var Unics := ReadConfigFile(UNICS_CFG);

  foreach Let in text
    if(Unics[Let].Iso)
      Let := Unics[Let].Iso;
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function ConvPlLettersRef(byref txt)

  if(txt.size() == error or !txt)
    return;
  endif

  var Unics := ReadConfigFile(UNICS_CFG);

  foreach Let in txt
    if(Unics[Let].Chr)
      Let := Unics[Let].Chr;
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function ClUniStr(byref text)

  if(!text)
    return CAscZ("");
  elseif(text.size() == error)
    return CAscZ(text);
  else
    return text;
  endif

endfunction

//-----------------------------------------------------------------------------

function CutUniWord(text, wLen)

  ConvToUnicode(text);
  
  if(text.size() > wLen)
    var nText := array;
    var i;

    for(i:=1; i<=wLen; i:=i+1)
      nText.append(text[i]);
    endfor

    return nText + CAscZ("...");
  endif

  return text;

endfunction

//-----------------------------------------------------------------------------
