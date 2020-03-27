//-----------------------------------------------------------------------------

function AISys_Handler_DefMercTriggers()

  AISys_Handler_AddTrigger(":bulks:bulk_check", AIHT_SUCCESSFUL_SELL);
  AISys_Handler_AddTrigger(":bulks:process_item", AIHT_ITEM_GIVEN, AIHF_RESULT_WAIT);
  AISys_Handler_AddTrigger(":craftquests:process_item", AIHT_ITEM_GIVEN, AIHF_RESULT_WAIT);
  AISys_Handler_AddTrigger(":lostitems:process_item", AIHT_ITEM_GIVEN, AIHF_RESULT_WAIT);
  AISys_Handler_AddTrigger(":miracleworker:process_item", AIHT_ITEM_GIVEN, AIHF_RESULT_WAIT);
  AISys_Handler_AddTrigger(":donations:process_item", AIHT_ITEM_GIVEN, AIHF_RESULT_WAIT);

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_NpcGuildTriggers()

  if(!IsGuildNpc(Self()))
    return;
  endif

  AISys_Handler_AddTrigger(":ngd_npcs:is_member", AIHT_CAN_SPEAK_WITH, AIHF_RESULT_WAIT);
  
endfunction

//-----------------------------------------------------------------------------
