//-----------------------------------------------------------------------------

use uo;

//-----------------------------------------------------------------------------

include "include/const/props";
include "include/client/effects";
include "include/client/sounds";
include "include/client/anims";
include "include/client/misc";
include "include/packets";

//=============================================================================
//-----------------------------------------------------------------------------
// Effects
//-----------------------------------------------------------------------------
//=============================================================================

function PlayStationaryEff( x, y, z, effect, speed, loop := 0, explode := 0, realm := _DEFAULT_REALM)

  if(CInt(x) and CInt(y) and z != error)
    PlayStationaryEffect(Cint(x),Cint(y),Cint(z),effect,speed,loop,explode,realm);
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PlayMovEffXYZ( srcx, srcy, srcz, dstx, dsty, dstz, effect, speed, loop := 0, explode := 0, realm := _DEFAULT_REALM)

  if(CInt(srcx) and CInt(srcy) and srcz != error and CInt(dstx) and CInt(dsty) and dstz != error)
    PlayMovingEffectXYZ(Cint(srcx),Cint(srcy),Cint(srcz),Cint(dstx),Cint(dsty),Cint(dstz),effect,speed,loop,explode,realm);
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PlayMovingEff( source, targ, effect, speed, loop := 0, explode := 0 )

  if(source and targ)
    if(CInt(source.x) and CInt(source.y) and source.z != error and CInt(targ.x) and CInt(targ.y) and targ.z != error)
      PlayMovingEffect(source,targ,effect,speed,loop,explode);
      return 1;
    endif
  endif
  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Animations
//-----------------------------------------------------------------------------
//=============================================================================

function PlayAction(Mob, Id, Delay := 0)

  if(!Mob.isa(POLCLASS_MOBILE) or !Id)
    return;
  endif

  if(CanPlayAction(Mob))
    if(Delay)
      SetObjProperty(Mob, PROP_PLAYACTION_DELAY, ReadGameClock() + Delay);
    endif

    if(IsPlayer(Mob))
      if(!IsHuman(Mob))
        if(GetEquipmentByLayer(Mob, LAYER_MOUNT))
          PlayPcAnim(Mob, Id);
          return;
        endif
      endif
    endif

    PerformAction(Mob, Id);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetPlayActionDelay(Mob, Delay)
      
  SetObjProperty(Mob, PROP_PLAYACTION_DELAY, ReadGameClock() + Delay);

endfunction

//-----------------------------------------------------------------------------

function CanPlayAction(Mob)

  if(GetObjProperty(Mob, PROP_PLAYACTION_DELAY) != error)
    if(GetObjProperty(Mob, PROP_PLAYACTION_DELAY) > ReadGameClock())
      return 0;
    else
      EraseObjProperty(Mob, PROP_PLAYACTION_DELAY);
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PlayHitAnim(Mob)

  if(IsHuman(Mob))
    PlayAction(Mob, ANIM_HIT);
  elseif(Mob.isa(POLCLASS_NPC))
    var cfgElem := GetNpcConfig(Mob);
    if(cfgElem.DmgAnim)
      PerformAction(Mob, CInt(cfgElem.DmgAnim));
    else
      PerformAction(Mob, ANIM_HIT_NPC);
    endif
  else
    PerformAction(Mob, ANIM_HIT_NPC);
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Water Functions
//-----------------------------------------------------------------------------
//=============================================================================

function MakeWaterSplash(who, targetsquare, Sound := 0x240)

  if(Sound)
    PlaySoundEffect(who,Sound);
  endif

  var x := who.x,y := who.y;

  PlayStationaryEff(targetsquare.x,targetsquare.y,targetsquare.z,FX_SPLASH,0x1f,0x1f,0,who.realm);

  if(who.dead)
    return 0;
  endif

  if(coordist(who.x,who.y,x,y) > 0)
    SendSysMessage(who,"Oddalil"+ggm(who,3)+" sie.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  return MakeNoise(who,0,750);
    
endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Make Noise
//-----------------------------------------------------------------------------
//=============================================================================

function MakeNoise(who, sound := 0, delay := 1500, Anim := 0, aNum := 4, EffId := 0)

  var x := who.x;
  var y := who.y;
  var Rlm := who.realm;
  var Hp := GetHp(who);
  var FullTIme := Delay*aNum;
  var EndClock := ReadMillisecondClock() + FullTime;
  var NextAnim := ReadMillisecondClock();

  while(EndClock > ReadMillisecondClock())
    if(ReadMillisecondClock() >= NextAnim)
      if(Anim)
        PerformAction(who,Anim);
      endif
      if(EffId)
        PlayObjectCenteredEffect(who, EffId, 0x10, 0x18);
      endif
      if(sound)
        PlaySoundEffect(who, sound);
      endif
      NextAnim := ReadMillisecondClock() + 1500;
    endif
    sleepms(500);
    if(who.x != x or who.y != y or who.realm != Rlm)
      SendSysMessage(who,"Oddalil"+ggm(who,3)+" sie.",FONT_NORMAL,COLOR_RED);
      return 0;
    elseif(who.dead or !who or who.paralyzed or who.frozen)
      return 0;
    elseif(IsPlayer(who) and !IsLoggedIn(who))
      return 0;
    endif

    var CurHp := GetHp(who);

    if(CurHp > Hp)
      Hp := CurHp;
    elseif(Hp - 8 > CurHp)
      SendBad(who, "Cos Ci zaszkodzilo!");
      return 0;
    endif
  endwhile

  return 1;

endfunction

//-----------------------------------------------------------------------------

function MakeNoiseDependant(who, Depends, sound := 0, delay := 1500, Anim := 0, aNum := 4, EffId := 0)

  var x := Depends.x;
  var y := Depends.y;
  var Rlm := Depends.realm;
  var wx := who.x;
  var wy := who.y;
  var wRlm := who.realm;
  var Hp := GetHp(who);
  var i;

  for(i:=1;i<=aNum;i:=i+1)
    if(Anim)
      PerformAction(who,Anim);
    endif
    if(EffId)
      PlayObjectCenteredEffect(who, EffId, 0x10, 0x18);
    endif
    sleepms(delay);
    if(sound)
      PlaySoundEffect(who, sound);
    endif
    if(Depends.x != x or Depends.y != y or Depends.realm != Rlm)
      SendBad(who,"Twoj cel oddalil sie.");
      SendBad(Depends,"Oddalil"+ggm(Depends,3)+" sie.");
      return 0;
    elseif(who.x != wx or who.y != wy or who.realm != wRlm)
      SendSysMessage(who,"Oddalil"+ggm(who,3)+" sie.",FONT_NORMAL,COLOR_RED);
      return 0;
    elseif(Depends.dead or who.dead or !who or !Depends or who.paralyzed or who.frozen)
      return 0;
    elseif(IsPlayer(Depends) and !IsLoggedIn(Depends))
      return 0;
    elseif(IsPlayer(who) and !IsLoggedIn(who))
      return 0;
    endif

    var CurHp := GetHp(who);

    if(CurHp > Hp)
      Hp := CurHp;
    elseif(Hp - 8 > CurHp)
      SendBad(who, "Cos Ci zaszkodzilo!");
      return 0;
    endif
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------
