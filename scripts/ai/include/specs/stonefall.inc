//-----------------------------------------------------------------------------

include "include/portals";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - czas trwania rzucania w sekundach
// 5 - zasieg specjala
// przyklad: StoneFall 90 50 10 9 50
//-----------------------------------------------------------------------------

function CheckStoneFall()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_STONEFALL][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_STONEFALL][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[StoneFall] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_STONEFALL][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_STONEFALL))
    return PerformStoneFall();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PlayRandomStoneFallPrepEffect()

  var Locs := GetRandomLocsNear(me, 15);

  foreach Loc in Locs
    PlayPcStEffect(Loc.x, Loc.y, Loc.z, Loc.realm, FX_SMOKE, 0, 0x10, 0x10);
  endforeach

  if(HaveChance(20))
    var Mobs := ListSpecialDamageMobs(me, 15);

    foreach Mob in Mobs
      if(HaveChance(50))
        PlayPcEffect(Mob, FX_EXPLODE_1, 0, 0x10, 0x10);
        PlaySoundEffect(Mob, SFX_FIRE1);
        DoMagicDamage(Mob, me, 15+RandomInt(10), 8, DMGTYPE_FIRE);
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function PerformStoneFallAnimation(Secs)

  var i;

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*zaczyna waliæ w ziemiê*"));
      4: SayUC(ToUnicode("*trzêsie ziemi¹*"));
    endcase

    PerformAction(me, ANIM_STUMBLE);
    PlaySoundEffect(me, SFX_SPELL_EARTHQUAKE);

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_STONEFALL))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_STONEFALL))
      PerformNpcSpecialBreak();
      return 0;
    endif

    PlayRandomStoneFallPrepEffect();
    
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformStoneFall()

  var lAttackDelayed := SetDelayedAttack(me, Specs[NSPEC_STONEFALL][4]);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[StoneFall] "+GetName(me)+": Perform with delay: "+Specs[NSPEC_STONEFALL][3]);
  endif

  var Secs := Specs[NSPEC_STONEFALL][4];

  SetNpcSpecPerforming(me, NSPEC_STONEFALL, 2);

  var Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_STONEFALL][5], LSDMF_ONLY_PLAYERS);

  foreach Mob in Mobs
    BlockPortals(Mob, CInt(Specs[NSPEC_STONEFALL][4]*2) );
  endforeach
  
  PlaySoundEffect(me, SFX_FIRE_PREPARE);

  if(PerformStoneFallAnimation(Secs))
    SayUC(ToUnicode("*trzesie lochem*"));

    PerformAction(me, 18);
    PlaySoundEffect(me, SFX_SPELL_EARTHQUAKE);
    PlaySoundEffect(me, SFX_FIRE1);
    PlaySoundEffect(me, SFX_FIRE_STRIKE);

    Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_STONEFALL][5]);

    PlayStoneFallEffect(Mobs, Specs[NSPEC_STONEFALL][5]);
  endif

  if(lAttackDelayed)
    EraseDelayedAttack(me);
  endif

  EraseNpcSpecPerforming(me, NSPEC_STONEFALL);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function PlayStoneFallEffect(byref Mobs, Range)

  foreach Mob in Mobs
    start_script(":specattack:stone_fall", array(me, Mob));
  endforeach

  var Locs := GetRandomLocsNear(me, Range);

  foreach Loc in Locs
    start_script(":specattack:stone_fall", array(me, Loc));
  endforeach

endfunction

//-----------------------------------------------------------------------------
