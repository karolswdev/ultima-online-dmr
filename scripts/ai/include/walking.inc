//-----------------------------------------------------------------------------

include "include/const/misc";

//-----------------------------------------------------------------------------

function InitWalk()

  WalkProps    := array;
  WalkProps[1] := GetObjProperty(me,PROP_ROUND_DESTS);

  if(WalkProps[1] and WalkProps[1].size())

    WalkProps[2] := GetObjProperty(me, "WalkDest");
    WalkProps[3] := CInt(GetObjProperty(me, "NextWalkPoint"));

    if(WalkProps[2])
      GetPath(WalkProps[2][1], WalkProps[2][2]);
      if(WalkProps[4] == error)
        LogNotice("[NpcWalk] InitWalk: "+me.serial+". Nie znaleziono sciezki w probie dojscia do punktu P: "+WalkProps[4].errortext+" L: "+WalkProps[2][1]+" "+WalkProps[2][2], LOGF_WORLD);
      endif
      WalkProps[5] := 1;
    endif

    CheckWalkDest();

    if(!GetObjProperty(me, "OldAnchor"))
      SetObjProperty(me, "OldAnchor", get_anchor());
    endif

    SetAnchor(me.x, me.y, NPCWALK_MAX_RANGE, 0);
  else
    WalkProps := error;
  endif

endfunction

//-----------------------------------------------------------------------------

function IsWalking()

  return WalkProps != error;
  
endfunction

//-----------------------------------------------------------------------------

function CheckWalkDest()

  if(WalkProps[1] != error)
    if(!WalkProps[2] or coordist(me.x, me.y, WalkProps[2][1], WalkProps[2][2]) <= 2 or WalkProps[5] > WalkProps[4].size() or !WalkProps[4])
      if(CInt(WalkProps[3]) <= 0)
        WalkProps[3] := 1;
      elseif(WalkProps[3] > WalkProps[1].size())
        WalkProps[3] := 1;
      endif

      if(WalkProps[2])
        if(coordist(me.x, me.y, WalkProps[2][1], WalkProps[2][2]) <= 2)
          drop_anchor(1);
        else
          if(!WalkProps[6]) 
            WalkProps[6] := 0;
          endif
          WalkProps[6] := WalkProps[6] + 1;

          if(WalkProps[4] and WalkProps[5] > WalkProps[4].size() and WalkProps[6] < 5)
            GetPath(WalkProps[2][1], WalkProps[2][2]);
            if(WalkProps[4] == error)
              LogNotice("[NpcWalk] CheckWalkDest: "+me.serial+". Nie znaleziono sciezki w probie dojscia do punktu P: "+WalkProps[4].errortext+" L: "+WalkProps[2][1]+" "+WalkProps[2][2], LOGF_WORLD);
            endif
            WalkProps[5] := 1;
            return 1;
          endif

          LogNotice("[NpcWalk] CheckWalkDest: "+me.serial+". Npc nie doszedl do lokacji! L: "+me.x+","+me.y+" "+WalkProps[2][1]+","+WalkProps[2][2], LOGF_WORLD);
        endif
      endif

      WalkProps[2] := WalkProps[1][WalkProps[3]];
      WalkProps[5] := 1;
      WalkProps[6] := 0;

      if(WalkProps[2])
        if(coordist(me.x, me.y, WalkProps[2][1], WalkProps[2][2]) > NPCWALK_MAX_RANGE)
          if(move_to_anchor() <= 0)
            LogNotice("[NpcWalk] CheckWalkDest: "+me.serial+". Odleglosc miedzy punktami za duza ("+NPCWALK_MAX_RANGE+" max) > "+coordist(me.x, me.y, WalkProps[2][1], WalkProps[2][2])+" "+me.x+","+me.y+" "+WalkProps[2][1]+","+WalkProps[2][2], LOGF_WORLD); 
          endif
          WalkProps[2] := error;
        else
          GetPath(WalkProps[2][1], WalkProps[2][2]);
          if(WalkProps[4] == error)
            if(move_to_anchor() <= 0)
              LogNotice("[NpcWalk] CheckWalkDest: "+me.serial+". Nie znaleziono sciezki P: "+WalkProps[4].errortext+" L: "+WalkProps[2][1]+" "+WalkProps[2][2], LOGF_WORLD);
            endif
          else
            WalkProps[3] := WalkProps[3] + 1;
          endif
        endif
      else
        LogNotice("[NpcWalk] CheckWalkDest: "+me.serial+". Nie znaleziono punktu "+WalkProps[3]+" w "+WalkProps[1], LOGF_WORLD);
      endif

      SetObjProperty(me, "NextWalkPoint", WalkProps[3]);
      SetObjProperty(me, "WalkDest", WalkProps[2]);
      SetWalkPause(SHORT_WALK_PAUSE + RandomInt(SHORT_WALK_PAUSE*3));
      return 0;
    endif

    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetPath(nx, ny)

  var nz := GetStandZ(nx ,ny, me.realm, GSF_GETMAPZ);

  if(nz == error)
    LogNotice("[NpcWalk] GetPathToNext: "+me.serial+". Lokacja x: "+nx+" y: "+ny+" "+me.realm+". Nie udalo sie pobrac coorda Z: - "+nz, LOGF_WORLD);
    return 0;
  endif

  WalkProps[4] := GetPathFromLoc(me.x, me.y, me.z, nx, ny, nz, me.realm);

  if(!WalkProps[4])
    WalkProps[4] := FindPath(me.x, me.y, me.z, nx, ny, nz, me.realm, 0, 25);
  endif

endfunction

//-----------------------------------------------------------------------------

function WalkToDest()

  if(IsWalkPause() or me.frozen or me.paralyzed)
    waittime := DEF_WAITTIME;
  else
    
    if(CheckWalkDest())
      if(WalkProps[4])
        if(WalkProps[5] <= WalkProps[4].size())
          waittime := !WalkTowardLocation(WalkProps[4][WalkProps[5]].x,WalkProps[4][WalkProps[5]].y);
          if(!coordist(me.x, me.y, WalkProps[4][WalkProps[5]].x,WalkProps[4][WalkProps[5]].y) or WalkProps[7])
            WalkProps[5] := WalkProps[5] + 1;
            WalkProps[7] := 0;
          else
            WalkProps[7] := 1;
          endif
        endif
      endif
    endif

  endif
    
endfunction

//-----------------------------------------------------------------------------

function IsWalkPause()

  if(CInt(GetObjProperty(me,"WalkPause")) > ReadGameClock())
    return 1;
  endif
  EraseObjProperty(me,"WalkPause");
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function SetWalkPause(Time := DEF_WALK_PAUSE_TIME)

  SetObjProperty(me,"WalkPause",ReadGameClock() + Time);
  waittime := CInt(DEF_WAITTIME / 4);

endfunction

//-----------------------------------------------------------------------------
