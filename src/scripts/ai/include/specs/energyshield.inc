//-----------------------------------------------------------------------------

include "include/portals";
include "include/mobile/npcs/attack";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - ilosc pkt obrazen jaka przyjmie zanim zdejmie tarcze

// przyklad: EnergyShield 60 50 10 300
//-----------------------------------------------------------------------------

function CheckEnergyShield()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_ENERGYSHIELD][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_ENERGYSHIELD][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[EnergyShield] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_ENERGYSHIELD][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_ENERGYSHIELD))
    return PerformEnergyShield();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformEnergyShield()

  var lSpec := Specs[NSPEC_ENERGYSHIELD];

  SetInvulToAttack(me);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[EnergyShield] "+GetName(me)+": Perform with delay: "+lSpec[3]);
  endif

  PlayPcEffect(me, FX_HEAL_EFFECT, 1321, 24, 24, 3);
  PlayPcEffect(me, FX_MARK_EFFECT, 1321, 16, 16, 3);
  PlaySoundEffect(me, SFX_ARCANECIRCLE);
  SayUC(ToUnicode("*energetyczna tarcza "+ggm(me, 16)+" chroni*"));

  SetNpcSpecPerforming(me, NSPEC_ENERGYSHIELD, lSpec[4]);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformEnergyShieldDowngrade(Npc)

  PlayPcEffect(Npc, FX_HEAL_EFFECT, 40, 24, 24, 3);
  PlaySoundEffect(Npc, SFX_LIGHTNING_ARROW);
        
endfunction

//-----------------------------------------------------------------------------

function DropEnergyShield(Npc)

  PlayPcEffect(Npc, FX_HEAL_EFFECT, 36, 24, 24, 3);
  PlayPcEffect(Npc, FX_MARK_EFFECT, 36, 16, 16, 3);
  PlaySoundEffect(Npc, SFX_ATTUNE);
  EraseInvulToAttack(Npc);
  UniPrintTextAbove(Npc, "*tarcza energetyczna rozpada siê*");

endfunction

//-----------------------------------------------------------------------------
