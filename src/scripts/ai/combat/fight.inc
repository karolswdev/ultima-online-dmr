//-----------------------------------------------------------------------------

include "include/poison";
include "include/mobile/misc";

include "ai/include/npccast";
include "ai/include/abilities";
include "ai/include/tamed";
include "ai/combat/aggro";

include ":magiceffects:summon";

//-----------------------------------------------------------------------------

const MAX_FIGHT_DIST         := 22;
const MAX_FLEE_DIST          := 21;

const SPIDER_WEB_DELAY          := 20;

//-----------------------------------------------------------------------------

var opponent := 0;
var sLoop    := 0;
var g_EndFightTime := 0;
var SpecFlag, Specs;

SetSpecs();

//-----------------------------------------------------------------------------

function Fight(opp)

  if(me.frozen)
    return;
  endif

  if(!opp)
    EndFight();
    return;
  elseif(opp == me or opp.dead or opp.hidden)
    EndFight();
    return;
  elseif(IsTamed(opp))
    if(opp.master == me)
      EndFight();
      return;
    endif
  elseif(opp.enabled("invul"))
    EndFight();
    return;
  endif

  SetAggro(opp);
  InformTamedAttack(opp);

  // dopiero tutaj sprawdzamy - inaczej majac GMa w zasiegu bedzie anulowal sobie mody, a przeciez walki nie rozpocznie.
  UnDoMod(me, MOD_MANA_REGEN);
  UnDoMod(me, MOD_HP_REGEN);

  if(!GetObjProperty(me, PROP_FIRST_ATTACKER))
    var attacker := opp;
    if(attacker.isa(POLCLASS_NPC))
      attacker := GetMaster(opp);
    endif
    if(IsPlayer(attacker))
      SetObjProperty(me, PROP_FIRST_ATTACKER, attacker.serial);
    endif
  endif

  if(GetPrevTamed(me) and IsPlayer(opp))
    EraseObjProperty(opp, PROP_LAST_TELE);
  endif

  if(IsWalking())
    SetWalkPause();
  endif
  
  if(!opponent and (!g_EndFightTime or (g_EndFightTime + 3 < ReadGameClock())))
    InitSpecs();
  endif

  ChangeOpponent(opp);
  prepare_for_fight();
  
  sLoop := 0;
  var HpPerc := GetHpPercent(me);
  var ManaPerc := GetManaPercent(me);

  while(me and opponent)
     if(me.frozen or opponent.dead or opponent.hidden or opponent.concealed or opponent.enabled("invul"))
       break;
     elseif(opponent == me)
       break;
     elseif(Distance(me,opponent) > MAX_FIGHT_DIST)
       break;
     endif

    process_combat_event();
    CheckAnchorDelay(HpPerc, ManaPerc);

    if(!opponent)
      break;
    endif

    if(!CloseDistance(opponent))
      waittime := 0;
    else
      waittime := 1;
      if(SpecFlag & NSPECF_KAMIKAZE)
        KamikazeExplode(me);
      endif
      if(!me.warmode or me.opponent != opponent)
        SetOpponent(opponent);
        SetWarMode(1);
      endif
    endif

    if(!ev)
      ev := wait_for_event(waittime);
    endif
    
    if(ev and IsSpecOn(NSPECF_DEBUG))
      syslog("NPC ["+hex(me.serial)+"] Event: "+hex(ev.type)+" Source: "+GetName(ev.source)+" Data: ["+ev+"]");
    endif

    case (ev.type)
        EVID_DAMAGED:         AggroNpcDamaged(me, ev.source, ev.damage);

                              if(ev.source and ev.source != opponent and ev.source != me)
                                if(ShouldChangeOpponent(ev.source))
                                  AggroNpcChanged(me);
                                  ChangeOpponent(ev.source);
                                endif
                              endif
                              
        EVID_ENGAGED:         AggroNpcDamaged(me, ev.source, 2); // nawet jak nie trafi to niech aggro wzrasta.

				/*

                              if(ev.source and ev.source != opponent and ev.source != me)
                                if(ShouldChangeOpponent(ev.source) or ev.others == "priormax")
                                  AggroNpcChanged(me);
                                  ChangeOpponent(ev.source);
                                endif
                              endif

*/
                              
        EVID_PEACEMADE:       MakePeace(ev.source,ev.others);
        /* TODO: goqsane - let's see if this makes sense
        EVID_FLEE:            RunAwayFeared(ev.source);
                              ChangeOpponent(0);
        */                    
        EVID_RUN:             if(ev.others == "priormax")
                                NpcMoveTo(ev, NMT_RUN);
                                if(ev)
                                  continue;
                                endif
                              endif
                              
        EVID_WALK:            if(ev.others == "priormax")
                                NpcMoveTo(ev, NMT_WALK);
                                if(ev)
                                  continue;
                                endif
                              elseif(ev.others == "Entice")
                                NpcMoveTo(ev, NMT_WALK);
                                EndFight();
                                DisableCombatEvents();
                                EnableMainEvents();
                                return;
                              endif

        EVID_FRIEND_HEAL:     AggroNpcOpponentHealed(me, ev.source, ev.healed);
                              CheckFriendHeal();

        EVID_REBORN:          Reborn();
                              
    endcase
    
    SeekNextTarget();
 
    if(IsSummoned(opponent))
      CheckAutoDispel();
    elseif(IsTamed(opponent))
      CheckProvokeTamed();
      CheckTamedMaster();
    endif

    CheckNpcSpecial();

    ev := 0;
  endwhile

  SeekNextTarget(1);

  EndFight();
  post_combat();
  
endfunction

//-----------------------------------------------------------------------------

function CheckAnchorDelay(byref HpPerc, byref ManaPerc)

  var CurHpPerc := GetHpPercent(me);
  var CurManaPerc := GetManaPercent(me);

  if(HpPerc - CurHpPerc >= 0.2 or ManaPerc - CurManaPerc >= 0.1)
    
    /*if(!is_anchor_delayed()) bedzie jednak bez tekstow, nie ma co spamowac
      var Name := GetName(me.opponent);

      if(Name)
        SayUC(ToUnicode("*zapamiêtuje "+Name+"*"));
      else
        SayUC(ToUnicode("*zapamiêtuje wroga*"));
      endif
    endif*/

    delay_anchor(3600);
    HpPerc := CurHpPerc;
    ManaPerc := CurManaPerc;
  endif

endfunction

//-----------------------------------------------------------------------------

function ShouldChangeOpponent(newOpp)

  if(!opponent)
    return 1;
  endif

  var Chance := GetAggroChangeChance();
  
  if(!(AiFlags & AIF_TMP_LOS))
    Chance := CInt(Chance * 2);
  endif

  if(Distance(me, opponent) > Distance(me, newOpp)) // wrog jest oddalony, a nowy cel jest blizej.
    Chance := CInt(Chance * 2);
  endif

  if(opponent.isa(POLCLASS_NPC) and IsPlayer(newOpp))
    Chance := CInt(Chance * 2);
  elseif(newOpp.isa(POLCLASS_NPC) and IsPlayer(opponent))
    Chance := CInt(Chance * 0.4);
  endif

  return HaveChance(Chance);

endfunction

//-----------------------------------------------------------------------------

function EndFight()

  if(opponent != 0 or me.warmode)
    SetOpponent(0);
    SetWarmode(0);
    AggroNpcReset();

    // byla walka? to ustaw odpal regenearcje
    DoMod(me, MOD_MANA_REGEN, IDLE_REGEN_RATE, 600);
    DoMod(me, MOD_HP_REGEN,   IDLE_REGEN_RATE, 600);
    EraseObjProperty(me, PROP_FIRST_ATTACKER);
  endif

  opponent := 0;
  ev       := 0;
  waittime := DEF_WAITTIME;
  g_EndFightTime := ReadGameClock();

endfunction

//-----------------------------------------------------------------------------

function CanFightEntered(who)

  if(!who)
    return 0;
  endif
  
  //aby uwolnione moby nie atakowaly ludzi, ktorzy przeszli przed chwila przez portal - aby gracze nie robili pulapek
  if(GetPrevTamed(me))
    if(ReadGameClock() < GetObjProperty(who,PROP_LAST_TELE) + 15)
      return 0;
    endif
  endif

  if(IsPeaceTime())
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SeekNewPlayerTarget()

  foreach Mob in ListMobilesInLineOfSight(me, Specs[NSPEC_THRESHOLD])
    if(Mob != opponent and IsPlayer(Mob) and !Mob.enabled("invul") and !Mob.dead)
      if(CanFightWith(Mob))
        if(CoordLosAt(me, Mob))
          UnHide(Mob);
          ChangeOpponent(Mob);
          break;
        endif
      endif
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function SeekNextTarget(Force := 0)

  if(AiFlags & (AIF_TMP_LOS | AIF_TMP_SPECLOS))
    sLoop := 0;
  else
    sLoop := sLoop + 1;
  endif

  if(sLoop > 60 or Force)
    if(Modulo(sLoop, 10) == 0 or Force)
      foreach Mob in ListMobilesInLineOfSight(me, Specs[NSPEC_THRESHOLD])
        if(Mob != opponent)
          if(CanFightWith(Mob))
            if(CoordLosAt(me,Mob))
              ChangeOpponent(Mob);
              sLoop := sLoop - 20;
              break;
            endif
          endif
        endif
      endforeach
    endif

    if(sLoop > 120)
      RunAwayFeared(opponent);
      EndFight();
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function ChangeOpponent(opp)

  if(opp)
    if(opponent != opp and !opp.enabled("invul") and opp != me)
      SetAggro(opp);
      InitRun(opponent);
      opponent := opp;
      SetOpponent(opponent);
      SetWarMode(1);
      TurnToward(opponent);
      InformTamedAttack(opponent);
    endif
  else
    EndFight();
  endif

endfunction

//-----------------------------------------------------------------------------

function InformTamedAttack(opponent)

  foreach Mob in ListMobilesNearLocationEx(me.x, me.y, LIST_IGNORE_Z, 15, LISTEX_FLAG_HIDDEN | LISTEX_FLAG_NORMAL, me.realm)
    if(IsTamed(Mob) and GetMaster(Mob) == me)
      SendChEvent(Mob, opponent, EVID_ENGAGED, "priormax");
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function EnableCombatEvents()
  EnableEvents(EVID_ENGAGED + EVID_DAMAGED);
  DisableEvents(EVID_SPEECH);
endfunction

//-----------------------------------------------------------------------------

function DisableCombatEvents()
  DisableEvents(EVID_ENGAGED + EVID_DAMAGED);
endfunction

//-----------------------------------------------------------------------------

function Flee()

  sleep(1);
  return;

  if((AiFlags & AIF_IMMOBILE) or me.frozen)
    sleep(1);
    return;
  else
    var runs  := 100;

    while(Distance(me, opponent) < MAX_FLEE_DIST and runs > 0)
      ev := wait_for_event( !WalkAwayFrom(opponent) );

      if(process_flee_event())
        break;
      endif

      runs := runs - 1;

      case (ev.type)
        EVID_DAMAGED:    if(ev.source)
                           if(GetBaseSkill(ev.source, SKILLID_ARCHERY) > 30)
                             if(RandomInt(100) < 20)
                               if((ev.source.npctemplate and CheckLineOfSight(me, ev.source)) or (Distance(me, ev.source) < Distance(me, opponent)) or (!CheckLineOfSight(me, opponent)) or !opponent or (RandomInt(3)==1))
                                 ChangeOpponent(ev.source);
                               endif
                             endif
                           else
                             if((ev.source.npctemplate and CheckLineOfSight(me, ev.source)) or (Distance(me, ev.source) < Distance(me, opponent)) or (!CheckLineOfSight(me, opponent)) or !opponent or (RandomInt(3)==1))
                               ChangeOpponent(ev.source);
                             endif
                           endif
                         endif

        EVID_PEACEMADE:  runs := 0;

        EVID_RUN:        if(ev.others == "priormax")
                           NpcMoveTo(ev, NMT_RUN);
                           runs := 0;
                         endif

        EVID_WALK:       if(ev.others == "priormax")
                           NpcMoveTo(ev, NMT_WALK);
                           runs := 0;
                         endif
      endcase
    endwhile

    if(Distance(me, opponent) >= MAX_FLEE_DIST)
      EndFight();
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckAutoDispel()

  if(SpecFlag & NSPECF_AUTODISPEL)
    if(Specs[NSPEC_AUTODISPEL][3] < ReadGameClock())
      if(Specs[NSPEC_AUTODISPEL][1] > RandomInt(100))
        CastSpell(me,opponent,MDISPEL_SPID);
      endif
      Specs[NSPEC_AUTODISPEL][3] := ReadGameClock() + Specs[NSPEC_AUTODISPEL][2];
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckProvokeTamed()

  if(SpecFlag & NSPECF_PROVOKETAMED)

    if(!GetNpcConfig(opponent).Mechanic)
      if(Specs[NSPEC_PROVOKETAMED][3] < ReadGameClock())
        if(Specs[NSPEC_PROVOKETAMED][1] * GetProvokeChanceMod() > RandomInt(100))
          ProvokeTamed();
        endif
        Specs[NSPEC_PROVOKETAMED][3] := ReadGameClock() + Specs[NSPEC_PROVOKETAMED][2];
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckTamedMaster()
  
  var HpPerc := 1.0 + 0.5 * (1.0 - GetHpPercent(me));
  var Chance := CInt(Min(100, 4.0 + Max(0, CDbl(GetMaxHp(me) / 20) - 20) * HpPerc));

  if(Chance > RandomInt(1000))
    var Master := GetMaster(opponent);
    
    if(Distance(me, Master) < 16 and !Master.dead and !Master.cmdlevel and (IsLoggedIn(Master) or !IsFullyLogout(Master)) )
      var Sight := GetNpcConfig(me).SpecialSight;
      if(Sight)
        if(Master.hidden)
          Unhide(Master);
        endif
      endif

      SetAggroTable(Master, 1+Sight);
      Fight(Master);
    else
      SeekNewPlayerTarget();
    endif
  elseif(Chance*0.5 > RandomInt(1000))
    SeekNewPlayerTarget();
  endif

endfunction

//-----------------------------------------------------------------------------

function ProvokeTamed()

  if(Specs[NSPEC_PROVOKETAMED][4][1] != "all")
    if( !(opponent.npctemplate in Specs[NSPEC_PROVOKETAMED][4]) )
      return 0;
    endif
  endif
  
  var ProvokedMaster   := GetMaster(opponent);
  var ProvokedMob      := opponent;
  var ProvokedOpp      := struct;
  ProvokedOpp.+Players := array;
  ProvokedOpp.+Tamed   := array;

  // zmieniamy opponenta na mastera
  ChangeOpponent(ProvokedMaster);

  // teraz zwierzaka sprowokowanego odoswajamy i zmieniamy mu target
  PrintTextAbove(ProvokedMob, "*argh*", FONT_NORMAL, COLOR_RED);
  UnTame(ProvokedMob);
  Sleepms(500);
  
  clear_event_queue();

  // teraz albo zwierzak zaatakuje kogo popadnie, albo ucieknie

  foreach Mob in ListMobilesNearLocation(me.x, me.y, me.z, Specs[NSPEC_THRESHOLD], me.realm)
    if(CanFightWith(Mob) and Mob != ProvokedMaster)
      if(IsPlayer(Mob))
        ProvokedOpp.Players.append(Mob);
      else
        ProvokedOpp.Tamed.append(Mob);
      endif
    endif
  endforeach

  if(ProvokedOpp.Players.size())
    ProvokedOpp := ProvokedOpp.Players[RandomInt(ProvokedOpp.Players.size()) + 1];
  elseif(ProvokedOpp.Tamed.size())
    ProvokedOpp := ProvokedOpp.Tamed[RandomInt(ProvokedOpp.Tamed.size()) + 1];
  else
    ProvokedOpp := ProvokedMaster;
  endif

  if(!ProvokedOpp)
    ProvokedOpp := ProvokedMaster;
  endif

  SendChEvent(ProvokedMob, ProvokedOpp, EVID_ENGAGED);

  if(RandomInt(100) > 50)

    SendChEvent(ProvokedMob, ProvokedOpp, EVID_FLEE);

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetProvokeChanceMod()

  return Max(0, (1 - (GetBaseSpecialAbility(opponent, NPC_ABLID_ENDURANCE) * 0.0012)));

endfunction

//-----------------------------------------------------------------------------

function CheckHpDrainSkill()

  if(Specs[NSPEC_HPDRAIN][4] < ReadGameClock())
    if(GetHp(me) < GetMaxHp(me))
      Specs[NSPEC_HPDRAIN][4] := ReadGameClock() + Specs[NSPEC_HPDRAIN][2];
      foreach Mob in ListMobilesNearLocation(me.x,me.y,me.z,Specs[NSPEC_HPDRAIN][3],me.realm)
        if(CanFightWith(Mob))
          PlayPcEffect(Mob, FX_CURSE_EFFECT, 31, 0xF,0xF);
          PlaySoundEffect(Mob,SFX_SPELL_CURSE);
          SendChEvent(Mob,me,EVID_ENGAGED);
          DealDamage(Mob,Specs[NSPEC_HPDRAIN][1],me);
          HealMobile(me,Specs[NSPEC_HPDRAIN][1], me);
        endif
      endforeach
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckPlagueSkill()

  if(Specs[NSPEC_PLAGUE][4] < ReadGameClock())
    Specs[NSPEC_PLAGUE][4] := ReadGameClock() + Specs[NSPEC_PLAGUE][2];

    foreach Mob in ListMobilesNearLocation(me.x,me.y,me.z,Specs[NSPEC_PLAGUE][3],me.realm)
      if(CanFightWith(Mob))
        PlayObjectCenteredEffect(Mob,FX_CURSE_EFFECT,0xF,0xF);
        PlaySoundEffect(Mob,SFX_SPELL_CURSE);
        SendChEvent(Mob,me,EVID_ENGAGED);
        SetPoison(Mob,Specs[NSPEC_PLAGUE][1],me);
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckAutoSpell()

  if(GetObjProperty(me,"AutoSpellCharges") == error)

    SetObjProperty(me,"AutoSpellCharges", CInt(GetNpcConfig(me).AutoSpellCharges));

  endif

  if(GetObjProperty(me,"AutoSpellCharges") > 0)
    if(Specs[NSPEC_AUTOSPELL][3] < ReadGameClock())
      Specs[NSPEC_AUTOSPELL][3] := ReadGameClock() + Specs[NSPEC_AUTOSPELL][2];
      SetObjProperty(me, "AutoSpellCharges", GetObjProperty(me,"AutoSpellCharges") - 1);
      CastSpell(me, opponent, Specs[NSPEC_AUTOSPELL][1], SPF_NOMANA);
    endif
  endif
  
endfunction

//-----------------------------------------------------------------------------

function DispelAllNear()

  if(Specs[NSPEC_DISPELALL] + 5 < ReadGameClock())
    foreach Mob in ListMobilesNearLocation(me.x, me.y, me.z, 2, me.realm)
      if(IsSummoned(Mob))
        if(CanFightWith(Mob))
          KillSummonedCreature(Mob);
        endif
      endif
    endforeach

    foreach Item in ListItemsNearLocation(me.x, me.y, me.z, 2, me.realm)
      if(GetObjProperty(Item, PROP_SUMMONED_FIELD))
        PlayStationaryEff( item.x, item.y, item.z, FX_SMOKE, 0xa, 0xa, 0, item.realm);
        DestroyItem(Item);
      endif
    endforeach

    Specs[NSPEC_DISPELALL] := ReadGameClock();
  endif

endfunction

//-----------------------------------------------------------------------------

function MakeAcidPool()

  if(Distance(me,opponent) > 1)
    if(Specs[NSPEC_ACIDPOOL][2] <= 0)
      Specs[NSPEC_ACIDPOOL][2] := CreateItemAtLoc(me.x, me.y, me.z, UOBJ_ACID_POOL, 1, me.realm);
      SetObjProperty(Specs[NSPEC_ACIDPOOL][2], "AcidDmg", Specs[NSPEC_ACIDPOOL][1]);
      Specs[NSPEC_ACIDPOOL][2] := 5;
    endif
    
    if(!Specs[NSPEC_ACIDPOOL][2])
      Specs[NSPEC_ACIDPOOL][2] := 5;
    endif
    
    Specs[NSPEC_ACIDPOOL][2] := Specs[NSPEC_ACIDPOOL][2] - 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function MakeSpiderwebPool()

  if(Distance(me,opponent) > 1 && CheckLineOfSight(me,opponent))
    if(( Specs[NSPEC_SPIDERWEBPOOL][2] < ReadGameClock() ) and ( 20 >= RandomInt(100)+1 )) // 20% jest spoko 
      Specs[NSPEC_SPIDERWEBPOOL][2] := ReadGameClock() + Specs[NSPEC_SPIDERWEBPOOL][1];
      PrintTextAbove(me, "*wystrzeliwuje siec*");
      PlayPcMovEffect(me, opponent, FX_FIREBALL, 1000, 5, 1, 1, 0, 3);
      CreateItemAtLoc(opponent.x, opponent.y, opponent.z, UOBJ_SPIDERWEB_POOL, 1, opponent.realm);

      if(checkSpiderWebDelay(opponent))
        SetObjProperty(opponent, "#SpiderWebDelay", ReadGameClock() + SPIDER_WEB_DELAY);
        TemporarySlowdown(opponent, 1);
      endif
    endif     
  endif

endfunction

//-----------------------------------------------------------------------------

function checkSpiderWebDelay(who)
  var delay := GetObjProperty(who, "#SpiderWebDelay");

  if(delay != error)
    if(CInt(delay) > ReadGameClock())
      return 0;
    endif

    EraseObjProperty(who, "#SpiderWebDelay");
  endif 
  return 1;
endfunction

//-----------------------------------------------------------------------------

function WhirlwindAttack()

  if(Specs[NSPEC_WHIRLWIND] > RandomInt(1000))
    PlayPcEffect(me, FX_HEAL_EFFECT, 1106, 0x30, 0x5A);
    var Dmg := RandomDiceRoll("4d4");
    Dmg := CInt((Dmg * GetHp(me)) / GetMaxHp(me));

    foreach Mob in ListMobilesNearLocation(me.x, me.y, me.z, 6, me.realm)
      if(CanFightWith(Mob))
        PlayPcEffect(opponent, FX_HEAL_EFFECT, 1106, 0x30, 0x5A);
        PlaySoundEffect(opponent, 0x527);
        if(IsTamed(opponent))
          DealManaDamage(opponent, Dmg*4, me);
          DealStaminaDamage(opponent, Dmg*2, me);
        else
          DealManaDamage(opponent, Dmg, me);
          DealStaminaDamage(opponent, Dmg, me);
        endif
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function ConcussionBlow()

  if(Specs[NSPEC_CONCUSSION] > RandomInt(500))
    if(Distance(me,opponent) <= 1 and DistZ(me, opponent) < 15)
      var Dmg   := CInt(GetDiceMaxValue(GetNpcConfig(me).AttackDamage) * 0.35);
      var HPerc := MinMax(CInt((GetHp(opponent) * 100) / GetMaxHp(opponent)), 20, 80);
    
      Dmg := CInt(Dmg*0.25) + CInt((Dmg*0.75) * ((100 - HPerc) * 0.01));

      PlaySoundEffect(opponent, SFX_SPELL_MIND_BLAST);
      PlayPcEffect(opponent, FX_GLOW, 997, 0x5, 0xF, 2);
      DealDamage(opponent,Dmg,me);
    endif
  endif
  
endfunction

//-----------------------------------------------------------------------------

function CheckAutoSummon()

  if(CInt(GetObjProperty(me,"#AutoSummonTime")) < ReadGameClock())
    var Parms := array;
    Parms.+controlable := 1;
    Parms.+nosleep     := 1;
    for i := 1 to Specs[12][2]
      SummonCreature(me, Specs[12][1], opponent, Specs[12][3], Parms);
    endfor
    SetObjProperty(me,"#AutoSummonTime",ReadGameClock() + Specs[12][3]);
  endif
  
endfunction

//-----------------------------------------------------------------------------

function FlameBreathAttack()

  if(Specs[NSPEC_FLAMEBREATH][2] < ReadGameClock())
    if(RandomInt(6) < 2)
      CastFlameBreath(me, opponent, Specs[NSPEC_FLAMEBREATH][1]);
    endif
    Specs[NSPEC_FLAMEBREATH][2] := Cint(ReadGameClock() + (RandomInt(4) + 4));
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckAutoHealing()

  if(Specs[NSPEC_AUTOHEALING][4] < ReadGameClock())
    if(RandomInt(100) < Specs[NSPEC_AUTOHEALING][3])
      HealMobile(me, CInt(GetMaxHp(me) * Specs[NSPEC_AUTOHEALING][1]), me);
    endif
    Specs[NSPEC_AUTOHEALING][4] := ReadGameClock() + Specs[NSPEC_AUTOHEALING][2] + RandomInt(4) - RandomInt(4);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckFriendHeal()

  if(GetNpcConfig(me).SpecialSight)
    if(ev.source != opponent and ev.source)
      if(ShouldChangeOpponent(ev.source))
        AggroNpcChanged(me);
        ChangeOpponent(ev.source);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function Reborn()

  if(GetObjProperty(me, "#Reborning") or GetObjProperty(me, "Reborned"))
    return;
  endif

  SetObjProperty(me, "#Reborning", 1);

  PerformAction(me, ANIM_NPC_LIEDOWN);
  sleep(1);
  PerformAction(me, ANIM_NPC_LIEDOWN);
  sleep(1);
  PerformAction(me, ANIM_ATTACK_2HAND_DOWN);
  PlayPcEffect(me, FX_FLAMESTRIKE, 0, 24, 24);
  PlayPcStEffect(me.x-1, me.y-1, me.z, me.realm, FX_FLAMESTRIKE, 0, 0x10, 0x10);
  PlayPcStEffect(me.x+1, me.y+1, me.z, me.realm, FX_FLAMESTRIKE, 0, 0x10, 0x10);
  PlayPcStEffect(me.x-1, me.y+1, me.z, me.realm, FX_FLAMESTRIKE, 0, 0x10, 0x10);
  PlayPcStEffect(me.x+1, me.y-1, me.z, me.realm, FX_FLAMESTRIKE, 0, 0x10, 0x10);
  UniPrintTextAbove(me, "*odradza sie*");
  PlayPcEffect(me, FX_HEAL_EFFECT, 44, 0x20, 0x20, 2);
  SetHp(me, GetMaxHp(me));
  SetMana(me, GetMaxMana(me));
  SetStamina(me, GetMaxStamina(me));
  
  EraseObjProperty(me, "#Reborning");
  EraseDelayedAttack(me);
  SetObjProperty(me, "Reborned", ReadGameClock());

endfunction

//-----------------------------------------------------------------------------
