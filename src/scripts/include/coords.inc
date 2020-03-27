//-----------------------------------------------------------------------------

use cfgfile;
use basic;
use math;

//-----------------------------------------------------------------------------

include "include/const/cfgs";

//-----------------------------------------------------------------------------

const DEF_MAP_WIDTH          := 5120;
const DEF_MAP_HEIGHT         := 4096;

const BR_X                   := 1323;
const BR_Y                   := 1624;

const LL_X                   := 5936;
const LL_Y                   := 3112;

const GSF_GETMAPZ            := 0x8000;

//-----------------------------------------------------------------------------

const TSF_LONG_REALM         := 0x01;

//-----------------------------------------------------------------------------

function ToSextant(x, y, Realm, Flags := 0)

  if(!Realm)
    Realm := _DEFAULT_REALM;
  endif

  var LongDir,LongDeg,LongMin;
  var LatDir,LatDeg,LatMin;
  var LandStr := "";
  
  if(Flags & TSF_LONG_REALM)
    case(Realm)
      BRITB_REALM:  LandStr := "Brytania, ";
      ILSHE_REALM:  LandStr := "Ilshenar, ";
      MALAS_REALM:  LandStr := "Malas, ";
      TOKUNO_REALM: LandStr := "Tokuno, ";
    endcase
  else
    case(Realm)
      BRITB_REALM:  LandStr := "Br, ";
      ILSHE_REALM:  LandStr := "Is, ";
      MALAS_REALM:  LandStr := "Ml, ";
      TOKUNO_REALM: LandStr := "Tk, ";
    endcase
  endif

  if(x <= DEF_MAP_WIDTH)
    LongDeg := CDbl((x - BR_X) * 360.000000) / DEF_MAP_WIDTH;
    LatDeg  := CDbl((y - BR_Y) * 360.000000) / DEF_MAP_HEIGHT;
  else
    LongDeg := CDbl((x - LL_X) * 360.000000) / DEF_MAP_WIDTH;
    LatDeg  := CDbl((y - LL_Y) * 360.000000) / DEF_MAP_HEIGHT;
    LandStr := "Zk, ";
  endif
  
  LongDir := "E";
  LatDir  := "S";
  
  if(LongDeg < 0)
    LongDeg := Abs(LongDeg);
    LongDir := "W";
  elseif(LongDeg > 180)
    LongDeg := 360 - LongDeg;
    LongDir := "W";
  endif
  
  if(LatDeg < 0)
    LatDeg  := Abs(LatDeg);
    LatDir  := "N";
  elseif(LatDeg > 180)
    LatDeg  := 360 - LatDeg;
    LatDir  := "N";
  endif

  LongMin := LongDeg - Floor(LongDeg);
  LatMin  := LatDeg  - Floor(LatDeg);

  LongDeg := Floor(LongDeg);
  LatDeg  := Floor(LatDeg);

  LongMin := CInt((CInt(LongMin * 10000000) * 60) / 10000000);
  LatMin  := CInt((CInt(LatMin  * 10000000) * 60) / 10000000);

  if((y > BR_Y and x <= DEF_MAP_WIDTH and LatDir == "N") or (y > LL_Y and x > DEF_MAP_WIDTH and LatDir == "N"))
    LatMin := LatMin + 1;
  endif

  if((x > BR_X and x <= DEF_MAP_WIDTH and LongDir == "W") or (x > LL_X and x > DEF_MAP_WIDTH and LongDir == "W"))
    LongMin := LongMin + 1;
  endif
  
  return LandStr + LatDeg + "o " + LatMin + "'" + LatDir + ", " + LongDeg + "o " + LongMin + "'" + LongDir;

endfunction

//-----------------------------------------------------------------------------

function ToCoordsBrit(LatDeg,LatMin,LatDir,LongDeg,LongMin,LongDir)

  var x := BR_X,y := BR_Y;
  
  if(LongDir == "W")
    if(LongDeg >= 93 and LongMin >= 2)
      LongDeg := 360 - LongDeg;
    else
      LongDeg := LongDeg * -1;
      LongMin := LongMin * -1;
    endif
  endif

  if(LatDir == "N")
    if(LatDeg >= 142 and LatMin >= 45)
      LatDeg := 360 - LatDeg;
    else
      LatDeg := LatDeg * -1;
      LatMin := LatMin * -1;
    endif
  endif
  
  LatMin := ((LatMin * 100) / 60) * 0.01;
  LongMin := ((LongMin * 100) / 60) * 0.01;

  LatDeg  := LatDeg + LatMin;
  LongDeg := LongDeg + LongMin;
  
  x := CInt(LongDeg * DEF_MAP_WIDTH / 360 + BR_X);
  y := CInt(LatDeg  * DEF_MAP_HEIGHT / 360 + BR_Y);
  
  if(LatDeg > 0)
    y := y + 1;
  endif
  
  if(LongDeg > 0)
    x := x + 1;
  endif

  return x+" "+y;
  
endfunction

//-----------------------------------------------------------------------------

function CoorDist( x1, y1, x2, y2 )

  if(x1 != error and y1 != error and x2 != error and y2 != error)
    if(Abs(x1 - x2) > Abs(y1 - y2))
      return Abs(x1 - x2);
    else
      return Abs(y1 - y2);
    endif
  endif

  return 0;
   
endfunction

//-----------------------------------------------------------------------------

function DistZ(Obj1,Obj2)

  if(Obj1.z != error and Obj2.z != error)
    return Abs(Obj1.z - Obj2.z);
  elseif(Obj1 != error and Obj2 != error)
    return Abs(CInt(Obj1) - CInt(Obj2));
  endif

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Line of Sight
//-----------------------------------------------------------------------------
//=============================================================================

const CSLF_CHECK_MOB     := 0x01;

//-----------------------------------------------------------------------------

function CheckSpecialLos(who,mob, Flags := 0)

  if(who == mob)
    return 1;
  endif
  
  if(Mob.container)
    return 1;
  endif

  if(DistZ(who,Mob) >= 20)
    return 0;
  endif

  if(coordist(who.x,who.y,Mob.x,Mob.y) > 15)
    return 0;
  endif

  case(coordist(who.x,who.y,Mob.x,Mob.y))
    0:
    1:       return 1;
    default: return CoordLosAt(who,Mob,Flags);
  endcase

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CheckSpecialLosAtPos(sx, sy, sz, sRealm, dx, dy, dz, dRealm)

  var Obj1    := struct;
  Obj1.+x     := sx;
  Obj1.+y     := sy;
  Obj1.+z     := sz;
  Obj1.+realm := sRealm;

  var Obj2    := struct;
  Obj2.+x     := dx;
  Obj2.+y     := dy;
  Obj2.+z     := dz;
  Obj2.+realm := dRealm;

  return CheckSpecialLos(Obj1, Obj2);

endfunction

//-----------------------------------------------------------------------------

function CoordLosAt(who, Obj, Flags := 0)

  if(!who or !Obj)
    return 0;
  endif

  if(who.isa(POLCLASS_NPC))
    if(who.MoveMode["S"])
      return CheckLineOfSight(who, Obj);
    endif
  endif

  if(Obj.isa(POLCLASS_NPC))
    if(Obj.MoveMode["S"])
      return CheckLineOfSight(Obj, who);
    endif
  endif

  if(!who.realm)
    return 0;
  endif

  var z;

  if(Obj.z < who.z)
    z := Obj.z;
  else
    z := who.z;
  endif

  var Coords := GetCoordsInLine(who.x, who.y, Obj.x, Obj.y);

  if(!Coords)
    return 0;
  endif

  var cZ, lastZ := who.z;

  foreach Coord in Coords
    cZ := GetStandZ(Coord.x, Coord.y, who.realm, z);

    if(cZ == error)
      return 0;
    elseif(Abs(cZ - lastZ) >= 20)
      return 0;
    elseif(Flags & CSLF_CHECK_MOB)
      if(ListMobilesNearLocation(Coord.x, Coord.y, cZ, 0, who.realm).size() > 0)
        return 0;
      endif
    endif

    lastZ := cZ;

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function NoCLosObj(Obj)

  if(Obj >= 580 and Obj <= 621)
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetStandZ(x,y,Rlm,z := error)

  var Stand := GetStand(x,y,Rlm,z);
  if(!Stand)
    return Stand;
  endif
  
  return Stand.z;

endfunction

//-----------------------------------------------------------------------------

function GetStand(x,y,Rlm,z := error)

  x := CInt(x);
  y := CInt(y);

  if(!x or !y or !Rlm)
    return error;
  endif

  var Stand;

  if(z == error or z > 128)
    if(z == GSF_GETMAPZ)
      z := GetMapInfo(x,y,Rlm);
      if(z != error)
        z := z.z;
      endif

      Stand := GetStandingHeight(x,y,z,Rlm);

      if(Stand == error)
        Stand := GetStandingHeight(x,y,z-19,Rlm);

        if(Stand == error)
           z := GetWorldHeight(x,y,Rlm);
        else
          return Stand;
        endif
      else
        return Stand;
      endif
    else
      z := GetWorldHeight(x,y,Rlm);
    endif
  endif

  if(z != error)
    Stand := GetStandingHeight(x,y,z,Rlm);
    if(Stand == error)
      Stand := GetStandingHeight(x,y,z-19,Rlm);
    endif

    if(Stand.z != error)
      Stand.z := CInt(Stand.z);
    endif

    return Stand;
  endif

  return z;

endfunction

//-----------------------------------------------------------------------------

const GPLF_IGNORE_Z          := 0x00000001;

//-----------------------------------------------------------------------------

function GetPathFromLoc(sx, sy, sz, dx, dy, dz, Rlm, Flags := 0)

  sx := CInt(sx);
  sy := CInt(sy);
  dx := CInt(dx);
  dy := CInt(dy);

  if(!sx or !sy or !dx or !dy or !Rlm)
    return 0;
  endif

  var z;

  if(dz > sz)
    z := sz;
  else
    z := dz;
  endif

  var Path := GetCoordsInLine(sx, sy, dx, dy);

  if(!Path)
    return 0;
  endif

  foreach Node in Path
    Node.+z := GetStandZ(Node.x, Node.y, Rlm, z);

    if(Node.z == error)
      if(Flags & GPLF_IGNORE_Z)
        Node.z := error;
      else
        return 0;
      endif
    endif
  endforeach

  return Path;

endfunction

//--------------------------------------------------------------------------------

function GetMobOnPath(who, Targ)

  var Path := GetPathFromLoc(who.x, who.y, who.z, Targ.x, Targ.y, Targ.z, who.realm, GPLF_IGNORE_Z);

  foreach Node in Path
    if(Node.z != error)
      foreach Mob in ListMobilesNearLocation(Node.x, Node.y, Node.z, 0, who.realm)
        if(Mob.x != who.x or Mob.y != who.y)
          return Mob;
        endif
      endforeach
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetBoxCoordsFromText(who, text, cmd)

  var sX, sY, eX, eY;
  var Realm := who.realm;
  var Range := -1;

  if(text.exists(5))
    sX := CInt(text[2]);
    sY := CInt(text[3]);
    eX := CInt(text[4]);
    eY := CInt(text[5]);

    if(text.exists(6))
      Realm := GetRealmById(text[6]);
      if(!Realm)
        SendSysMessage(who, "Bledny realm: "+text[6], FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif
  elseif(text.exists(2) and !text.exists(3) and text[2] != "?")
    Range := CInt(text[2]);

    if(Range < 0)
      SendSysMessage(who, "Bledny zasieg: "+text[2], FONT_NORMAL, COLOR_RED);
      return 0;
    endif

    sX := who.x - Range;
    sY := who.y - Range;
    eX := who.x + Range;
    eY := who.y + Range;
  else
    SendSysMessage(who, "Blednie podany zasieg lub coordy: ."+cmd+" "+text[1]+" [Range/sX sY eX eY [Rlm]]", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  var Width  := Realms()[Realm].width;
  var Height := Realms()[Realm].height;

  if(sX < 0)
    sX := 0;
  endif
  if(sY < 0)
    sY := 0;
  endif
  if(eX > Width)
    eX := Width;
  endif
  if(eY > Height)
    eY := Height;
  endif

  if(sX > eX or sY > eY or !Realm)
    SendSysMessage(who, "Bledne coordy ["+sX+", "+sY+" / "+eX+", "+eY+" "+Realm+"]", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(Range == -1)
    SendSysMessage(who, "Przeszukiwanie obszaru ["+sX+", "+sY+" / "+eX+", "+eY+" "+Realm+"]:", FONT_NORMAL, COLOR_GREEN);
  else
    SendSysMessage(who, "Przeszukiwanie w zasiegu ["+Range+"] ["+sX+", "+sY+" / "+eX+", "+eY+" "+Realm+"]:", FONT_NORMAL, COLOR_GREEN);
  endif

  var Box    := struct;
  Box.+sX    := sX;
  Box.+sY    := sY;
  Box.+eX    := eX;
  Box.+eY    := eY;
  Box.+Realm := Realm;

  return Box;

endfunction

//-----------------------------------------------------------------------------

// Zwraca strukture, zawierajaca losowa najblizsza lokacje, na ktorej mozna stac.
function GetNearestStandingLoc(x,y,z,realm,range)

  if (range < 1)
    return error;
  endif

  var loc := struct{"dist","x","y","z"};
  var locs := array;
  
  var i,j,temp;
  for( i:= x-range; i<= x+range;i:=i+1 )
    for ( j:= y-range; j<= y+range; j:=j+1 )
      temp := GetStand(i,j,realm,z);
      if (temp)
        loc.z := temp.z;
      endif
     
      if ( temp )
        loc.dist := CoorDist( x, y, i, j );
        loc.x := i;
        loc.y := j;
        locs.append(loc);
     endif
    endfor
  endfor

  if (locs.size())
    var MinDist := range +1;
    var MinDistArray;
    j:=0;
    for ( i:= 1 ; i<= locs.size() ; i:= i+1)
      if (locs[i].dist < MinDist)
        MinDist := locs[i].dist;
        MinDistArray := array;
      endif
      if (locs[i].dist == MinDist)
        MinDistArray.append(locs[i]);
      endif
    endfor
    
    j := RandomInt(MinDistArray.size())+1;
    return struct{"x":=MinDistArray[j].x,"y":=MinDistArray[j].y,"z":=MinDistArray[j].z};
  else
    return error;
  endif
  
endfunction