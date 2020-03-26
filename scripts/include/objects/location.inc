//--------------------------------------------------------------------------------

function GetLocMods(Range := 1)

  var Mods := {
    {-1,0},
    {0,-1},
    {0,1},
    {1,0},
    {1,1},
    {-1,1},
    {1,-1},
    {-1,-1} };

  if(Range > 2)
    var i, n;
    for(i:=3; i<=Range; i+=1)
      for(n:=1; n<=8; n+=1)
        Mods.append(array(Mods[n][1]*Range, Mods[n][2]*Range));
      endfor
    endfor
  elseif(Range == 2)
    // dla szybkosci, podwojne mody predefiniowane
    Mods += {
      {-2,0},
      {0,-2},
      {0,2},
      {2,0},
      {2,2},
      {-2,2},
      {2,-2},
      {-2,-2},
      {0, 0}
    };
  endif

  return Mods;

endfunction

//--------------------------------------------------------------------------------

function BuildLocStruct(x, y, z, realm)

  var Loc := struct;
  
  Loc.+x := x;
  Loc.+y := y;
  Loc.+z := z;
  Loc.+realm := realm;

  return Loc;

endfunction

//--------------------------------------------------------------------------------

function LocStruct()

  var Loc := struct;
  Loc.+x := 0;
  Loc.+y := 0;
  Loc.+z := 0;
  Loc.+realm := "";

  return Loc;

endfunction

//-----------------------------------------------------------------------------

function GetLocStruct(Obj)

  var Loc := LocStruct();

  Loc.x := CInt(Obj.x);
  Loc.y := CInt(Obj.y);
  Loc.z := CInt(Obj.z);
  Loc.realm := Obj.realm;

  return Loc;

endfunction

//-----------------------------------------------------------------------------

function InterpolateLoc(sLoc, dLoc, maxRange)

  var Loc := sLoc;

  Loc.x += MinMax(dLoc.x - sLoc.x, maxRange * -1, maxRange);
  Loc.y += MinMax(dLoc.y - sLoc.y, maxRange * -1, maxRange);

  return Loc;

endfunction

//-----------------------------------------------------------------------------

function FindStandableLocToDir(srcLoc, Dir, Range)

  var Mods := GetDirLocMods(Dir);
  var Loc := GetLocStruct(srcLoc);
  Loc.x := Loc.x + Mods[1]*Range;
  Loc.y := Loc.y + Mods[2]*Range;
  
  return FindStandableLocInRange(Loc, srcLoc, Range);

endfunction

//-----------------------------------------------------------------------------

function FindStandableLocInRange(Loc, srcLoc, Range)

  var lCount := Range*8;
  var m := 0;
  Loc.z := GetStandZ(Loc.x, Loc.y, Loc.realm, Loc.z);

  while(Loc.z == error and lCount >= 0)

    if(Loc.x > srcLoc.x + Range)
      m := 1;
      Loc.x -= 1;
    elseif(Loc.y > srcLoc.y + Range)
      m := 2;
      Loc.y -= 1;
    elseif(Loc.x < srcLoc.x - Range)
      m := 3;
      Loc.x += 1;
    elseif(Loc.y < srcLoc.y - Range)
      m := 0;
      Loc.y += 1;
    endif

    case(m)
      0: Loc.x += 1;

      1: Loc.y += 1;
         
      2: Loc.x -= 1;
         
      3: Loc.y -= 1;
    endcase

    lCount -= 1;
 
    Loc.z := GetStandZ(Loc.x, Loc.y, Loc.realm, srcLoc.z);

  endwhile

  if(lCount > 0)
    return Loc;
  else
    return error;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetRandomLocMod(byref Mods, Range)

  var Mod := Mods[RandomInt(Mods.size())+1];

  Mod[1] *= RandomInt(Range+1);
  Mod[2] *= RandomInt(Range+1);

  return Mod;

endfunction

//-----------------------------------------------------------------------------

function FindStandableLocNear(srcLoc, Range)

  var Mods := GetLocMods();
  var Count := 0;
  var Loc := GetLocStruct(srcLoc);

  while(Count < 10)
    var Mod := GetRandomLocMod(Mods, Range);
    Loc.x := srcLoc.x + Mod[1];
    Loc.y := srcLoc.y + Mod[2];
    Loc.z := GetStandZ(Loc.x, Loc.y, Loc.realm);

    if(Loc.z != error)
      break;
    endif

    Count += 1;
  endwhile

  return Loc;

endfunction

//-----------------------------------------------------------------------------

function FindStandableLocBetween(srcLoc, dstLoc, Range)

  var Loc := InterpolateLoc(srcLoc, dstLoc, Range);

  return FindStandableLocInRange(Loc, srcLoc, Range);

endfunction

//-----------------------------------------------------------------------------

function GetLocString(Loc)

  if(Loc.x != error and Loc.realm)
    return Loc.x+","+Loc.y+","+Loc.z+" "+Loc.realm;
  endif

  return "<brak>";

endfunction

//-----------------------------------------------------------------------------

function GetLocFromString(sLoc)

  sLoc := SplitWords(sLoc);
  var Loc := LocStruct();

  if(sLoc.size() == 4)
    Loc.x := CInt(sLoc[1]);
    Loc.y := CInt(sLoc[2]);
    Loc.z := CInt(sLoc[3]);
    Loc.realm := sLoc[4];

    return Loc;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetLocDist(sLoc, dLoc)

  if(sLoc.realm != dLoc.realm)
    return error;
  endif

  return CoorDist(sLoc.x, sLoc.y, dLoc.x, dLoc.y);

endfunction

//-----------------------------------------------------------------------------

function GetLocsInCircumference(srcLoc, Range)
  
  if(Range == 0)
    return array(srcLoc);
  endif
  
  var Loc := LocStruct();
  Loc.x := srcLoc.x - Range;
  Loc.y := srcLoc.y - Range;
  var lStep := 1;
  var Locs := array;
  
  while(lStep < 5)
    case(lStep)
      1:
        Loc.x += 1;
        
        if(Loc.x == srcLoc.x+Range)
          lStep := 2;
        endif
        
      2: 
        Loc.y += 1;
        if(Loc.y == srcLoc.y+Range)
          lStep := 3;
        endif
        
      3: 
        Loc.x -= 1;
        if(Loc.x == srcLoc.x-Range)
          lStep := 4;
        endif
        
      4: 
        Loc.y -= 1;
        if(Loc.y == srcLoc.y-Range)
          lStep := 5;
        endif
        
    endcase
    
    Loc.z := GetStandZ(Loc.x, Loc.y, srcLoc.realm);
    Locs.append(Loc);
  endwhile
  
  return Locs;

endfunction

//-----------------------------------------------------------------------------

function GetRandomLocsNear(srcLoc, Range, Amt := 0)

  if(!Amt)
    Amt := Range*2;
  endif
  
  var Count := CInt(Amt*1.5); // maksymalna ilosc prob odnalezienia lokacji na ktorych da sie stanac.
  var dblRange := Range*2;
  var sx := srcLoc.x-Range;
  var sy := srcLoc.y-Range;
  var Locs := array;

  while(Amt > 0 and Count > 0)
    var Loc := LocStruct();
    Loc.x := sx+RandomInt(dblRange);
    Loc.y := sy+RandomInt(dblRange);
    Loc.z := GetStandZ(Loc.x, Loc.y, srcLoc.realm);

    if(Loc.z != error)
      Loc.realm := srcLoc.realm;
      Locs.append(Loc);
      Amt -= 1;
    endif

    Count -= 1;
  endwhile

  return Locs;

endfunction

//--------------------------------------------------------------------------------

function GetLocRangeFromString(lRange)

  if(lRange)
    lRange := SplitWords(lRange);

    if(lRange.size() == 5)
      lRange[1] := CInt(lRange[1]);
      lRange[2] := CInt(lRange[2]);
      lRange[3] := CInt(lRange[3]);
      lRange[4] := CInt(lRange[4]);

      return lRange;
    endif
  endif

  return error;

endfunction

//--------------------------------------------------------------------------------

function IsInLocRange(Obj, Range)

  if(Obj.realm == Range[5])
    if(Obj.x >= Range[1] and Obj.x <= Range[3])
      if(Obj.y >= Range[2] and Obj.y <= Range[4])
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsInLocHeightRange(Obj, Range)

  if(Obj.realm == Range[6])
    if(Obj.x >= Range[1] and Obj.x <= Range[3])
      if(Obj.y >= Range[2] and Obj.y <= Range[4])
        if(abs(Obj.z - Range[5]) < 5)
          return 1; 
        endif
      endif
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetLocHeightRangeFromString(lRange)

  if(lRange)
    lRange := SplitWords(lRange);

    if(lRange.size() == 6)
      lRange[1] := CInt(lRange[1]);
      lRange[2] := CInt(lRange[2]);
      lRange[3] := CInt(lRange[3]);
      lRange[4] := CInt(lRange[4]);
      lRange[5] := CInt(lRange[5]);

      return lRange;
    endif
  endif

  return error;

endfunction

//--------------------------------------------------------------------------------
