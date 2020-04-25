///////////////////////////////////////////////////////////////////////////////
// bank.inc: global bankbox functions
//
// public functions:
//      FindBankBox( character)      returns World Bank bankbox for a character
//      WipeBankBox( character )     does what you might think.
//      OpenWorldBank()              returns handle to World Bank.
///////////////////////////////////////////////////////////////////////////////
use os;

include "include/objtypes/misc";
include "include/items";

function FindBBankBox(bankser)

    var worldbank       := OpenWorldBBank();
    var bank_obj_name   := "Bankbox of " + bankser;
    var bankbox         := FindRootItemInStorageArea(worldbank, bank_obj_name);

    if(!bankbox)
        bankbox := CreateRootItemInStorageArea( worldbank, bank_obj_name, 0xe76 );
        syslog("Tworzenie nowej skrzynki dla worka dla  " + bankbox.name);
    endif

    if(bankbox.objtype != UOBJ_BANKBOX)
        var Storag := CreateItemAtLoc(6104, 1183, 0, UOBJ_BANKBOX, 1);
        if(Storag)
            print("przenoszenie przedmiotow ze starej do nowej skrzynki bankowej");
            foreach item in ListRootItemsInContainer(bankbox)
                MoveItemToContainer(item,Storag);
            endforeach
            if(DestroyRootItemInStorageArea( worldbank, bank_obj_name ))
                print("niszczenie starej skrzynki" + bankbox.name);
                bankbox := CreateRootItemInStorageArea( worldbank, bank_obj_name, 0xe76);
                print("tworzenie nowej " + bankbox.name);
            endif
            foreach item in ListRootItemsInContainer(Storag)
                MoveItemToContainer(item,bankbox);
            endforeach
            DestroyItem(Storag);
        endif
    endif

    if(!bankbox)
        syslog("Unable to find or create bankbox for " + bankser );
    endif

    return bankbox;

endfunction

function WipeBBankBox(bankser)

    var worldbank       := OpenWorldBank();
    var bank_obj_name   := "Bankbox of " + bankser;
    return DestroyRootItemInStorageArea(worldbank, bank_obj_name);

endfunction

function OpenWorldBBank()

    var bank := FindStorageArea("Bags of Banking");
    if(!bank)
        bank := CreateStorageArea("Bags of Banking");
    endif

    if(!bank)
        syslog("Unable to open or create bags of banking!");
    endif

    return bank;

endfunction
