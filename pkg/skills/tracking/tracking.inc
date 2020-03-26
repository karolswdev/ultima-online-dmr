//-----------------------------------------------------------------------------

const TRACKF_WITH_HIDDEN     := 0x01;
const TRACKF_HIDDEN_CHANCE   := 0x02;
const TRACKF_HALF_EFFICENT   := 0x04;
const TRACK_HIDDEN_DISTMAX   := 15;
const TRACK_HIDDEN_RANGE     := 16;

//-----------------------------------------------------------------------------

function GetTrackingRange(who)

  return CInt(GetEffectiveSkill(who, SKILLID_TRACKING) * 1.2 + 10);

endfunction

//-----------------------------------------------------------------------------

function GetTrackNearbyMobs(who, Range, byref Nearby, SkId, Flags := 0)

  foreach Mob in ListObjectsInBox(who.x-Range, who.y-Range, -127, who.x+Range, who.y+Range, 127, who.realm)
    if(Mob.isa(POLCLASS_MOBILE))
      if(CanTrackMobile(who, Mob, SkId, Flags))
        if(IsPlayer(Mob))
          if(Distance(who,Mob) < CInt(Range / 2))
            Nearby.append(Mob);
          endif
        else
          Nearby.append(Mob);
        endif
      endif
    endif
    Sleepms(1);
  endforeach
  
endfunction

//-----------------------------------------------------------------------------

function CanTrackMobile(who, Mob, SkId, Flags)

  if(Mob.cmdlevel or Mob.dead or Mob.concealed or Mob == who)
    return 0;
  endif
  
  if(IsInGa(Mob.x, Mob.y, Mob.realm))
    if(!IsInTourneyIsland(Mob.x, Mob.y))
      return 0;
    endif
  endif

  if(IsPlayer(Mob) and Mob.hidden)
    if(IsBitOn(Flags, TRACKF_WITH_HIDDEN))
      return 1;
    elseif(IsBitOn(Flags, TRACKF_HIDDEN_CHANCE))
      var Chance := GetTrackHiddenChance(who, Mob);
      if(CheckSkill(who, SkId, Chance, 0))
        return 1;
      endif
    endif

    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetTrackHiddenChance(who, Mob)

  if(Distance(who, Mob) < TRACK_HIDDEN_DISTMAX)
    return GetDetectChance(who, Mob) + 3;
  endif

endfunction 

//-----------------------------------------------------------------------------

function StopTracking(who)

  if(GetObjProperty(who, "#TrackingCounter"))
    SetObjProperty(who, "#StopTrack", 1);
  else
    SendSysMessage(who, "Nikogo nie sledzisz.", FONT_NORMAL, COLOR_GREEN);
  endif

endfunction

//-----------------------------------------------------------------------------

function TrackMobile(who, Range, Mob, Flags := 0, SkId := SKILLID_TRACKING)

  if(CInt(GetObjProperty(who, "#TrackMob")) == Mob.serial)
    SendSysMessage(who, "Juz sledzisz te osobe!", FONT_NORMAL, COLOR_RED);
    return;
  endif

  if(IsPlayer(Mob))
    Range := CInt(Range / 2);
  endif

  NoLongerBusy(who);
  Detach();

  EraseObjProperty(who, "#StopTrack");
  SetObjProperty(who, "#TrackingCounter", GetPid());
  SetObjProperty(who, "#TrackMob", Mob.serial);

  var Duration := CInt((10.0 + GetEffectiveSkill(who, SkId) / 4)); // max minuta sledzenia, potem reset.
  var bStopped := 0;

  if(IsBitOn(Flags, TRACKF_HALF_EFFICENT))
    Duration := CInt(Duration * 0.5);
  endif

  var timeClock := ReadGameClock() + Duration;
  var dstLoc := GetLocStruct(Mob);
  var hidTime := 0;
  var Dist;
  var wasHidden := Mob.hidden;

  if(Mob.hidden)
    dstLoc.x += CInt(TRACK_HIDDEN_RANGE/2) - RandomInt(TRACK_HIDDEN_RANGE);
    dstLoc.y += CInt(TRACK_HIDDEN_RANGE/2) - RandomInt(TRACK_HIDDEN_RANGE);
  endif

  var arrowCounter := 0;

  while(Mob and !Mob.dead and timeClock > ReadGameClock())
    
    Dist := coordist(who.x, who.y, dstLoc.x, dstLoc.y);
     
    if(Dist <= 2 or Dist > range)
      break;
    endif

    if(arrowCounter >= 4)
      arrowCounter := 0;
    endif

    if(arrowCounter >= 2)
      if(Dist > 25)
        var Loc := InterpolateLoc(GetLocStruct(who), dstLoc, 25);
        SendQuestArrow(who, Loc.x, Loc.y);
      else
        SendQuestArrow(who, dstLoc.x, dstLoc.y);
      endif
    else
      SendQuestArrow(who);
    endif

    if(GetPid() != GetObjProperty(who, "#TrackingCounter"))
      break;
    elseif(GetObjProperty(who, "#StopTrack"))
      EraseObjProperty(who, "#StopTrack");
      bStopped := 1;
      break;
    endif

    sleepms(750);
    arrowCounter := arrowCounter + 1;

    if(!wasHidden)
      dstLoc := GetLocStruct(Mob);
      
      if(Mob.hidden)
        if(!hidTime)
          hidTime := ReadGameClock();
        endif
      elseif(hidTime + 5 >= ReadGameClock())
        wasHidden := 1;
        hidTime := 0;
        dstLoc.x += CInt(TRACK_HIDDEN_RANGE/2) - RandomInt(TRACK_HIDDEN_RANGE);
        dstLoc.y += CInt(TRACK_HIDDEN_RANGE/2) - RandomInt(TRACK_HIDDEN_RANGE);
      endif
    endif

  endwhile

  SendQuestArrow(who);

  if(bStopped)
    SendSysMessage(who, "Przerwal"+ggm(who,3)+" sledzenie.", FONT_NORMAL, COLOR_ORANGE);
  elseif(Mob.dead or !Mob or coordist(who.x, who.y, dstLoc.x, dstLoc.y) > 6)
    if(IsHuman(Mob))
      SendSysMessage(who, "Stracil"+ggm(who,3)+" trop osoby "+Mob.name+".",FONT_NORMAL,COLOR_RED);
    else
      SendSysMessage(who, "Stracil"+ggm(who,3)+" trop stworzenia "+Mob.name+".",FONT_NORMAL,COLOR_RED);
    endif
  else
    if(IsHuman(Mob))
      SendSysMessage(who, "Odnalazl"+ggm(who,3)+" ostatni trop osoby "+Mob.name+".",FONT_NORMAL,COLOR_BLUE);
    else
      SendSysMessage(who, "Odnalazl"+ggm(who,3)+" ostatni trop stworzenia "+Mob.name+".",FONT_NORMAL,COLOR_BLUE);
    endif
  endif

  if(GetPid() == GetObjProperty(who, "#TrackingCounter"))
    EraseObjProperty(who, "#TrackingCounter");
    EraseObjProperty(who, "#TrackMob");
  endif

endfunction

//-----------------------------------------------------------------------------
