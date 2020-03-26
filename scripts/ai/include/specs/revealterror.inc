//-----------------------------------------------------------------------------

include "include/portals";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - czas trwania rzucania w sekundach
// 5 - zasieg specjala
// przyklad: RevealTerror 120 50 8 6 50
//-----------------------------------------------------------------------------

function CheckRevealTerror()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_REVEALTERROR][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_REVEALTERROR][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[RevealTerror] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_REVEALTERROR][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_REVEALTERROR))
    return PerformRevealTerror();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformRevealTerrorAnimation(Secs, byref Mobs)

  var i;
  var Level := 6;

  foreach Mob in Mobs
    UndoMod(Mob, MOD_LIGHT);
    DoMod(Mob, MOD_LIGHT, Level, 10, UMDF_NOCHECK);
  endforeach

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*loch ogarnia terror*"));
    endcase

    if(Modulo(i, 2) == 0)
      PerformAction(me, 10);
      PlaySoundEffect(me, SFX_FOCUSATTACK);
    else
      PerformAction(me, 12);
      PlaySoundEffect(me, SFX_FEINT);
    endif

    sleepms(800);
    
    foreach Mob in Mobs
      Mob.setlightlevel(Level, 10);
    endforeach

    Level += 2;
    PlayTerrorSound();
    
    if(!IsNpcSpecPerforming(me, NSPEC_REVEALTERROR))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(700);
    
    foreach Mob in Mobs
      Mob.setlightlevel(Level, 10);
    endforeach

    Level += 2;
    PlayTerrorSound();

    if(!IsNpcSpecPerforming(me, NSPEC_REVEALTERROR))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformRevealTerror()

  var lAttackDelayed := SetDelayedAttack(me, Specs[NSPEC_REVEALTERROR][4]);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[RevealTerror] "+GetName(me)+": Perform with delay: "+Specs[NSPEC_REVEALTERROR][3]);
  endif

  var Secs := Specs[NSPEC_REVEALTERROR][4];

  SetNpcSpecPerforming(me, NSPEC_REVEALTERROR, 150);

  var Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_REVEALTERROR][5], LSDMF_ONLY_PLAYERS);

  foreach Mob in Mobs
    BlockPortals(Mob, CInt(Specs[NSPEC_REVEALTERROR][4]*1.5) );
  endforeach

  if(PerformRevealTerrorAnimation(Secs, Mobs))
  
    SayUC(ToUnicode("Poznajcie terror! Hahah!"));
    
    PlayTerrorSound();
    CreateTerrorArea();
    PlayTerrorSound();

    foreach Mob in Mobs
      Mob.setlightlevel(1, 1);
      PlayTerrorSound(Mob);
    endforeach

    PerformAction(me, 18);

  endif

  if(lAttackDelayed)
    EraseDelayedAttack(me);
  endif

  EraseNpcSpecPerforming(me, NSPEC_REVEALTERROR);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function PlayTerrorSound(Mob := 0)

  if(!Mob)
    Mob := me;
  endif

  case(RandomInt(9))
    1:  PlaySoundEffect(Mob, 0x4B3);
    2: PlaySoundEffect(Mob, 0x4DF);
    3: PlaySoundEffect(Mob, 0x182);
    4: PlaySoundEffect(Mob, 0x1CF);
    5: PlaySoundEffect(Mob, 0x1DB);
    6: PlaySoundEffect(Mob, 0x25B);
    7: PlaySoundEffect(Mob, 0x25C);
    8: PlaySoundEffect(Mob, 0x284);
    default: PlaySoundEffect(Mob, 0x282);
  endcase

endfunction

//-----------------------------------------------------------------------------

function GetTerrorObjType()

  case(RandomInt(11))
    1: return 0x1ce6;
    2: return 0x1ceb;
    3: return 0x1ce1;
    4: return 0x1cf2;
    5: return 0x1D0a;
    6: return 0x1d10;
    7: return 0x1d11;
    8: return 0x1cf9;
    9: return 0x1d0d;
    default: return 0x1ce3;
  endcase

endfunction

//-----------------------------------------------------------------------------

function CreateTerrorArea()

  var Locs := GetRandomLocsNear(me, Specs[NSPEC_REVEALTERROR][5], Specs[NSPEC_REVEALTERROR][5]*4);
  var Items := array;
  
  foreach Loc in Locs
    var Item := PullTempItemAtLoc(Loc.x, Loc.y, Loc.z, GetTerrorObjType(), Loc.realm);

    if(Item)
      SetName(Item, "resztki");
    endif

    if(Item and HaveChance(15))
    
      SetObjProperty(Item, "SpawnBaseGroup", 59);
      SetObjProperty(Item, "SpawnText", "*wype³za*");

      if(!start_script(":specattack:spawnitem_control", Item))
        LogError("[CheckNpcSpecialAttack]: Nie udalo sie uruchomic skryptu: :specattack:spawnitem_control!");
      endif

    endif
    
    PlayPcStEffect(Loc.x, Loc.y, Loc.z, Loc.realm, FX_FLAMESTRIKE, 999, 30, 30, 2);
    Items.append(Item);
  endforeach
  
  ReturnTempItemDelayed(Items, 120);

endfunction

//-----------------------------------------------------------------------------
