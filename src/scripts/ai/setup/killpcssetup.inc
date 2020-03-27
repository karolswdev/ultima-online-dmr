//-----------------------------------------------------------------------------

include "default_setup";

//-----------------------------------------------------------------------------

var NextIdle       := ReadGameClock();

me.concealed := 0;

SetUpAr();
SaveAiSerial();
CheckLifeTimers();
check_frozen();
check_alignment();
InitWalk();

drop_dungeon_anchor();
