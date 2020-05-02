use uo;
use os;

const DEFAULT_REALM := "britannia";

function CreateMoongates()

  var moongates := {
    {1176,	2457,	0, "Cannath"},
    {1290,	682,	-1, "Tulos"},
    {1316,	3073,	0, "Naggaroth"},
    {598,	2617,	20, "Kopalnia Gliny"},
    {761,	284,	10, "Lodowa Osada"},
    {795,	873,   0, "Wedrowna Osada"}
      };
  var moongate;
  syslog( "Creating moongates." );
  foreach gate in moongates
    foreach thing in ListItemsNearLocation(gate[1], gate[2], gate[3], 0, 1);
      if(thing.objtype != 0x1887b)
        DestroyItem(thing);
      endif
    endforeach
	moongate := CreateItemAtLocation( gate[1], gate[2], gate[3], "systemmoongate2", 1, 1);
    SetObjProperty(moongate, "City", gate[4]);
    moongate.facing := 29;
    moongate.decayAt := 0;
    moongate.movable := 0;
  endforeach
endfunction
