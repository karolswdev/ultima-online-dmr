var MAX_X, MAX_Y, MIN_Y, MIN_X, RLM;

MAX_X := 948;
MAX_Y := 641;

MIN_X := 932;
MIN_Y := 633;

RLM := "malas";

function CreateNewTile()

    var theX := RandomIntMinMax(MIN_X, MAX_X);
    
    var theY := RandomIntMinMax(MIN_Y, MAX_Y);
    
	if(theX && theY)
	    var theTile := CreateItemAtLocation(theX, theY, -90, 0x647, 1, "malas");
	    theTile.movable := 0;
	    PrintTextAbove(theTile, "*", FONT_NORMAL, COLOR_BLUE);
	    if(!theTile)
		AlertAdmins(theTile.errortext);
	    endif
	endif
	
endfunction

function CreateNewHealTile()

    var theX := RandomIntMinMax(MIN_X, MAX_X);
    var theY := RandomIntMinMax(MIN_Y, MAX_Y);
    
	if(theX && theY)
	    var theTile := CreateItemAtLocation(theX, theY, -90, 0x3888, 1, "malas");
	    theTile.movable := 0;
	    PrintTextAbove(theTile, "+", FONT_NORMAL, COLOR_BLUE);
	    if(!theTile)
			AlertAdmins(theTile.errortext);
	    endif
	endif
	
endfunction

function AnnounceRoD()

    BroadCast("[Ring Smierci] Za 5 minut nastapi kolejna batalia.", FONT_NORMAL, COLOR_RED);
    
    sleep(240);
    
    BroadCast("[Ring Smierci] Za dwie minuty otworza sie portale pod miastem Nowa Brytania.", FONT_NORMAL, COLOR_RED);
    
    sleep(60);
    
    BroadCast("[Ring Smierci] Za minute otworza sie portale na batalie.", FONT_NORMAL, COLOR_RED);
    
    sleep(60);
    
    RodGates();
    
endfunction

function RoDGates()

    // happy inn
    
    var theGate := CreateItemAtLocation(786, 1441, 0, 0x646, 1, "britannia_alt");
	if(theGate)
	    BroadCast("[Ring Smierci] Portal na batalie otworzyl sie pod Nowa Brytania.", FONT_NORMAL, COLOR_RED);
	    
	    sleep(60);
	    
	    BroadCast("[Ring Smierci] Za minute nastapi zamkniecie bram.", FONT_NORMAL, COLOR_RED);
	    
	    sleep(70);
	    
	endif
	
    DestroyItem(theGate);

endfunction

    
function CheckWinner(who)

    if(GetGlobalProperty("RoDMembers").size() > 1)
	return 0;
    else
	BroadCast("[Ring Smierci] Wygrana - " + who.name, FONT_NORMAL, COLOR_RED);
	sleepms(500);
	BroadCast("[Ring Smierci] Wygrana - " + who.name + "!", FONT_NORMAL, COLOR_RED);
	sleepms(500);
	BroadCast("[Ring Smierci] Wygrana - " + who.name + "!!", FONT_NORMAL, COLOR_RED);
	sleepms(500);
	BroadCast("[Ring Smierci] Wygrana - " + who.name + "!!!", FONT_NORMAL, COLOR_RED);
	
	if(!GetObjProperty(who, "HonorPoints"))
	    SetObjProperty(who, "HonorPoints", 12);
	else
	    SetObjProperty(who, "HonorPoints", CInt(GetObjProperty(who, "HonorPoints")) + 12);
	endif
    endif
    
endfunction
	
	    
    
    



