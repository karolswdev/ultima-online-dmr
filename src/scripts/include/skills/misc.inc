//--------------------------------------------------------------------------------

include "include/attributes/main";

//================================================================================
//--------------------------------------------------------------------------------
// Restricted Points
//--------------------------------------------------------------------------------
//================================================================================

const GRPF_FOR_BASE := 0x01;

//--------------------------------------------------------------------------------

function GetRestrictedPoints(who, skillid, diff, points, Flags := 0)

  var skill;
  if(Flags & GRPF_FOR_BASE)
    skill := GetBaseSkill(who, skillid);
  else
    skill := GetEffectiveSkill(who, skillid);
  endif

  if(skill > (diff - 20))
    if((skill - diff) <= 5)
      points := points;
    elseif((skill - diff) <= 10)
      points := (points * 3) / 4;
    elseif((skill - diff) <= 15)
      points := points / 2;
    elseif((skill - diff) <= 20)
      points := points / 4;
    else
      points := 0;
    endif
  endif
  
  if(points < 0)
   points := 0;
  endif
  return points;

endfunction

//--------------------------------------------------------------------------------

function GetHalfRestrictedPoints(who, skillid, diff, points)

  var skill := GetEffectiveSkill(who, skillid);
  if(skill > (diff - 20))
    if((skill - diff) <= 5)
      points := points;
    elseif((skill - diff) <= 10)
      points := (points * 3) / 4;
    elseif((skill - diff) <= 20)
      points := points / 2;
    endif
  endif
  
  if(points < 0)
   points := 0;
  endif
  return points;

endfunction

//--------------------------------------------------------------------------------

function FindSkillidByIdentifier(skid)

  case(lower(skid))
    "wrestling":     return SKILLID_WRESTLING;
    "fencing":       return SKILLID_FENCING;
    "swords":        return SKILLID_SWORDSMANSHIP;
    "swordsmanship": return SKILLID_SWORDSMANSHIP;
    "mace":          return SKILLID_MACEFIGHTING;
    "macefighting":  return SKILLID_MACEFIGHTING;
    "axes":          return SKILLID_AXESMANSHIP;
    "axesmanship":   return SKILLID_AXESMANSHIP;
    "archery":       return SKILLID_ARCHERY;
  endcase

  return 0;

endfunction

//--------------------------------------------------------------------------------
