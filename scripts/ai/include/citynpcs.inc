//-----------------------------------------------------------------------------

include "ai/include/ainpcs";
include "include/packets";
include "ai/include/npch";
include ":banking:gold";
include "include/logerror";
include "include/player/young";
include "include/mobile/npcs/city_funcs";
include ":miracleworker:miracles";
include ":mq_npcs:common";

//-----------------------------------------------------------------------------

const CITYNPC_MAXTRSK        := 250;
const CITYNPC_TEACHAMT       := 100;

const CNTIF_DONT_TAKEGOLD    := 0x01;

//-----------------------------------------------------------------------------

function CheckNpcTakeItem(me, who, Item, Flags := 0)

  var cfgElem := GetNpcConfig(me);
  var Data := struct;
  Data.+npc := me;
  Data.+source := who;
  Data.+param := Item;

  if(run_script(":lostitems:process_item", Data) == 1)
    return 1;
  endif

  if(me.npctemplate == "miracleworker")

    if(run_script(":miracleworker:process_item", Data))
      return 1;
    endif

  elseif(me.npctemplate == "royalmerchant")

    if(run_script(":donations:process_item", Data))
      return 1;
    endif

  elseif(cfgElem.CanGiveCraftQuest)
    
    var Res := run_script(":craftquests:process_item", Data);
    
    if(Res)
      if(Res == 2)
         start_script(":craftquests:process_completion", Data);
      endif
      return 1;
    endif

  endif

  if(Item.objtype == UOBJ_GOLDCOIN and !IsBitOn(Flags, CNTIF_DONT_TAKEGOLD))
    if(RandomInt(100) < (who.gold / 100))
      case(RandomInt(3))
        0:       Say("Hm.. zlotem nie pogardze.");
        1:       Say("Dziekuje, zlota nigdy za malo !");
        default: Say("Wielkie dzieki, zawsze sie przyda troche zlota!");
      endcase
      DestroyItem(Item);
      return 1;
    elseif(who.gold < 1000)
      case(RandomInt(3))
        1: Say("Wygladasz na biednego, nie chce Ci zabierac reszty pieniedzy.");
        default: Say("Nie trzeba, tobie sie bardziej przydadza niz mi.");
      endcase
    endif
    MoveToPack(Item,who);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function NpcTakeItem(me, who, Item)

  if(!CheckNpcTakeItem(me, who, Item))
    MoveToPack(Item, who);
  endif

endfunction

//-----------------------------------------------------------------------------

function CanTrade(who)

  if(!City)
    return 1;
  endif

  CheckPetInside(who);
  foreach Npc in ListMobilesNearLocation(who.x,who.y,who.z,3,who.realm)
    if(IsTamed(Npc))
      CheckPetInside(Npc);
    endif
  endforeach

  var Vamp := IsVampireCity(City) and (GetEffectiveSkill(who,SKILLID_VAMPIRE) >= 40);
  var Wolf := IsWearwolfCity(City) and (GetEffectiveSkill(who,SKILLID_WEARWOLF) >= 40);

  case(CityAl)
    CITYAL_EVIL: if(!IsRealMurderer(who))
                   Say("Wynos sie stad!");
                   return 0;
                 endif
                 
    CITYAL_GOOD: if(!Vamp and !Wolf)
                   if(IsRealMurderer(who))
                     Say("Wynos sie z tego miasta!");
                     return 0;
                   endif
                 endif
  endcase

  if(IsGuardChasing(who))
    Say("Wynos sie stad.");
    return 0;
  endif

  if(IsNaked(who))
    Say("Won stad z tym golym tylkiem!");
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CityNpcLookAround()

  foreach Mob in ListMobilesNearLocationEx(me.x,me.y,me.z,5,LISTEX_FLAG_HIDDEN + LISTEX_FLAG_NORMAL,me.realm)
    CheckPetInside(Mob);

    if(CityAl == CITYAL_GOOD)
      if(CInt(GetObjProperty(me,"#NextMurdYell")) < ReadGameClock())
        if(Mob.murderer and IsPlayer(Mob))
          if(GetBaseSkill(Mob, SKILLID_VAMPIRE) >= 40)
            if(IsVampireCity(City))
              continue;
            endif
          endif
          
          if(GetBaseSkill(Mob, SKILLID_WEARWOLF) >= 40)
            if(IsWearwolfCity(City))
              continue;
            endif
          endif

          if(!InformGuards(Mob,"Straz! Tu jest morderca!", GMR_MURD))
            Say("Wynos sie stad natychmiast!");
          else
            break;
          endif

          SetObjProperty(me,"#NextMurdYell",ReadGameClock() + MURD_YELL_TIME);
        endif
      endif
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function CheckPetInside(Mob)

  if(Mob.cmdlevel or !City)
    return;
  endif

  if(GetEquipmentByLayer(Mob,LAYER_MOUNT) or IsTamed(Mob))
    if(IsInBuilding(Mob.x,Mob.y,Mob.realm))
      if(IsTamed(Mob))
        if(Distance(GetMaster(Mob), me) < 15)
          Mob := GetMaster(Mob);
        endif
      endif
      
      if(Mob.cmdlevel)
        return;
      endif

      if(GetObjProperty(Mob, "#InformedBy") == me.serial and CInt(GetObjProperty(Mob, "#InformedTime")) < ReadGameClock())
        if(!InformGuards(Mob, "Straz! Brac tego z tym brudnym zwierzem!", GMR_ANIMAL_INB))
          Say("Wynos sie z tym zwierzem bo pozalujesz!");
        endif

        if(CityAl == CITYAL_NEUTRAL)
          SetGuardChase(Mob, GDCF_ALLCITIES);
        else
          SetGuardChase(Mob);
        endif
      else
        Say(GetName(Mob) + " zabieraj tego brudnego zwierza stad bo wezwe straze!");
        SetObjProperty(Mob, "#InformedBy", me.serial);
        SetObjProperty(Mob, "#InformedTime", ReadGameClock() + 6);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CityNpcAttack(who)

  if(IsWalking())
    SetWalkPause();
  endif

  if(!InformGuards(who, "Straz! Pomocy!", GMR_NPCATTACKED))
    Say("Wynos sie stad! Natychmiast!");
  endif

  SetGuardChase(who);

endfunction

//-----------------------------------------------------------------------------

function CityNpcPush(who)

  if(PushTimer + 5 < ReadGameClock())
    PushTimer := ReadGameClock();

    if(IsSetFlag(me, NPCF_NO_WANDER))
      case(RandomInt(5))
        1: Say("Gdzie to sie pchasz na chama ?");
        3: Say("Przestan sie na mnie pchac bo zawolam straze!");
      endcase
    else
  
      if(MoveIfBlocking(who))
        Say("Nie musisz sie tak rozpychac..");
      else
        TurnToward(who);

        case(RandomInt(5))
          1:       Say("Czemu sie tak pchasz?"); 
          2:       Say("Malo miejsca masz?");
          3:       Say("Czy Ci w czyms przeszkadzam?");
          default: Say("Nie rozpychaj sie tak.");
        endcase
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function DefWander()

  CheckBegPurse();

  if(!IsWalking() or IsWalkPause())
    DoWander();
    check_anchor();

    var Data := struct;
    Data.+npc := me;
    Data.+source := 0;

    if(CanNpcHaveMiniQuest(me))
      start_script(":mq_npcs:check_mq", Data);
    endif
  endif

  NextWander := ReadGameClock() + WANDER_TIME + RandomInt(CInt(WANDER_TIME / 3));

endfunction

//-----------------------------------------------------------------------------

function InformGuards(who, Text, Gmr := 0)

  var Inf := 0;

  if(!who)
    return 0;
  endif

  if(Gmr)
    Gmr := {"Gmr", Gmr};
  endif

  foreach Npc in ListMobilesNearLocation(who.x,who.y,who.z,16,who.realm)
    if(Npc.isa(POLCLASS_NPC))
      if(IsTownGuard(Npc))
        SendChEvent(Npc,who,EVID_ENGAGED,Gmr);
        Inf := 1;
      endif
    endif
  endforeach

  if(!Inf)
    var GmrTxt;
    case(Gmr[2])
      GMR_CHASING:        GmrTxt := "Scigany przez innego straznika";
      GMR_ANIMAL_INB:     GmrTxt := "Zwierzak w budynku";
      GMR_NOT_MURD:       GmrTxt := "Nie morderca!?";
      GMR_TO_WEAK_MURD:   GmrTxt := "Za slaby morderca!?";
      GMR_MURD:           GmrTxt := "Morderca";
      GMR_CRIMINAL:       GmrTxt := "Kryminalista";
      GMR_STEALING:       GmrTxt := "Kradziez";
      GMR_ATTACKED:       GmrTxt := "Atak na jakiegos Npca";
      GMR_PLAYER_ATTACK:  GmrTxt := "Atak na gracza";
      GMR_INMEMORY:       GmrTxt := "Pamietany";
      
      GMR_CITYCHASING:    var GChase := GetObjProperty(who, PROP_GUARD_CHASE);
                          if(GChase.Flags & GDCF_ALLCITIES)
                            GmrTxt := "Scigany we wszystkich miastach";
                          else
                            GmrTxt := "Scigany w "+City;
                          endif

      GMR_NPCATTACKED:    GmrTxt := "Atak na Npca";
      default:            GmrTxt := "Brak";
    endcase

    SetCriminal(who, 1, "InformGuards", "Npc "+me.name+" ["+me.serial+"] chcial wezwac straznikow z powodu: "+GmrTxt+", ale straznikow nie bylo.");
  else
    Say(Text);
  endif

  return Inf;

endfunction

//-----------------------------------------------------------------------------

function SetGuardChase(who, Flags := 0)

  var GuardChase := GetObjProperty(who, PROP_GUARD_CHASE);
  if(!GuardChase.Cities)
    GuardChase         := struct;
    GuardChase.+Cities := dictionary;
    GuardChase.+Flags  := 0;
    GuardChase.+aTime  := 0;
  endif

  if(Flags & GDCF_ALLCITIES)
    SetBitOn(GuardChase.Flags, GDCF_ALLCITIES);
    GuardChase.aTime := ReadGameClock();
  endif

  GuardChase.Cities[City] := ReadGameClock();
  SetObjProperty(who, PROP_GUARD_CHASE, GuardChase);

endfunction

//-----------------------------------------------------------------------------

function IsGuardChasing(who)

  if(who.dead or who.hidden or who.concealed or Distance(me, who) > 15)
    return 0;
  endif

  var Chase := GetObjProperty(who, PROP_GUARD_CHASE);

  if(Chase.Flags & GDCF_ALLCITIES)
    if(Chase.aTime + GUARD_REMEMBER_TIME > ReadGameClock())
      return 1;
    else
      Chase.aTime := 0;
      SetBitOff(Chase.Flags, GDCF_ALLCITIES);
      SetObjProperty(who, PROP_GUARD_CHASE, Chase);
    endif
  elseif(Chase.Cities.exists(City))
    if(Chase.Cities[City] + GUARD_REMEMBER_TIME > ReadGameClock())
      return 1;
    else
      Chase.Cities.erase(City);
      SetObjProperty(who, PROP_GUARD_CHASE, Chase);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function MoveifBlocking(who)

  if((GetStandingHeight(me.x-1,me.y,me.z,me.realm).z == error and GetStandingHeight(me.x+1,me.y,me.z,me.realm).z == error) or 
     (GetStandingHeight(me.x,me.y-1,me.z,me.realm).z == error and GetStandingHeight(me.x,me.y+1,me.z,me.realm).z == error) or
     (GetStandingHeight(me.x+1,me.y-1,me.z,me.realm).z == error and GetStandingHeight(me.x+1,me.y+1,me.z,me.realm).z == error and GetStandingHeight(me.x+1,me.y,me.z,me.realm).z != error) or
     (GetStandingHeight(me.x-1,me.y-1,me.z,me.realm).z == error and GetStandingHeight(me.x-1,me.y+1,me.z,me.realm).z == error and GetStandingHeight(me.x-1,me.y,me.z,me.realm).z != error) or
     (GetStandingHeight(me.x-1,me.y-1,me.z,me.realm).z == error and GetStandingHeight(me.x+1,me.y-1,me.z,me.realm).z == error and GetStandingHeight(me.x,me.y-1,me.z,me.realm).z != error) or
     (GetStandingHeight(me.x-1,me.y+1,me.z,me.realm).z == error and GetStandingHeight(me.x+1,me.y+1,me.z,me.realm).z == error and GetStandingHeight(me.x,me.y+1,me.z,me.realm).z != error) )

    WalkAwayFrom(who);
    WalkAwayFrom(who);
    return 1;
  endif

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Train Skills
//-----------------------------------------------------------------------------
//=============================================================================

function GetMerchantTeachSkills(me)

  var mercElem   := GetNpcMercConfig(me);
  var SkillIDs   := GetConfigStringArray(mercElem, "TeachSkill");
  
  if(!SkillIDs)
    return dictionary;
  endif
  
  var Skills     := dictionary;
  var SkElem     := 0;
  
  foreach AttrId in SkillIDs
    var SkId := GetSkillIdByAttributeId(AttrId);
    SkElem := GetSkillConfigElem(SkId);
    if(SkElem.name)
      Skills[SkId] := CStr(SkElem.name);
    endif
  endforeach

  return Skills;

endfunction

//-----------------------------------------------------------------------------

function GetMerchantTeachSkillsArray(me)

  var mercElem   := GetNpcMercConfig(me);

  if(!mercElem)
    LogError("TeachSkill: Nieznaleziono MercType dla template: "+me.npctemplate+"/"+me.serial);
    return array;
  endif

  var SkillIDs   := GetConfigStringArray(mercElem, "TeachSkill");

  foreach SkillID in SkillIDs
    SkillID := GetSkillIdByAttributeId(SkilliD);
  endforeach

  return SkillIDs;
  
endfunction

//-----------------------------------------------------------------------------

function GetMerchantPerfectSkillsArray(me)

  var mercElem   := GetNpcMercConfig(me);

  if(!mercElem)
    LogError("PerfectSkill: Nieznaleziono MercType dla template: "+me.npctemplate+"/"+me.serial);
    return array;
  endif

  var SkillIDs   := GetConfigStringArray(mercElem, "PerfectSkill");

  foreach SkillID in SkillIDs
    SkillID := GetSkillIdByAttributeId(SkilliD);
  endforeach

  SkillIDs += GetMerchantTeachSkillsArray(me);

  return SkillIDs;
  
endfunction

//-----------------------------------------------------------------------------

function GetMerchantTeachSkillArrayForClass(me, Class)

  var mercSkills  := GetMerchantTeachSkillsArray(me);
  var Skills      := array;
  var ClassSkills := GetCharClassSkills(Class);

  foreach Skill in mercSkills
    if(Skill in ClassSkills)
      Skills.append(Skill);
    endif
  endforeach

  return Skills;

endfunction

//-----------------------------------------------------------------------------

function GetMerchantPerfectSkillArrayForClass(me, Class)

  var mercSkills  := GetMerchantPerfectSkillsArray(me);
  var Skills      := array;
  var ClassSkills := GetCharClassSkills(Class);

  foreach Skill in mercSkills
    if(Skill in ClassSkills)
      Skills.append(Skill);
    endif
  endforeach

  return Skills;

endfunction

//-----------------------------------------------------------------------------

function TrainSkill(me, who)

  if(!CheckLineOfSight(me, who))
    return;
  endif

  sleepms(RandomInt(1000));

  if(GetObjProperty(who, "#SkTraining"))
    Say("Wybierz lepiej umiejetnosc.");
    return;
  endif

  var whoSer := who.serial;
  SetObjProperty(who, "#SkTraining", me.serial);
  NpcTrainPlayerSkill(me, who);

  if(!who)
    who := FindPlayerBySerial(whoSer);
  endif

  EraseObjProperty(who, "#SkTraining");

endfunction

//-----------------------------------------------------------------------------

function CanNpcTrainSkill(who, SkId)
  
  if(SkId == SKILLID_MAGERY or SkId == SKILLID_ARCHERY)
    if(!IsNewPlayer(who))
        Say("Nie moge Cie nauczyc tej umiejetnosci, juz za pozno na nauke takiej umiejetnosci.");
        return 0;
    elseif(GetBaseSkill(who, __ifelse(SkID == SKILLID_ARCHERY, SKILLID_MAGERY, SKILLID_ARCHERY)))
        Say("Nie moge Cie nauczyc tej umiejetnosci, ani lucznik nie zrozumie magii, ani mag lucznictwa.");
        return 0;
    endif
  elseif(SkId == SKILLID_NECRO or SkId == SKILLID_DRUID)
    if(GetBaseSkill(who, SKILLID_MAGERY) == 0)
      if(!IsNewPlayer(who))
        Say("Nie moge Cie nauczyc tej umiejetnosci, nie wygladasz ani mlodo ani na maga.");
        return 0;
      else
        Say("Mlodys, dam Ci wiec jeszcze szanse nauczyc sie tej magii, ucz sie i korzystaj.");
      endif
    endif
  elseif(SkId == SKILLID_VAMPIRE)
    if(!IsVampire(who))
      Say("Nie widze by w twoich zylach plynela krew wampira! Wynos sie stad zanim wyssam z ciebie ostatnia krople krwi!");
      return 0;
    endif
  else
    case(SkId)
      SKILLID_PALADINISM:
      SKILLID_FANATICISM:   Say("Tej umiejetnosci nie da sie w taki prosty sposob nauczyc.");
                            return 0;
      SKILLID_KENSAI:
      SKILLID_BARBARIAN:    if(IsCharClass(who, CLASS_MAGE))
                              Say("Pff... maga? Idz lepiej do liceum w Moonglow postudiowac prastare ksiegi.");
                              return 0;
                            else
                              if(GetBaseSkillBaseValue(who, SKILLID_PALADINISM))
                                Say("Jestes juz Paladynem, czy Ci to nie wystarcza?");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_FANATICISM))
                                Say("Mroczny Rycerz? Hm.. nie wydaje mi sie.");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_KENSAI) and SkId != SKILLID_KENSAI)
                                Say("Umiesz juz sztuke walki Kensai, czyz to nie wystarczajaco?");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_BARBARIAN) and SkId != SKILLID_BARBARIAN)
                                Say("Barbarzynca? Nie, nie, twoj umysl tyle nie pomiesci.");
                                return 0;
                              elseif(GetBaseSkillBaseValue(who, SKILLID_VAMPIRE) or GetBaseSkillBaseValue(who, SKILLID_WEARWOLF) or IsVampire(who) or IsWearWolf(who))
                                Say("Sadze, ze nie uda mi sie nauczyc Ciebie tej umiejetnosci.");
                                return 0;
                              endif
                            endif
    endcase
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function NpcTrainPlayerSkill(me, who)

  case (RandomInt(4))
    0:         Say("Czego pragniesz sie nauczyc?");
    1:         Say("Potrafie uczyc owych umiejtnosci...");
    2:         Say("Spojrz czego nauczyc moge.");
    default:   Say("Nauka wazna rzecz, udziele jej za drobna kwote.");
  endcase

  var tEv := SendNpcHandleEvent(EVID_NH_TRAIN_GUMP,who,me);

  if(tEv.skId)
    tEv.skId := tEv.skId - 1;
    var SkElem := GetSkillConfigElem(tEv.skId);
    if(SkElem)
      var TeachAmt := CITYNPC_MAXTRSK - GetBaseSkillBaseValue(who,tEv.skId);
      
      if(TeachAmt <= 0)
        Say("Nie naucze cie niczego wiecej o umiejetnosci "+SkElem.name+".");
        return;
      elseif(TeachAmt > CITYNPC_TEACHAMT)
        TeachAmt := CITYNPC_TEACHAMT;
      endif

      if(!CanNpcTrainSkill(who, tEv.skId))
        return;
      endif
      
      if(!SpendGold(who,TeachAmt))
        Say("Oczekuje drobnej zaplaty za nauczenie Cie tej umiejetnosci w wysokosci "+TeachAmt+" gp.");
        return;
      endif

      Say("Wybral"+ggm(who,3)+" "+SkElem.name+", dobrze.");
      Say("*naucza*");
      sleepms(2000+RandomInt(1000));
      
      if(Distance(who,me) > 3)
        Say("No i uciekl"+ggm(who,4));
        return;
      endif
      
      Say("*tlumaczy*");
      sleepms(4000+RandomInt(1000));
      
      if(Distance(who,me) > 3)
        Say("No i uciekl"+ggm(who,4));
        return;
      endif
      
      Say("Gotowe.");
      if(GetBaseSkillBaseValue(who,tEv.skId) + TeachAmt <= CITYNPC_MAXTRSK)
        SetBaseSkillBaseValue(who,tEv.skId,GetBaseSkillBaseValue(who,tEv.skId) + TeachAmt);
      endif
    endif
  else
    case(RandomInt(4))
      0:         Say("Zapraszam innym razem.");
      1:         Say("Zegnam.");
      2:         Say("Dowidzenia.");
      default:   Say("Innym razem wiec.");
    endcase
  endif

endfunction

//-----------------------------------------------------------------------------

function AnswerQuestion(ev)

  TurnToward(ev.source);

  if(ev.question)
    Say(ev.question);
  endif
  
  var Time := ReadGameClock() + 15;
  var nEv;

  var Event := struct;
  Event.+type := EVID_QUESTION;
  Event.+source := ev.source;
  Event.+result := -1;

  while(Time > ReadGameClock())
    nEv := wait_for_event(1);

    case(nEv.type)
      EVID_SPEECH:        if(CheckLineOfSight(me, nEv.source))

                            TurnToward(nEv.source);
                            if(nEv.source == ev.source)
                              var lowerText := lower(nEv.text);

                              foreach Answer in (ev.answers)
                               
                                if(lowerText[_Answer_iter])
                                  Event.result := Answer;
                                  ev.script.sendevent(Event);
                                  return;
                                endif

                              endforeach

                            else
                              Say(GetName(nEv.source)+", nie tobie zadal"+ggm(me, 8)+" pytanie...");
                            endif
                          endif

      EVID_ITEM_GIVEN:    MoveToPack(ev.item, ev.source);

      EVID_QUEST_MSG:     var Data := struct;
                          Data.+npc := me;
                          Data.+param := ev;
                           
                          start_script(":ngd_npcs:quest_msg", Data);
    endcase

    if(Distance(me, ev.source) >= 3)
      Say("Juz odchodzisz?");
      Say("Coz za kultura, nawet odpowiedziec ciezko...");
      Say("*wzdycha*");
      sleepms(500);
      TurnAwayFrom(ev.source);
    endif

  endwhile

  ev.script.sendevent(Event);

  return;

endfunction

//-----------------------------------------------------------------------------

function NpcBusyLoop(Npc, who, LoopNum, text)

  var i := 0;

  for(i:=1; i<=LoopNum; i+=1)

    NpcSpeechJustSay(Npc, who, text);
    sleep(2);
    if(Distance(Npc, who) >= 3)
      NpcSpeechJustSay(Npc, who, "Gdziez on"+ggm(who, GGM_A)+" polazl"+ggm(who, GGM_A)+"?");
      NpcSpeechJustSay(Npc, who, "*zastanawia sie*");
      NpcSpeechJustSay(Npc, who, "Eh... raz chca a zaraz nie chca...");
      return 0;
    endif

  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function StdCityNpcHandleEvent(byref ev)
 
  if(!ev)
    return 0;
  endif

  case(ev.type)
    EVID_ENGAGED:                   
    EVID_DAMAGED:            CityNpcAttack(ev.source);
                             ev := 0;
                             return 1;

    EVID_SPEECH:             CheckNpcSpeech(ev.source, ev.text);
                             ev := 0;
                             return 1;

    EVID_PUSH:               CityNpcPush(ev.source);
                             ev := 0;
                             return 1;
        
    EVID_QUEST_MSG:          var Data := struct;
                             Data.+npc := me;
                             Data.+param := ev;
                           
                             start_script(":ngd_npcs:quest_msg", Data);
                             ev := 0;
                             return 1;

    EVID_ITEM_GIVEN:         NpcTakeItem(me, ev.source, ev.item);
                             ev := 0;
                             return 1;

    EVID_QUESTION:           AnswerQuestion(ev);
    

    EVID_DOUBLECLICKED:      if(RandomInt(100) < 5)
                               Say("hm?");
                               TurnToward(ev.source);
                             elseif(RandomInt(100) < 10)
                               Say("Przestan mnie zaczepiac");
                               TurnToward(ev.source);
                             elseif(RandomInt(100) < 15)
                               Say("Odczep sie");
                               TurnAwayFrom(ev.source);
                             endif

                             ev := 0;
                             return 1;
  endcase

  return StdMovNpcHandleEvent(ev);

endfunction

//-----------------------------------------------------------------------------

function CanHandleEvent(byref ev)

  case (ev.type)
    EVID_ENGAGED:                   
    EVID_DAMAGED:            
    EVID_SPEECH:             
    EVID_RUN_FOLLOW:
    EVID_WALK_FOLLOW:
    EVID_RUN:
    EVID_WALK:
    EVID_CMERC_BOUGHT:       
    EVID_CMERC_SOLD:
    EVID_DOUBLECLICKED:
    EVID_PUSH:               
    EVID_ITEM_GIVEN:         
    EVID_QUEST_MSG:          return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function StdCityNpcEventLoop()

  if(IsWalking())
    if(!IsWalkPause())
      WalkToDest();
    endif
  endif

  if(NextWander < ReadGameClock())
    DefWander();
    CityNpcLookAround();
  endif

endfunction

//-----------------------------------------------------------------------------
