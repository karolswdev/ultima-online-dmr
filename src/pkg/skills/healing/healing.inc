//-----------------------------------------------------------------------------

include "include/packets";
include "include/attack";
include "include/resurrect";
include "../pkg/systems/vampire/common";

//-----------------------------------------------------------------------------

const RES_ANIMAL_DELAY       := 43200;
const MIN_RES_ANIMAL_DELAY   := 21600;
const RES_ANIMAL_INGREDIENT  := 0xdc0a;

const RES_LOOSEAMT_ABILITY   := 5; // w dziesietnych
const RES_LOOSEAMT_SKILL     := 1;

const RES_MAX_AMOUNT         := 10;

//-----------------------------------------------------------------------------

function IsHealing(who,patient,msg := "leczyc",msg2 := "osobe")

  if(!GetObjProperty(who,"#HealTimer") or GetObjProperty(who,"#HealTimer") != Clock)
    if(patient == who)
      SendSysMessage(who,"Przestal"+ggm(who,3)+" sie "+msg+".",FONT_NORMAL,COLOR_RED);
    else
      SendSysMessage(who,"Przestal"+ggm(who,3)+" "+msg+" "+msg2+" "+patient.name+".",FONT_NORMAL,COLOR_RED);
      SendSysMessage(patient,who.name + " przestal Cie "+msg+".",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetHealCycles(who,patient)

  var healing;
  
  if(IsPlayer(patient))
    healing := GetBaseSkillBaseValue(who,SKILLID_HEALING);
  else
    healing := GetBaseSkillBaseValue(who,SKILLID_VETERINARY);
  endif
  
  var Check   := 0;
  var cycles  := 2;
  var plvl    := GetPoisonLevel(patient);

  if(patient == who)
    check := Cint(GetBaseDexterity(who) / 1.5) + Cint(healing / 24);
  elseif(IsPlayer(patient))
    check := Cint(GetBaseDexterity(who)) + Cint(healing / 16);
  else
    check := Cint(GetBaseDexterityBaseValue(who) * 1.75);
  endif
  
  if(patient == who or IsPlayer(patient))
    cycles := Max(160 - check, 20);
  else
    check := CDbl(MinMax(check, 500, 900) - 500.0) / 400.0;
    cycles := 80 - CInt(45.0 * check);  // pomiedzy 35 a 80.
  endif

  if(plvl)
    cycles := cycles + CInt(plvl * 4);
  endif

  //Wydluzenie leczenia u wampira w dzien (tylko w strefie palenia slonca)
  var VampEffSkill;
  var hour := GetHour();
  if(IsVampire(who) and hour < VAMPIRE_HOUR and hour >= VAMPIRE_END_HOUR and IsInVampBurningArea(who))
    VampEffSkill := CInt(GetEffectiveSkillBaseValue(who, SKILLID_VAMPIRE));
    if(VampEffSkill < 1050)
      var Diff := 50 - Max(VampEffSkill - 1000, 0);
      cycles := cycles + Diff;
    endif
  endif
  return cycles;

endfunction

//-----------------------------------------------------------------------------

function CanResurrectNpc(who, Corpse)
  
  var NpcTemp  := GetObjProperty(Corpse, PROP_NPCTEMP);
  var cfgElem := GetNpcConfig(NpcTemp);

  if(!Corpse.isa(POLCLASS_CORPSE))
    SendSysMessage(who,"To nie zwloki!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

/*
  if(Corpse.item_count > 1)
    SendSysMessage(who,"Oproznij plecak zwierzecia przed wskrzeszeniem.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  */

  if(GetObjProperty(Corpse,"cut"))
    SendSysMessage(who,"Cialo juz sie nie nadaje...",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(GetObjProperty(Corpse,"NpcMount") or IsQuestItem(Corpse))
    SendSysMessage(who,"Dusza odmawia powrotu!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(GetLastHitter(Corpse) == who)
    if(cfgElem.Mechanic)
      SendSysMessage(who,"Czesci nie nadaja sie do zlozenia.",FONT_NORMAL,COLOR_RED);
    else
      SendSysMessage(who,"Dusza odmawia powrotu do zlego mistrza!",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  var Master := FindPlayerBySerial(GetObjProperty(Corpse, PROP_MASTER));

  if(!Master)
    if(cfgElem.Mechanic)
      SendSysMessage(who,"Czesci nie nadaja sie do zlozenia.", FONT_NORMAL, COLOR_RED);
    else
      SendSysMessage(who,"Dusza odmawia powrotu!",FONT_NORMAL,COLOR_RED);
    endif
    return 0;
  endif

  if(who != Master and !IsFriend(Master,who) and Master.acctname != who.acctname and !IsGM(who))
    SendSysMessage(who,"Ta dusza postrzega Cie jako wroga. Nie wroci do ciebie!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  
  if(!cfgElem.Mechanic and CInt(GetObjProperty(Corpse, "ResurrectCount")) >= RES_MAX_AMOUNT)
    SendBad(who, "Dusza tego zwierzecia juz nie wroci, zbyt czesto wracala w ostatnich chwilach.");
    return 0;
  endif
  
  /*
  if(!cfgElem.Mechanic and RES_ANIMAL_INGREDIENT)
    var Item := FindItemInContainer(who.backpack, RES_ANIMAL_INGREDIENT);
    
    if(!Item)
      SendBad(who, "Nie masz przy sobie potrzebnego skladnika "+GetDescForItem(RES_ANIMAL_INGREDIENT)+"!");
      return 0;
    endif
  endif
  */
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function ResurrectNpc(who, Corpse, DelayMod := 1)

  var NpcTemp := GetObjProperty(Corpse, PROP_NPCTEMP);

  if(!NpcTemp or !who)
    return 0;
  endif
  
  var cfgElem := GetNpcConfig(NpcTemp);
  
  if(!cfgElem.Mechanic and RES_ANIMAL_INGREDIENT)
    /*
    var Item := FindItemInContainer(who.backpack, RES_ANIMAL_INGREDIENT);
    
    if(!Item)
      SendBad(who, "Nie masz przy sobie potrzebnego skladnika "+GetDescForItem(RES_ANIMAL_INGREDIENT)+"!");
      return 0;
    endif
    
    if(!SubtractAmount(Item, 1))
      SendBad(who, "Nie masz przy sobie potrzebnego skladnika "+GetDescForItem(RES_ANIMAL_INGREDIENT)+"!");
      return 0;
    endif
	*/
  endif

  var x,y,z,realm, cont;
  cont := Corpse.container;

  if(cont)
    cont  := GetTopCont(cont);

    // GetTopCont zbiera najwyzszy kontener, wiec nie dojdzie do playera/npeca.
    if(cont.container and cont.container.isa(POLCLASS_MOBILE))
      cont := cont.container;
    endif

    // musi byc mobile, inaczej bedzie kopa (stworzy gdzies 0x0 - 200x200 npeca na danym realmie)
    if(!cont.isa(POLCLASS_MOBILE))
      cont := who;
    endif

  else
    cont := Corpse;

    if(!cont.isa(POLCLASS_CORPSE)) // dla pewnosci, czy na pewno zwloki ? jak nie to pozycja playera.
      cont := who;
    endif
  endif

  x     := cont.x;
  y     := cont.y;
  z     := cont.z;
  realm := cont.realm;
  
  var NewNpc := CreateNpc(NpcTemp,x,y,z,realm,0,NPCF_NO_UPKEEP | NPCF_NO_LOOT);

  if(!NewNpc)
    NewNpc := CreateNpc(NpcTemp,who.x,who.y,who.z,who.realm,0,NPCF_NO_UPKEEP | NPCF_NO_LOOT);
    if(!NewNpc)
      if(cfgElem.Mechanic)
        SendSysMessage(who,"Czesci wydaja sie byc zepsute...", FONT_NORMAL, COLOR_RED);
      else
        SendSysMessage(who,"Cos przeszkadza w przywroceniu do zycia tego zwierzecia.",FONT_NORMAL,COLOR_RED);
      endif
      return 0;
    endif
  endif

  var resCount := CInt(GetObjProperty(Corpse, "ResurrectCount"));
  var resTime := CInt(GetObjProperty(Corpse,"RessurectTime"));
  
  var NecroTime := GetNecroControlledTime(Corpse);
  
  if(NecroTime)
    SetNecroControl(NewNpc, who, NecroTime);
  endif
    
  LoadNpcAttrData(NewNpc, Corpse);
  SetHp(NewNpc, 1);
  SetMana(NewNpc,1);
  SetStamina(NewNpc,1);
  
  var Name := GetName(Corpse);
  Name["Zwloki "] := "";
  SetName(NewNpc, Name);

  if(!DestroyItem(Corpse))
    if(cfgElem.Mechanic)
      SendSysMessage(who,"Czesci wydaja sie byc zepsute...", FONT_NORMAL, COLOR_RED);
    else
      SendSysMessage(who,"Cos przeszkadza w przywroceniu do zycia tego zwierzecia.",FONT_NORMAL,COLOR_RED);
    endif
    KillNPC(NewNpc);
    return 0;
  endif

  var Delay := (RES_ANIMAL_DELAY * DelayMod);

  if(Delay < MIN_RES_ANIMAL_DELAY)
    Delay := MIN_RES_ANIMAL_DELAY;
  endif

  // ustawiamy ilosc wskrzeszen, jesli jest wieksza rowna dwa, znaczy ze mozna znow resnac wiec mozemy ustawic 1
  if( resTime < ReadGameClock() )
    resCount := 1;
  else
    resCount += 1;
  endif
  SetObjProperty(NewNpc, "ResurrectCount",resCount);
  
  // sprawdzamy, czy nie trzeba owalic npeca za kolejne wskrzeszenie
  if(!cfgElem.Mechanic)

    if(ResCount >= 2)
      LooseNpcExperience(NewNpc);
      SendSysMessage(who,"To zwierze umarlo ponownie i utracilo troche wiedzy na temat swoich umiejetnosci.",FONT_NORMAL,COLOR_ORANGE);
    endif

  endif

  //zwiekszamy delay
  SetObjProperty(NewNpc,"RessurectTime",ReadGameClock() + Delay);
  PlaySoundEffect(who, SFX_SPELL_MANA_VAMPIRE);
  sleepms(100);
  PlayPcEffect(NewNpc, FX_SMOKE, 1000, 0xf, 0xf, 3);

  return NewNpc;

endfunction

//-----------------------------------------------------------------------------

function LooseNpcExperience(NewNpc)

  var CfgElem := GetNpcConfig(NewNpc);
  
  for i := 0 to SKILLID__HIGHEST
    var AttrId := GetAttributeIdBySkillId(i);
    var BaseVal := CInt(GetConfigString(CfgElem, AttrId))*10;
    var Curval := GetBaseSkillBaseValue(NewNpc, i);
    
    if(CurVal - RES_LOOSEAMT_SKILL >= BaseVal)
      SetBaseSkillBaseValue(NewNpc, i, CurVal - RES_LOOSEAMT_SKILL);
    endif
  endfor
  
  for i := 1 to NPC_ABL_MAXID 
    var Curval := GetBaseSpecialAbility(NewNpc, i);
    if(CurVal > 0)
      SetBaseSpecialAbility(NewNpc, i, Max(CurVal - RES_LOOSEAMT_ABILITY, 0));
    endif
  endfor

endfunction

//-----------------------------------------------------------------------------

function ResurrectPc(who, patient, SkillId := error)

  if(who == patient)
    return;
  endif

  var Healing := GetEffectiveSkill(who, SKILLID_HEALING);
  var Anatomy := GetEffectiveSkill(who, SKILLID_ANATOMY);

  while(counter <= 10)
    PlayPcEffect(patient, FX_HEAL_EFFECT, 90, 0x1f, 0x1f, 4);
    PlaySoundEffect(patient, SFX_SPELL_CUNNING);
    sleep(1);

    if(Distance(who, patient) > 1)
      SendSysMessage(who,"Ta osoba jest za daleko.",FONT_NORMAL,COLOR_RED);
      SendSysMessage(patient, who.name + " przerwal" + ggm(who, 4) + " probe wskrzeszenia.",FONT_NORMAL,COLOR_GREEN);
      break;
    endif

    if(!IsHealing(who,patient,"wskrzeszac") or !patient.dead)
      return;
    endif

    if(counter >= 10)
      if(SkillId == error)
        if(GetBaseSkill(who,SKILLID_ANATOMY) >= 81 and GetBaseSkill(who,SKILLID_HEALING) >= 81)
          if(CheckSkill(who, SKILLID_HEALING, 85, 0) and RandomInt(100) < CInt((Healing + Anatomy) * 0.75))
            SkillId := 1;
          endif
        endif
      else
        SkillId := CheckMoreSkill(who, SkillId, 85, 0);
      endif

      if(SkillId)
        if(Resurrection(patient, 1))
          AwardPoints(who, SKILLID_HEALING, 100);
          PlayPcEffect(patient, FX_SMOKE, 1000, 0xf, 0xe, 3);
          SendSysMessage(who, "Wskrzesil" + ggm(who, 3) + " osobe " + patient.name+".",FONT_NORMAL,COLOR_BLUE);
          return 1;
        endif
      endif
    endif
    counter := counter + 1;
  endwhile

  SendSysMessage(who, "Nie udalo Ci sie wskrzesic tej osoby.",FONT_NORMAL,COLOR_RED);
  return 0;

endfunction

//-----------------------------------------------------------------------------

function UseBandage(who, objtype, targSer := 0)
  
  if(!(objtype in {UOBJ_BLOODYBANDAGES, UOBJ_BANDAGE}))
    return;
  endif

  var bandage := FindItemInContainer(who.backpack, objtype);

  if(!bandage)
    var Bloody := "";
    if (objtype == UOBJ_BLOODYBANDAGES)
      Bloody := "zakrwawionych ";
    endif
    SendSysMessage(who, "Nie masz " + Bloody + "bandazy.", FONT_NORMAL, COLOR_RED);
    return;
  endif

  targSer := CInt(targSer);

  if(targSer)
    SetObjProperty(who, "#HealingTarget", targSer);
  else
    EraseObjProperty(who, "#HealingTarget");
  endif

  if(!UseItem(bandage, who))
    EraseObjProperty(who, "#HealingTarget");
  endif

endfunction

//-----------------------------------------------------------------------------
