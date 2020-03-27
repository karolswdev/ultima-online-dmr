//-----------------------------------------------------------------------------
// Created: 2010-06-13
//-----------------------------------------------------------------------------

include "include/gumps";

//-----------------------------------------------------------------------------

const BQG_EDITABLE           := 0x00000001;
const BQG_SHOWTITLES         := 0x00000002;
const BQG_SHOWEXTRA          := 0x00000004;
const BQG_RETVAL             := 0x00000008;
const BQG_ZEROFILL           := 0x00000010;
const BQG_ADDABLE            := 0x00000020;
const BQG_NOTIPSCROLL        := 0x00000040;
const BQG_MODIFY_MODE        := 0x00000080;
const BQG_BACK_PAPER         := 0x00000100;

const BQG_MAX_MODLOC         := 20;

//-----------------------------------------------------------------------------

const FCLR_TEBOX             := 1153;

//================================================================================
//--------------------------------------------------------------------------------
// Build Quest Gump Utility
//--------------------------------------------------------------------------------
//================================================================================

function BQGCheckBox(x, y, Flags, Check, Id)

  if(Flags & BQG_EDITABLE)
    if(Check)
      GButton(x, y, 0x869, 0x868, Id);
    else
      GButton(x, y, 0x867, 0x868, Id);
    endif
  else
    if(Check)
      GGumpPic(x, y, 0x869);
    else
      GGumpPic(x, y, 0x867);
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function BQGInit(Title, Width, Height, bId := 0, Multi := 0, PosX := 20, PosY := 20)

  GInit(PosX, PosY);
  if(Multi)
    GMode(GMODE_MULTIHD, GSMODE_PACKET);
  else
    GMode(GMODE_NORMAL, GSMODE_PACKET);
  endif
  GSetUnicode();
  GResPic(0, 0, 3600, Width, Height);
  GTextLine(30, 20, 999, Title);

  if(bId != error)
    GButton(Width-100, 20, 2714, 2715, bId);
    GTextLine(Width-80, 20, 560, "Wroc");
  endif

endfunction

//-----------------------------------------------------------------------------

function BQGAddTextBox(x, y, Width, Height, Color, byref Text, Flags, Lines := 1, Id := 0, Title := 0, Set := 1)

  if(Title and Flags & BQG_SHOWTITLES)
    GTextLine(x-80, y, 670, Title);
  endif

  var ResTile := 0xBB8;

  if(Flags & BQG_BACK_PAPER)
    ResTile := 0x2454;
  endif

  GResPic(x-10, y-3, ResTile, Width+20, (Lines*Height)+6);

  if(Flags & BQG_EDITABLE and Id)
    if(Lines > 1)
      var i := Set-1;
      while(Lines > 0)
        GTextEntry(x, y, Width, Height, Color, Id+i, ClUniStr(Text[i+1]));
        Lines := Lines - 1;
        y     := y + Height;
        i     := i + 1;
      endwhile
    elseif(Lines)
      GTextEntry(x, y, Width, Height, Color, Id, ClUniStr(Text));
    endif
  else
    if(Lines > 1)
      var i := Set-1;
      while(Lines > 0)
        if(Text[i+1])
          GTextCrop(x, y, Width, Height, Color, Text[i+1]);
        endif
        Lines := Lines - 1;
        y     := y + Height;
        i     := i + 1;
      endwhile
    elseif(Lines and Text)
      GTextCrop(x, y, Width, Height, Color, Text);
    endif
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Other Common Funcs
//-----------------------------------------------------------------------------
//=============================================================================

function GetDataKeys(Data, Title, Flags := 0, QsSet := 0, QsEnd := 0)

  var Keys := array;
  var aKey;
  
  foreach Key in (Data.Keys())
    if(Key[Title])
      aKey        := Key;
      aKey[Title] := "";
      if(QsSet and QsEnd > QsSet)
        if(QsSet > CInt(aKey) or QsEnd < CInt(aKey))
          continue;
        endif
      endif

      if(Flags & BQG_ZEROFILL)
        aKey := ZeroFill(aKey, 4);
      endif
      Keys.append(aKey);
    endif
  endforeach

  return Keys;

endfunction

//--------------------------------------------------------------------------------

function BQSelectEntry(who, Entries)

  BQGInit("Ilosc: "+Entries.size(), 400, 300, 0, 1);
  GAddPageData(365,18,2650,2651,365,267,2648,2647);

  GTextLine(30,  40, 999, "Wybierz");
  GTextLine(100, 40, 999, "Id");
  GTextLine(150, 40, 999, "Nazwa");
  GMultiPage();

  var i := 1;

  foreach Entry in Entries
    GCheckNextPage(10);
    GButton(70, 63+GNPCount()*20, 2117, 2118, i);

    if(_Entry_iter != Entry)
      GTextLine(100,60+GNPCount()*20, 550, _Entry_iter);
    endif
    GTextLine(150,60+GNPCount()*20, 550, Entry);
    i := i + 1;
  endforeach

  var Res := CInt(GSend(who).butid);

  if(Res)
    if(TypeOf(Entries) == "Dictionary")
      var Keys := Entries.keys();
      return Keys[Res];
    elseif(TypeOf(Entries) == "Array")
      return CInt(Res);
    endif

  endif

  return error{"errortext" := "Nie wybrano wpisu."};

endfunction

//-----------------------------------------------------------------------------
