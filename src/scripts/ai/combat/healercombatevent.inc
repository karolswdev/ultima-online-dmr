//-----------------------------------------------------------------------------

include "include/attributes/main";
include "include/poison";

//-----------------------------------------------------------------------------

function process_combat_event()

  if(GetMana(me) > 14)
    spellattack();
    if (GetHp(me) < (GetMaxHp(me) / 2))
      CastSpell(me,me,GHEAL_SPID);
      if(GetPoisonLevel(me))
        CastSpell(me,me,CURE_SPID);
      endif
    endif
  endif
  
  if ((GetHp(me) * 100/GetMaxHp(me)) < GetFleePoint())
    flee();
  endif
  
endfunction

//-----------------------------------------------------------------------------

function spellattack()

  if(LastSpellTime < ReadGameClock())
    if(CheckLineOfSight(me, opponent) and Distance(me, opponent) <= 15)
      if(opponent != me)
        CastOffensiveSpell(me,opponent);

        foreach myfriend in ListMobilesNearLocation(me.x, me.y, me.z, Specs[NSPEC_THRESHOLD], me.realm)
          healerstuff(myfriend);
        endforeach
      endif
    endif

    var STime := GetSpellTime();
    LastSpellTime := ReadGameClock() + RandomInt(STime) + STime;
  endif
  
endfunction

//-----------------------------------------------------------------------------

function prepare_for_fight()

  if (!IsReflected(me))
    CastSpell(me,me,REFLECT_SPID);
  endif
  DisableMainEvents();
  EnableCombatEvents();

endfunction

//-----------------------------------------------------------------------------

function post_combat()
  DisableCombatEvents();
  EnableMainEvents();
  sleepms(250);
  look_around();
endfunction

//-----------------------------------------------------------------------------

function process_flee_event()

  if(GetMana(me) > 14)
    if((GetHp(me) < (GetMaxHP(me) / 2)) and (RandomInt(8) >= 3))
      CastSpell(me, me, GHEAL_SPID);
      return 0;
    endif
    if((GetPoisonLevel(me) >= 1) and (RandomInt(8) >= 3))
      CastSpell(me, me, CURE_SPID);
      return 0;
    endif
    SpellAttack();
  endif
  
  if(((GetHp(me) * 100) / GetMaxHp(me)) >= GetFleePoint())
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function healerstuff(mobile)

  if(!mobile.npctemplate or mobile.npctemplate["guard"] or IsTamed(mobile))
    return;
  elseif(mobile == opponent)
    return;
  endif
  
  if(!CheckLineOfSight(me,mobile))
    return;
  endif
  
  if(GetPoisonLevel(mobile) > 0)
    TurnToward(mobile);
    CastSpell(me,mobile,CURE_SPID);
  elseif(GetHp(mobile) < CInt(GetMaxHp(mobile) / 2))
    TurnToward(mobile);
    CastSpell(me,mobile,GHEAL_SPID);
  endif
  
endfunction

//-----------------------------------------------------------------------------
