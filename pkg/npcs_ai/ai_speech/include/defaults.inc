//-----------------------------------------------------------------------------

include ":ngd_npcs:common";
include ":mq_npcs:common";

//-----------------------------------------------------------------------------

function AISys_Speech_DefMercTriggers()

  AISys_Speech_AddTrigger("kupi", ":ai_merchant:buy", AI_SPT_FULL_SENTENCE, AIHF_SEND_MERC_DATA);

  AISys_Speech_AddTrigger("sprzedac wszystko z pojemnika", ":ai_merchant:sell_all_cont", AI_SPT_FULL_SENTENCE, AIHF_SEND_MERC_DATA);
  AISys_Speech_AddTrigger("sprzedac wszystko", ":ai_merchant:sell_all", AI_SPT_FULL_SENTENCE, AIHF_SEND_MERC_DATA); // ten drugi, aby event wyzej byl pierwszy w razie wypowiedzenia suffixa 'z pojemnika'!
  AISys_Speech_AddTrigger("sprzeda", ":ai_merchant:sell", AI_SPT_FULL_SENTENCE, AIHF_SEND_MERC_DATA);

  AISys_Speech_AddTrigger("zamowien", ":craftquests:show_quests", AI_SPT_FULL_SENTENCE | AI_SPT_SPOKE_NPC_NAME);
  AISys_Speech_AddTrigger("przedmiot", ":lostitems:check_lostitem", AI_SPT_FULL_SENTENCE | AI_SPT_SPOKE_NPC_NAME);

  AISys_Speech_AddTrigger("przesun", ":ai_merchant:push", AI_SPT_SPOKE_NPC_NAME);

  AISys_Speech_AddTrigger("ucz", ":merc_train:train_skill", AI_SPT_FULL_SENTENCE | AI_SPT_SPOKE_NPC_NAME);
  
  AISys_Speech_AddTrigger("wita", ":quests:npc_speech", AI_SPT_FULL_SENTENCE | AI_SPT_NO_CAN_SPEAK_TRIG, AIHF_RESULT_WAIT);

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_NpcGuildTriggers()

  if(!IsGuildNpc(Self()))
    return;
  endif

  AISys_Speech_AddTrigger("wita", ":ngd_npcs:hello", AI_SPT_FULL_SENTENCE | AI_SPT_NO_CAN_SPEAK_TRIG, AIHF_RESULT_WAIT);
  AISys_Speech_AddTrigger("awans", ":ngd_npcs:promotion", AI_SPT_FULL_SENTENCE | AI_SPT_NO_CAN_SPEAK_TRIG);
  AISys_Speech_AddTrigger("zadanie", ":ngd_npcs:process_quest", AI_SPT_FULL_SENTENCE | AI_SPT_SPOKE_NPC_NAME | AI_SPT_NO_CAN_SPEAK_TRIG);
  AISys_Speech_AddTrigger(array("zapis", "gildi"), ":ngd_npcs:sign_in", AI_SPT_FULL_SENTENCE | AI_SPT_NO_CAN_SPEAK_TRIG);
  AISys_Speech_AddTrigger(array("wypis", "gildi"), ":ngd_npcs:resign", AI_SPT_FULL_SENTENCE | AI_SPT_NO_CAN_SPEAK_TRIG);
  AISys_Speech_AddTrigger(array("wybra", "umiejetnosc"), ":ngd_npcs:choose_skill", AI_SPT_FULL_SENTENCE | AI_SPT_NO_CAN_SPEAK_TRIG);

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_MiniQuestTriggers()

  if(!CanNpcHaveMiniQuest(Self()))
    return;
  endif

  AISys_Speech_AddTrigger("wita", ":mq_npcs:hello", AI_SPT_FULL_SENTENCE | AI_SPT_SPOKE_NPC_NAME | AI_SPT_NO_CAN_SPEAK_TRIG, AIHF_RESULT_WAIT);
  AISys_Speech_AddTrigger("pomo", ":mq_npcs:give_quest", AI_SPT_FULL_SENTENCE | AI_SPT_SPOKE_NPC_NAME);
  AISys_Speech_AddTrigger("przypomnij", ":mq_npcs:remind", AI_SPT_FULL_SENTENCE | AI_SPT_SPOKE_NPC_NAME);

  AISys_Handler_AddTrigger(":mq_npcs:check_mq", AIHT_ON_WANDER/*, AIHF_RESULT_WAIT*/);

endfunction

//-----------------------------------------------------------------------------

