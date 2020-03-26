//-----------------------------------------------------------------------------

include "../pkg/magic/magiceffects/summon";
include "include/npcs";
include "include/mobile/age";
include "include/items";
include "ai/include/npch";

//-----------------------------------------------------------------------------

function SellAnimal(who)

  if(IsWalking())
    SetWalkPause();
  endif

  say("Ktore zwierze chcial"+ggm(who,1)+" sprzedac?");

  ev         := struct;
  ev.+type   := EVID_NH_NPC_SELECT;
  ev.+source := who;
  ev.+npc    := me;
  ev.+los    := 1;
  ev.+master := 1;

  StdNpcHandleWait(ev);

  clear_event_queue();

  if(ev)
    var pet := ev.pet;
    if(!pet)
      return;
    endif

    var buyamount := EvaluateNpcPrice(ev.pet);
    if(CreateItemInPack(who, UOBJ_GOLDCOIN, buyamount))
      say("Stworzenie " + pet.name + " jest warte " + buyamount + "gp");
      KillNPC(pet,1);
    else
      Say("Nie masz miejsca w plecaku");
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function GetTicketNpcName(item)

  var NpcTemp := GetObjProperty(item, PROP_NPCTEMP);
  var NpcName := GetNpcConfig(NpcTemp).name;

  if(NpcName["<random>"])
    NpcName := GetObjProperty(item, "NpcName");
  endif

  return NpcName;

endfunction

//-----------------------------------------------------------------------------

function Save_Pet_Data(player, pet)

  if(pet)

    var ticket := CreateItemInPack(player, UOBJ_TICKET, 1);

    ticket.usescript := "";

    var rName := GetNpcConfig(pet).Name;
    var dName := GetNpcConfig(pet).DescName;

    if(rName["<random>"]) 

      if(dName)
        rName := dName;
      else
        rName["<random> - "] := "";
        rName["<random>"] := "";
      endif
      
    endif
    
    if(!rName)
      ticket.name := "Bilet na niezidentyfikowanego zwierza zwanego " + pet.name + ".";
    elseif(rName != pet.name)
      ticket.name := "Bilet na zwierza " + rName + " zwanego " + pet.name + ".";
    else
      ticket.name := "Bilet na zwierza " + pet.name + ".";
    endif

    SaveNpcAttrData(pet, ticket);
    SetObjProperty(ticket, "StableTime", ReadGameClock());
    KillNPC(pet,1);

  endif

endfunction

//-----------------------------------------------------------------------------

function IsPetTicket(ticket)

  if(ticket.objtype == UOBJ_TICKET)
    if(GetObjProperty(ticket, PROP_NPCTEMP))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function Load_Ticket_Data(player, ticket)

  if(ticket.objtype == UOBJ_TICKET)
    if(IsWalking())
      SetWalkPause();
    endif

    var pettemplate := GetObjProperty(ticket, PROP_NPCTEMP);

    if(!pettemplate)
      say("Dzieki, ale nie potrzebuje tego");
      return 0;
    endif

    if(!CanControlTamed(pettemplate,player))
      say("Nie masz szans na kontrolowanie tego zwierza");
      return 0;
    endif

    if(!ReserveItem(ticket))
      return 0;
    endif
    say("Oh, Poczekaj chwilke, zaraz przyprowadze twego zwierza.");
    sleep(2);
    var newpet := CreateNPC(pettemplate, 6096, 1191, 0,_DEFAULT_REALM,DEF_PARMS,NPCF_NO_UPKEEP | NPCF_NO_LOOT);
    LoadNpcAttrData(newpet, ticket, player);
    DestroyItem(ticket);
    MoveObject(newpet,me.x,me.y,me.z,me.realm,MOVECHAR_FORCELOCATION);
	SetHappiness(newpet, MAX_HAPPY_LVL);
	PrintTextAbove(newpet, "*szczesliwe*");
    Say("Opiekuj sie nim, i nie zapominaj go karmic!");
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function EvaluateNpcPrice(pet)

  var Price := CInt(GetNpcConfig(pet).VendorBuysFor);
  
  if(!Price)
    price := 10 + CInt(GetStrength(pet) * 2);
  
    var IsAnimal := (GetNpcConfig(pet).script == "animal");

    if(!IsAnimal)
      price *= 7;
    endif
  endif

  var age := GetAge(pet);

  if(age > 1)
    Price += (Price * 0.02 * ToMax(CInt(age/2), 10));
  endif

  var Abl := GetBaseNpcAbilities(pet);
  var Sum := GetArraySum(Abl);

  if(Sum > 0)
    var SumMax := NPC_ABL_MAXID * 1000;

    Price += CInt((Price * 0.7) * CDbl(Sum) / SumMax);
  endif

  return CInt(price/8);

endfunction

//-----------------------------------------------------------------------------

function Stable(player)

  say("Pokaz mi twego zwierzaka.");

  ev         := struct;
  ev.+type   := EVID_NH_NPC_SELECT;
  ev.+source := player;
  ev.+npc    := me;
  ev.+los    := 1;
  ev.+master := 1;

  Set_Critical(1);
  StdNpcHandleWait(ev);

  if(ev.pet)
    SetObjProperty(ev.pet, "#IsStabling", 1);
  endif
  Set_Critical(0);

  clear_event_queue();

  if(ev)
    var pet := ev.pet;
    if(!pet)
      return;
    endif

    if(IsSkillSum(pet) or IsQuestItem(pet))
      Say("Nie moge przechowac tego zwierzecia.");
      EraseObjProperty(pet, "#IsStabling");
      return;
    endif

    if(!HavePetEmptyPack(Pet))
      Say("Czy ja wygladam na magazyniera? Juka tego zwierza nie jest pusta.");
      EraseObjProperty(pet, "#IsStabling");
      return;
    endif

    var price := Min(100 + CInt(EvaluateNpcPrice(pet) * 0.5), 5000);
    var IsAnimal := (GetNpcConfig(pet).script == "animal");

	/*
    if(GetGold(player) < price)
      if(!IsAnimal)
        Say("Potrzebujesz "+price+" gp by przechowac to dzikie stworzenie!");
      else
        Say("Potrzebujesz "+price+" gp by przechowac to zwierze.");
      endif

      EraseObjProperty(pet, "#IsStabling");
      return;
    endif
	*/

    if(!SpendGold(player, price))
      Say("Nie masz "+price+" gp.");
      EraseObjProperty(pet, "#IsStabling");
      return;
    endif
    
    say("Trzymaj ten bilet i przyjdz z nim kiedy bedziesz chcial"+ggm(player,4)+" zwierza " + pet.name + " z powrotem.");
    Save_Pet_Data(player, pet);
  endif

  if(ev.pet)
    EraseObjProperty(ev.pet, "#IsStabling");
  endif

endfunction

//-----------------------------------------------------------------------------

function HavePetEmptyPack(Pet)

  foreach Item in EnumerateItemsInContainer(Pet.backpack)
    if(Item.objtype != UOBJ_MOUNT and Item.objtype != UOBJ_ARROW and Item.objtype != UOBJ_BOLTS)
      return 0;
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AppraiseAnimal(who)

  TurnToward(who);
  Say("Pokaz mi to zwierze...");

  ev := SendNpcHandleEvent(EVID_NH_NPC_SELECT,who,me,"Wpisz cene");

  if(ev == error)
    return;
  endif

  if(!CheckLineOfSight(me, ev.pet) or Distance(me, ev.pet) > 7)
    Say("Nie widze tego...");
    return;
  endif

  if(!GetMaster(ev.pet))
    say("Nie jestem pewien.");
    return;
  endif

  var cfgElem := GetNpcConfig(ev.pet);

  if(!IsSummoned(ev.pet) and !GetNecroControlledTime(ev.pet) and !cfgElem.Mechanic)
    var buyamount := EvaluateNpcPrice(ev.pet);
    Say("Stworzenie jest warte "+buyamount+"gp");
  elseif(IsHuman(ev.pet))
    say("Czy ja wygladam na wlasciciela karczmy?!");
  else
    say("Nie jestem pewien...");
  endif

endfunction

//-----------------------------------------------------------------------------
