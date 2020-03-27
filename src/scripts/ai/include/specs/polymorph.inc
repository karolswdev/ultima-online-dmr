//-----------------------------------------------------------------------------

include "include/portals";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - czas trwania rzucania w sekundach
// 5 - typ obrazen specjala
// 6 - template w jaki sie zmienia
// 7 - poziom obrazen
// przyklad: Polymorph 120 50 10 10 fire fireelemental 30
//-----------------------------------------------------------------------------

function CheckPolymorph()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_POLYMORPH][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_POLYMORPH][3] + RandomInt(CInt((50-HpPerc*100)/5));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[Polymorph] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_POLYMORPH][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_POLYMORPH))
    return PerformPolymorph();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformPolymorphAnimation(Secs)

  var i;

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: SayUC(ToUnicode("*przechodz¹ "+ggm(me,16)+" ciarki*"));
      4: SayUC(ToUnicode("*zaczyna zmieniaæ kszta³t*"));
    endcase

    if(Modulo(i, 2) == 0)
      PerformAction(me, ANIM_CAST_NPC);
      if(Specs[NSPEC_POLYMORPH][5] == "fire")
        PlaySoundEffect(me, SFX_FIRE_PREPARE);  
      endif
    endif

    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_POLYMORPH))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_POLYMORPH))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformPolymorph()

  var lAttackDelayed := SetDelayedAttack(me, Specs[NSPEC_POLYMORPH][4]);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[Polymorph] "+GetName(me)+": Perform with delay: "+Specs[NSPEC_POLYMORPH][3]);
  endif

  var Secs := Specs[NSPEC_POLYMORPH][4];

  SetNpcSpecPerforming(me, NSPEC_POLYMORPH, 150, Specs[NSPEC_POLYMORPH][5]);

  var Mobs := ListSpecialDamageMobs(me, Specs[NSPEC_POLYMORPH][5], LSDMF_ONLY_PLAYERS);

  foreach Mob in Mobs
    BlockPortals(Mob, CInt(Specs[NSPEC_POLYMORPH][4]*1.5) );
  endforeach

  if(PerformPolymorphAnimation(Secs))
    PerformAction(me, 18);
    PlayPcEffect(me, FX_SMOKE, 0, 0x10, 0x10);
    PlaySoundEffect(me, SFX_SPELL_POLYMORPH);

    if(Specs[NSPEC_POLYMORPH][5] == "fire")
      var Elem := GetNpcConfig(Specs[NSPEC_POLYMORPH][6]);
      DoMod(me, MOD_FIRE_DMG, Specs[NSPEC_POLYMORPH][7], 60); 
      DoMod(me, MOD_RUN_SPEED, RUNSPEED_FAST, 60); 

      if(Elem)
        DoMod(me, MOD_GRAPHIC, Elem.ObjType, 60); 

        if(Elem.Color)
          DoMod(me, MOD_COLOR, Elem.Color, 60); 
        endif
      endif
    endif

    Mobs := ListSpecialDamageMobs(me, 2);
    DealPolymorphDamage(Mobs, Specs[NSPEC_POLYMORPH][5]);
  endif

  if(lAttackDelayed)
    EraseDelayedAttack(me);
  endif

  EraseNpcSpecPerforming(me, NSPEC_POLYMORPH);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function DealPolymorphDamage(byref Mobs, Type)

  foreach Mob in Mobs
  
    if(IsSpecOn(NSPECF_DEBUG))
      syslog("[Polymorph] "+GetName(me)+": Perform on mobile: "+GetName(Mob));
    endif
    
    var DmgPerc := Specs[NSPEC_POLYMORPH][7];

    if(Type == "fire")
      DoMagicDamage(Mob, me, CInt(DmgPerc/2)+RandomInt(DmgPerc), 8, DMGTYPE_FIRE);
      PlayPcEffect(Mob, FX_FLAMESTRIKE, 0, 0x10, 0x10);
      PlaySoundEffect(Mob, SFX_FIRE2);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
