//-----------------------------------------------------------------------------

include "include/portals";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - czas trwania rzucania w sekundach
// 5 - zasieg specjala
// 6 - na co rzuca? all - staty/witale/skille
// 7 - o ile obniza (staty*3, witale*2, skille*1)
// 8 - czas trwania klatwy
// przyklad: SpecCurse 60 50 10 10 50 all 5 45
//-----------------------------------------------------------------------------

function CheckSpecCurse()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_CURSE][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_CURSE][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[SpecCurse] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_CURSE][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_CURSE))
    return PerformSpecCurse();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformSpecCurseAnimation(Secs)

  var i;

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*zbiera si³y*"));
      4: SayUC(ToUnicode("*ogarnia go mroczna si³a*"));
    endcase

    if(Modulo(i, 2) == 0)
      PerformAction(me, 10);
      PlaySoundEffect(me, SFX_PUNCH);
    else
      PerformAction(me, 12);
      PlaySoundEffect(me, SFX_FEINT);
    endif

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_CURSE))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_CURSE))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformSpecCurse()

  var lAttackDelayed := SetDelayedAttack(me, Specs[NSPEC_CURSE][4]);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[SpecCurse] "+GetName(me)+": Perform with delay: "+Specs[NSPEC_CURSE][3]);
  endif

  var Secs := Specs[NSPEC_CURSE][4];

  SetNpcSpecPerforming(me, NSPEC_CURSE, 150);

  var Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_CURSE][5], LSDMF_ONLY_PLAYERS);

  foreach Mob in Mobs
    BlockPortals(Mob, CInt(Specs[NSPEC_CURSE][4]*1.5) );
  endforeach

  if(PerformSpecCurseAnimation(Secs))
    SayUC(ToUnicode("*rzuca mroczn¹ kl¹twê*"));

    PerformAction(me, 18);
    PlaySoundEffect(me, SFX_SMOKE_BOMB);
    PlaySoundEffect(me, SFX_SPELL_MASS_CURSE);

    Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_CURSE][5]);

    DealSpecCurse(Mobs);
    PlaySpecCurseEffect(Specs[NSPEC_CURSE][5]);
  endif

  if(lAttackDelayed)
    EraseDelayedAttack(me);
  endif

  EraseNpcSpecPerforming(me, NSPEC_CURSE);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function DealSpecCurse(byref Mobs)

  foreach Mob in Mobs
  
    if(IsSpecOn(NSPECF_DEBUG))
      syslog("[SpecCurse] "+GetName(me)+": Perform on mobile: "+GetName(Mob));
    endif
    
    //var Type := Specs[NSPEC_CURSE][6];
    var ModAmt := 0 - Specs[NSPEC_CURSE][7];
    var ModDur := Specs[NSPEC_CURSE][8] + RandomInt(CInt(Specs[NSPEC_CURSE][8]/2));

    // na razie jest tylko all, wiec nie robie na darmo warunkow. 
    // jak bedzie trzeba to sie zrobi, ale miejsce na typ jest jakby co.
    // if(Type == "all")
    UndoMod(Mob, MOD_STR, UMDF_NOCHECK);
    UndoMod(Mob, MOD_DEX, UMDF_NOCHECK);
    UndoMod(Mob, MOD_INT, UMDF_NOCHECK);
    UndoMod(Mob, MOD_LIFE, UMDF_NOCHECK);
    UndoMod(Mob, MOD_MANA, UMDF_NOCHECK);
    UndoMod(Mob, MOD_STAMINA, UMDF_NOCHECK);
    
    for SkID:=0 to SKILLID__HIGHEST
      if(CanSkillMod(Mob, SkID) == 0)
        UndoSkillMod(Mob, SkID);
      endif
    endfor

    DoMod(Mob, MOD_STR, ModAmt*3, modDur);
    DoMod(Mob, MOD_DEX, ModAmt*3, modDur);
    DoMod(Mob, MOD_INT, ModAmt*3, modDur);
    DoMod(Mob, MOD_LIFE, ModAmt*2, modDur);
    DoMod(Mob, MOD_MANA, ModAmt*2, modDur);
    DoMod(Mob, MOD_STAMINA, ModAmt*2, modDur);
    
    for SkID:=0 to SKILLID__HIGHEST
      if(GetBaseSkill(Mob, SkID) >= 10)
        DoSkillMod(Mob, SkID, ModAmt, ModDur);
      endif
    endfor

    PlayPcEffect(Mob, FX_CURSE_EFFECT, 999, 16, 16, 3);
    PlaySoundEffect(Mob, SFX_SPELL_MASS_CURSE);
    PlaySoundEffect(Mob, SFX_SPELL_CURSE);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function PlaySpecCurseEffect(Range)

  var Locs := GetRandomLocsNear(me, Range);

  foreach Loc in Locs
    PlayPcStEffect(Loc.x, Loc.y, Loc.z, Loc.realm, FX_SMOKE, 999, 16, 16, 3);
  endforeach

endfunction

//-----------------------------------------------------------------------------
