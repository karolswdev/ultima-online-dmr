use uo;
use os;

const DEFAULT_REALM := "britannia_alt";

function CreateMoongates()

  var moongates := {
    {769,	1431,	0, "Nowa Britannia"},
    {1747,	552,	0, "Iceport"},
    {1703,	907,	0, "Castle Rock"},
    {1301,	1948,	-1, "Heartrock"},
    {1998,	2547,	1, "Haddinge"},
    {1522,	1681,   0, "Morska Osada"}
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
