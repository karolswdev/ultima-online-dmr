//-----------------------------------------------------------------------------

include "specs/mindrage";
include "specs/conflagration";
include "specs/energyshield";
include "specs/summonminions";
include "specs/corpseeater";
include "specs/corpsespitter";
include "specs/abyssforce";
include "specs/createitem";
include "specs/stonefall";
include "specs/bladedance";
include "specs/revealterror";
include "specs/mindcontrol";
include "specs/regeneration";
include "specs/polymorph";
include "specs/curse";

//-----------------------------------------------------------------------------

function PerformNpcSpecialBreak()

  SayUC(ToUnicode("*rozz³oszczony*"));
  PerformAction(me, 4);
  sleepms(600);
  SayUC(ToUnicode("*arghh*"));
  PerformAction(me, 6);
  sleepms(600);
  SayUC(ToUnicode("*grr*"));
  PerformAction(me, 5);

endfunction

//-----------------------------------------------------------------------------

function FindSpecClosestHostile()

  var Mob  := me.opponent;

  if(!Mob)
    Mob := ListHostiles(me, 10);
    
    if(Mob.size())
      Mob := Mob[1];
    else
       Mob := ListSpecialDamageMobs(me, 10);
       
       if(Mob.size())
         Mob := Mob[1];
       else
         return 0;
       endif
    endif
  endif
  
  return Mob;

endfunction

//-----------------------------------------------------------------------------

function FindSpecFarestHostile()

  var Mobs := ListSpecialDamageMobs(me, 15);
  var FarMob := 0;
  var maxDist := 0;

  foreach Mob in Mobs
    if(Distance(Mob, me) > maxDist or !FarMob)
      FarMob := Mob;
      maxDist := Distance(Mob, me);
    endif
  endforeach
  
  return FarMob;

endfunction

//-----------------------------------------------------------------------------

function CreateSpawnBaseItem(Loc, Graphic, ControlScript)
  
  var Item := PullTempItemAtLoc(Loc.x, Loc.y, Loc.z, Graphic, Loc.realm);
  
  if(Item)
    start_script(ControlScript, Item);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
