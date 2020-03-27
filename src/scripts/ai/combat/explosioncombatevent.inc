//-----------------------------------------------------------------------------

include "include/attributes/main";
include "include/items";

include ":alchemy:potion";

//-----------------------------------------------------------------------------

function process_combat_event()

  ThrowPotionTo(opponent);
  if ((GetHp(me) * 100/GetMaxHp(me)) < GetFleePoint())
    flee();
  endif
 
endfunction

//-----------------------------------------------------------------------------

function post_combat()

  DisableCombatEvents();
  EnableMainEvents();
  sleep(1);
  look_around();

endfunction

//-----------------------------------------------------------------------------

function prepare_for_fight()

  DisableMainEvents();
  EnableCombatEvents();

endfunction

//-----------------------------------------------------------------------------

function ThrowPotionTo(who)

  if(GetObjProperty(me, "FastThrow"))
    Detach();
	var potion2 := CreateItemInCont(me.backpack,UOBJ_EXPLOSION_POTION,1);
    ThrowExplosionPotion(me,who,potion2);
  endif
	
  if(GetObjProperty(me,"ExplosionTimer") < ReadGameClock())
    if(Distance(me,who) > 3 and RandomInt(100) < 50)
      var potion := CreateItemInCont(me.backpack,UOBJ_EXPLOSION_POTION,1);
      ThrowExplosionPotion(me,who,potion);
      SetObjProperty(me,"ExplosionTimer",ReadGameClock() + 10);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function process_flee_event()

  if(((GetHp(me) * 100) / GetMaxHp(me)) >= GetFleePoint())
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------
