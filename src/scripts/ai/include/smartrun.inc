//-----------------------------------------------------------------------------

const SR_DEF_FPATH_RANGE     := 12;

//-----------------------------------------------------------------------------

var vSmartRun;

//-----------------------------------------------------------------------------

function InitSmartRunLoc()

  if(!vSmartRun)
    vSmartRun  := struct;
    vSmartRun.+NoMove;
    vSmartRun.+sRunPath;
    vSmartRun.+sRunIdx;
    vSmartRun.+sRunCount;
    vSmartRun.+runLoc := struct;
    vSmartRun.+OpenDoorChance;
  endif

  vSmartRun.NoMove     := 1;
  vSmartRun.sRunPath   := 0;
  vSmartRun.sRunIdx    := 1;
  vSmartRun.sRunCount  := 0;
  vSmartRun.runLoc.x   := 0;
  vSmartRun.runLoc.y   := 0;
  vSmartRun.OpenDoorChance := 100;

endfunction

//-----------------------------------------------------------------------------

function SmartRunOpenDoorsChance(Chance)

  if(vSmartRun)
    vSmartRun.OpenDoorChance := Chance;
  endif

endfunction

//-----------------------------------------------------------------------------

function InitSmartRunMob(followTarg)

  InitSmartRunLoc();

  if(vSmartRun.LastL == error)
    vSmartRun.+LastL   := 0;
    vSmartRun.+LastOpp := 0;
    vSmartRun.+LastP;
    vSmartRun.+CantReach;
    vSmartRun.+LastL;
  endif

  if(vSmartRun.LastOpp != followTarg.serial)
    vSmartRun.LastL      := 0;
    vSmartRun.LastP      := {me.x,me.y};
    vSmartRun.CantReach  := 0;
  endif

  vSmartRun.LastOpp := followTarg.serial;

endfunction

//-----------------------------------------------------------------------------

function SmartRunMobSucceded(followTarg)

  if(vSmartRun.LastL or vSmartRun.sRunCount or vSmartRun.CantReach)
    InitSmartRunMob(followTarg);
    vSmartRun.LastL     := 0;
    vSmartRun.CantReach := 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function SmartRunToward(followTarg)

  SetBitOn(AiFlags, AIF_TMP_LOS);
  SetBitOn(AiFlags, AIF_TMP_SPECLOS);

  if(!followTarg)
    return 0;
  endif

  if(!CheckLineOfSight(me, followTarg))
    SetBitOff(AiFlags, AIF_TMP_LOS);
    SetBitOff(AiFlags, AIF_TMP_SPECLOS);
  elseif(!CheckSpecialLos(me, followTarg, CSLF_CHECK_MOB))
    SetBitOff(AiFlags, AIF_TMP_SPECLOS);
  endif

  if(!vSmartRun.LastP)
    vSmartRun.LastP := {me.x,me.y};
  endif

  vSmartRun.LastL += 1;

  if(vSmartRun.LastL > 25)

    if(vSmartRun.NoMove) // jesli nasz npc sie nie rusza

      if(vSmartRun.LastP[1] == me.x and vSmartRun.LastP[2] == me.y)
        CheckStuck(followTarg);
      endif

    elseif(coordist(vSmartRun.LastP[1], vSmartRun.LastP[2], me.x, me.y) < 4) 

      // jesli sie rusza, ale nie moze dotrzec do celu...

      vSmartRun.CantReach += 1;
      if(vSmartRun.CantReach >= 4)
        vSmartRun.CantReach := 0;
        if(!ForceReachEnemy(followTarg) == -1) // cos poszlo nie tak (npc uciekl)
          EndFight();
        endif
      endif
    else

      vSmartRun.CantReach := 0;

    endif

    vSmartRun.LastP  := {me.x,me.y};
    vSmartRun.LastL  := 0;
    vSmartRun.NoMove := 1;

  endif

  if(followTarg.multi.isa(POLCLASS_BOAT))
    if(me.multi != followTarg.multi)
      
      var cfgElem := GetNpcConfig(me);

      if(!cfgElem.NoBoatFear and !cfgElem.MoveMode["S"])
        if(RunAwayFeared(followTarg))
          EndFight();
        endif
        return 1;
      endif
    endif
  endif

  return SmartRunTowardLoc(followTarg.x, followTarg.y, followTarg.z);

endfunction

//-----------------------------------------------------------------------------

function SmartRunOpenDoors()

  if(vSmartRun.OpenDoorChance >= 60)
    if(vSmartRun.OpenDoorChance > RandomInt(110))
      OpenDoors();
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SmartRunTowardLoc(x, y, z)

  if(me.frozen or me.paralyzed)
    return 0;
  endif

  if(vSmartRun.sRunCount > 32)
    if(!(AiFlags & AIF_TMP_LOS))
      SmartRunOpenDoors();
      if(RunByPath(x, y, z))
        return 1;
      endif
    elseif(!(AiFlags & AIF_TMP_SPECLOS))
      SmartRunOpenDoors();
      if(RunByPath(x, y, z))
        return 1;
      endif
    else
      ClearPath();
    endif
  else
    if(!(AiFlags & AIF_TMP_LOS))
      SmartRunOpenDoors();
      vSmartRun.sRunCount += 4;
    elseif(!(AiFlags & AIF_TMP_SPECLOS))
      SmartRunOpenDoors();
      vSmartRun.sRunCount += 1;
    else
      ClearPath();
    endif
  endif

  if(!x or !y)
    return 0;
  elseif(!vSmartRun.runLoc.x or !vSmartRun.runLoc.y)
    vSmartRun.runLoc.x := x;
    vSmartRun.runLoc.y := y;
  elseif(coordist(me.x, me.y, x, y) < 3)
    vSmartRun.runLoc.x := x;
    vSmartRun.runLoc.y := y;
  endif

  if(abs(vSmartRun.runLoc.x - x) > 1)
    vSmartRun.runLoc.x := x;
  endif

  if(abs(vSmartRun.runLoc.y - y) > 1)
    vSmartRun.runLoc.y := y;
  endif

  if(RunTowardLocation(vSmartRun.runLoc.x, vSmartRun.runLoc.y))
    vSmartRun.NoMove := 0;
    return 1;
  else
    if(!GetStamina(me) or me.paralyzed or me.frozen)
      vSmartRun.NoMove := 0;
    endif
    vSmartRun.LastL += 1;
  endif

  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function CheckStuck(followTarg)

  var Stand := GetStand(me.x, me.y, me.realm, me.z);

  if(!Stand.multi)
    if(Stand.z == error or (vSmartRun.LastP[1] == me.x and vSmartRun.LastP[2] == me.y and vSmartRun.NoMove))
      LogNotice("[CheckStuck] NpcStuck Mob: "+me.serial+"/"+me.npctemplate+" Loc: "+me.x+" "+me.y+" "+me.z+" "+me.realm, LOGF_WORLD);

      // czas sprobowac drastyczniejszych metod!
      if(!ForceReachEnemy(followTarg) == -1) // cos poszlo nie tak (npc uciekl)
        EndFight();
      endif
    endif
  endif

  vSmartRun.LastP := 0;

endfunction

//-----------------------------------------------------------------------------

function ClearPath()

  vSmartRun.sRunCount := -16;
  vSmartRun.sRunPath  := 0;
  vSmartRun.sRunIdx   := 1;

endfunction

//-----------------------------------------------------------------------------

function RunByPath(x, y, z)

  if(PolCore().uptime < 700)
	return 0;
  endif

  if(!me or !x or !y)
    return 0;
  endif

  if(!vSmartRun.sRunPath or vSmartRun.sRunIdx > vSmartRun.sRunPath.size())
    var Flags  := 0;
    var cfgElem := GetNpcConfig(me);

    // ja pierdu, kurwa trzeba przerzezbic te chujowe po maxie AI! kurwica bierze, jak trzeba tu cos zmienic
    // gdziez dawac takie chujowe warunki na IsTamed...
    if(cfgElem.OpensDoors or IsTamed(me)) 
      Flags := FP_IGNORE_DOORS;
    endif

    vSmartRun.sRunPath  := FindPath(me.x, me.y, me.z, x, y, z, me.realm, Flags, 15);

    if(!vSmartRun.sRunPath)
      if(!vSmartRun.sRunPath.errortext["Failed to find"])
        LogNotice("[Smartrun] NpcPath "+me.serial+" Nie znaleziono sciezki! "+me.x+" "+me.y+" "+me.z+" "+me.realm+" - "+x+" "+y+" "+z+" "+me.realm+" : "+vSmartRun.sRunPath.errortext, LOGF_WORLD);
      endif
      vSmartRun.sRunCount := -96;
    endif
    vSmartRun.sRunIdx := 1;
  endif

  return RunByRunPath();

endfunction

//-----------------------------------------------------------------------------

function RunByRunPath()

  if(vSmartRun.sRunPath)
    if(vSmartRun.sRunIdx <= vSmartRun.sRunPath.size())
      vSmartRun.sRunIdx += 1;

      if(me.x != vSmartRun.sRunPath[vSmartRun.sRunIdx-1].x or me.y != vSmartRun.sRunPath[vSmartRun.sRunIdx-1].y)
        if(!RunTowardLocation(vSmartRun.sRunPath[vSmartRun.sRunIdx-1].x, vSmartRun.sRunPath[vSmartRun.sRunIdx-1].y))
          return 0;
        elseif(me.x != vSmartRun.sRunPath[vSmartRun.sRunIdx-1].x or me.y != vSmartRun.sRunPath[vSmartRun.sRunIdx-1].y)
          return RunTowardLocation(vSmartRun.sRunPath[vSmartRun.sRunIdx-1].x, vSmartRun.sRunPath[vSmartRun.sRunIdx-1].y);
        endif
      endif

      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ForceReachEnemy(followTarg)

  var ox := me.x;
  var oy := me.y;
  var oz := me.z;

  // sanity check

  if(CanNpcTeleport(me, followTarg))
    // probujemy teleportnac npeca
    if(MoveObject(me, followTarg.x, followTarg.y, followTarg.z, followTarg.realm))

      LogNotice("[Smartrun] Mob ["+me.serial+"/"+me.npctemplate+"] Teleported: "+ox+","+oy+","+oz+" do "+followTarg.x+","+followTarg.y+","+followTarg.z+" realm: "+me.realm, LOGF_WORLD);
      PlayPcStEffect( ox, oy, oz, me.realm, FX_SMOKE, 45, 24, 24, 2);
      PlayPcEffect( me, FX_SMOKE, 45, 16, 16, 2);
      PlaySoundEffect( followTarg, SFX_SPELL_TELEPORT );
      
      case(RandomInt(3))
        1: Say("*argrrrhh*");
        2: Say("*grrrrh*");
      endcase
    
      // resetujemy dane o stucku
      vSmartRun.LastL      := 0;
      vSmartRun.LastP      := {me.x,me.y};
      vSmartRun.CantReach  := 0;
      return 1;
    endif

  endif

  // jak cos poszlo nie tak, niech probuje uciekac
  if(RunAwayFeared(followTarg))
    return -1;
  endif

  return 0;

endfunction
        
//-----------------------------------------------------------------------------

function CanNpcTeleport(me, followTarg)
  
  if(me.frozen or me.paraylzed) // check dla pewnosci
    return 0;
  elseif(me.multi or followTarg.multi) // nie chcemy by skakal do domow
    return 0;
  elseif(me.realm != followTarg.realm or coordist(me.x, me.y, followTarg.x, followTarg.y) > 20)  // sanity check
    return 0;
  elseif(IsNpcGuard(me) or IsInChampLoc(me.x, me.y, me.realm) or IsSummoned(me) or IsTamed(me) or IsSummoned(followTarg) or IsTamed(followTarg))
    return 0; 
  elseif(!CheckLineOfSight(me, followTarg) or !CheckLineOfSight(followTarg, me)) // jak zaden check z wyzej nie przejdzie, to dopiero tutaj sprawdzamy
    return 0;
  elseif(!CheckLosAt(me, followTarg.x, followTarg.y, followTarg.z))
    return 0;
  endif
  
  return 1;

endfunction

//-----------------------------------------------------------------------------
