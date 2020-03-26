//-----------------------------------------------------------------------------

const AI_MOVEMENT_MAX_ERRORS           := 16;
const AI_MOVEMENT_CHECK_COUNT          := 5;  // po ilu cyklach sprawdzac, czy do NPC jest blokowany widok.
const AI_MOVEMENT_STUCK_COUNT          := 48; // po ilu cyklach sprawdzac, czy NPC oddalil sie od poprzedniej lokacji (coutner resetowany, gdy dotrze do celu)
const AI_MOVEMENT_KEEP_DIST_COUNT      := 15; // ilosc powtorzen, a wlasciwie sekund po ktorych NPC stwierdzi, ze powinien uciekac, jesli ofc bedzie w trybie uciekajacym (flee, runaway, keep dist)
const AI_MOVEMENT_MIN_KEEP_DIST_COUNT  := -40; // do ilu powtorzen uciekac, az mu sie znudzi (w przypadku, gdy NPC bedzie goniony)
const AI_MOVEMENT_FROZEN_MAX_COUNT     := 60;  // po ilu cyklach (sekundach) npc powinien zrezygnowac z celu, jesli jest zamrozony/sparalizowany

const AI_MOVEMENT_DEF_PAUSE_TIME       := 30;
const AI_MOVEMENT_MAX_PATH_DIST        := 75;

const AI_MOVEMENT_SAFE_DISTANCE        := 14;
const AI_MOVEMENT_GOOD_DISTANCE        := 10;

const AI_DEF_WANDER_WAIT               := 30; // seconds

//-----------------------------------------------------------------------------

const AI_MOVETYPE_RUN                  := 1;
const AI_MOVETYPE_WALK                 := 2;
const AI_MOVETYPE_FOLLOW_RUN           := 3;
const AI_MOVETYPE_FOLLOW_WALK          := 4;
const AI_MOVETYPE_FLEE                 := 5;
const AI_MOVETYPE_RUN_AWAY             := 6;
const AI_MOVETYPE_KEEP_DISTANCE        := 7;

//-----------------------------------------------------------------------------
