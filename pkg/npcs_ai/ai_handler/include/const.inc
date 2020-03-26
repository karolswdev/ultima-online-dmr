//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const AI_HANDLER_DEF_DURATION          := 300; // w sekundach, max czas dzialania skryptu asynchronicznego (start_script)
const AI_HANDLER_MAX_SYNC_DURATION     := 2000; // w milisekundach - max czas dzialania skryptu synchronicznego (run_script) - nie ubija skryptu jesli czas przekroczy, ale wyrzuca blad do LogError

//-----------------------------------------------------------------------------
// Handler Triggers
//-----------------------------------------------------------------------------

const AIHT_SUCCESSFUL_SELL             := 0x0001;
const AIHT_ITEM_GIVEN                  := 0x0002;
const AIHT_CAN_SPEAK_WITH              := 0x0004;
const AIHT_QUEST_MSG                   := 0x0008;
const AIHT_ON_WANDER                   := 0x0010;
const AIHT_ENTER_SLEEP                 := 0x0020; // moze przyjac jako result zasieg budzenia w enter/left area, w formie structa res.wakeRange
const AIHT_EXIT_SLEEP                  := 0x0040;

//-----------------------------------------------------------------------------

const AIHF_RESULT_WAIT                 := 0x0001;
const AIHF_PASS_DURATION               := 0x0002;

const AIHF_SEND_MERC_DATA              := 0x1000;

//-----------------------------------------------------------------------------
