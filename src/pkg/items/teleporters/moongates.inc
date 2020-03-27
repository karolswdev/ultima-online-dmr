use uo;
use os;
use cfgfile;

include "include/items";

const UOBJ_MOONGATE := 0x16202;

function CreateMoongates()

    var MoonCfg  := ReadConfigFile(MOONGATE_CFG);
    var Cities   := GetConfigStringKeys(MoonCfg);
    var MoonGate := 0;

    foreach City in Cities
        MoonGate := 0;
        foreach Item in ListItemsNearLocation(CInt(MoonCfg[City].X),CInt(MoonCfg[City].Y),CInt(MoonCfg[City].Z), CInt(MoonCfg[City.Realm]))
	    print("Trying to create moongate for " + City);
            if(Item.objtype == UOBJ_MOONGATE)
                MoonGate := Item;
            else
				  PrintTextAbove(item, "*");
//                DestroyItem(Item);
            endif
        endforeach
        if(!MoonGate)
	    print("No moongate in " + City + " ... creating");
            MoonGate := CreateItemAtLoc(CInt(MoonCfg[City].X),CInt(MoonCfg[City].Y),CInt(MoonCfg[City].Z), UOBJ_MOONGATE, 1, MoonCfg[City].Realm);
			if(Moongate)
				print("Created moongate for " + City);
			else
				print("MOONGATE WAS NOT CREATED FOR CITY... WHY? " + City);
			endif
        endif
        if(!GetObjProperty(MoonGate,"City"))
            SetObjProperty(MoonGate,"City",City);
        elseif(GetObjProperty(MoonGate,"City") != City)
            SetObjProperty(MoonGate,"City",City);
        endif
        MoonGate.facing := 29;
        MoonGate.movable := 0;
    endforeach

endfunction

