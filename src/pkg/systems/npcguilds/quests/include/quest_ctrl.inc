//-----------------------------------------------------------------------------
// Modified: 2005-06-26
//-----------------------------------------------------------------------------

include "include/logerror";

//-----------------------------------------------------------------------------

const QTPROP_CTRLPID         := "#ControlPid";
const QTPROP_TICKETS         := "QuestTickets";
const QTPROP_LASTCOMP        := "StageLastComp";
const QTPROP_TRUN            := "TicketRun";
const QTPROP_ITEMEXPTIME     := "ExpTime";
const QTPROP_ITEMCRTIME      := "QItemCrTime";
const QTPROP_ITEMINFO        := "QuestInfo";
const QTPROP_QTSER           := "QTicketSerial";
const QTPROP_QUESTER_SERIAL  := "#QuesterSerial";
const QTPROP_QUEST_ARROW     := "QuestArrow";

const QUEST_MIN_CRRANGE      := 30;
const QDEF_ITEMEXPTIME       := 1200;

const QT_COMP_COLOR          := 1420;
const QT_FINISH_COLOR        := 999;
const QT_FAIL_COLOR          := 1214;
const QT_NORMAL_COLOR        := 1118;

//-----------------------------------------------------------------------------

const EVQID_NPC_SPEECH       := 0x00000001;
const EVQID_ITEM_USE         := 0x00000002;
const EVQID_PLAYER_DEATH     := 0x00000003;
const EVQID_ITEM_CREATE      := 0x00000004;
const EVQID_HARVEST          := 0x00000005;
const EVQID_SKILL            := 0x00000006;
const EVQID_FINISH           := 0x00000007;
const EVQID_SET_NPCNAME      := 0x00000008;
const EVQID_NPC_DEATH        := 0x00000009;
const EVQID_STOP             := 0x00000010;
const EVQID_RESET            := 0x00000011;
const EVQID_QUEST_ARROW      := 0x00000012;

//-----------------------------------------------------------------------------

function SendQuestEvent(who, EvID, Type, byref Object)

  if(!IsPlayer(who))
    return;
  endif

  var Tickets := GetObjProperty(who, QTPROP_TICKETS);
  var Ticket;

  if(!Tickets or !Tickets.size())
    return 0;
  endif

  foreach TicketSer in Tickets
    Ticket := FindItemBySerial(TicketSer);
    if(Ticket)
      SendQuestTicketEvent(Ticket, who, EvID, Type, Object);
    endif
  endforeach

  return Ticket;

endfunction

//-----------------------------------------------------------------------------

function SendQuestTicketEvent(Ticket, who, EvID, Type, byref Object)

  var Pid := CInt(GetObjProperty(Ticket, QTPROP_CTRLPID));
  if(!Pid)
    LogError("SendQuestTicketEvent: Brak pidu na bilecie ["+Ticket.serial+"]");
  else
    Pid := GetProcess(Pid);
    if(!Pid)
      LogError("SendQuestTicketEvent: Nie istniejacy Pid ["+CInt(GetObjProperty(Ticket, QTPROP_CTRLPID))+"] na bilecie ["+Ticket.serial+"]");
    else
      var tEv := struct;

      tEv.+eId     := EvID;
      tEv.+Source  := who;
      tEv.+Object  := Object;
      tEv.+Type    := Type;

      if(!Pid.sendevent(tEv))
        LogError("SendQuestTicketEvent: Nie udalo sie wyslac eventa do Pida ["+CInt(GetObjProperty(Ticket, QTPROP_CTRLPID))+"] na bilecie ["+Ticket.serial+"]");
      else
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AddQuestTicketToPack(who, Item)

  var QuestTickets := GetObjProperty(who, QTPROP_TICKETS);
  if(!QuestTickets)
    QuestTickets := array;
  endif

  if(!(Item.serial in QuestTickets))
    QuestTickets.append(Item.serial);
    SetObjProperty(who, QTPROP_TICKETS, QuestTickets);
    SetObjProperty(Item, QTPROP_TRUN, ReadGameClock());
  endif

endfunction

//-----------------------------------------------------------------------------

function RemoveQuestTicketFromPack(who, Item)

  if(RemoveQuestTicketSerial(who, Item.serial))
    EraseObjProperty(Item, QTPROP_TRUN);
  endif

endfunction

//-----------------------------------------------------------------------------

function RemoveQuestTicketSerial(who, Serial)

  if(!who.isa(POLCLASS_MOBILE))
    return 0;
  endif

  var QuestTickets := GetObjProperty(who, QTPROP_TICKETS);
  if(Serial in QuestTickets)
    QuestTickets.erase(RetIndex(QuestTickets, Serial));
    SetObjProperty(who, QTPROP_TICKETS, QuestTickets);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckQuestNpcDeath(corpse)

  var TicketSer := GetObjProperty(corpse, QTPROP_QTSER);

  if(TicketSer)
    var Ticket := FindItemBySerial(TicketSer);
    if(Ticket)
      SendQuestTicketEvent(Ticket, 0, EVQID_NPC_DEATH, 0, corpse);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
