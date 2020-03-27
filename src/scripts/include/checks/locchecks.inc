//-----------------------------------------------------------------------------

include "include/const/misc";
include "include/str";
include "include/findcity";
include "include/objects/location";

include ":housing:finding";
include ":eventcore:finding";

//-----------------------------------------------------------------------------

const LTILE_JUNGLE           := "jungle";
const LTILE_GRASS            := "grass";
const LTILE_FOREST           := "forest";
const LTILE_DIRT             := "dirt";
const LTILE_SAND             := "sand";
const LTILE_SNOW             := "snow";

//=============================================================================
//-----------------------------------------------------------------------------
// Is Near Anvil / Forge
//-----------------------------------------------------------------------------
//=============================================================================

function IsNearAnvil(who)

  foreach item in ListItemsNearLocation(who.x, who.y, who.z,1,who.realm)
    if ((item.objtype == UOBJ_ANVIL1) or (item.objtype == UOBJ_ANVIL2))
      return item;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsNearForge(who)

  foreach item in ListItemsNearLocation(who.x, who.y, who.z,1,who.realm)
    if(item.objtype == UOBJ_SMALL_FORGE or (item.objtype >= UOBJ_FORGE_START and item.objtype <= UOBJ_FORGE_END))
      return item;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsNearCauldron(who)

  foreach item in ListItemsNearLocation(who.x, who.y, who.z,1,who.realm)
    if(item.objtype == UOBJ_CAULDRON or Item.objtype == UOBJ_CAULDRON2)
      return item;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsNearFireChimsey(who)

  foreach item in ListItemsNearLocation(who.x, who.y, who.z,1,who.realm)
    if(item.graphic == 0x0937 or item.graphic == 0x0945)
      return 1;
    elseif(item.graphic == 0x0953 or item.graphic == 0x0961)
      return 1;
    elseif(item.graphic == 0x0475 or item.graphic == 0x047B)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsNearCampFire(who)

  foreach item in ListItemsNearLocation(who.x, who.y, who.z,1,who.realm)
    if(item.objtype == 0xDE3)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsNearOven(who)

  foreach item in ListItemsNearLocation(who.x, who.y, who.z,1,who.realm)
    if(item.objtype >= 0x0461 and item.objtype <= 0x046f)
      return 1;
    elseif(item.objtype >= 0x092b and item.objtype <= 0x0934)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Champion Loc Checks
//--------------------------------------------------------------------------------
//================================================================================

function GetChampMainLoc()

  var ChampCfg := ReadConfigFile(CHAMP_CFG);

  if(!ChampCfg)
    LogError("GetChampMainLoc: Nie udalo sie odczytac konfiga mistrzow.");
  else
    var Loc := SplitWords(GetConfigString(ChampCfg["Places"],"Main"));
    if(Loc)
      Loc[1] := CInt(Loc[1]);
      Loc[2] := CInt(Loc[2]);
      Loc[3] := CInt(Loc[3]);
      Loc[4] := CInt(Loc[4]);
    endif
    return Loc;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInChampLoc(x, y, realm)

  var CLoc := GetChampMainLoc();

  if(x >= CLoc[1] and y >= CLoc[2] and x <= CLoc[3] and y <= CLoc[4] and realm == CLoc[5])
    return 1;
  endif

  return 0;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Teleporting Checks
//--------------------------------------------------------------------------------
//================================================================================

const TCF_NOCHECK_DUNG       := 0x00000001;
const TCF_CHECK_CHAMP        := 0x00000002;
const TCF_NOCHECK_BLD        := 0x00000004;
const TCF_NOCHECK_MULTI      := 0x00000008;

//--------------------------------------------------------------------------------

function NoTeleportZone( x, y , z := -150, Realm := _DEFAULT_REALM, Flags := 0)

  if(!Realm)
    Realm := _DEFAULT_REALM;
  endif

  if(z == error or !x or !y)
    return 1;
  endif

  if(z < -127 or z > 127)
    z := GetWorldHeight(x,y,Realm);
  endif

  if(z != error and !(Flags & TCF_NOCHECK_MULTI))
    if(GetStandingHeight(x,y,z,Realm).multi)
      return 1;
    endif
  endif

  if(!(Flags & TCF_NOCHECK_DUNG))
    if(IsDungeon(x,y,Realm))
      return 1;
    endif
  elseif(Flags & TCF_CHECK_CHAMP)
    if(IsInChampLoc(x,y,Realm))
      return 1;
    endif
  endif

  if(!(Flags & TCF_NOCHECK_BLD))
    if(IsInBuilding(x,y,Realm))
      return 1;
    else
      var Houses := GetNearHouses(x, y, z, Realm);
      foreach House in Houses
        var Dims := GetMultiDimensions(House.graphic);
        if((House.x+Dims.xmin) <= x and (House.x+Dims.xmax) >= x and (House.y+Dims.ymin) <= y and (House.y+Dims.ymax) >= y)
          return 1;
        endif
      endforeach
    endif
  endif

  var Loc := LocStruct();
  Loc.x := x;
  Loc.y := y;
  Loc.z := z;
  Loc.realm := realm;

  if(FindEventAreaByLoc(Loc))
    return 1;
  elseif(IsInGa(x,y, Realm))
    return 1;
  endif

  var ntzCfg := ReadConfigFile(NTZONE_CFG);
  var mId    := GetConfigMaxIntKey(ntzCfg);
  var i;

  for(i:=1;i<=mId;i:=i+1)
    Loc := SplitWords(ntzCfg[i].Loc);
    if(Loc)
      if(x >= CInt(Loc[1]) and x <= CInt(Loc[3]) and y >= CInt(Loc[2]) and y <= CInt(Loc[4]) and Realm == Loc[5])
        return 1;
      endif
    endif
  endfor
 
  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsWaterInRange(x, y, z, realm, range)

  var lx, ly;
  __ParamUnused(z); // przynajmniej na razie

  for(lx := x-range; lx<=x+range; lx+=2)

    if(IsWater(lx, y, LIST_IGNORE_Z, realm, IWF_ONLY_MAP))
      return 1;
    endif

  endfor

  for(ly := y-range; ly<=y+range; ly+=2)

    if(IsWater(x, ly, LIST_IGNORE_Z, realm, IWF_ONLY_MAP))
      return 1;
    endif

  endfor

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsPeacefulZone(x, y, realm)

  var ntzCfg := ReadConfigFile(PEACEFUL_CFG);
  var mId    := GetConfigMaxIntKey(ntzCfg);
  var i;

  for(i:=1;i<=mId;i:=i+1)
    var Loc := SplitWords(ntzCfg[i].Loc);
    if(Loc)
      if(x >= CInt(Loc[1]) and x <= CInt(Loc[3]) and y >= CInt(Loc[2]) and y <= CInt(Loc[4]) and Realm == Loc[5])
        return 1;
      endif
    endif
  endfor
 
  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsInGa(x, y, Realm)

  if(Realm != REALM_BRITANNIA)
    return 0;
  endif

  if(x >= 5500 and y >= 2200 and x <= 5600 and y <= 2250)
    return 1;
  endif

  if(x >= 5367 and x <= 6144 and y >= 1025 and y <= 1280)
    return 1;
  elseif(x >= 5888 and x <= 6144 and y >= 510 and y <= 1280)
    return 1;
  elseif(x >= 5632 and x <= 6144 and y >= 1771 and y <= 2303)
    return 1;
  elseif(x >= 5135 and x <= 6144 and y >= 2048 and y <= 2303)
    return 1;
  elseif(x >= 5120 and x <= 5375 and y >= 256 and y <= 515)
    return 1;
  elseif(x >= 5376 and x <= 6144 and y >= 1536 and y <= 1792)
    return 1;
  endif

  return 0;
endfunction

//--------------------------------------------------------------------------------

function IsDungeon( x, y, Realm := _DEFAULT_REALM )

  var DungCfg := ReadConfigFile(DUNGS_CFG);
  var MaxKey  := GetConfigMaxIntKey(DungCfg);

  for i:=1 to MaxKey
    if(DungCfg[i].realm == Realm)
      if(DungCfg[i].sx != error)
        if(x >= CInt(DungCfg[i].sx) and x <= CInt(DungCfg[i].ex) and y >= CInt(DungCfg[i].sy) and y <= CInt(DungCfg[i].ey))
          return 1;
        endif
      elseif(DungCfg[i].isx != error)
        if(x <= CInt(DungCfg[i].isx) or x >= CInt(DungCfg[i].iex) or y <= CInt(DungCfg[i].isy) or y >= CInt(DungCfg[i].iey))
          return 1;
        endif
      endif
    endif

  endfor

  return 0;

endfunction

//-----------------------------------------------------------------------------

const IWF_ONLY_MAP := 0x01;

//-----------------------------------------------------------------------------

function IsWater(x, y, z, Realm, Flags := 0)

  x := CInt(x);
  y := CInt(y);

  if(!x or !y or z == error or !Realm)
    return 0;
  endif

  var MapInfo := GetMapInfo(x, y, Realm);

  if(!(Flags & IWF_ONLY_MAP))
    foreach Item in ListStaticsAtLocation(x, y, z, 0, Realm)
      if(Abs(CInt(Item.z)-z) <= 2 or z == LIST_IGNORE_Z)
        if(IsWaterObjType(Item.objtype))
          return 1;
        else
          return 0;
        endif
      endif
    endforeach

    foreach Item in ListItemsAtLocation(x, y, z, Realm)
      if(Abs(CInt(Item.z)-z) <= 2 or z == LIST_IGNORE_Z)
        if(IsWaterObjType(Item.objtype))
          return 1;
        else
          return 0;
        endif
      endif
    endforeach
  else
    // tutaj sprawdzamy specjalny przypadek rzek i brzegow.
    // no bo oczywiscie musieli skomplikowac kurwa sprawe i landtile jest dopiero od 10-15 kratki od brzegu.
    // wczesniej jest wypelnienie landtile brazowym jednolitym backgroundem, na co nakladali dopiero statiki wody.
    // a tutaj chcemy sprawdzic po prostu, czy to jest pieprzona rzeka czy morze, a nie chcemy zlapac itemow takich jak 
    // woda w definicji multi domu.

    foreach Item in ListStaticsAtLocation(x, y, z, 0, Realm)
      if(Abs(CInt(Item.z)-z) <= 2 or z == LIST_IGNORE_Z)
        if(IsWaterObjType(Item.objtype))
          if(MapInfo.landtile >= 0x4C and MapInfo.landtile <= 0x6F)
            return 1;
          endif
        else
          return 0;
        endif
      endif
    endforeach
  endif

  if(IsWaterLandTile(MapInfo.landtile))
    if(z == LIST_IGNORE_Z)
      return 1;
    elseif(Abs(MapInfo.z - z) <= 2)
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsWaterNearby(x, y, Realm)

  if(IsWater(x, y, LIST_IGNORE_Z, Realm))
    return 1;
  elseif(IsWater(x+5, y+5, LIST_IGNORE_Z, Realm))
    return 1;
  elseif(IsWater(x-5, y-5, LIST_IGNORE_Z, Realm))
    return 1;
  elseif(IsWater(x+5, y-5, LIST_IGNORE_Z, Realm))
    return 1;
  elseif(IsWater(x-5, y+5, LIST_IGNORE_Z, Realm))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsWaterObjType(Obj)

  if(Obj >= 0x1797 and Obj <= 0x179c)
    return 1;
  elseif(Obj >= 0x346e and Obj <= 0x3485)
    return 1;
  elseif(Obj >= 0x3490 and Obj <= 0x34ab)
    return 1;
  elseif(Obj >= 0x34b5 and Obj <= 0x34d5)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsWaterLandTile(Tile)

  if(Tile >= 0x00a8 and Tile <= 0x00ab)
    return 1;
  elseif(Tile >= 0x0136 and Tile <= 0x0137)
    return 1;
  elseif(Tile >= 0x3ff0 and Tile <= 0x3ff3)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsDeepWater (x, y, Realm)

  var info := GetMapInfo (x, y, Realm);

  if ((info.landtile < 168) or (info.landtile > 171))
    return 0;
  endif

  info := GetMapInfo (x + 25, y, Realm);
  if ((info.landtile < 168) or (info.landtile > 171))
    return 0;
  endif

  info := GetMapInfo (x, y + 25, Realm);
  if ((info.landtile < 168) or (info.landtile > 171))
    return 0;
  endif

  info := GetMapInfo (x - 25, y, Realm);
  if ((info.landtile < 168) or (info.landtile > 171))
    return 0;
  endif

  info := GetMapInfo (x, y - 25, Realm);
  if ((info.landtile < 168) or (info.landtile > 171))
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function IsOnWater(x,y,Realm)

  var Land := GetMapInfo(x,y,Realm);

  if((Land.landtile >= 168 and Land.landtile <= 171) or (Land.landtile >= 310 and Land.landtile <= 311))
    return 1;
  endif

  foreach Stc in ListStaticsAtLocation(x, y, -127, 0, Realm)
    if(Stc.objtype >= 6038 and Stc.objtype <= 6066)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsInUmbra(who)

  if(who.realm != MALAS_REALM)
    return 0;
  endif

  if(who.x >= 1988 and who.y >= 1036 and who.x <= 2166 and who.y <= 1202)
    return 1;
  endif

  if(who.x >= 2230 and who.y >= 1036 and who.x <= 2436 and who.y <= 1446)
    return 1;
  endif

  if(IsInCity(who,"umbra"))
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function IsOnGreen(x, y, Realm)

  var Land := GetMapInfo(x, y, Realm).landtile;

  if(Land >= 0x3 and Land <= 0x6)
    return 1;
  elseif(Land >= 0x7D and Land <= 0x8A)
    return 1;
  elseif(Land >= 0xAC and Land <= 0xDB)
    return 1;
  elseif(Land >= 0xF8 and Land <= 0xFB)
    return 1;
  elseif(Land >= 0x108 and Land <= 0x10B)
    return 1;
  elseif(Land >= 0x15D and Land <= 0x164)
    return 1;
  elseif(Land >= 0x1A4 and Land <= 0x1A7)
    return 1;
  elseif(Land >= 0x27A and Land <= 0x27D)
    return 1;
  elseif(Land >= 0x292 and Land <= 0x295)
    return 1;
  elseif(Land >= 0x3CB and Land <= 0x3CE)
    return 1;
  elseif(Land >= 0x579 and Land <= 0x590)
    return 1;
  elseif(Land >= 0x5E3 and Land <= 0x618)
    return 1;
  elseif(Land >= 0x66B and Land <= 0x66E)
    return 1;
  elseif(Land >= 0x6A1 and Land <= 0x6C2)
    return 1;
  elseif(Land >= 0x6DE and Land <= 0x6E1)
    return 1;
  elseif(Land >= 0x73F and Land <= 0x742)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsOnDirt(x, y, Realm)

  var Land := GetMapInfo(x, y, Realm).landtile;

  if(Land >= 0x71 and Land <= 0x78)
    return 1;
  elseif(Land >= 0xE8 and Land <= 0xEB)
    return 1;
  elseif(Land >= 0x377 and Land <= 0x37A)
    return 1;
  elseif(Land >= 0x3A5 and Land <= 0x3A8)
    return 1;
  elseif(Land >= 0x553 and Land <= 0x556)
    return 1;
  elseif(Land >= 0x637 and Land <= 0x63A)
    return 1;
  elseif(Land >= 0x7AE and Land <= 0x7B1)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsTile(Tile, tName)

  var LandCfg := ReadConfigFile(LANDTILES_CFG);

  if(LandCfg[Tile].Name == tName)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsSwampTile(Tile)

  if(Tile >= 0x3D65 and Tile <= 0x3EF0)
    return 1;
  endif

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// No Combat Zone
//-----------------------------------------------------------------------------
//=============================================================================

function ChNocZone(who)

  if(GetGlobalProperty("NocZone"))
    if(!who.cmdlevel)
      return IsInNocZone(who);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInNocZone(who)

  foreach Zone in GetGlobalProperty("NocZones")
    if(who.realm == Zone[5])
      if(who.x >= Zone[1] and who.y >= Zone[2] and who.x <= Zone[3] and who.y <= Zone[4])
        return 1;
      endif
    endif
  endforeach
  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInShipPort(who)

  foreach Stc in ListStaticsAtLocation(who.x, who.y, LIST_IGNORE_Z, 0, who.realm)
    if(Stc.objtype >= 1993 and Stc.objtype <= 2000)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsNearShipPort(who)

  foreach Stc in ListStaticsNearLocation(who.x, who.y, LIST_IGNORE_Z, 8, 0, who.realm)
    if(Stc.objtype >= 1993 and Stc.objtype <= 2000)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsNpcNear(x, y, Realm, Range, Template, Flag := 0)

  foreach Npc in ListMobilesNearLocation(x, y, LIST_IGNORE_Z, Range, Realm)
    if(Npc.isa(POLCLASS_NPC))
      if(Npc.npctemplate == Template)
        return Npc;
      elseif(Flag and (Npc.npctemplate == "elf"+Template or Npc.npctemplate == "tokuno"+Template))
        return Npc;
      endif
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInNoHouseArea(x,y,realm)
  
  var CfgFile := ReadConfigFile(":mcfg:nohouse_area");
 
  foreach Area in GetConfigStringKeys( CfgFile )
    if(CfgFile[Area].realm == realm and CfgFile[Area].sX <= x and CfgFile[Area].eX >= x and CfgFile[Area].sY <= y and CfgFile[Area].eY >= y )  
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
/*
  __NONPVP__ DELETED
function IsInNonPvPArea(x,y,realm)
  
  var CfgFile := ReadConfigFile(":mcfg:nonpvp_area");
 
  foreach Area in GetConfigStringKeys( CfgFile )
    if(CfgFile[Area].realm == realm and CfgFile[Area].sX <= x and CfgFile[Area].eX >= x and CfgFile[Area].sY <= y and CfgFile[Area].eY >= y )  
      return 1;
    endif
  endforeach

  return 0;

endfunction
*/