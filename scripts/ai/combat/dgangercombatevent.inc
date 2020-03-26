//-----------------------------------------------------------------------------

include "include/poison";
include "include/items";

//-----------------------------------------------------------------------------

function prepare_for_fight()

  DisableMainEvents();
  me.hidden := 1;
  foreach it in ListEquippedItems(me)
    if(!it.isa(POLCLASS_CONTAINER))
      DestroyItem(it);
    endif
  endforeach

  if(!me.backpack)
    var p := CreateItemAtLoc(me.x, me.y, me.z, 0x0e75, 1);
    if(!equipitem(me, p))
      DestroyItem(p);
    endif
  endif

  me.graphic  := opponent.graphic;
  me.color    := opponent.color;
  me.name     := opponent.name;

  var what;
  foreach it in ListEquippedItems(opponent)
    what := CreateItemInCont(me.backpack, it.objtype, 1);
    what.color := it.color;
    what.graphic := it.graphic;
    if (it.name)
      what.name := it.name;
    endif
    if(EquipItem(me,what))
      SetFlag(what, ITEMF_NO_LOOT);
    else
      DestroyItem(what);
    endif
  endforeach

  var weaponone := GetEquipmentByLayer( me,  LAYER_HAND1 );
  var weapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 );

  if ( (weaponone.objtype == 0x13B2) or (weapontwo.objtype == 0x13B2) )
    runaway := 0;
    what := CreateItemInInv(me.backpack,0xf3f,50);
    SetFlag(what, ITEMF_NO_LOOT);
  elseif ( (weaponone.objtype == 0x0f4f) or (weapontwo.objtype == 0x0f4f) or (weaponone.objtype == 0x13fd) or (weapontwo.objtype == 0x13fd) )
    runaway := 0;
    what := CreateItemInInv(me.backpack,0x1bfb,50);
    SetFlag(what, ITEMF_NO_LOOT);
  else
    RunAway := 100;
  endif

  me.hidden := 0;

  if ( (!weaponone) and (!weapontwo) )
    spellcast := 1;
  else
    spellcast := 0;
  endif

  EnableCombatEvents();

endfunction

//-----------------------------------------------------------------------------

function process_combat_event()

  if (spellcast)
    spellattack();
    if(GetHp(me) < (GetMaxHp(me) / 2))
      CastSpell(me,me,GHEAL_SPID);
      if(GetPoisonLevel(me) > 1)
        CastSpell(me,me,CURE_SPID);
        CastSpell(me,opponent,POISON_SPID);
      endif
    endif
  endif

  if((GetHp(me) * 100/GetMaxHp(me)) < GetFleePoint())
    flee();
  endif

endfunction

//-----------------------------------------------------------------------------

function spellattack()

  if (!GetObjProperty(me,"#lastbreath"))
    SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
  endif

  if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
    return;
  endif

  if (RandomInt(2) == 1)
    sleep(1);
    CastOffensiveSpell(me,opponent);
  endif

  SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);

endfunction

//-----------------------------------------------------------------------------

function post_combat()

  DisableCombatEvents();
  EnableMainEvents();

  sleep(1);
  look_around();
  me.hidden  := 1;
  me.graphic := me.objtype;
  me.color   := me.truecolor;
  me.name    := "sobowtor";
  
  foreach Item in ListEquippedItems(me)
    if(!item.isa(POLCLASS_CONTAINER))
      DestroyItem(item);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function process_flee_event()

  if(((GetHp(me) * 100) / GetMaxHp(me)) >= GetFleePoint())
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------
