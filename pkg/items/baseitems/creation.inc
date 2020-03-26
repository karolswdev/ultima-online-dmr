//-----------------------------------------------------------------------------
// Modified: 2005-07-05
//-----------------------------------------------------------------------------

include "../pkg/items/statics/statics";
include "../pkg/items/baseitems/baseitems";

//-----------------------------------------------------------------------------

const CRI_NONE               := 0x00000000;
const CRI_TO_BIM             := 0x00000001;
const CRI_TO_STC             := 0x00000002;
const CRI_LOCKED             := 0x00000004;

//-----------------------------------------------------------------------------

function CreateMarkedItem(ObjType, x, y, z, Rlm, Flags, who, Color := 0, Name := 0)

  var nItem := 0;

  if(Flags & CRI_TO_BIM)
    var cFlags := 0;
    if(Flags & CRI_LOCKED)
      SetBitOn(cFlags, BIF_LOCKED);
    endif

    var Nr := AddBaseItem(ObjType, x, y, z, Rlm, Color, Name, cFlags);
    if(Nr)
      SendSysMessage(who,"Dodano Bi, Nr ["+Nr+"] O: ["+ObjType+"] ["+x+","+y+","+z+" "+Rlm+"]",FONT_NORMAL,COLOR_GREEN);
    else
      SendSysMessage(who,"Nie udalo sie stworzyc BI, ["+Nr+"].",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

    nItem := CreateBaseItem(Nr);
    if(!nItem)
      SendSysMessage(who, nItem.errortext, FONT_NORMAL, COLOR_RED);
      DeleteBaseItem(Nr);
      return 0;
    endif
  else
    nItem := CreateItemAtLoc(x, y, z, ObjType, 1, Rlm);
    if(!nItem)
      SendSysMessage(who, "Blad O: ["+ObjType+"] ["+x+","+y+","+z+"]: "+nItem,FONT_NORMAL,COLOR_RED);
      return 0;
    endif

    nItem.color := Color;
    if(Name)
      SetName(nItem, Name);
    endif

    if(Flags & CRI_TO_STC)
      SetItemStc(nItem, who);
      SendSysMessage(who, "Dodano Stc O: ["+ObjType+"] ["+x+","+y+","+z+"]",FONT_NORMAL,COLOR_GREEN);
    else
      SendSysMessage(who, "Stworzono Item O: ["+ObjType+"] ["+x+","+y+","+z+"]",FONT_NORMAL,COLOR_GREEN);
    endif
  endif

  return nItem;

endfunction

//-----------------------------------------------------------------------------

function CreateComplexItems(who, text, Flags)
  
  var Cfg   := ReadConfigFile(CPX_ITEMS_CFG);
  var Elem  := FindConfigElem(Cfg, text[2]);

  if(!Elem)
    var Msg := "";
    foreach CpxName in GetConfigStringKeys(Cfg)
      if(!Msg)
        Msg := CpxName;
      else
        Msg := Msg+"/"+CpxName;
      endif
    endforeach
    SendSysMessage(who,"cx ["+Msg+"]",FONT_NORMAL,COLOR_RED);
  else

    var x   := who.x;
    var y   := who.y;
    var z   := who.z;
    var Rlm := who.realm;

    foreach Item in GetConfigStringArray(Elem, "Item")
      Item := SplitWords(Item);

      foreach ItmProp in Item
        ItmProp := CInt(ItmProp);
      endforeach

      if(!CreateMarkedItem(Item[1], x+Item[2], y+Item[3], z+Item[4], Rlm, Flags, who, Item[5]))
        break;
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function MakeItemsFromSet(who, text, Flags)

  var SetCfg := ReadConfigFile(SET_ITEMS_CFG);

  case(text[2])
    "dr":         MakeBaseDoors(who,text, Flags);
                  return;
    "sign":       MakeBaseSign(who,text, Flags);
                  return;
    "str":        MakeBaseStairs(who,text, Flags);
                  return;
    "chr":        MakeBaseChairs(who,text, Flags);
                  return;
    "tab":        MakeBaseTable(who,text, Flags);
                  return;
    "carp":       MakeBaseCarpet(who, text, Flags);
                  return;
    "deed":       MakeBaseDeed(who, text, Flags);
                  return;
     "?":         SendSysMessage(who,"set [dr sign str chr tab carp deed "+GetConfigString(SetCfg["Cmds"],"AllCmds")+"]",FONT_NORMAL,COLOR_RED);
                  return;
  endcase
  
  var Split;

  if(text[2])
    var cName := GetConfigString(SetCfg["Cmds"],text[2]);
    if(cName)
      Split := SplitWords(cName);
      MakeBaseItems(who,text,GetCfgObjTypes(Split[1]),CInt(Split[2]), Flags);
    else
      SendSysMessage(who, "Bledny set ["+text[2]+"]", FONT_NORMAL, COLOR_RED);
    endif
  else
    SendSysMessage(who,"Zla opcja set [set ?]", FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function MakeBaseDoors(who, text, Flags)

  var SetCfg := ReadConfigFile(SET_ITEMS_CFG);
  var Doors       := GetConfigStringArray(SetCfg["Doors"],"Doors");
  var i           := 1;
  var DoorsType   := Doors[CInt(text[3])];
  var Direction   := 0;
  var Orient      := 0;
  var Cut         := 0;
  var Res;

  text[4] := lower(text[4]);
  text[5] := lower(text[5]);
  
  if(CStr(text[4]) == "n" or CStr(text[4]) == "w")
    Direction := CStr(text[4]);
  endif
  
  case(Direction)
    "n": if(CStr(text[5]) == "w" or CStr(text[5]) == "e")
           Orient := CStr(text[5]);
         endif
    "w": if(CStr(text[5]) == "n" or CStr(text[5]) == "s")
           Orient := CStr(text[5]);
         endif
  endcase
  
  if(Direction)
    Direction := Upper(Direction);
  endif
  if(Orient)
    Orient    := Upper(Orient);
  endif
  
  if(CInt(text[6]) > 0 and Cint(text[6]) <= 2)
    Cut := CInt(Text[6]);
  endif
  
  if(!DoorsType)

    GInit(30,30);
    GResPic(0,0,3600,350,650);
    GPage();
    GAddPageData(317,18,2650,2651,317,615,2648,2647);

    foreach Door in Doors
      GCheckNextPage(6);
      GButton(30,(83+(GNPCount()*100)),2117,2118,i);
      GTextLine(60,(80+(GNPCount()*100)),560,i + ". " + Door);
      GTilePic(250,(30+(GNPCount()*100)),CInt(SetCfg[Door].Obj_NW1));
      i := i + 1;
    endforeach
  
    Res := GSend(who)[0];
  
    if(!Res)
      return;
    endif

    DoorsType := Doors[Res];

  endif
  
  if(!Direction)
    Direction := BuildDirGump(who,CInt(SetCfg[DoorsType].Obj_NW1),CInt(SetCfg[DoorsType].Obj_WN1));
  endif

  if(!Orient)

    GInit( 30 , 30 );
    GPage();

    GResPic(0, 0, 3600, 220, 250);
    GButton( 140, 210, 2121, 2120, 3 );

    GTextLine(80,30,540,"Strona");

    case(Direction)
      "N": GButton(40,    60, 2714, 2715, 1);
           GTextLine(60,  60, 640, "W");
           GTilePic(30,   90, CInt(SetCfg[DoorsType].Obj_NW1)+1);

           GButton(140,   60, 2714, 2715, 2);
           GTextLine(160, 60, 640, "E");
           GTilePic(140,  90, CInt(SetCfg[DoorsType].Obj_NE1)+1);

      "W": GButton(40,    60, 2714, 2715, 3);
           GTextLine(60,  60, 640, "N");
           GTilePic(30,   90, CInt(SetCfg[DoorsType].Obj_WN1)+1);

           GButton(140,   60, 2714, 2715, 4);
           GTextLine(160, 60, 640, "S");
           GTilePic(140,  90, CInt(SetCfg[DoorsType].Obj_WS1)+1);

    endcase
    
    Res  := GSend(who)[0];

    case(Res)
      1: Orient := "W";
      2: Orient := "E";
      3: Orient := "N";
      4: Orient := "S";
      default: SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
               return;
    endcase
  endif

  if(!DoorsType or !Direction or !Orient)
    SendSysMessage(who,"Type [1-"+len(Doors)+"] , Direct [N O: [W/E] / W O: [N/S]] Other [1/2] 1 - First Door, 2 - Sec Door, None - Both",FONT_NORMAL,COLOR_RED);
    SendSysMessage(who,"set dr 1 N W 2",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  var Targ  := who;
  
  text := CheckArray(text);
  
  if("trg" in text)
    Targ := LocTarget(who);
  endif

  if("lock" in text)
    SetBitOn(Flags, CRI_LOCKED);
  endif

  if(!Targ)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  var Obj1 := GetConfigInt(SetCfg[DoorsType],"Obj_"+Direction+Orient+"1");
  var Obj2 := GetConfigInt(SetCfg[DoorsType],"Obj_"+Direction+Orient+"2");
  var Door1,Door2;
  
  if(Obj1 and Obj2)
    case(Cut)
      1:  Door1 := CreateMarkedItem(Obj1, Targ.x, Targ.y, Targ.z, who.realm, Flags, who);
          if(!Door1)
            SendSysMessage(who,"Nie udalo sie dodac Drzwi "+Door1, FONT_NORMAL, COLOR_RED);
            return;
          endif

      2:  Door2 := CreateMarkedItem(Obj2, Targ.x, Targ.y, Targ.z, who.realm, Flags, who);
          if(!Door2)
            SendSysMessage(who,"Nie udalo sie dodac Drzwi "+Door2, FONT_NORMAL, COLOR_RED);
            return;
          endif

      0:  Door1 := CreateMarkedItem(Obj1, Targ.x, Targ.y, Targ.z, who.realm, Flags, who);
          if(!Door1)
            SendSysMessage(who,"Nie udalo sie dodac Drzwi "+Door1, FONT_NORMAL, COLOR_RED);
            return;
          endif

          var ModX := 0;
          var ModY := 0;

          case(Direction)
            "N": ModY := 1;
            "W": ModX := 1;
          endcase

          Door2 := CreateMarkedItem(Obj2, Targ.x+ModX, Targ.y+ModY, Targ.z, who.realm, Flags, who);
          if(!Door2)
            SendSysMessage(who,"Nie udalo sie dodac Drzwi "+Door2, FONT_NORMAL, COLOR_RED);
            return;
          endif
    endcase
  else
    SendSysMessage(who,"Bledne wpisy w configu ["+SET_ITEMS_CFG+"] DT: "+DoorsType+" ,Obj1 "+Obj1+" Obj2 "+Obj2,FONT_NORMAL,COLOR_RED);
  endif
  
endfunction

//-----------------------------------------------------------------------------

function MakeBaseSign(who,text, Flags)

  var ObjType  := CInt(text[3]);
  var Dir      := 0;
  var ObjTypes := GetSignsObjTypes();

  if(!ObjType)
    ObjType := BuildObjTypeGump(who,ObjTypes);
  elseif(len(ObjTypes) >= ObjType)
    ObjType := ObjTypes[ObjType];
  else
    ObjType := 0;
  endif
  
  if(!ObjType)
    SendSysMessage(who,"set sign [1-"+len(ObjTypes)+"] [N/W] [nam] [trg]",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(ObjType < UOBJ_DIR_SIGN_START or ObjType > UOBJ_DIR_SIGN_END)
    text[4] := lower(text[4]);
  
    if(CStr(text[4]) == "n" or CStr(text[4]) == "w")
      Dir := CStr(text[4]);
    else
      Dir := BuildDirGump(who,ObjType,ObjType - 1);
    endif
    Dir := Upper(Dir);
    
    if(Dir == "W")
      ObjType := ObjType - 1;
    endif
    
  endif

  var Name := 0;

  text := CheckArray(text);

  if("nam" in text)
    Name := SendTextEntryGump(who,"Wpisz nazwe",TE_CANCEL_ENABLE);
    if(!Name)
      SendSysMessage(who,"Anuloawno.",FONT_NORMAL,COLOR_RED);
      return;
    endif
  endif
  
  var Targ := who;
  
  if("trg" in text)
    Targ := LocTarget(who);
  endif
  
  if(!Targ)
    SendSysMessage(who,"Anuloawno.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  CreateMarkedItem(ObjType, Targ.x, Targ.y, Targ.z, who.realm, Flags, who, 0, Name);

endfunction

//-----------------------------------------------------------------------------

function BuildObjTypeGump(who,ObjTypes,PageCount := 10)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  GInit(30,30);
  GResPic(0,0,3600,850,450);
  GAddPageData(817,18,2650,2651,817,415,2648,2647);
  GPage();

  var w := 3,i := 1;

  for(i:=1;i<=len(ObjTypes);i:=i+1)
    if(w >= 3)
      w := 0;
      GCheckNextPage(PageCount);
    endif
    GButton(  (24+(w*250)),(27+(GNPCount()*((12/PageCount)*35))),2714,2715,ObjTypes[i]);
    GTilePic( (39+(w*250)),(27+(GNPCount()*((12/PageCount)*35))),ObjTypes[i]);
    GTextLine((84+(w*250)),(27+(GNPCount()*((12/PageCount)*35))),550,i+". "+ClStr(ItemCfg[ObjTypes[i]].desc));
    w := w + 1;
  endfor
  
  return GSend(who)[0];

endfunction

//-----------------------------------------------------------------------------

function BuildDirGump(who,Obj1 := 0,Obj2 := 0,Obj3 := 0,Obj4 := 0)

    GInit( 30 , 30 );
    GPage();

    var Width := 220;
    if(Obj3)
      Width := 320;
    endif
    if(Obj4)
      Width := 420;
    endif
    
    GResPic(0, 0, 3600, Width, 250);
    GButton( Width-80, 210, 2121, 2120, 30 );

    GTextLine(Width / 3,30,540,"Kierunek");

    GButton(40,    60, 2714, 2715, 1);
    GTextLine(60,  60, 640, "N");
    if(Obj1)
      GTilePic(30,   90, Obj1);
    endif
    
    GButton(140,   60, 2714, 2715, 2);
    GTextLine(160, 60, 640, "W");
    if(Obj2)
      GTilePic(140,  90, Obj2);
    endif

    if(Obj3)
      GButton(240,   60, 2714, 2715, 3);
      GTextLine(260, 60, 640, "S");
      GTilePic(240,  90, Obj3);
    endif

    if(Obj4)
      GButton(340,   60, 2714, 2715, 4);
      GTextLine(360, 60, 640, "E");
      GTilePic(340,  90, Obj4);
    endif

    var Res  := GSend(who)[0];

    case(Res)
      1: return "N";
      2: return "W";
      3: return "S";
      4: return "E";
      default: SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
               exit;
    endcase
    
endfunction

//-----------------------------------------------------------------------------
// Stairs
//-----------------------------------------------------------------------------

function MakeBaseStairs(who,text, Flags)

  var SetCfg      := ReadConfigFile(SET_ITEMS_CFG);
  var Stairs      := GetConfigStringArray(SetCfg["Stairs"],"Stairs");
  var i           := 1;
  var StairsType  := Stairs[CInt(text[3])];
  var Direction   := 0;
  var Res;

  text[4] := lower(text[4]);

  if(CStr(text[4]) == "n" or CStr(text[4]) == "s" or CStr(text[4]) == "e" or CStr(text[4]) == "w" or CStr(text[4]) == "m")
    Direction := CStr(text[4]);
  endif

  if(Direction)
    Direction := Upper(Direction);
  endif

  if(!StairsType)

    GInit(30,30);
    GResPic(0,0,3600,350,650);
    GPage();
    GAddPageData(317,18,2650,2651,317,615,2648,2647);

    foreach Stair in Stairs
      GCheckNextPage(10);
      GButton(30,(33+(GNPCount()*60)),2117,2118,i);
      GTextLine(60,(30+(GNPCount()*60)),560,i + ". " + Stair);
      GTilePic(260,(30+(GNPCount()*60)),CInt(SetCfg[Stair].ObjN));
      i := i + 1;
    endforeach

    Res := GSend(who)[0];

    if(!Res)
      return;
    endif

    StairsType := Stairs[Res];

  endif

  if(!Direction)

    Direction := BuildDirGump(who,CInt(SetCfg[StairsType].ObjN),CInt(SetCfg[StairsType].ObjW),CInt(SetCfg[StairsType].ObjS),CInt(SetCfg[StairsType].ObjE));

  endif

  if(!StairsType or !Direction)
    SendSysMessage(who,"Type [1-"+len(Stairs)+"] , Direct [N O: [N/S/W/E/M] trg",FONT_NORMAL,COLOR_RED);
    SendSysMessage(who,"set str N trg",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Targ := who;

  text := CheckArray(text);

  if("trg" in text)
    Targ := LocTarget(who);
  endif

  if(!Targ)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Obj := GetConfigInt(SetCfg[StairsType],"Obj"+Direction);

  if(Obj)
    CreateMarkedItem(Obj, Targ.x, Targ.y, Targ.z, who.realm, Flags, who);
  else
    SendSysMessage(who,"Bledne wpisy w configu ["+SET_ITEMS_CFG+"] ST: "+StairsType+" ,Obj "+Obj,FONT_NORMAL,COLOR_RED);
  endif
  
endfunction

//-----------------------------------------------------------------------------
//  Chairs
//-----------------------------------------------------------------------------

function MakeBaseChairs(who,text, Flags)

  var SetCfg      := ReadConfigFile(SET_ITEMS_CFG);
  var Chairs      := GetConfigStringArray(SetCfg["Chairs"],"Chairs");
  var i           := 1;
  var ChairsType  := Chairs[CInt(text[3])];
  var Direction   := 0;
  var Res;

  text[4] := lower(text[4]);

  if(CStr(text[4]) == "n" or CStr(text[4]) == "s" or CStr(text[4]) == "e" or CStr(text[4]) == "w")
    Direction := CStr(text[4]);
  endif

  if(Direction)
    Direction := Upper(Direction);
  endif

  if(!ChairsType)

    GInit(30,30);
    GResPic(0,0,3600,350,650);
    GPage();
    GAddPageData(317,18,2650,2651,317,615,2648,2647);

    foreach Chair in Chairs
      GCheckNextPage(10);
      GButton(30,(33+(GNPCount()*60)),2117,2118,i);
      GTextLine(60,(30+(GNPCount()*60)),560,i + ". " + Chair);
      GTilePic(260,(30+(GNPCount()*60)),CInt(SetCfg[Chair].ObjN));
      i := i + 1;
    endforeach

    Res := GSend(who)[0];

    if(!Res)
      return;
    endif

    ChairsType := Chairs[Res];

  endif

  if(!Direction)

    Direction := BuildDirGump(who,CInt(SetCfg[ChairsType].ObjN),CInt(SetCfg[ChairsType].ObjW),CInt(SetCfg[ChairsType].ObjS),CInt(SetCfg[ChairsType].ObjE));

  endif

  if(!ChairsType or !Direction)
    SendSysMessage(who,"Type [1-"+len(Chairs)+"] , Direct [N O: [N/S/W/E/M] [trg]",FONT_NORMAL,COLOR_RED);
    SendSysMessage(who,"set chr N trg",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Targ := who;

  text := CheckArray(text);

  if("trg" in text)
    Targ := LocTarget(who);
  endif

  if(!Targ)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Obj := GetConfigInt(SetCfg[ChairsType],"Obj"+Direction);

  if(Obj)
    CreateMarkedItem(Obj, Targ.x, Targ.y, Targ.z, who.realm, Flags, who);
  else
    SendSysMessage(who,"Bledne wpisy w configu ["+SET_ITEMS_CFG+"] ST: "+ChairsType+" ,Obj "+Obj,FONT_NORMAL,COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------
// Tables
//-----------------------------------------------------------------------------

function MakeBaseTable(who,text, Flags)

  var SetCfg      := ReadConfigFile(SET_ITEMS_CFG);
  var Tables      := GetConfigStringArray(SetCfg["Tables"],"Tables");
  var i           := 1;
  var TableType   := Tables[CInt(text[3])];
  var Direction   := 0;
  var Res;
  var Width := 0,Height := 0;

  if(!TableType)

    GInit(30,30);
    GResPic(0,0,3600,350,650);
    GPage();
    GAddPageData(317,18,2650,2651,317,615,2648,2647);

    foreach Table in Tables
      GCheckNextPage(10);
      GButton(30,(33+(GNPCount()*60)),2117,2118,i);
      GTextLine(60,(30+(GNPCount()*60)),560,i + ". " + Table);
      if(CInt(SetCfg[Table].ObjM))
        GTilePic(260,(30+(GNPCount()*60)),CInt(SetCfg[Table].ObjM));
      elseif(CInt(SetCfg[Table].Obj_WM))
        GTilePic(260,(30+(GNPCount()*60)),CInt(SetCfg[Table].Obj_WM));
      elseif(CInt(SetCfg[Table].Obj_N))
        GTilePic(260,(30+(GNPCount()*60)),CInt(SetCfg[Table].Obj_N));
      elseif(CInt(SetCfg[Table].Obj_NS))
        GTilePic(260,(30+(GNPCount()*60)),CInt(SetCfg[Table].Obj_NS));
      endif
      i := i + 1;
    endforeach

    Res := GSend(who)[0];

    if(!Res)
      return;
    endif

    TableType := Tables[Res];

  endif

  if(SetCfg[TableType].ObjC_N)
    if(CInt(text[4]))
      Width := CInt(text[4]);
    endif
    if(CInt(text[5]))
      Height := CInt(text[5]);
    endif
    Direction := 1;
  else
    text[4] := lower(text[4]);

    if(CStr(text[4]) == "n" or CStr(text[4]) == "w")
      Direction := CStr(text[4]);
    endif

    if(SetCfg[TableType].ObjC_E and CStr(text[4]) == "e")
      Direction := CStr(text[4]);
    endif

    if(SetCfg[TableType].ObjC_S and CStr(text[4]) == "s")
      Direction := CStr(text[4]);
    endif

    if(Direction)
      Direction := Upper(Direction);
    endif

    if(!Direction)
      if(SetCfg[TableType].Obj_N)
        Direction := BuildDirGump(who,CInt(SetCfg[TableType].Obj_N),CInt(SetCfg[TableType].Obj_W),CInt(SetCfg[TableType].Obj_S),CInt(SetCfg[TableType].Obj_E));
      else
        Direction := BuildDirGump(who,CInt(SetCfg[TableType].Obj_NS),CInt(SetCfg[TableType].Obj_WE));
      endif
    endif
    
    if(CInt(text[5]))
      Width := CInt(text[5]);
    endif
    
  endif

  if(!TableType or !Direction)
    SendSysMessage(who,"set tab [type] [N/W] [Height]",FONT_NORMAL,COLOR_RED);
    SendSysMessage(who,"set tab [type] [Width] [Height] (LargeTables)",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Targ := who;

  text := CheckArray(text);

  if("trg" in text)
    Targ := LocTarget(who);
  endif

  if(!Targ)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var w,h;
  
  if(SetCfg[TableType].ObjC_N)

    var WidthM := Width;
    var HeightM := Height;
    var ObjDown := CInt(SetCfg[TableType].ObjM);
    var ObjLeft := CInt(SetCfg[TableType].ObjM);
    var ObjRight := CInt(SetCfg[TableType].ObjM);
    var ObjUp := CInt(SetCfg[TableType].ObjM);

    if(!Width)
      WidthM := 1;
    endif
    if(!Height)
      HeightM := 1;
    endif

    if(Width and !Height)
      Height := 1;
    elseif(!Width and Height)
      Width := 1;
    endif

    if(SetCfg[TableType].ObjD)
      ObjDown := CInt(SetCfg[TableType].ObjD);
    endif
    if(SetCfg[TableType].ObjU)
      ObjUp := CInt(SetCfg[TableType].ObjU);
    endif
    if(SetCfg[TableType].ObjL)
      ObjLeft := CInt(SetCfg[TableType].ObjL);
    endif
    if(SetCfg[TableType].ObjR)
      ObjRight := CInt(SetCfg[TableType].ObjR);
    endif

    if(!CreateMarkedItem(CInt(SetCfg[TableType].ObjC_N),Targ.x,Targ.y,Targ.z,who.realm, Flags, who))
      return 0;
    endif
    if(!CreateMarkedItem(CInt(SetCfg[TableType].ObjC_E),Targ.x+WidthM,Targ.y,Targ.z,who.realm, Flags, who))
      return 0;
    endif
    if(!CreateMarkedItem(CInt(SetCfg[TableType].ObjC_W),Targ.x,Targ.y+HeightM,Targ.z,who.realm, Flags, who))
      return 0;
    endif
    if(!CreateMarkedItem(CInt(SetCfg[TableType].ObjC_S),Targ.x+WidthM,Targ.y+HeightM,Targ.z,who.realm, Flags, who))
      return 0;
    endif

    for(w:=0;w<=(Width);w:=w+1)
      for(h:=0;h<=(Height);h:=h+1)
        if((w != 0 and h != 0) and (h != Height and w != Width))
          if(CInt(SetCfg[TableType].ObjM))
            if(CreateMarkedItem(CInt(SetCfg[TableType].ObjM),Targ.x+w,Targ.y+h,Targ.z,who.realm, Flags, who))
              return 0;
            endif
          endif
        elseif(h == Height and w != 0 and w != Width)
          if(CreateMarkedItem(ObjDown,Targ.x+w,Targ.y+h,Targ.z,who.realm, Flags, who))
            return 0;
          endif
        elseif(h == 0 and w != 0 and w != Width)
          if(CreateMarkedItem(ObjUp,Targ.x+w,Targ.y+h,Targ.z,who.realm, Flags, who))
            return 0;
          endif
        elseif(w == 0 and h != 0 and h != Height)
          if(CreateMarkedItem(ObjLeft,Targ.x+w,Targ.y+h,Targ.z,who.realm, Flags, who))
            return 0;
          endif
        elseif(w == Width and h != 0 and h != Height)
          if(CreateMarkedItem(ObjRight,Targ.x+w,Targ.y+h,Targ.z,who.realm, Flags, who))
            return 0;
          endif
        endif
      endfor
    endfor
    
  elseif(SetCfg[TableType].Obj_N)

    if(!Width)
      Width := 0;
    endif

    var Tx := Targ.x,Ty := Targ.y;

    for(i:=0;i<=Width;i:=i+1)
      case(Direction)
        "S":
        "N": Ty := Targ.y + i;
        "E":
        "W": Tx := Targ.x + i;
      endcase
      if(!CreateMarkedItem(GetConfigInt(SetCfg[TableType],"Obj_"+Direction),Tx,Ty,Targ.z,who.realm, Flags, who))
        return 0;
      endif
    endfor
    
  else

    if((!GetConfigInt(SetCfg[TableType],"Obj_NM") and Direction == "N") or (!GetConfigInt(SetCfg[TableType],"Obj_WM") and Direction == "W"))
      Width := 0;
    endif
    
    case(Direction)
      "N":  if(!CreateMarkedItem(GetConfigInt(SetCfg[TableType],"Obj_NN"),Targ.x,Targ.y,Targ.z,who.realm, Flags, who))
              return 0;
            endif

            if(!CreateMarkedItem(GetConfigInt(SetCfg[TableType],"Obj_NS"),Targ.x,Targ.y+1+Width,Targ.z,who.realm, Flags, who))
              return 0;
            endif

            for(i:=1;i<=(Width);i:=i+1)
              if(!CreateMarkedItem(GetConfigInt(SetCfg[TableType],"Obj_NM"),Targ.x,Targ.y+i,Targ.z,who.realm, Flags, who))
                return 0;
              endif
            endfor

      "W":  if(!CreateMarkedItem(GetConfigInt(SetCfg[TableType],"Obj_WW"),Targ.x,Targ.y,Targ.z,who.realm, Flags, who))
              return 0;
            endif

            if(!CreateMarkedItem(GetConfigInt(SetCfg[TableType],"Obj_WE"),Targ.x+Width+1,Targ.y,Targ.z,who.realm, Flags, who))
              return 0;
            endif

            for(i:=1;i<=(Width);i:=i+1)
              if(!CreateMarkedItem(GetConfigInt(SetCfg[TableType],"Obj_WM"),Targ.x+i,Targ.y,Targ.z,who.realm, Flags, who))
                return 0;
              endif
            endfor
    endcase

  endif
   
endfunction

//-----------------------------------------------------------------------------
// Posts
//-----------------------------------------------------------------------------

function MakeBaseItems(who,text,ObjTypes,PageCount, Flags)

  var ObjType  := 0;
  if(CInt(text[3]) and len(ObjTypes) >= CInt(text[3]))
    ObjType := ObjTypes[CInt(text[3])];
  endif
  
  if(CInt(PageCount) < 1)
    PageCount := 3;
  endif

  if(!ObjType)
    ObjType := BuildObjTypeGump(who,ObjTypes,PageCount);
  endif

  if(!ObjType)
    SendSysMessage(who,"set "+text[2]+" [1-"+len(ObjTypes)+"]",FONT_NORMAL,COLOR_RED);
    return;
  endif

  text := CheckArray(text);
  var Name := 0;

  if("nam" in text)
    Name := SendTextEntryGump(who,"Wpisz nazwe",TE_CANCEL_ENABLE);
    if(!Name)
      SendSysMessage(who,"Anuloawno.",FONT_NORMAL,COLOR_RED);
      return;
    endif
  endif

  var Targ := who;

  if("trg" in text)
    Targ := LocTarget(who);
  endif

  if(!Targ)
    SendSysMessage(who,"Anuloawno.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  CreateMarkedItem(ObjType,Targ.x,Targ.y,Targ.z,who.realm,Flags, who, 0, Name);

endfunction

//-----------------------------------------------------------------------------
// ObjTypes
//-----------------------------------------------------------------------------

function GetCfgObjTypes(Type)

  var SetCfg := ReadConfigFile(SET_ITEMS_CFG);
  var ObjTypes := {}, i;
  var Ots := GetConfigStringArray(SetCfg[Type],"ObjTypes");

  foreach Ot in Ots
    Ot := SplitWords(Ot);
    if(!CInt(Ot[2]))
      Ot[2] := Ot[1];
    endif
    for(i:=CInt(Ot[1]);i<=CInt(Ot[2]);i:=i+1)
      ObjTypes.append(i);
    endfor
  endforeach
  
  return ObjTypes;

endfunction

//-----------------------------------------------------------------------------

function GetSignsObjTypes()

  var ObjTypes := {},Graph;

  for(Graph := UOBJ_SHOP_SIGN_START;Graph <= UOBJ_SHOP_SIGN_END;Graph := Graph + 2)
    ObjTypes.append(Graph);
  endfor

  ObjTypes.append(2966);
  ObjTypes.append(3086);
  ObjTypes.append(0xC44);
  ObjTypes.append(0x1F29);

  for(Graph := UOBJ_GUILD_SIGN_START;Graph <= UOBJ_GUILD_SIGN_END;Graph := Graph + 2)
    ObjTypes.append(Graph);
  endfor

  for(Graph := UOBJ_DIR_SIGN_START;Graph <= UOBJ_DIR_SIGN_END;Graph := Graph + 1)
    ObjTypes.append(Graph);
  endfor

  for(Graph := UOBJ_SIGN_HOLDER_START;Graph <= UOBJ_SIGN_HOLDER_END;Graph := Graph + 2)
    ObjTypes.append(Graph);
  endfor

  for(Graph := UOBJ_BANNER_START;Graph <= UOBJ_BANNER_END;Graph := Graph + 2)
    ObjTypes.append(Graph);
  endfor

  return ObjTypes;

endfunction

//-----------------------------------------------------------------------------

function MakeBaseCarpet(who, text, Flags)

  var SetCfg := ReadConfigFile(":tailoring:tailoring");
  var Keys := array;
  var Type := CInt(text[3]);
  var i    := 1;

  foreach Key in GetConfigIntKeys(SetCfg)
    if(SetCfg[Key].ChooseSize)
      Keys.append(Key);
    endif
  endforeach

  if(!Type or !Keys[Type])
    GInit(30,30);
    GResPic(0,0,3600,350,650);
    GPage();
    GAddPageData(317,18,2650,2651,317,615,2648,2647);

    foreach ObjType in Keys
      GCheckNextPage(10);
      GButton(30,(38+(GNPCount()*60)),2117,2118,i);
      GTextLine(60,(35+(GNPCount()*60)),560,i + ". " + SetCfg[ObjType].Name);
      GTilePic(250,(30+(GNPCount()*60)),CInt(SetCfg[ObjType].Graphic));
      i := i + 1;
    endforeach
  
    Type := GSend(who)[0];
  
    if(!Type)
      return;
    endif
  endif

  var ObjType := Keys[Type];
  var Width   := CInt(text[4]);
  var Height  := CInt(text[5]);

  if(Width <= 0)
    Width := 0;
  elseif(Width >= 32)
    Width := 32;
  endif

  if(Height <= 0)
    Height := 0;
  elseif(Height >= 32)
    Height := 32;
  endif

  var MiddG   := CInt(SetCfg[ObjType].Graphic);
  var LeftG   := CInt(SetCfg[ObjType].Left);
  var RightG  := CInt(SetCfg[ObjType].Right);
  var UpG     := CInt(SetCfg[ObjType].Up);
  var DownG   := CInt(SetCfg[ObjType].Down);
  var Corner1 := CInt(SetCfg[ObjType].Corner1);
  var Corner2 := CInt(SetCfg[ObjType].Corner2);
  var Corner3 := CInt(SetCfg[ObjType].Corner3);
  var Corner4 := CInt(SetCfg[ObjType].Corner4);
  var x       := who.x;
  var y       := who.y;
  var z       := who.z;
  var Rlm     := who.realm;
  var w,h;

  if(!MiddG or !LeftG or !RightG or !UpG or !DownG or !Corner1 or !Corner2 or !Corner3 or !Corner4)
    SendSysMessage(who, "Brak jednego z potrzebnych wpisow O: ["+ObjType+"] ["+MiddG+","+LeftG+","+RightG+","+UpG+","+DownG+","+Corner1+","+Corner2+","+Corner3+","+Corner4+"].", FONT_NORMAL, COLOR_RED);
    return;
  endif
  
  for(h:=0;h<Height;h:=h+1)
    for(w:=0;w<Width;w:=w+1)
      CreateMarkedItem(MiddG, x+w, y-h, z+1, Rlm, Flags, who);
    endfor
  endfor

  for(h:=0;h<(Height);h:=h+1)
    CreateMarkedItem(LeftG,x-1,y-h,z+1, Rlm, Flags, who);
  endfor

  for(h:=0;h<(Height);h:=h+1)
    CreateMarkedItem(RightG,x+Width,y-h,z+1, Rlm, Flags, who);
  endfor

  for(w:=0;w<(Width);w:=w+1)
    CreateMarkedItem(DownG,x+w,y+1,z+1, Rlm, Flags, who);
  endfor

  for(w:=0;w<(Width);w:=w+1)
    CreateMarkedItem(UpG,x+w,y-Height,z+1, Rlm, Flags, who);
  endfor
  
  CreateMarkedItem(Corner1,x-1,y-Height,z+1, Rlm, Flags, who);
  CreateMarkedItem(Corner2,x+Width,y-Height,z+1, Rlm, Flags, who);
  CreateMarkedItem(Corner3,x+Width,y+1,z+1, Rlm, Flags, who);
  CreateMarkedItem(Corner4,x-1,y+1,z+1, Rlm, Flags, who);

endfunction

//-----------------------------------------------------------------------------

function MakeBaseDeed(who, text, Flags)

  var SetCfg       := ReadConfigFile(":carpentry:deeds");
  var DeedObjTypes := GetConfigIntKeys(SetCfg);
  var ObjTypes     := {0,0,0,0};
  var Type         := CInt(text[3]);
  var Direction, Graphic, i;

  if(!Type or !DeedObjTypes[Type])
    GInit(30,30);
    GResPic(0,0,3600,350,650);
    GPage();
    GAddPageData(317,18,2650,2651,317,615,2648,2647);

    i := 1;

    foreach ObjType in DeedObjTypes
      GCheckNextPage(6);
      GButton(30,(38+(GNPCount()*100)),2117,2118,i);
      GTextLine(60,(35+(GNPCount()*100)),560,i + ". " + SetCfg[ObjType].Name);

      if(SetCfg[ObjType].ObjType_N)
        Graphic := CInt(SetCfg[ObjType].ObjType_N);
      elseif(SetCfg[ObjType].ObjType_S)
        Graphic := CInt(SetCfg[ObjType].ObjType_S);
      elseif(SetCfg[ObjType].ObjType_W)
        Graphic := CInt(SetCfg[ObjType].ObjType_W);
      elseif(SetCfg[ObjType].ObjType_E)
        Graphic := CInt(SetCfg[ObjType].ObjType_E);
      endif

      GTilePic(250,(30+(GNPCount()*100)),Graphic);
      i := i + 1;
    endforeach
  
    Type := GSend(who)[0];
  
    if(!Type)
      return;
    endif
  endif

  var Element := SetCfg[DeedObjTypes[Type]];

  if(Element.ObjType_S)
    ObjTypes[1]   := SplitWords(Element.ObjType_S);
    if(text[4] == "s")
      Direction := 1;
    endif
  endif
  if(Element.ObjType_E)
    ObjTypes[2]   := SplitWords(Element.ObjType_E);
    if(text[4] == "e")
      Direction := 2;
    endif
  endif
  if(Element.ObjType_N)
    ObjTypes[3]   := SplitWords(Element.ObjType_N);
    if(text[4] == "n")
      Direction := 3;
    endif
  endif
  if(Element.ObjType_W)
    ObjTypes[4]   := SplitWords(Element.ObjType_W);
    if(text[4] == "w")
      Direction := 4;
    endif
  endif
  
  if(!Direction)
    Direction := MakeDeedDirMenu(who, ObjTypes);
  endif

  if(!Direction)
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  var Mods      := {0,0,0,0};
  if(Element.Mods_S)
    Mods[1]       := SplitWords(Element.Mods_S);
  endif
  if(Element.Mods_E)
    Mods[2]       := SplitWords(Element.Mods_E);
  endif
  if(Element.Mods_N)
    Mods[3]       := SplitWords(Element.Mods_N);
  endif
  if(Element.Mods_W)
    Mods[4]       := SplitWords(Element.Mods_W);
  endif
  
  var MainOT      := ObjTypes[Direction];
  var MainMods    := Mods[Direction];
  var x           := who.x;
  var y           := who.y;
  var z           := who.z;
  var Rlm         := who.realm;
  
  i := 1;
  foreach ObjType in MainOT
    if(!CreateMarkedItem(ObjType,x+CInt(MainMods[i]),y+CInt(MainMods[i+1]),z+CInt(MainMods[i+2]), Rlm, Flags, who))
      return 0;
    endif
    i := i + 3;
  endforeach
  
endfunction

//-----------------------------------------------------------------------------

function MakeDeedDirMenu(who, ObjTypes)
  
  var menu := CreateMenu("Wybierz kierunek");
  var i,Ids := {},label := "";
  for(i := 1; i <= len(ObjTypes); i := i + 1)
    case(i)
      1:  label := "Poludnie";
      2:  label := "Wschod";
      3:  label := "Polnoc";
      4:  label := "Zachod";
    endcase
    if(ObjTypes[i])
      Ids.append(i);
      AddMenuItem(menu, CInt(ObjTypes[i][1]), label);
    endif
  endfor

  if(!Ids.size())
    return 0;
  endif

  if(len(Ids) == 1)
    return CInt(Ids[1]);
  endif

  var choice := SelectMenuItem2(who, menu).index;

  if(!choice)
    return 0;
  endif

  return CInt(Ids[choice]);
  
endfunction

//-----------------------------------------------------------------------------
