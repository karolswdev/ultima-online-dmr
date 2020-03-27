//------------------------------------------------------------------------

function GetMassSpellRangeFactor(Loc, Range)

  var Areas := GetGlobalProperty("#MassSpellAreas");

  if(!Areas or Range <= 0)
    return 1.0;
  endif

  var Factor := 1.0;
  var Count := 0;

  CheckMassSpellExpired(Areas);

  foreach Area in Areas
    var Dist := coordist(Area.Loc.x, Area.Loc.y, Loc.x, Loc.y);
    var CurRange := Max(Area.Range, Range);
    if(Dist < CurRange and Area.Loc.realm == Loc.realm)
      if(Count == 0)
        Factor := ((CDbl(CurRange-Dist)/CDbl(CurRange)) * 0.2);
      else
        Factor := Factor * ((CDbl(CurRange-Dist)/CDbl(CurRange)) * 0.1); // im blizej tym gorzej.
      endif
      Count += 1;
    endif
  endforeach

  return Max(Factor, 0.05);

endfunction

//------------------------------------------------------------------------

function CheckMassSpellExpired(byref Areas)

  var i := Areas.size();
  var bDel := 0;

  while(i > 0)
    if(Areas[i].Expires < ReadGameClock())
      Areas.erase(i);
      bDel := 1;
    endif

    i -= 1;
  endwhile

  if(bDel)
    SetGlobalProperty("#MassSpellAreas", Areas);
  endif

endfunction

//------------------------------------------------------------------------

function AddMassSpellGlobalArea(Obj, Range, Time)

  var Data := struct;
  Data.+Loc := GetLocStruct(Obj);
  Data.+Range := Range;
  Data.+Expires := ReadGameClock() + Time;

  var Areas := GetGlobalProperty("#MassSpellAreas");
  
  if(!Areas or TypeOf(Areas) != VAR_ARRAY)
    Areas := array;
  endif

  Areas.append(Data);
  SetGlobalProperty("#MassSpellAreas", Areas);

endfunction

//------------------------------------------------------------------------
