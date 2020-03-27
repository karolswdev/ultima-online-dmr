//-----------------------------------------------------------------------------

include "include/portals";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - czas specjala
// 5 - do ilu % regeneruje sie
// przyklad: Regeneration 60 50 10 4 80
//-----------------------------------------------------------------------------

function CheckSpecRegeneration()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_REGENERATION][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_REGENERATION][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[SpecRegeneration] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_REGENERATION][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_REGENERATION))
    return DoSpecRegeneration();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformSpecRegenerationAnimation(Secs)

  var i;

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*skupia sie*"));
    endcase

    PlaySoundEffect(me, 0x516);
    PerformAction(me, 10);

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_REGENERATION))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_REGENERATION))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DoSpecRegeneration()

  SetInvulToAttack(me);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[SpecRegeneration] "+GetName(me)+": Perform with delay: "+Specs[NSPEC_REGENERATION][3]);
  endif

  var Secs := Specs[NSPEC_REGENERATION][4];

  SetNpcSpecPerforming(me, NSPEC_REGENERATION, 6);

  if(PerformSpecRegenerationAnimation(Secs))
    SetNpcSpecPerforming(me, NSPEC_REGENERATION, 15);
    SayUC(ToUnicode("*zaczyna regeneracje*"));

    PerformSpecRegeneration();
  endif

  EraseDelayedAttack(me);

  EraseNpcSpecPerforming(me, NSPEC_REGENERATION);
  DoNpcSpecDelay(me, NSPEC_REGENERATION);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function BreakRegen(byref CoverItem)

  if(CoverItem)
    ReturnTempItem(CoverItem);
    PlayPcStEffect(me.x, me.y, me.z, me.realm, FX_SMOKE, 1421, 14, 14, 2);
  endif

  me.concealed := 0;

endfunction

//-----------------------------------------------------------------------------

function PerformSpecRegeneration()

  var Mobs := ListSpecialDamageMobs(me, 30);
  var MaxHpPerc := Specs[NSPEC_REGENERATION][5];
  var Count := 0;
  var CoverItem := 0;
  
  if(Specs[NSPEC_REGENERATION][6])
    me.concealed := 1;
    CoverItem := PullTempItemAtLoc(me.x, me.y, me.z, Specs[NSPEC_REGENERATION][6], me.realm);
    SetObjProperty(CoverItem, "SpecialNpc", me.serial);
    PlayPcStEffect(me.x, me.y, me.z, me.realm, FX_SMOKE, 1421, 14, 14, 2);
  endif

  while(me and Mobs.size())
    
    PlaySoundEffect(me, 0x516);
    if(!me.concealed)
      PerformAction(me, 16);
    endif
    clear_event_queue();

    PlaySpecRegenerationEffect();
    DealSpecRegenerationDamage();

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_REGENERATION))
      BreakRegen(CoverItem);
      PerformNpcSpecialBreak();
      return;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_REGENERATION))
      BreakRegen(CoverItem);
      PerformNpcSpecialBreak();
      return;
    endif

    HealMobile(me, 42+RandomInt(25), me);
    HealMana(me, 42+RandomInt(25), me);

    if(MaxHpPerc < GetHpPercent(me))
      break;
    endif

    if(Modulo(Count, 2) == 0)
      Mobs := ListSpecialDamageMobs(me, 30);
    endif

    Count += 1;
    
  endwhile
  
  BreakRegen(CoverItem);

endfunction

//-----------------------------------------------------------------------------

function PlaySpecRegenerationEffect()
  
  if(!me.concealed)
    PlayPcEffect(me, FX_SMOKE, 1421, 14, 14, 2);
  endif
  PlayPcStEffect(me.x-1, me.y-1, me.z, me.realm, FX_FLAMESTRIKE, 1421, 30, 30, 2);
  PlayPcStEffect(me.x-1, me.y+1, me.z, me.realm, FX_FLAMESTRIKE, 1421, 30, 30, 2);
  PlayPcStEffect(me.x+1, me.y+1, me.z, me.realm, FX_FLAMESTRIKE, 1421, 30, 30, 2);
  PlayPcStEffect(me.x+1, me.y-1, me.z, me.realm, FX_FLAMESTRIKE, 1421, 30, 30, 2);

endfunction

//-----------------------------------------------------------------------------

function DealSpecRegenerationDamage()

  foreach Mob in ListMobilesNearLocation(me.x, me.y, me.z, 1, me.realm)
    if(IsPlayer(Mob) or IsPlayerTamed(Mob))
      DealDamage(Mob, RandomInt(15)+20);
      PlayPcEffect(Mob, FX_SMOKE, 1421, 28, 28, 2);
      PlaySoundEffect(Mob, SFX_SMOKE_BOMB);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
