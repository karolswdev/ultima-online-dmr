//-----------------------------------------------------------------------------

include "include/portals";
include "include/items/temp_storage";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac sile otchlani
// 3 - szansa na rzucenie w loopie
// 4 - czas trwania rzucania w sekundach
// 5 - zasieg specjala

// przyklad: AbyssForce 30 50 5 8 50
//-----------------------------------------------------------------------------

function CheckAbyssForce()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_ABYSSFORCE][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_ABYSSFORCE][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[AbyssForce] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_ABYSSFORCE][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_ABYSSFORCE))
    return PerformAbyssForce();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformAbyssForceAnimation(Secs)

  var i;

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*przywo³uje si³ê otch³ani*"));
      4: SayUC(ToUnicode("*przyci¹ga otch³añ!*"));
    endcase

    if(Modulo(i, 2) == 0)
      PerformAction(me, 10);
      PlaySoundEffect(me, SFX_FOCUSATTACK);
    else
      PerformAction(me, 12);
      PlaySoundEffect(me, SFX_FEINT);
    endif

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_ABYSSFORCE))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_ABYSSFORCE))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformAbyssForce()

  SetInvulToAttack(me);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[AbyssForce] "+GetName(me)+": Perform with delay: "+Specs[NSPEC_ABYSSFORCE][3]);
  endif

  var Secs := Specs[NSPEC_ABYSSFORCE][4];

  SetNpcSpecPerforming(me, NSPEC_ABYSSFORCE, 4);

  var Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_ABYSSFORCE][5], LSDMF_ONLY_PLAYERS);

  foreach Mob in Mobs
    BlockPortals(Mob, CInt(Specs[NSPEC_ABYSSFORCE][3]*1.5) );
  endforeach

  if(PerformAbyssForceAnimation(Secs))
    SayUC(ToUnicode("*loch ogarnia otch³añ*"));

    PerformAction(me, 18);
    PlaySoundEffect(me, SFX_SMOKE_BOMB);
    SetNpcSpecPerforming(me, NSPEC_ABYSSFORCE, 6);

    PerformAbyssForceLoop();
  endif

  EraseInvulToAttack(me);

  EraseNpcSpecPerforming(me, NSPEC_ABYSSFORCE);
  DoNpcSpecDelay(me, NSPEC_ABYSSFORCE);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformAbyssForceLoop()
  
  var Dmg := 1;
  var Count := 0;
  var Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_ABYSSFORCE][5]);

  while(me and Mobs.size())
    
    PerformAction(me, 10);
    PlaySoundEffect(me, SFX_FOCUSATTACK);
    clear_event_queue();

    if(Modulo(Count, 2) == 0)
     
      PlayAbyssForceEffect(Mobs, Specs[NSPEC_ABYSSFORCE][5]);

      foreach Mob in Mobs
        DealAbyssForceDamage(Mob, Dmg, Dmg==1);
      endforeach

      Dmg += 2;
    endif

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_ABYSSFORCE))
      PerformNpcSpecialBreak();
      return;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_ABYSSFORCE))
      PerformNpcSpecialBreak();
      return;
    endif

    if(Modulo(Count, 2) == 0)
      Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_ABYSSFORCE][5]);
    endif
    
    Count += 1;
    HealMobile(me, 2, me);
    HealMana(me, 2, me);
    
  endwhile
  
endfunction

//-----------------------------------------------------------------------------

function DealAbyssForceDamage(Mob, Dmg, bForce := 1)

  var LastEffects := CInt(GetObjProperty(Mob, "#LastAbyss"));

  if(CanMod(Mob, MOD_LIGHT) != 0 or GetModAmount(Mob, MOD_LIGHT) != 25)
    UndoMod(Mob, MOD_LIGHT, UMDF_WAIT);
    DoMod(Mob, MOD_LIGHT, 25, 10);
  endif

  if(LastEffects < ReadGameClock() or bForce)
    TemporarySlowdown(Mob, 9);
    SetObjProperty(Mob, "#LastAbyss", ReadGameClock() + 10);
  endif

  DealDamage(Mob, Dmg, me);
  PlayPcEffect(Mob, FX_MARK_EFFECT, 1209, 16, 16, 4);
  PlaySoundEffect(Mob, SFX_FEINT);

endfunction

//-----------------------------------------------------------------------------

function PlayAbyssForceEffect(byref Mobs, Range)

  foreach Mob in Mobs
    PlayPcEffect(Mob, FX_EXPLODE_2, 1321, 24, 24, 4);
  endforeach

  var Locs := GetRandomLocsNear(me, Range);

  foreach Loc in Locs
    PlayPcStEffect(Loc.x, Loc.y, Loc.z, Loc.realm, FX_EXPLODE_2, 1321, 24, 24, 4);
  endforeach

endfunction

//-----------------------------------------------------------------------------
