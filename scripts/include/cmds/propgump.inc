//-----------------------------------------------------------------------------

include "include/misc";

//-----------------------------------------------------------------------------
// Object Propperty Gump
//-----------------------------------------------------------------------------

const PG_MAX_X	        := 450;
const PG_MAX_Y          := 500;
const PG_TEXT_LINES     := 20;

//-----------------------------------------------------------------------------

function BuildCPropGump(who, Object)

  var PropNames,Prop;
  var Res     := 1;
  var gWidth  := PG_MAX_X+150;
  var PropOptions := struct;
  var i;

  PropOptions.+ShowHex := 0;

  while(Res and IsLoggedIn(who))

    GInit(0, 30);
    GMultiHeaderInit();
    GResPic(0, 0,  BKGND_SCROLL, gWidth, PG_MAX_Y);

    if(!PropOptions.ShowHex)
      GTextLine(70, PG_MAX_Y-30, COLOR_RED, "Show Hex");
    else
      GTextLine(70, PG_MAX_Y-30 , COLOR_RED, "Show Int");
    endif

    GTextLine(gWidth-140, PG_MAX_Y-30, COLOR_BLUE, "Odswiez");

    GButton(50, PG_MAX_Y-27,0x4b9,0x4ba,2001);
    GButton(gWidth-160, PG_MAX_Y-27,0x4b9,0x4ba,2006);
    GButton(200,PG_MAX_Y-33,0x0ef,0x0f0,2002); // update button
    GButton(300,PG_MAX_Y-33,0x0f1,0x0f3,2003); // cancel button
    GButton(200,37,0x4b9,0x4ba,2004);          // add cprop button
    GButton(gWidth-40,7,0x01f,0x01f,2005);

    GTextLine(42, 9, COLOR_GREEN,"Prop Editor:");
    GTextCrop(140,10, 200, 25, 0, GetName(Object));
    GTextLine(35, 35, COLOR_RED,  "Del");
    GTextLine(220,35, COLOR_RED,  "Add Cprop");

    GMultiPage();

    PropNames := GetObjPropertyNames(Object);

    GAddPageData(gWidth - 33, 38, 0x0fa , 0x0fb, gWidth - 33, PG_MAX_Y - 35, 0x0fc, 0x0fd);
  
    for(i:=1;i<=PropNames.size();i:=i+1)
      if(GNPCount() == 0)
        GGumpPic(gWidth-68,PG_MAX_Y-37,0xa54);
        GGumpPic(gWidth-65,PG_MAX_Y-33,0x8b1+((GMultiCurPage()-1)%8));
      endif
 
      GCheckNextPage(PG_TEXT_LINES);
     
      if(PropNames[i])
        GTextCrop(50,(60+(GNPCount()*20)),200,20,999,PropNames[i]);
        Prop := GetObjProperty(Object,PropNames[i]);
        if(Prop.size() != error)
          GTextCrop(180,(60+(GNPCount()*20)),(gWidth-240),20,999, CutWord(CStr(Pack(Prop)), 80));
          GButton(gWidth-40,(63+(GNPCount()*20)),0x4b9,0x4ba,4000+i);
        else
          GTextEntry(180,(60+(GNPCount()*20)),(gWidth-240),20,__IFElse(IsNumber(Prop), 1420, 999),i,PGToggleHex(Prop, PropOptions.ShowHex));
        endif
        GButton(30,(63+(GNPCount()*20)),0x4b9,0x4ba,8000+i);
      endif
    endfor

    Res := GSend(who);

    if(!Object)
      return 2;
    elseif(!Res[0] or !Res)
      break;
    elseif(!IsLoggedIn(who))
      return 0;
    elseif(Res[0] > 8000 and Res[0] < 10000)
      SendSysMessage(who,"Kasacja propa "+PropNames[Res[0]-8000],FONT_NORMAL,COLOR_GREEN);
      if(YesNo(who,"Skasowac?"))
        EraseObjProperty(Object,PropNames[Res[0]-8000]);
      endif
    elseif(Res[0] > 4000 and Res[0] < 6000)
      var PropVal := GetObjProperty(Object, PropNames[Res[0]-4000]);
      ShowPropSpecs(who,Object,PropNames[Res[0]-4000], PropVal, PropOptions);
    elseif(Res[0] == 2002)
      var ThisProp, NewProp;

      for(i:=1;i<=len(PropNames);i:=i+1)
        ThisProp := GetObjProperty(Object,PropNames[i]);
        NewProp  := CroppedId(Res[i]);

        if(Res[i] != error)
          if(ThisProp.size() == error)
            if(CInt(NewProp) and len(CStr(CInt(NewProp))) == len(NewProp) or len(CStr(hex(NewProp))) == len(NewProp))
              if(ThisProp != CInt(NewProp))
                SendSysMessage(who, "Zmieniono IntProp: "+PropNames[i]+" z ["+ThisProp+"] na ["+CInt(NewProp)+"]", FONT_NORMAL, COLOR_GREEN);
                SetObjProperty(Object, PropNames[i], CInt(NewProp));
              endif
            else
              if(ThisProp != NewProp)
                SendSysMessage(who, "Zmieniono StrProp: "+PropNames[i]+" z ["+ThisProp+"] na ["+NewProp+"]", FONT_NORMAL, COLOR_GREEN);
                SetObjProperty(Object, PropNames[i], NewProp);
              endif
            endif
          endif
        endif
      endfor
    else
    
      case(Res[0])
        2003:   return 1;

        2001:   PropOptions.ShowHex := (!PropOptions.ShowHex);

        2005:   if(gWidth == PG_MAX_X + 300)
                  gWidth := PG_MAX_X;
                else
                  gWidth := gWidth + 150;
                endif

        2004:   var NewPropName := SendTextEntryGump(who,"Wpisz nazwe propa");
                if(!NewPropName)
                  SendSysMessage(who,"Anulowano.", FONT_NORMAL, COLOR_RED);
                endif

                var NewPropValue := SendTextEntryGump(who,"Wpisz wartosc propa");
                if(!NewPropValue)
                  NewPropValue := "";
                endif

                SetObjProperty(Object, NewPropName, NewPropValue);
                SendSysMessage(who,"Dodano propa "+NewPropName+" - ["+NewPropValue+"].",FONT_NORMAL,COLOR_GREEN);

        2006:    ; // refresh
        default: Res := 0;
      endcase

    endif

  endwhile

  return 1;

endfunction

//-----------------------------------------------------------------------------

function ShowPropSpecs(who, Object, PropName, PropVal, byref PropOptions)

  var gWidth  := PG_MAX_X+150;
  var i;

  while(IsLoggedIn(who))
    GInit(0, 30);
    GMultiHeaderInit();
    GResPic(0, 0,  BKGND_SCROLL, gWidth, PG_MAX_Y);

    if(!PropOptions.ShowHex)
      GTextLine(70, PG_MAX_Y-30, COLOR_RED, "Show Hex");
    else
      GTextLine(70, PG_MAX_Y-30 , COLOR_RED, "Show Int");
    endif

    GButton(50, PG_MAX_Y-27,0x4b9,0x4ba,2001);
    GButton(300,PG_MAX_Y-33,0x0f1,0x0f3,2003); // cancel button
    GButton(gWidth-40,7,0x01f,0x01f,2005);

    GTextLine(42, 9, COLOR_GREEN,"Prop Editor:");
    GTextCrop(140,10, 400, 25, 0, PropName);

    GMultiPage();

    GAddPageData(gWidth - 33, 38, 0x0fa , 0x0fb, gWidth - 33, PG_MAX_Y - 35, 0x0fc, 0x0fd);

    i := 0;

    foreach Memb in PropVal
      if(GNPCount() == 0)
        GGumpPic(gWidth-68,PG_MAX_Y-37,0xa54);
        GGumpPic(gWidth-65,PG_MAX_Y-33,0x8b1+((GMultiCurPage()-1)%8));
      endif

      GCheckNextPage(PG_TEXT_LINES);
     
      GTextCrop(50,(60+(GNPCount()*20)),200,20,999,_Memb_iter);
      if(Memb.size() != error)
        GTextCrop(180,(60+(GNPCount()*20)),(gWidth-240),20,999, CutWord(CStr(Pack(Memb)), 80));
        GButton(gWidth-40,(63+(GNPCount()*20)),0x4b9,0x4ba,4000+i);
      else
        GTextCrop(180,(60+(GNPCount()*20)),(gWidth-240),20,__IFElse(IsNumber(Memb), 1420, 999),PGToggleHex(Memb, PropOptions.ShowHex));
      endif
      i += 1;
    endforeach

    var Res := GSend(who);
    
    if(Res[0] == 2001)
      PropOptions.ShowHex := (!PropOptions.ShowHex);

    elseif(Res[0] >= 4000 and Res[0] < 6000)
      var Idx := CInt(Res[0])-4000;
      i := 0;
      foreach Memb in PropVal
        if(Idx == i)
          ShowPropSpecs(who, Object, PropName+"."+_Memb_iter, Memb, PropOptions);
          break;
        endif
        i += 1;
      endforeach
    else
      break;
    endif
  endwhile

endfunction

//-----------------------------------------------------------------------------

function PGToggleHex(byref Check, ShowHex)

  if(TypeOf(Check) == "Integer")
    if(ShowHex)
      return Hex(check);
    else
      return CInt(check);
    endif
  else
    return Check;
  endif

endfunction

//-----------------------------------------------------------------------------
