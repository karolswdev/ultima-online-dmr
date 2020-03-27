//-----------------------------------------------------------------------------

include "include/const/move";
include "include/coords";
include "include/move";
include "include/packets";

//-----------------------------------------------------------------------------

const CGTF_NO_DISPEL_ABLE    := 0x01;
const CGTF_BLACK_GATE        := 0x02;
const CGTF_RED_GATE          := 0x04;
const CGTF_ONE_WAY           := 0x08;

const CGTF_NO_HOSTILES       := 0x0100;

//=============================================================================
//-----------------------------------------------------------------------------
// Gates & Teling
//-----------------------------------------------------------------------------
//=============================================================================

function CreateGateTravel(Sx, Sy, Sz, SRlm, Dx, Dy, Dz, DRlm, Flags := 0, Creator := 0)

  Sx := CInt(Sx);
  Sy := CInt(Sy);
  Dx := CInt(Dx);
  Dy := CInt(Dy);
  
  if(!DRlm)
    DRlm := _DEFAULT_REALM;
  endif
  
  if(!SRlm)
    SRlm := _DEFAULT_REALM;
  endif

  if(!Sx or !Sy or Sz == error or !Dx or !Dy or Dz == error)
    return 0;
  endif

  Sz := CInt(Sz);
  Dz := CInt(Dz);

  var Gate1,Gate2;
  
  //goqsane: 07-21-2018
  SetBitOff(Flags, CGTF_ONE_WAY);

  if(Creator.isa(POLCLASS_MOBILE))
    if(Creator.murderer)
      SetBitOn(Flags, CGTF_RED_GATE);
    endif
  endif

  if(IsBitOn(Flags, CGTF_BLACK_GATE))
    Gate1 := CreatePortal(UOBJ_BLACKGATE, Sx, Sy, Sz, SRlm, 0);
    if(!IsBitOn(Flags, CGTF_ONE_WAY))
      Gate2 := CreatePortal(UOBJ_BLACKGATE, Dx, Dy, Dz, DRlm, 0);
    endif
  elseif(IsBitOn(Flags, CGTF_RED_GATE))
    Gate1 := CreatePortal(UOBJ_REDGATE, Sx, Sy, Sz, SRlm, 0);
    if(!IsBitOn(Flags, CGTF_ONE_WAY))
      Gate2 := CreatePortal(UOBJ_REDGATE, Dx, Dy, Dz, DRlm, 0);
    endif
  else
    Gate1 := CreatePortal(UOBJ_BLUEGATE, Sx, Sy, Sz, SRlm, 0);

    if(!IsBitOn(Flags, CGTF_ONE_WAY))
      Gate2 := CreatePortal(UOBJ_BLUEGATE, Dx, Dy, Dz, DRlm, 0);
    endif
  endif

  PlaySoundEffect( Gate1, SFX_SPELL_GATE_TRAVEL );

  if(Gate2)
    PlaySoundEffect( Gate2, SFX_SPELL_GATE_TRAVEL );
  endif

  if(!(Flags & CGTF_NO_DISPEL_ABLE))
    SetObjProperty( gate1, "DestGateSer", gate2.serial);

    if(Gate2)
      SetObjProperty( gate2, "DestGateSer", gate1.serial);
    endif
  endif

  SetObjProperty(gate1, "DestX", Dx);
  SetObjProperty(gate1, "DestY", Dy);
  SetObjProperty(gate1, "DestZ", Dz);
  SetObjProperty(gate1, "DestRlm", DRlm);

  if(Gate2)
    SetObjProperty(gate2, "DestX", Gate1.x);
    SetObjProperty(gate2, "DestY", Gate1.y);
    SetObjProperty(gate2, "DestZ", Gate1.z);
    SetObjProperty(gate2, "DestRlm", SRlm);
  endif

  if(Creator.isa(POLCLASS_MOBILE))
    SetObjProperty(gate1, "Creator", Creator.serial);
    SetObjProperty(gate2, "Creator", Creator.serial);
  endif

  if(Flags & CGTF_NO_HOSTILES)
    SetObjProperty(gate1, "NoHostiles", 1);
    
    if(Gate2)
      SetObjProperty(gate2, "NoHostiles", 1);
    endif
  endif

  ShowPortal(array(Gate1,Gate2));

  var i;
  for(i:=0; i<30; i+=1)
    PlayGateEffect(Gate1);
    
    if(Gate2)
      PlayGateEffect(Gate2);
    endif
    sleep(1);
  endfor
  
  HidePortal(Gate1);
  DestroyItem(Gate1);
  
  if(Gate2)
    HidePortal(Gate2);
    DestroyItem(Gate2);
  endif
 
  return 1;

endfunction

//--------------------------------------------------------------------------------

function PlayGateEffect(Gate)

  PlayPcStEffect(Gate.x-1, Gate.y-1, Gate.z, Gate.realm, FX_HEAL_EFFECT, 3, 0x10, 0x10, 3);
  PlayPcStEffect(Gate.x+1, Gate.y-1, Gate.z, Gate.realm, FX_HEAL_EFFECT, 3, 0x10, 0x10, 3);
  PlayPcStEffect(Gate.x-1, Gate.y+1, Gate.z, Gate.realm, FX_HEAL_EFFECT, 3, 0x10, 0x10, 3);
  PlayPcStEffect(Gate.x+1, Gate.y+1, Gate.z, Gate.realm, FX_HEAL_EFFECT, 3, 0x10, 0x10, 3);

endfunction

//--------------------------------------------------------------------------------

function IsPortal(Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  return CInt(ItemCfg[Item.objtype].Portal);

endfunction

//--------------------------------------------------------------------------------

function BlockPortals(who, Delay)

  SetObjProperty(who, "#TeleDelayed", ReadGameClock()+Delay);

endfunction

//--------------------------------------------------------------------------------

function PortalTele(who, x, y, z, Rlm := 0, Type := MOVETYPE_PORTAL)

  if(!who.isa(POLCLASS_MOBILE))
    return 0;
  endif

  x := CInt(x);
  y := CInt(y);
  
  if(!x or !y or z == error)
    SendSysMessage(who,"Docelowe miejsce wydaje sie nie istniec.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  
  var Delayed := CInt(GetObjProperty(who, "#TeleDelayed"));

  if(Delayed) 
    if(Delayed > ReadGameClock())
      SendSysMessage(who, "Jakas dziwna sila powstrzymuje Cie...", FONT_NORMAL, COLOR_RED_LIGHT);
      return 0;
    else
      EraseObjProperty(who, "#TeleDelayed");
    endif
  endif

  z := CInt(z);
  
  if(!Rlm)
    Rlm := who.realm;
  endif

  if(CanPortalTele(who))
    var Sx   := who.x;
    var Sy   := who.y;
    var Sz   := who.z;
    var SRlm := who.realm;
    var Res  := MoveObject(who,x, y, z, Rlm, 0, Type);
    
    if(!Res)
      var Mods := GetLocMods(2);
      
      foreach Item in ListItemsAtLocation(x, y, z, Rlm)
        if(GetObjProperty(Item, PROP_SUMMONED_FIELD))
          DestroyItem(Item);
        elseif(Item.movable)
          var i := 1;
          while(i < Mods.size())
            if(MoveObject(Item,x+Mods[i][1], y+Mods[i][2], GetStandZ(x+Mods[i][1], y+Mods[i][2], Rlm, z), Rlm))
              break;
            endif
            i := i + 1;
          endwhile
          if(i == Mods.size())
            DestroyItem(Item);
          endif
        endif
      endforeach

      Res := MoveObject(who, x, y, z, Rlm, 0, Type);
    endif
    
    if(Res)
      SetObjProperty(who, PROP_LAST_TELE, ReadGameClock());
      MoveTamedCreaturesToLocation(who, x, y, z, Rlm, 8, Sx, Sy, Sz, SRlm);
      return 1;
    else
      SendSysMessage(who,"Portal wydaje sie byc za slaby.",FONT_NORMAL,COLOR_RED);
    endif
  endif
  return 0;

endfunction

//--------------------------------------------------------------------------------

function CanPortalTele(who)

  if(WasJustMoving(who))
    return 0;
  endif
  return 1;

endfunction

//--------------------------------------------------------------------------------

function CreatePortal(Obj,x,y,z,Rlm,ShowIt := 1)

  var Portal       := CreateItemAtTempLoc(Obj);
  Portal.invisible := 1;
  Portal.graphic   := INVIS_ITEM_GRAP;

  MoveObject(Portal, x, y, z, Rlm, MOVEITEM_FORCELOCATION | MOVEITEM_IGNOREMOVABLE);
  Portal.movable   := 0;
  if(ShowIt)
    ShowPortal(Portal);
  endif
  return Portal;

endfunction

//--------------------------------------------------------------------------------

function SetPortalLoc(Gate, DestX, DestY, DestZ, DestRealm)

  SetObjProperty(Gate, "DestX", DestX);
  SetObjProperty(Gate, "DestY", DestY);
  SetObjProperty(Gate, "DestZ", DestZ);
  SetObjProperty(Gate, "DestRlm", DestRealm);

endfunction

//--------------------------------------------------------------------------------

function SetCarryPortalProperty(Gate, Prop, Value, Method := "set")
    
  SetObjProperty(Gate, "CarryProp", array(Prop, Value, Method));

endfunction

//--------------------------------------------------------------------------------

function ShowPortal(Portal,RealGraph := 1)

  if(!Portal)
    return;
  endif
  
  if(!Portal.size())
    Portal := array(Portal);
  endif
  
  var Data := array;
  var i,n;
  var gMax := 0;
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  for(i:=1;i<=Portal.size();i:=i+1)
    Data[i] := array;
    
    if(RealGraph)
      if(ItemCfg[Portal[i].objtype].graphic)
        Data[i][1] := CInt(ItemCfg[Portal[i].objtype].graphic);
      else
        Data[i][1] := Portal[i].objtype;
      endif
    else
      Data[i][1] := Portal[i].graphic;
    endif
  
    Portal[i].invisible := 0;

    case(Data[i][1])
      UOBJ_REDGATE:    Data[i][2] := UOBJ_RDGATE_ASET; Data[i][3] := UOBJ_RDGATE_AEND;
      UOBJ_BLACKGATE:  Data[i][2] := UOBJ_BCGATE_ASET; Data[i][3] := UOBJ_BCGATE_AEND;
      UOBJ_WHITEGATE:  Data[i][2] := UOBJ_WHGATE_ASET; Data[i][3] := UOBJ_WHGATE_AEND;
      default:         Data[i][2] := UOBJ_BLGATE_ASET; Data[i][3] := UOBJ_BLGATE_AEND;
    endcase
    
    if(gMax < abs(Data[i][3]-Data[i][2]))
      gMax := abs(Data[i][3]-Data[i][2]);
    endif
  endfor
  
  for(n:=0;n<gMax;n:=n+1)
    for(i:=1;i<=Portal.size();i:=i+1)
      if(Data[i][2]+n < Data[i][3])
        Portal[i].graphic := Data[i][2]+n;
      endif
    endfor
    sleepms(200);
  endfor
  
  for(i:=1;i<=Portal.size();i:=i+1)
    Portal[i].graphic := Data[i][1];
  endfor

endfunction

//--------------------------------------------------------------------------------

function HidePortal(Portal,RealGraph := 1)

  if(!Portal)
    return;
  endif

  if(!Portal.size())
    Portal := array(Portal);
  endif

  var Data := array;
  var i,n;
  var gMax := 0;
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  for(i:=1;i<=Portal.size();i:=i+1)
    Data[i] := array;
    
    if(RealGraph)
      if(ItemCfg[Portal[i].objtype].graphic)
        Data[i][1] := CInt(ItemCfg[Portal[i].objtype].graphic);
      else
        Data[i][1] := Portal[i].objtype;
      endif
    else
      Data[i][1] := Portal[i].graphic;
    endif
  
    Portal[i].invisible := 0;

    case(Data[i][1])
      UOBJ_REDGATE:    Data[i][2] := UOBJ_RDGATE_ASET; Data[i][3] := UOBJ_RDGATE_AEND;
      UOBJ_BLACKGATE:  Data[i][2] := UOBJ_BCGATE_ASET; Data[i][3] := UOBJ_BCGATE_AEND;
      UOBJ_WHITEGATE:  Data[i][2] := UOBJ_WHGATE_ASET; Data[i][3] := UOBJ_WHGATE_AEND;
      default:         Data[i][2] := UOBJ_BLGATE_ASET; Data[i][3] := UOBJ_BLGATE_AEND;
    endcase
    
    if(gMax < abs(Data[i][3]-Data[i][2]))
      gMax := abs(Data[i][3]-Data[i][2]);
    endif
  endfor
  
  for(n:=0;n<gMax;n:=n+1)
    for(i:=1;i<=Portal.size();i:=i+1)
      if(Data[i][2]+n < Data[i][3])
        Portal[i].graphic := Data[i][3]-n;
      endif
    endfor
    sleepms(200);
  endfor
  
  for(i:=1;i<=Portal.size();i:=i+1)
    Portal[i].graphic := Data[i][3]-1;
  endfor

endfunction

//-----------------------------------------------------------------------------

function MoveTamedCreaturesToLocation(master,x,y,z,Rlm,range,Sx := 0,Sy := 0,Sz := error,SRlm := 0)
  
  if(master.dead)
    return;
  endif

  if(!Sx)
    Sx := Master.x;
  endif

  if(!Sy)
    Sy := Master.y;
  endif

  if(Sz == error)
    Sz := Master.z;
  endif
  
  if(!Rlm)
    Rlm := _DEFAULT_REALM;
  endif

  if(!SRlm)
    SRlm := Master.realm;
  endif
  

  foreach critter in ListMobilesNearLocation( Sx, Sy, LIST_IGNORE_Z, range, SRlm)
    if(GetObjProperty(critter,"#Following") == Master.serial or GetObjProperty(critter,"Rescued") == Master.serial)
      if(CheckLosAt(critter, Sx, Sy, Sz) or CoordLosAt(critter,struct{"x":=Sx,"y":=Sy,"z":=Sz,"realm":=SRlm}))
        if(!IsNecroControl(critter))
          MoveObject( critter, Cint(x), Cint(y), Cint(z), Rlm, MOVECHAR_FORCELOCATION);
        endif
      endif
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
