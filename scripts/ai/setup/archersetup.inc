//-----------------------------------------------------------------------------

include "default_setup";

//-----------------------------------------------------------------------------

var NextIdle       := ReadGameClock();
var runaway        := 0;

SetUpAr();
SaveAiSerial();
CheckLifeTimers();
check_frozen();
check_alignment();
InitWalk();

drop_dungeon_anchor();
