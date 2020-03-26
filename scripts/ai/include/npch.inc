//-----------------------------------------------------------------------------

include "include/checks/objchecks";
include "ai/include/tamed";

//-----------------------------------------------------------------------------

const NH_WAIT_TIME                     := 60;

//-----------------------------------------------------------------------------
// Events
//-----------------------------------------------------------------------------

const EVID_NH_BARBER_GUMP            := 0x00003001;
const EVID_NH_NPC_SELECT             := 0x00003002;
const EVID_NH_ITEM_SELECT            := 0x00003003;
const EVID_NH_TEGUMP_SELECT          := 0x00003004;
const EVID_NH_RENT_HOUSE             := 0x00003005;
const EVID_NH_RENT_TIME              := 0x00003006;
const EVID_NH_YESNO_GUMP             := 0x00003007;
const EVID_NH_TRAIN_GUMP             := 0x00003008;
const EVID_NH_SEL_WCID_GUMP          := 0x0000300B;
const EVID_NH_HOUSE_GUMP             := 0x0000300C;
const EVID_NH_AUCTION_GUMP           := 0x0000300D;
const EVID_NH_TRAVEL_GUMP            := 0x0000300E;
const EVID_NH_ROYAL_GUMP             := 0x00003010;
const EVID_NH_GAMBLE_GUMP            := 0x00003011;
const EVID_NH_HALLOWEEN_GUMP		 := 0x00003012;

//-----------------------------------------------------------------------------

function SendNpcHandleEvent(EvID, who, me, oth := 0)

  var hEv     := struct;
  hEv.+type   := EvID;
  hEv.+source := who;
  hEv.+npc    := me;
  hEv.+oth    := oth;
  
  StdNpcHandleWait(hEv);
  return hEv;

endfunction

//-----------------------------------------------------------------------------

function StdNpcHandleWait(byref Event)

  var Script := start_script("::handle_npc", Event);
  var wEv;

  if(!Script)

    LogError("Nie udalo sie uruchomic skryptu dla handle_npc: "+Script);
    wEv := error;

  else
  
    var Res := -100;
    var Timer := ReadGameClock() + NH_WAIT_TIME;

    while(Timer > ReadGameClock())

      wEv := wait_for_event(1);

      if(wEv.type == Event.type)
        if(Event.source == wEv.source)
          Res := 1;
        endif
      elseif(wEv.type == EVID_ITEM_GIVEN)
        MoveToPack(wEv.item, wEv.source);
      elseif(wEv.type == EVID_QUEST_MSG)

        var Data := struct;
        Data.+npc := me;
        Data.+param := ev;
        
        start_script(":ngd_npcs:quest_msg", Data);
      endif

      if(Res != -100)
        break;
      else
        wEv := 0;
      endif
      
      if(Distance(Event.source, Event.npc) > 3)
        Say("Niegrzecznie tak uciekac!");
        Res := 0;
        wEv := error;
        break;
      endif
      
    endwhile

    clear_event_queue();

    if(Res == -100)
      Say("Widze, ze nie mozesz sie zdecydowac.");
      wEv := error;
    endif
  endif

  if(Script)
    CloseGump(Event.source, Script.pid);
  endif

  if(wEv)
    HandleSentEvent(wEv);
  endif

  Event := wEv;

endfunction

//-----------------------------------------------------------------------------

function HandleSentEvent(byref Ev)

  case(Ev.type)
    EVID_NH_NPC_SELECT:   return HandleNpcEvent(Ev);
    EVID_NH_ITEM_SELECT:  return HandleItemEvent(Ev);
  endcase
  
endfunction

//-----------------------------------------------------------------------------

function HandleNpcEvent(byref Ev)

  if(!Ev)
    return;
  endif

  if(!StandardMobCheck(Ev.source,Ev.pet,NO_BUSY_CHECK))
    Ev.pet := 0;
  endif

  if(Ev.pet)
    if(Ev.pet == me)
      Say("?!");
      Ev.pet := 0;
    endif
  endif

  if(Ev.pet)
    if(Ev.los)
      if(!CheckLineOfSight(Ev.npc,Ev.pet) or Distance(Ev.npc,Ev.pet) > 5)
        Say("Nie widze tego...");
        Ev.pet := 0;
      endif
    endif
  endif

  if(Ev.pet)
    if(IsHuman(Ev.pet))
      Say("Czy ja wygladam na wlasciciela karczmy?!");
      Ev.pet := 0;
    elseif(!Ev.pet.isa(POLCLASS_NPC))
      Say("To nie zwierze.");
      Ev.pet := 0;
    endif
  endif

  if(Ev.master != 0 and Ev.pet)
    var Master := GetMaster(Ev.pet);
    if(!Master)
      Say("To zwierze nie posiada wlasciciela.");
      Ev.pet := 0;
    endif

    if(Ev.pet)
      if(Master != Ev.source)
        Say("To nie jest twoje zwierze.");
        Ev.pet := 0;
      elseif(IsSummoned(Ev.pet) or GetNecroControlledTime(Ev.pet) or GetNpcConfig(Ev.pet).Mechanic)
        Say("Nie moge rozpoznac tego stworzenia.");
        Ev.pet := 0;
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function HandleItemEvent(byref Ev)

  if(!StandardItemCheck(Ev.source,Ev.item,NO_BUSY_CHECK))
    ReleaseItem(Ev.item);
    Ev.item := 0;
  endif

  if(Ev.item)
    ReleaseItem(Ev.item);
  endif

endfunction

//-----------------------------------------------------------------------------
