//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const AI_DEF_EVENT_WAIT                := 30; // seconds
const AI_SLEEP_EVENT_WAIT              := 120; // czekamy 120s na evencik w trybie spania

const AI_NO_ACTION_SLEEP_WAIT          := 60; //900; // ile czasu bez zadnej akcji musi uplynac do zasniecia npeca, w sekundach
const AI_DEF_WAKE_RANGE                := 15; // domyslny zasieg na obudzenie npeca ze snu.

//-----------------------------------------------------------------------------
// Npc States
//-----------------------------------------------------------------------------

const NPCSTATE_PREPARING               := 1;
const NPCSTATE_WAITING                 := 2;
const NPCSTATE_SLEEPING                := 3;
const NPCSTATE_FIGHTING                := 4;

//-----------------------------------------------------------------------------
// Npc AI Sub system flags
//-----------------------------------------------------------------------------

const AI_SYS_MOVEMENT                  := 0x01;
const AI_SYS_SPEECH                    := 0x02;
const AI_SYS_MERCHANT                  := 0x04;
const AI_SYS_FIGHT                     := 0x08;

//-----------------------------------------------------------------------------
// Event Results
//-----------------------------------------------------------------------------

const EVRES_UNKNOWN                    := 0;

const EVRES_SUCCESS                    := 1;
const EVRES_PROCESS                    := 2;

const EVRES_FAILED                     := -1;

//-----------------------------------------------------------------------------
