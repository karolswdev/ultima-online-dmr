//-----------------------------------------------------------------------------

include "include/portals";

//-----------------------------------------------------------------------------
// Dane w Data: 
// 1 - odstep czasu pomiedzy specjalem
// 2 - od jakiego % hp zaczyna rzucac
// 3 - szansa na rzucenie w loopie wyrazona w procentach
// 4 - czas trwania rzucania w sekundach
// 5 - zasieg specjala - czyli zasieg w jakim zostana stworzone moby
// 6 - grupa npecow ze :spawnnet:groups lub konkretny npctemplate
// 7 - ilosc npecow do stworzenia
// 8 - typ przywolania, portal/random
// przyklad: SummonMinions 90 40 9 3 15 31 15 random
// przyklad: SummonMinions 90 40 9 3 10 ushionidoppel 5 hidden
//-----------------------------------------------------------------------------

function CheckSummonMinions()

  var HpPerc := GetHpPercent(me);

  if(HpPerc > Specs[NSPEC_SUMMONMINIONS][2])
    return 0;
  endif

  var Chance := Specs[NSPEC_SUMMONMINIONS][3] + RandomInt(CInt((50-HpPerc*100)/3));

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[SummonMinions] "+GetName(me)+": Chance: "+Chance+"% Hp: "+CInt(HpPerc*100)+"% Limit: "+CInt(Specs[NSPEC_SUMMONMINIONS][2]*100)+"%");
  endif

  if(!HaveChance(Chance))
    return 0;
  endif

  if(CheckNpcSpecDelay(NSPEC_SUMMONMINIONS))
    return PerformSummonMinions();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function PerformSummonAnimation(Secs)

  var i;
  PlaySoundEffect(me, SFX_MEDITATION);

  for(i:=1;i<=Secs;i+=1)
    case(i)
      1: if(Specs[NSPEC_SUMMONMINIONS][8] == "hidden")
           SayUC(ToUnicode("*wzywa posi³ki*"));
         else
           SayUC(ToUnicode("*przywo³uje s³ugusów*"));
         endif
    endcase

    PerformAction(me, ANIM_CAST_NPC);
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_SUMMONMINIONS))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
    sleepms(500);
    
    if(!IsNpcSpecPerforming(me, NSPEC_SUMMONMINIONS))
      PerformNpcSpecialBreak();
      return 0;
    endif
    
  endfor

  PerformAction(me, ANIM_CAST_AREA);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function PerformSummonMinions()

  var lSpec := Specs[NSPEC_SUMMONMINIONS];
  var lAttackDelayed := SetDelayedAttack(me, lSpec[4]);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("[SummonMinions] "+GetName(me)+": Perform with delay: "+lSpec[3]);
  endif

  var Secs := lSpec[4];

  SetNpcSpecPerforming(me, NSPEC_SUMMONMINIONS, 2);

  if(PerformSummonAnimation(Secs))

    case(lSpec[8])
    
      "random":
        var Mobs := ListSpecialDamageMobs(me, lSpec[5]);

        if(Mobs.size())
          SayUC(ToUnicode("*wskazuje na pomioty*"));
        endif

        var Locs := GetRandomLocsNear(me, lSpec[5], lSpec[7]);
        
        foreach Loc in Locs
          var Mob := CreateSummonedMinion(me, Loc, Mobs, lSpec);
          
          if(Mob and IsSpecOn(NSPECF_DEBUG))
            syslog("[SummonMinions] "+GetName(me)+": Summoned: "+GetName(Mob));
          elseif(!Mob)
            LogError("[SummonMinions] "+me.npctemplate+": Blad: "+Mob.errortext);
          endif    
        endforeach
        
      "hidden":
        var Mobs := ListSpecialDamageMobs(me, lSpec[5]);
        var Locs := GetRandomLocsNear(me, lSpec[5], lSpec[7]+1);

        var PrimLoc := Locs[1];
        PlayPcEffect(me, FX_FLAMESTRIKE, 999, 0x20, 0x20, 2);
        foreach Mob in ListHostiles(me)
          Mob.SetWarMode(0);
        endforeach
        me.concealed := 1;
        sleepms(1200);
        Locs.erase(1);
        MoveObject(me, PrimLoc.x, PrimLoc.y, PrimLoc.z, PrimLoc.realm, MOVEOBJECT_FORCELOCATION);
        
        foreach Loc in Locs
          var Mob := CreateSummonedMinion(me, Loc, Mobs, lSpec);
          
          if(Mob and IsSpecOn(NSPECF_DEBUG))
            syslog("[SummonMinions] "+GetName(me)+": Summoned: "+GetName(Mob));
          elseif(!Mob)
            LogError("[SummonMinions] "+me.npctemplate+": Blad: "+Mob.errortext);
          endif    

          SetHp(Mob, CInt(GetMaxHp(Mob) * GetHpPercent(me)));
        endforeach

        me.concealed := 0;
        PlayPcEffect(me, FX_SMOKE, 0, 0x10, 0x10, 1);
        
      "portal":
        var Mobs := ListSpecialDamageMobs(me, lSpec[5]);
        
        start_script(":specattack:summon_portal", array(me, Mobs, lSpec));
        
        if(Mobs.size())
          SayUC(ToUnicode("*wskazuje na pomioty*"));
        endif
        
        SayUC(ToUnicode("*przygl¹da siê*"));
        sleep(1);
        clear_event_queue();
        var MaxSecWait := ReadGameClock() + 10 + lSpec[7]*6;
        var StartClock := ReadGameClock();
        
        while(me and ReadGameClock() < MaxSecWait)
          ev := wait_for_event(1);
          
          if(ev)
            if(ev.type == EVID_ENGAGED or ev.type == EVID_DAMAGED)
              if(StartClock+3 < ReadGameClock()) // pierwsze 3 sekundy poczeka.
                break;
              endif
            endif
          endif
          
          HealMobile(me, 2, me);
          HealMana(me, 2, me);
        endwhile
    endcase

  endif

  if(lAttackDelayed)
    EraseDelayedAttack(me);
  endif

  EraseNpcSpecPerforming(me, NSPEC_SUMMONMINIONS);
  DoNpcSpecDelay(me, NSPEC_SUMMONMINIONS);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CreateSummonedMinion(me, Loc, byref Mobs, byref Spec)

  var Template := GetNpcTemplateFromGroup(Spec[6]);
  
  if(!Template)
    return Template;
  endif
  
  var Parms := array;
  Parms.+controlable := 1;
  Parms.+nosleep     := 1;
  Parms.+canloot     := 1;

  var Mob := SummonCreature(me, Template, Loc, 1800, Parms);
  
  PlaySoundEffect(Mob, SFX_SPELL_SUMMON_CREATURE);

  PlayPcEffect(Mob, FX_SMOKE, 0, 0x10, 0x10, 1);
  
  if(Mobs.size())
    var Enemy := Mobs[RandomInt(Mobs.size())+1];
    SendChEvent(Mob, Enemy, EVID_ENGAGED);
  endif
  
  return Mob;
  
endfunction

//-----------------------------------------------------------------------------
