//--------------------------------------------------------------------------------

include "include/gumps";

//================================================================================
//--------------------------------------------------------------------------------
// Yes No
//--------------------------------------------------------------------------------
//================================================================================

function YesNo(me, prompt, StX := 10, StY := 10)

  GInit();
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GStartPos(StX, StY);
  GNoDispose();
  GGumpPic( 0, 0, 2070 );
  GButton( 30, 75, 2128, 2129, 1 );
  GButton( 90, 75, 2119, 2120, 0 );
  GTextLine( 40, 30, 52, prompt);

  return GSend( me ).butid;

endfunction

//--------------------------------------------------------------------------------

function YesNoEx(me, prompt, StX := 150, StY := 300, Width := 550)

  var Lines;

  if(IsVarType(prompt, VAR_ARRAY))
    Lines := array;
    var curIdx := 1;
    var cBreak := CAsc("|");
    foreach Char in prompt
      if(!Lines[curIdx])
        Lines[curIdx] := array;
      endif

      if(Char == cBreak)
        curIdx += 1;
        continue;
      endif
      
      Lines[curIdx].append(Char);
    endforeach
  else
    Lines := SplitWords(prompt, "|");
  endif

  var Height := 80+Lines.size()*20;
  var i;

  GInit();
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GStartPos(StX, StY);
  GSetUnicode();
  GNoDispose();
  GResPic(0, 0, 0xA28, Width, Height);
  GButton( (Width/2)-60, Height-45, 2128, 2129, 1 );
  GButton( (Width/2), Height-45, 2119, 2120, 0 );
  
  for(i:=0; i<Lines.size(); i+=1)
    GTextLine( 40, 25+i*20, 1152, ToUnicode(Lines[i+1]));
  endfor

  return GSend( me ).butid;

endfunction

//--------------------------------------------------------------------------------

function ShowInfoEx(me, info, StX := 150, StY := 300, Width := 450)

  var Lines := SplitWords(info, "|");
  var Height := 80+Lines.size()*20;
  var i;

  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GInit(StX, StY);
  GSetUnicode();
  GNoDispose();
  GResPic(0, 0, 0xA28, Width, Height);
  GButton( (Width/2)-30, Height-45, 2128, 2129, 1 );

  for(i:=0; i<Lines.size(); i+=1)
    GTextLine( 40, 25+i*20, 1152, ToUnicode(Lines[i+1]));
  endfor

  return GSend( me ).butid;

endfunction

//-----------------------------------------------------------------------------

function SelectItemGump(who, byref Items)

  var i := 0;
  GInit(100,100);
  GResPic(0,0,3600,300,100);

  foreach Item in Items
    GButton(30,30+(i*20),2714,2715,Item);
    GTextLine(50,30+(i*20),560,GetDescForItem(Item));
    i := i + 1;
  endforeach

  return GSend(who)[0];

endfunction

//-----------------------------------------------------------------------------

function SelectListGump(who, byref ListDict, pTitle := "")

  var i := 0;
  var lWidth := 250;
  var lHeight := min(60+ListDict.size()*20, 360);

  GInit(100,100);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GSetUnicode();

  GResPic(0,0,3600,lWidth,lHeight);
  GAddPageData(lWidth-35,18,2650,2651,lWidth-35,lHeight-35,2648,2647);

  GTextLine(45,20,999, pTitle);
  GPage();

  var Keys := ListDict.keys();

  foreach Item in ListDict
    GCheckNextPage(15);
    GButton(20,40+(GNPCount()*20),2714,2715,i+1);
    GTextLine(45,40+(GNPCount()*20),560, Item);
    i := i + 1;
  endforeach

  var Idx := GSend(who).butid;

  if(!Idx)
    return error;
  endif

  return Keys[Idx];

endfunction

//-----------------------------------------------------------------------------

function AddMultiHeader(byref Header, pTitle, pWidth, pColor := 560)
  
  if(!Header) 
    Header := array;
  endif

  var Data := struct;
  Data.+width := pWidth;
  Data.+title := pTitle;
  Data.+color := pColor;

  Header.append(Data);

endfunction

//-----------------------------------------------------------------------------

function SelectMultiListGump(who, Headers, byref ListDict)

  var i := 0;
  GInit(100,100);
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GSetUnicode();

  var lWidth := 65;
  
  foreach Item in Headers
    lWidth += Item.width;
  endforeach

  GResPic(0,0,3600,lWidth,360);
  GAddPageData(lWidth-35,18,2650,2651,lWidth-35,325,2648,2647);

  var lPos := 0;

  foreach Item in Headers
    GTextLine(45+lPos, 20, 999, Item.title);
    lPos += Item.width;
  endforeach

  GPage();

  var Keys := ListDict.keys();
  var lIdx := 1;

  foreach Item in ListDict
    GCheckNextPage(15);
    GButton(20,40+(GNPCount()*20),2714,2715,lIdx);

    lPos := 0;

    for(i:=1; i<=Headers.size(); i+=1)
      GTextLine(45+lPos,40+(GNPCount()*20), Headers[i].color, Item[i]);
      lPos += Headers[i].width;
    endfor
    lIdx += 1;
  endforeach

  var Idx := GSend(who).butid;

  if(!Idx)
    return error;
  endif

  return Keys[Idx];

endfunction

//--------------------------------------------------------------------------------

