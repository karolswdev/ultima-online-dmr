//-----------------------------------------------------------------------------
// Modified: 2005-09-29
//-----------------------------------------------------------------------------

use uo;
use os;
use polsys;

//-----------------------------------------------------------------------------

include "include/unicode";
include "include/checks/stdchecks";

//================================================================================
//--------------------------------------------------------------------------------
// Gump Utility
//--------------------------------------------------------------------------------
//================================================================================

const BKGND_SILVERTRIM  := 0x0053;  // Fancy, dark-grey backing
const BKGND_SCROLL      := 0x09D8;  // Unrolling scroll style
const BKGND_GOLDTRIM    := 0x0A3C;  // Gold-press, black backing
const BKGND_PAPER       := 0x0BB8;  // Very thin edging
const BKGND_PAPER_EX    := 0x0DAC;  //  \_with fancy inner border
const BKGND_BLACKSLAB   := 0x0E10;  // Raised edge (blocky)
const BKGND_STONESLAB   := 0x13BE;  // Plain, small corners
const BKGND_STONE       := 0x13EC;  // Marble stone, rounded edge
const BKGND_STONELIGHT  := 0x242C;
const BKGND_BEIGESTONE  := 0x1400;

//-----------------------------------------------------------------------------

const GMODE_NORMAL      := 1;
const GMODE_MULTIHD     := 2;
const GMODE_MULTIPAGE   := 3;
const GMULTI_PAGEID     := 0x0FFF0000;

//-----------------------------------------------------------------------------

const GSMODE_NORMAL     := 1;
const GSMODE_PACKET     := 2;
const GSMODE_DYNAMIC    := 3;

//-----------------------------------------------------------------------------

var GumpData;

//-----------------------------------------------------------------------------

function GInit(StX := 0, StY := 0)

  GumpData          := array;
  GumpData.+layout  := array;
  GumpData.+data    := array;
  GumpData.+x       := StX;
  GumpData.+y       := StY;
  GumpData.+page    := 0;
  GumpData.+cpage   := -1;
  GumpData.+npage   := 0;
  GumpData.+mode    := GMODE_NORMAL;
  GumpData.+smode   := GSMODE_NORMAL;
  GumpData.+res     := 0;
  GumpData.+Unicode := 0;

  GumpData.+mlayout := array;
  GumpData.+mdata   := array;

endfunction

//-----------------------------------------------------------------------------

function GStartPos(StX := 0, StY := 0)

  GumpData.x     := StX;
  GumpData.y     := StY;

endfunction

//-----------------------------------------------------------------------------

function GMode(Mode, sMode)

  GumpData.smode  := sMode;

  if(Mode != GMODE_NORMAL)
    GMultiHeaderInit();
  else
    GumpData.mode := Mode;
  endif

  if(GumpData.smode == GSMODE_NORMAL)
    GumpData.Unicode := 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function GSetUnicode()

  if(GumpData.smode != GSMODE_NORMAL)
    GumpData.Unicode := 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function GAddToLayout(byref gData)

  if(GumpData.mode == GMODE_NORMAL)
    GumpData.layout.append(gData);
  elseif(GumpData.mode == GMODE_MULTIHD)
    GumpData.multihd[1].append(gData);
  elseif(GumpData.mode == GMODE_MULTIPAGE)
    GumpData.mlayout[GumpData.multipage].append(gData);
  endif

endfunction

//-----------------------------------------------------------------------------

function GAddToData(byref gData)

  case(GumpData.mode)
    GMODE_NORMAL:     GumpData.data.append(gData);
                      CheckDataCorrection(GumpData.data[GumpData.data.size()]);
    GMODE_MULTIHD:    GumpData.multihd[2].append(gData);
                      CheckDataCorrection(GumpData.multihd[2][GumpData.multihd[2].size()]);
    GMODE_MULTIPAGE:  GumpData.mdata[GumpData.multipage].append(gData);
                      CheckDataCorrection(GumpData.mdata[GumpData.multipage][GumpData.mdata[GumpData.multipage].size()]);
  endcase

endfunction

//-----------------------------------------------------------------------------

function CheckDataCorrection(byref gData)

  if(gData == error)
    if(GumpData.Unicode)
      gData := array;
    else
      gData := "";
    endif
  else
    if(GumpData.Unicode)
      ConvToUnicode(gData);
    else
      ConvToAscii(gData);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GGetDataIndex()

  if(GumpData.mode == GMODE_NORMAL)
    return GumpData.data.size();
  elseif(GumpData.mode == GMODE_MULTIHD)
    return GumpData.multihd[2].size();
  elseif(GumpData.mode == GMODE_MULTIPAGE)
    return GumpData.multihd[2].size() + GumpData.mdata[GumpData.multipage].size();
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GNoClose()

  CheckGumpData();
  GAddToLayout("noclose");

endfunction

//-----------------------------------------------------------------------------

function GNoDispose()

  CheckGumpData();
  GAddToLayout("nodispose");

endfunction

//-----------------------------------------------------------------------------

function GNoMove()

  CheckGumpData();
  GAddToLayout("nomove");

endfunction

//-----------------------------------------------------------------------------

function GPage(Page := -1)

  CheckGumpData();
  
  if(Page == -1)
    Page   := CInt(GumpData.page);
  endif
  
  GAddToLayout("page "+CStr(CInt(Page)));
  GumpData.page := CInt(Page) + 1;

endfunction

//-----------------------------------------------------------------------------

function GetNextPage()

  return CInt(GumpData.page);

endfunction

//-----------------------------------------------------------------------------

function GetCurrentPage()

  return (CInt(GumpData.page) - 1);

endfunction

//-----------------------------------------------------------------------------

function GetCurMultiPage()

  if(GumpData.mode == GMODE_MULTIPAGE)
    return GumpData.curmulti;
  endif

endfunction

//-----------------------------------------------------------------------------

function SetCurMultiPage(nId)

  if(GumpData.mode == GMODE_MULTIPAGE)
    if(!GumpData.curmulti)
      GumpData.+curmulti;
    endif

    if(GumpData.mdata.size() < nId)
      GumpData.curmulti := GumpData.mdata.size();
    else
      GumpData.curmulti := nId;
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GetPrevPage()

  return (CInt(GumpData.page) - 2);

endfunction

//-----------------------------------------------------------------------------

function GNPCount()

  return CInt(GumpData.cpage);

endfunction

//-----------------------------------------------------------------------------

function ResetPageCounter()

  GumpData.cpage := -1;
  
endfunction

//-----------------------------------------------------------------------------

function GResPic( StX , StY , GumpPic, EndX, EndY  )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif
  
  GAddToLayout( "resizepic " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(GumpPic)) + " " + CStr(CInt(EndX)) + " " + CStr(CInt(EndY)));
  
endfunction

//-----------------------------------------------------------------------------

function GTextLine( StX, StY, Color, byref Text )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "text " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(Color)) + " " + GGetDataIndex());
  GAddToData(Text);

endfunction

//-----------------------------------------------------------------------------4

function GButton( StX, StY, ButIdx1, ButIdx2, GumpID )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "button " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(ButIdx1)) + " " + CStr(CInt(ButIdx2)) + " 1 0 " + CStr(CInt(GumpID)));

endfunction

//-----------------------------------------------------------------------------

function GButtonnTileArt( StX, StY, ButIdx1, ButIdx2, GumpID , TilePic, Hue, Tx, Ty)

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
    Tx  := CInt(GumpData.x) + CInt(Tx);
    Ty  := CInt(GumpData.y) + CInt(Ty);
  endif
  GAddToLayout( "buttontileart " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(ButIdx1)) + " " + CStr(CInt(ButIdx2)) + " 1 0 " + CStr(CInt(GumpID)) + " " + CStr(TilePic) + " " + CStr(Hue) + " " + CStr(Tx) + " " + CStr(Ty)) ;

endfunction

//-----------------------------------------------------------------------------



function GButtonPage( StX, StY, ButIdx1, ButIdx2, Page )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "button " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(ButIdx1)) + " " + CStr(CInt(ButIdx2)) + " 0 " + CStr(CInt(Page)));

endfunction

//-----------------------------------------------------------------------------

function GTextEntry( StX, StY, EndX, EndY, Color, GumpID, byref Text)

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "textentry " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(EndX)) + " " + CStr(CInt(EndY)) + " " + CStr(CInt(Color)) + " " + CStr(CInt(GumpID)) + " " + GGetDataIndex());
  GAddToData(Text);

  if(GumpData.mode == GMODE_MULTIHD)
    GumpData.mhdentry[GumpId] := GumpData.multihd[2].size();
  elseif(GumpData.mode == GMODE_MULTIPAGE)
    GumpData.mpentry[GumpData.multipage][GumpId] := GumpData.mdata[GumpData.multipage].size();
  endif

endfunction

//-----------------------------------------------------------------------------

function GTextCrop( StX, StY, EndX, EndY, Color, byref Text )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "croppedtext " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(EndX)) + " " + CStr(CInt(EndY)) + " " + CStr(CInt(Color)) + " " + GGetDataIndex());
  GAddToData(Text);

endfunction

//-----------------------------------------------------------------------------

function GGumpPic( StX, StY, GumpIdx, Color := 0 )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "gumppic " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(GumpIdx)) + " hue=" + CStr(CInt(Color)));

endfunction

//-----------------------------------------------------------------------------

function GTilePic( StX, StY, TileIdx )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "tilepic " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(TileIdx)));

endfunction

//-----------------------------------------------------------------------------

function GTilePicHue( StX, StY, TileIdx, Color := 0)

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout( "tilepichue " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(TileIdx)) + " " + CStr(CInt(Color)));

endfunction

//-----------------------------------------------------------------------------

function GAddPageData(StX ,StY, PBut1Idx1, PBut1Idx2, EndX, EndY, PBut2Idx1, PBut2Idx2);

  GumpData.npage := { StX ,StY, PBut1Idx1, PBut1Idx2, EndX, EndY, PBut2Idx1, PBut2Idx2 };

endfunction

//-----------------------------------------------------------------------------

// StPage == GetCurrentPage();
// FirstPage must exist before it, (0);

function GCheckNextPage(PageCount, StPage := 0 )

  if(GumpData.mode == GMODE_NORMAL)

    if(GumpData.cpage == -1)
      GPage();
      GumpData.cpage := 0;
      return 1;
    endif

    if(GumpData.cpage >= (PageCount - 1))
      if(GumpData.npage)
        GButtonPage( GumpData.npage[5], GumpData.npage[6], GumpData.npage[7], GumpData.npage[8], GetNextPage()  );
      endif
      GumpData.cpage := 0;
      GPage();
      if(GumpData.npage)
        if((GetNextPage() - 2) > StPage)
          GButtonPage( GumpData.npage[1], GumpData.npage[2], GumpData.npage[3], GumpData.npage[4], (GetNextPage() - 2)  );
        endif
      endif
      return 1;
    else
      GumpData.cpage := GumpData.cpage + 1;
    endif

  else

    if(GumpData.cpage >= (PageCount - 1))
      if(GumpData.npage)
        GMultiButtonPage( GumpData.npage[5], GumpData.npage[6], GumpData.npage[7], GumpData.npage[8], GMultiCurPage() + 1);
      endif
      GumpData.cpage := 0;
      GMultiPage();
      if(GumpData.npage)
        if((GMultiCurPage() - 1) > StPage)
          GMultiButtonPage( GumpData.npage[1], GumpData.npage[2], GumpData.npage[3], GumpData.npage[4], (GMultiCurPage() - 1)  );
        endif
      endif
      return 1;
    else
      GumpData.cpage := GumpData.cpage + 1;
    endif

  endif

  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function GSetRadioGroup( group )

  CheckGumpData();
  GAddToLayout( "group " + CStr(CInt(group)) );
  
endfunction

//-----------------------------------------------------------------------------

function GRadioButton( StX, StY, RadIdx1, RadIdx2, GumpID, State := 0)

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  if(State)
    State := 1;
  else
    State := 0;
  endif
  
  GAddToLayout("radio " + CStr(CInt(StX)) + " " + CStr(CInt(StY)) + " " + CStr(CInt(RadIdx1)) + " " + CStr(CInt(RadIdx2)) + " " + State + " " + CStr(CInt(GumpID)));

endfunction

//-----------------------------------------------------------------------------

function GCheckBox( StX, StY, ButIdxOff, ButIdxOn, GumpID, State := 0)

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  if(State)
    State := 1;
  else
    State := 0;
  endif
  
  GAddToLayout("checkbox " + CStr(CInt(StX)) + " " + CStr(CInt(StY)) + " " + CStr(CInt(ButIdxOff)) + " " + CStr(CInt(ButIdxOn)) + " " + State + " " + CStr(CInt(GumpID)));

endfunction

//-----------------------------------------------------------------------------

function GCheckerTrans( StX, StY, EndX, EndY )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout("checkertrans " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(EndX)) + " " + CStr(CInt(EndY)));

endfunction

//-----------------------------------------------------------------------------

function GGumppicTiled(StX, StY, EndX, EndY, GumpID)

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout("gumppictiled " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(EndX)) + " " + CStr(CInt(EndY)) + " " + CStr(CInt(GumpID)));

endfunction

//-----------------------------------------------------------------------------

function GHtmlGumpColor(StX, StY, EndX, EndY, MsgID, bg := 0, scroll := 0, color := 16777215)

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout("xmfhtmlgumpcolor " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(EndX)) + " " + CStr(CInt(EndY)) + " " + CStr(CInt(MsgID)) + " " + CStr(CInt(bg)) + " " + CStr(CInt(scroll)) + " " + CStr(CInt(color)));

endfunction

//-----------------------------------------------------------------------------

function GHtmlGump( StX, StY, EndX, EndY, byref Text, BackID, ScrollBar )

  CheckGumpData();

  if(GumpData.smode == GSMODE_NORMAL)
    StX := CInt(GumpData.x) + CInt(StX);
    StY := CInt(GumpData.y) + CInt(StY);
  endif

  GAddToLayout("htmlgump " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(EndX)) + " " + CStr(CInt(EndY)) + " " + GGetDataIndex() + " " + CStr(CInt(BackID)) + " " + CStr(ScrollBar));
  GAddToData(Text);

endfunction

//-----------------------------------------------------------------------------

function CheckGumpData()

  if(!GumpData.layout or !GumpData.data)
    GInit();
  endif

  if(GumpData.mode == GMODE_MULTIPAGE)
    if(!GumpData.mlayout[GumpData.multipage])
      GumpData.mlayout[GumpData.multipage] := array;
    endif
    if(!GumpData.mdata[GumpData.multipage])
      GumpData.mdata[GumpData.multipage] := array;
    endif
  endif

  sleepms(1);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Sending
//-----------------------------------------------------------------------------
//=============================================================================

function GSend(who)

  GBuildData();

  if(GumpData.smode == GSMODE_DYNAMIC)
    return GPacketSend(who);
  endif

  if(GumpData.mode != GMODE_NORMAL or GumpData.smode == GSMODE_PACKET)

    if(GumpData.smode == GSMODE_PACKET)
      if(!GPacketSend(who))
        return error;
      endif
    endif

    while(IsLoggedIn(who))

      if(GumpData.smode == GSMODE_NORMAL)
        GumpData.res := SendDialogGump(who, GumpData.layout, GumpData.data);
        if(GumpData.res == error)
          return GumpData.res;
        endif

        if(!GCheckResult())
          break;
        endif

      elseif(GumpData.smode == GSMODE_PACKET)

        if(GWaitForResponse(who))
          break;
        endif

      else
        return 0;
      endif

    endwhile
  else
    GumpData.res := SendDialogGump(who, GumpData.layout, GumpData.data);
  endif

  return GumpData.res;

endfunction

//-----------------------------------------------------------------------------

function GCheckResult()

  if(GumpData.mode != GMODE_NORMAL)
    var Id;

    if(GumpData.smode == GSMODE_NORMAL)
      Id := CInt(GumpData.res[0]);
    else
      Id := GumpData.res.butid;
    endif

    GSaveResData();

    if(Id > GMULTI_PAGEID)
      GumpData.curmulti := Id - GMULTI_PAGEID;
      if(GumpData.curmulti > GumpData.mdata.size())
        GumpData.curmulti := GumpData.mdata.size();
      endif

      GBuildData();
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GSaveResData()

  if(GumpData.mode != GMODE_NORMAL)
    foreach Id in (GumpData.mhdentry)
      GumpData.multihd[2][Id] := CroppedId(GumpData.res[_Id_iter]);
    endforeach

    foreach Id in (GumpData.mpentry[GumpData.curmulti])
      GumpData.mdata[GumpData.curmulti][Id] := CroppedId(GumpData.res[_Id_iter]);
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function GGetEntryData(Id)

  if(GumpData.mode == GMODE_NORMAL)
    return CroppedId(GumpData.res[Id]);
  else
    if(GumpData.mhdentry.exists(Id))
      return GumpData.multihd[2][ GumpData.mhdentry[Id] ];
    endif

    var p := 1;
    
    foreach EntryPage in (GumpData.mpentry)
      if(EntryPage.exists(Id))
        return GumpData.mdata[p][ EntryPage[Id] ];
      endif
      p := p + 1;
    endforeach
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GBuildData()

  if(GumpData.mode == GMODE_MULTIPAGE)
    if(!GumpData.curmulti)
      GumpData.+curmulti;
      GumpData.curmulti := 1;
    endif

    GumpData.layout := GumpData.multihd[1] + GumpData.mlayout[GumpData.curmulti];
    GumpData.data   := GumpData.multihd[2] + GumpData.mdata[GumpData.curmulti];
  endif

endfunction

//-----------------------------------------------------------------------------

function GPacketSend(who)

  var gId := GetPid();
  var Pc  := CreatePacket(0xb0, MSGLEN_VARIABLE);

  Pc.SetInt32(3,who.serial);
  Pc.SetInt32(7,gId);
  Pc.SetInt32(11,GumpData.x);
  Pc.SetInt32(15,GumpData.y);

  var n       := 21;
  var Layout  := "";
  var DataArr := array;
  var i;

  foreach LayData in (GumpData.layout)
    Layout := Layout+"{ "+LayData+" }";
  endforeach

  Pc.SetString(n, Layout, 1);
  n := n + len(Layout)+1;

  Pc.SetInt16(19,n - 21);
  Pc.SetInt16(n,GumpData.data.size());
  n := n + 2;

  foreach DataElem in (GumpData.data)
    if(GumpData.Unicode)
      Pc.SetInt16(n, DataElem.size());
      n := n + 2;
      for(i:=1;i<=DataElem.size();i:=i+1)
        Pc.SetInt16(n, DataElem[i]);
        n := n + 2;
        sleepms(2);
      endfor
    else
      DataArr := CAscZ(DataElem);
      Pc.SetInt16(n, DataArr.size());
      n := n + 2;
      for(i:=1;i<=DataArr.size();i:=i+1)
        Pc.SetInt16(n, DataArr[i]);
        n := n + 2;
        sleepms(2);
      endfor
    endif
  endforeach

  SetObjProperty(who, "#CsG_"+gId, ReadGameClock() );
  return Pc.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------

function GGetResult()

  return GumpData.res;

endfunction

//-----------------------------------------------------------------------------

function GWaitForResponse(who)

  GumpData.res := wait_for_event(1);

  if(!IsLoggedIn(who))
    return 0;
  elseif(GumpData.res)
    if(!GCheckResult())
      return 1;
    else
      GPacketSend(who);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CloseGumpEx(who, Id, Res := 0)

  if(!CInt(Id))
    return 0;
  endif

  var Packet := CreatePacket(0xBF, MSGLEN_VARIABLE);

  Packet.SetInt16(3, 0x0004);
  Packet.SetInt32(5, CInt(Id));
  Packet.SetInt32(9, CInt(Res));

  var cRes := Packet.SendPacket(who);
  
  if(GetObjProperty(who, "#CsG_"+Id))
    EraseObjProperty(who, "#CsG_"+Id);

    var Proc := GetProcess(Id);

    if(Proc)
      var gEv       := struct;
      gEv.+butid    := Res;
      gEv.+switches := array;
      gEv.+text     := dictionary;
      gEv.+unitext  := dictionary;
      Proc.sendevent(gEv);
    endif
  endif

  return cRes;

endfunction

//-----------------------------------------------------------------------------

function FullCloseGump(who, Id)

  if(!CloseGumpEx(who, Id))
    return 0;
  endif

  sleepms(20);
  clear_event_queue();

endfunction

//-----------------------------------------------------------------------------
// GumpTextEntry Crop Id
//-----------------------------------------------------------------------------

function CroppedId(Str)

  if(Str["<uninit"])
    return "";
  endif

  if(Str[2] == ":")
    Str := Str[4,len(Str)];
  elseif(Str[3] == ":")
    Str := Str[5,len(Str)];
  elseif(Str[4] == ":")
    Str := Str[6,len(Str)];
  endif
  
  if(Str)
    return Str;
  else
    return "";
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Multi Gump Utility
//-----------------------------------------------------------------------------
//=============================================================================

function GMultiHeaderInit()

  CheckGumpData();
  GumpData.mode := GMODE_MULTIHD;

  if(!GumpData.multihd)
    GumpData.+multihd;
  endif

  if(!GumpData.mhdentry)
    GumpData.+mhdentry;
  endif

  if(!GumpData.mpentry)
    GumpData.+mpentry;
  endif

  GumpData.multihd  := array(array, array);
  GumpData.mhdentry := dictionary;
  GumpData.mpentry  := array;

endfunction

//-----------------------------------------------------------------------------

function GMultiPage()

  GumpData.mode := GMODE_MULTIPAGE;

  if(!GumpData.multipage)
    GumpData.+multipage;
    GumpData.multipage := 1;
  else
    GumpData.multipage := CInt(GumpData.multipage) + 1;
  endif

  GumpData.mlayout[GumpData.multipage] := array;
  GumpData.mdata[GumpData.multipage]   := array;
  GumpData.mpentry[GumpData.multipage] := dictionary;

endfunction

//-----------------------------------------------------------------------------

function GMultiButtonPage( StX, StY, ButIdx1, ButIdx2, pID )

  if(GumpData.mode == GMODE_MULTIPAGE)
    if(pID >= 1 and pID <= GumpData.multipage + 1)
      CheckGumpData();

      if(GumpData.smode == GSMODE_NORMAL)
        StX := CInt(GumpData.x) + CInt(StX);
        StY := CInt(GumpData.y) + CInt(StY);
      endif

      GAddToLayout( "button " + CStr(StX) + " " + CStr(StY) + " " + CStr(CInt(ButIdx1)) + " " + CStr(CInt(ButIdx2)) + " 1 0 " + CStr(CInt(GMULTI_PAGEID+pID)));
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GMultiCurPage()

  return GumpData.multipage;

endfunction

//-----------------------------------------------------------------------------
