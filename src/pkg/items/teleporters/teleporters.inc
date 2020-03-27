//--------------------------------------------------------------------------------

use datafile;

//--------------------------------------------------------------------------------

include "include/log";
include "include/items";
include "include/objtypes/misc";

include "common";

//--------------------------------------------------------------------------------

const SHOW_COLOR     := 63;
const HIDE_COLOR     := 40;

const CHANGE_GRAPHIC := 0x0F6C; //blue moongate
const CHANGE_NAME    := "Portal";
const TELE_CFG       := ":datacfg:teleporters";
const TELE_DATA      := "teleporters";

//--------------------------------------------------------------------------------

function Show(who,Range := 5)

  var num := 0;
  foreach item in ListItemsNearLocationOfType(who.x,who.y,who.z,Range,UOBJ_TELE,who.realm)
    item.graphic   := CHANGE_GRAPHIC;
    item.invisible := 0;
    item.name      := ("Teleport, x: " + GetObjProperty(item,"DestX") + " y: "+GetObjProperty(item,"DestY") + " z: " + GetObjProperty(item,"DestZ"));
    num := num + 1;
  endforeach
  SendSysMessage(who, "Wszystkie odkryte: " + num,FONT_NORMAL,SHOW_COLOR);

endfunction

//--------------------------------------------------------------------------------

function Hide(who,Range := 5)

  var num := 0;
  foreach item in ListItemsNearLocationOfType(who.x,who.y,who.z,Range,UOBJ_TELE,who.realm)
    item.graphic   := 1;
    item.invisible := 1;
    num := num + 1;
  endforeach
  SendSysMessage(who, "Wszystkie schowane: " + num,FONT_NORMAL,HIDE_COLOR);

endfunction

//--------------------------------------------------------------------------------

function GlobalHide(who)

  var Data := GetTeleData();
  var num  := 0;
  var Elem,Rlm;
  
  foreach Tele in (Data.Keys())
    if(CInt(Tele))
      Elem := Data.FindElement(CStr(Tele));
      if(Elem)
        Rlm := Elem.GetProp("Rlm");
        if(!Rlm)
          Rlm := _DEFAULT_REALM;
        endif
        
        foreach item in ListItemsNearLocationOfType(Elem.GetProp("X"),Elem.GetProp("Y"),Elem.GetProp("Z"),0,UOBJ_TELE,Rlm)
          if(item.graphic == CHANGE_GRAPHIC)
            item.graphic := 1;
            item.invisible := 1;
            num := num + 1;
          endif
        endforeach
      endif
    endif
  endforeach

  SendSysMessage(who, "Wszystkie schowane: " + num + " Wszystkie Portale: " + (len(Data.Keys())-1),FONT_NORMAL,HIDE_COLOR);

endfunction

//--------------------------------------------------------------------------------

function GlobalShow(who)

  var Data := GetTeleData();
  var num  := 0;
  var Elem,Rlm;

  foreach Tele in (Data.Keys())
    if(CInt(Tele))
      Elem := Data.FindElement(CStr(Tele));
      if(Elem)
        Rlm := Elem.GetProp("Rlm");
        if(!Rlm)
          Rlm := _DEFAULT_REALM;
        endif
      
        foreach item in ListItemsNearLocationOfType(Elem.GetProp("X"),Elem.GetProp("Y"),Elem.GetProp("Z"),0,UOBJ_TELE,Rlm)
          if(item.graphic == 1)
            item.graphic   := CHANGE_GRAPHIC;
            item.invisible := 0;
            item.name      := CHANGE_NAME;
            num := num + 1;
          endif
        endforeach
      endif
    endif
  endforeach

  SendSysMessage(who, "Wszystkie odkryte: " + num + " Wszystkie Portale: " + (len(Data.Keys())-1),FONT_NORMAL,SHOW_COLOR);

endfunction

//--------------------------------------------------------------------------------

function AddTeleToData(x,y,z,Rlm,dx,dy,dz,DRlm,who := 0)

  if(!x or !y or z == error or !dx or !dy or dz == error)
    if(who)
      SendSysMessage(who, "Bledne coordy ["+x+","+y+","+z+"] ["+dx+","+dy+","+dz+"]",FONT_NORMAL,HIDE_COLOR);
    endif
    return;
  endif

  if(x == dx and y == dy and z == dz and Rlm == DRlm)
    if(who)
      SendSysMessage(who, "Miejsce docelowe i zrodlowe jest takie same. ["+x+","+y+","+z+"] ["+dx+","+dy+","+dz+"]",FONT_NORMAL,HIDE_COLOR);
    endif
    return;
  endif

  if(GetStandingHeight(x,y,z,Rlm).z == error or GetStandingHeight(dx,dy,dz,DRlm).z == error)
    if(who)
      SendSysMessage(who,"Bledne miejsce docelowe lub zrodlowe.",FONT_NORMAL,HIDE_COLOR);
    endif
    return;
  endif

  var Items := ListItemsNearLocationOfType(x,y,z,0,UOBJ_TELE,Rlm);

  if(Items.size())
    if(who)
      SendSysMessage(who, "W tym miejscu jest juz portal.",FONT_NORMAL,HIDE_COLOR);
    endif
    return;
  endif

  Items := ListItemsNearLocationOfType(dx,dy,dz,0,UOBJ_TELE,DRlm);

  if(Items.size())
    if(who)
      SendSysMessage(who, "W tym miejscu jest juz portal.",FONT_NORMAL,HIDE_COLOR);
    endif
    return;
  endif

  var Data := GetTeleData();
  var Elem,ERlm;
  
  foreach Tele in (Data.Keys())
    if(CInt(Tele))
      Elem := Data.FindElement(CStr(Tele));
      if(Elem)
        ERlm := Elem.GetProp("Rlm");
        if(!Rlm)
          Rlm := _DEFAULT_REALM;
        endif
        
        if((x == CInt(Elem.GetProp("X")) and y == CInt(Elem.GetProp("Y")) and z == CInt(Elem.GetProp("Z")) and Rlm == ERlm) or (dx == CInt(Elem.GetProp("X")) and dy == CInt(Elem.GetProp("Y")) and dz == CInt(Elem.GetProp("Z")) and DRlm == ERlm))
          if(who)
            SendSysMessage(who,"W wybranym miejscu istnieje portal.",FONT_NORMAL,HIDE_COLOR);
          endif
          return;
        endif
      endif
    endif

    Sleepms(1);
  endforeach
  
  var Nr := GetTeleNextElemNr(Data);
  if(Nr)
    var NewElem := Data.CreateElement(CStr(Nr));
    if(NewElem)
      NewElem.SetProp("X",x);
      NewElem.SetProp("Y",y);
      NewElem.SetProp("Z",z);
      NewElem.SetProp("Rlm",Rlm);
      NewElem.SetProp("DestX",dx);
      NewElem.SetProp("DestY",dy);
      NewElem.SetProp("DestZ",dz);
      NewElem.SetProp("DestRlm",DRlm);
      if(who)
        SendSysMessage(who,"Stworzono Tele Nr ["+Nr+"] ["+x+","+y+","+z+"] ["+Rlm+"]  - ["+dx+","+dy+","+dz+"] ["+DRlm+"]",FONT_NORMAL,COLOR_GREEN);
      endif
      
      return Nr;
    endif
  endif
  SendSysMessage(who,"Nie udalo sie stworzyc Tele.",FONT_NORMAL,COLOR_RED);
  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function GetTeleNextElemNr(Data)

  var Nr    := CInt(len(Data.Keys()));
  var Elem  := Data.FindElement("Empty");
  if(!Elem)
    Elem := Data.CreateElement("Empty");
  endif
  var Empty := Elem.GetProp("EmptyNumbers");
  if(!Empty.size() or !Empty)
    Empty := {};
  else
    if(Empty[1])
      Nr := CInt(Empty[1]);
      Empty.erase(1);
    endif
  endif
  Elem.SetProp("EmptyNumbers",Empty);
  return Nr;

endfunction

//--------------------------------------------------------------------------------

function AddNewTele(who)

  var x := who.x,y := who.y,z := who.z,Rlm := who.realm;
  
  SendSysMessage(who, "Coordy portala - x: "+x+" y: "+y+" z: "+z+" Rlm: "+Rlm+".",FONT_NORMAL,SHOW_COLOR);
  SendSysMessage(who, "Wskaz miejsce docelowe portala. Mozesz uzyc .goto lub .goxyz.",FONT_NORMAL,SHOW_COLOR);
  
  if(!YesNo(who,"Juz?"))
    SendSysMessage(who, "Anulowano.",FONT_NORMAL,HIDE_COLOR);
    return;
  endif
  
  AddTeleToData(x,y,z,Rlm,who.x,who.y,who.z,who.realm,who);

endfunction

//--------------------------------------------------------------------------------

function GetInfo(who)

  SendSysMessage(who, "Wskaz Portal.",FONT_NORMAL,SHOW_COLOR);
  var Targ := ObjTarget(who,TGTOPT_NOCHECK_LOS);

  if(!Targ)
    SendSysMessage(who, "Anulowano.",FONT_NORMAL,HIDE_COLOR);
    return;
  endif

  if(targ.objtype != UOBJ_TELE)
    SendSysMessage(who, "To nie portal.",FONT_NORMAL,HIDE_COLOR);
    return;
  endif

  if(!GetobjProperty(targ,"DestX") or !GetObjProperty(targ,"DestY"))
    SendSysMessage(who, "Ten portal nie posiada coordow miejsca docelowego!",FONT_NORMAL,HIDE_COLOR);
    return;
  endif

  var Nr := GetTeleNr(Targ);

  SendSysMessage(who, "Numer portala: "+Nr,FONT_NORMAL,SHOW_COLOR);
  var DestRlm := GetObjProperty(targ, "DestRlm");
  if(!DestRlm)
    DestRlm := _DEFAULT_REALM;
  endif
  SendSysMessage(who, "Miejsce docelowe: [" + GetObjProperty(targ, "DestX")+","+GetObjProperty(targ, "DestY")+","+GetObjProperty(targ, "DestZ")+"] ["+DestRlm+"]",FONT_NORMAL,SHOW_COLOR );

endfunction

//--------------------------------------------------------------------------------

function GetTeleData()

  var Data := OpenDataFile(TELE_DATA);
  if(!Data)
    Data := CreateDataFile(TELE_DATA);
    Data.CreateElement("Empty");
  endif
  return Data;

endfunction

//--------------------------------------------------------------------------------

function FindCreatedTele(Nr,Elem := 0)

  if(!Elem)
    var Data := GetTeleData();
    Elem := Data.FindElement(CStr(Nr));
  endif
  
  var Rlm := Elem.GetProp("Rlm");
  if(!Rlm)
    Rlm := _DEFAULT_REALM;
  endif
  
  foreach item in ListItemsNearLocationOfType(CInt(Elem.GetProp("X")),CInt(Elem.GetProp("Y")),CInt(Elem.GetProp("Z")),0,UOBJ_TELE,Rlm)
    if(GetTeleNr(item) == CInt(Nr))
      return item;
    endif
  endforeach
  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function FindTeleElem(Nr)

  var Data := GetTeleData();
  return Data.FindElement(CStr(Nr));

endfunction

//--------------------------------------------------------------------------------

function CreateTele(Nr,Elem := 0,who := 0)

  if(!Elem)
    Elem := FindTeleElem(Nr);
  endif
  
  var Rlm := Elem.GetProp("Rlm");
  if(!Rlm)
    Rlm := _DEFAULT_REALM;
  endif

  var DRlm := Elem.GetProp("DestRlm");
  if(!DRlm)
    DRlm := _DEFAULT_REALM;
  endif

  if(!FindCreatedTele(Nr,Elem))
    var NewTele := CreateItemAtLoc(CInt(Elem.GetProp("X")),CInt(Elem.GetProp("Y")),CInt(Elem.GetProp("Z")),UOBJ_TELE,1,Rlm);
    if(NewTele)
      SetObjProperty(NewTele,"DestX",CInt(Elem.GetProp("DestX")));
      SetObjProperty(NewTele,"DestY",CInt(Elem.GetProp("DestY")));
      SetObjProperty(NewTele,"DestZ",CInt(Elem.GetProp("DestZ")));
      SetObjProperty(NewTele,"DestRlm",DRlm);
      SetObjProperty(NewTele,"TeleNr",Nr);
      if(who)
        SendSysMessage(who,"Stworzono Portal ["+Nr+"] , ["+CInt(Elem.GetProp("X"))+","+CInt(Elem.GetProp("Y"))+","+CInt(Elem.GetProp("Z"))+"] ["+Rlm+"]",FONT_NORMAL,COLOR_GREEN);
      endif
      return NewTele;
    else
      if(who)
        SendSysMessage(who,"Nie udalo sie stworzyc portala ["+Nr+"]",FONT_NORMAL,COLOR_RED);
      endif
      return error;
    endif
  endif
  return 0;

endfunction

//--------------------------------------------------------------------------------

function DestroyTele(Nr)

  var Tele := FindCreatedTele(Nr);
  if(Tele)
    ReleaseItem(Tele);
    if(!DestroyItem(Tele))
      return 0;
    endif
  endif
  return 1;

endfunction

//--------------------------------------------------------------------------------

function DeleteTele(Nr,who := 0)

  var Elem := FindTeleElem(Nr);
  if(!Elem)
    if(who)
      SendSysMessage(who,"Portal ["+Nr+"] nie istnieje.",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  if(!DestroyTele(Nr))
    if(who)
      SendSysMessage(who,"Nie mozna znsizczyc portala ["+Nr+"].",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  var Data := GetTeleData();
  if(who)
    SendSysMessage(who,"Skasowano Portal ["+Nr+"] ["+Elem.GetProp("X")+","+Elem.GetProp("Y")+","+Elem.GetProp("Z")+"] ["+Elem.GetProp("Rlm")+"]",FONT_NORMAL,COLOR_GREEN);
  endif
  var Empty     := Data.FindElement("Empty");
  var EmptyProp := Empty.GetProp("EmptyNumbers");
  if(!EmptyProp.size() or !EmptyProp)
    EmptyProp := {};
  endif
  EmptyProp.append(Nr);
  Empty.SetProp("EmptyNumbers",EmptyProp);
  Data.DeleteElement(CStr(Nr));
  return 1;

endfunction

//--------------------------------------------------------------------------------

function EnlNr(Nr)

  case(len(Nr))
    1: Nr := "000"+Nr;
    2: Nr := "00"+Nr;
    3: Nr := "0"+Nr;
  endcase
  return Nr;

endfunction

//--------------------------------------------------------------------------------

function GoToTele(who,Nr)

  if(!CInt(Nr))
    SendSysMessage(who,"Bledny Numer.",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  var Elem := FindTeleElem(Nr);
  if(Elem)
    var Rlm := Elem.GetProp("Rlm");
    if(!Rlm)
      Rlm := _DEFAULT_REALM;
    endif
    MoveObject(who,CInt(Elem.GetProp("X"))+1,CInt(Elem.GetProp("Y")),CInt(Elem.GetProp("Z")),Rlm,MOVECHAR_FORCELOCATION);
    SendSysMessage(who,"Przenoszenie do Portala ["+Nr+"]",FONT_NORMAL,COLOR_GREEN);
  else
    SendSysMessage(who,"Portal ["+Nr+"] nie istnieje.",FONT_NORMAL,COLOR_RED);
  endif
  
endfunction

//--------------------------------------------------------------------------------
