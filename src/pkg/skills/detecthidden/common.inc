//-----------------------------------------------------------------------------

function GetDetectChance( finder, hider )

  if(!CheckLineOfSight(finder,hider) or hider.graphic == INVIS_MOB_GRAP)
    return 0;
  endif
  
  var hide_skill := GetEffectiveSkill( hider, SKILLID_HIDING ) - 10;

  if(hide_skill < 0)
    hide_skill := 0;
  endif

  if(IsPlayer(finder) and IsPlayer(hider))
    hide_skill := hide_skill + (GetDexterity(hider) - GetDexterity(finder));
  endif
  
  if(hide_skill < 0)
    hide_skill := 0;
  endif

  return hide_skill;
  
endfunction

//-----------------------------------------------------------------------------
