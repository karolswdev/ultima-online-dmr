//-----------------------------------------------------------------------------

include "include/portals";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - czas trwania rzucania w sekundach
// 5 - wielkosc obra¿en, a wlasciwie taki wspolczynnik sily moda
// przyklad: BladeDance 60 50 10 8 20
//-----------------------------------------------------------------------------

function CheckBladeDance()

  var ManaPerc := GetManaPercent(me);

  if(ManaPerc > Specs[NSPEC_BLADEDANCE][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_BLADEDANCE][3] + RandomInt(CInt((50-ManaPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[BladeDance] "+GetName(me)+": Chance: "+Chance+"% Mana: "+CInt(ManaPerc*100)+"% Limit: "+CInt(Specs[NSPEC_BLADEDANCE][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_BLADEDANCE))
    return PerformBladeDance();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformBladeDanceAnimation(Secs)

  var i;

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*potêguje w sobie gniew*"));
      4: SayUC(ToUnicode("*ogarnia go sza³*"));
    endcase

    if(Modulo(i, 2) == 0)
      PerformAction(me, 10);
      PlaySoundEffect(me, SFX_FOCUSATTACK);
    else
      PerformAction(me, 12);
      PlaySoundEffect(me, SFX_FEINT);
    endif

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_BLADEDANCE))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_BLADEDANCE))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformBladeDance()

  var lAttackDelayed := SetDelayedAttack(me, Specs[NSPEC_BLADEDANCE][4]);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[BladeDance] "+GetName(me)+": Perform with delay: "+Specs[NSPEC_BLADEDANCE][3]);
  endif

  var Secs := Specs[NSPEC_BLADEDANCE][4];

  SetNpcSpecPerforming(me, NSPEC_BLADEDANCE, 3);

  var Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_BLADEDANCE][5], LSDMF_ONLY_PLAYERS);

  foreach Mob in Mobs
    BlockPortals(Mob, CInt(Specs[NSPEC_BLADEDANCE][4]*1.5) );
  endforeach

  if(PerformBladeDanceAnimation(Secs))
    SayUC(ToUnicode("*poznaj moj¹ z³oœæ*"));

    PerformAction(me, 18);
    PlaySoundEffect(me, SFX_FIRE2);
    PlaySoundEffect(me, SFX_WHIRLWIND);
    
    ApplyBladeDanceMods();
  endif

  if(lAttackDelayed)
    EraseDelayedAttack(me);
  endif

  EraseNpcSpecPerforming(me, NSPEC_BLADEDANCE);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function ApplyBladeDanceMods()

  var Mod := Specs[NSPEC_BLADEDANCE][5];

  DoMod(me, MOD_FIRE_DMG, Mod+RandomInt(Max(1, CInt(Mod/2))), 30);
  DoMod(me, MOD_POISON_DMG, CInt(Mod*0.4), 30);
  DoMod(me, MOD_MANA_STOLEN, Mod*2+RandomInt(CInt(Mod/2)), 30);
  DoMod(me, MOD_MANA_REGEN, IDLE_REGEN_RATE, 30);

  PlayPcEffect(me, FX_EXPLODE_3, 1244, 16, 16, 4);

endfunction

//-----------------------------------------------------------------------------
