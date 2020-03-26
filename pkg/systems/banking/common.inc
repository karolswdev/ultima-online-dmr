//-----------------------------------------------------------------------------

use storage;

//-----------------------------------------------------------------------------

include "include/objtypes/misc";
include "include/const/misc";
include "include/misc";
include "include/names";
include "include/logerror";

//---------------------------------------------------------------------

const CHEQUE_MIN_CASHOUT               := 1000;
const CHEQUE_SIGN_COST                 := 500;
const CHEQUE_MAX_CASHOUT               := 5000000;
const GOLD_MAX_CASHOUT                 := 600000;

const CREATE_BANK_ACCOUNT_COST         := 10000;

//================================================================================
//--------------------------------------------------------------------------------
// Bank
//--------------------------------------------------------------------------------
//================================================================================

function FindBankBox( character )

  if(!character or !character.isa(POLCLASS_MOBILE) or !character.serial)
    return 0;
  endif

  var worldbank     := OpenWorldBank();
  var bank_obj_name := BANKBOX_PREFIX + character.serial;
  var bankbox       := FindRootItemInStorageArea( worldbank, bank_obj_name );

  if(!bankbox)
    bankbox := CreateRootItemInStorageArea( worldbank, bank_obj_name, UOBJ_BANKBOX );
    LogNotice("[FindBankBox] "+GetRealName(character)+"/"+character.serial+" Tworzenie nowej skrzynki bankowej.", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
  endif
  
  if(!bankbox)
    LogError("[FindBankBox] "+GetRealName(character)+"/"+character.serial+" Nie udalo sie stworzyc skrzyni bankowej: "+bankbox, LOGF_SCRIPTS);
  endif
  
  return bankbox;
  
endfunction

//--------------------------------------------------------------------------------

function WipeBankBox( character )

  var worldbank := OpenWorldBank();
  var bank_obj_name := BANKBOX_PREFIX + character.serial;

  if(DestroyRootItemInStorageArea( worldbank, bank_obj_name ))
    LogNotice("[WipeBankBox] "+GetRealName(character)+"/"+character.serial+" Cyszczenie skrzynki bankowej.", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function OpenWorldBank()

  var bank := FindStorageArea( BANK_STORAGE );
  if (!bank)
    bank := CreateStorageArea( BANK_STORAGE );
  endif
  if (!bank)
    LogError("[OpenWorldBank] Nie udalo sie stworzyc globalnego banku!", LOGF_SCRIPTS);
  endif

  return bank;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// StoreBox
//--------------------------------------------------------------------------------
//================================================================================

function FindStoreBox( character )

  if(!character or !character.isa(POLCLASS_MOBILE) or !character.serial)
    return 0;
  endif

  var worldbank      := OpenWorldBank();
  var store_obj_name := STOREBOX_PREFIX + character.serial;
  var StoreBox       := FindRootItemInStorageArea( worldbank, store_obj_name );

  if(!StoreBox)
    StoreBox := CreateRootItemInStorageArea( worldbank, store_obj_name, UOBJ_STOREBOX );
    LogNotice("[FindStoreBox] "+GetRealName(character)+"/"+character.serial+" Tworzenie magazynu.", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
  endif
  
  if(!StoreBox)
    LogError("[FindStoreBox] "+GetRealName(character)+"/"+character.serial+" Nie udalo sie stworzyc magazynu: "+StoreBox, LOGF_SCRIPTS);
  endif
  
  return StoreBox;
  
endfunction

//-----------------------------------------------------------------------------

function WipeStoreBox( character )

  var worldbank := OpenWorldBank();
  var store_obj_name := STOREBOX_PREFIX + character.serial;

  if(DestroyRootItemInStorageArea( worldbank, store_obj_name ))
    LogNotice("[WipeStoreBox] "+GetRealName(character)+"/"+character.serial+" Czyszczenie magazynu", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HaveBankAccount(who)

  return GetBankAccountAmount(who) != error;

endfunction

//-----------------------------------------------------------------------------

function GetBankAccountAmount(Box)

  if(IsPlayer(Box))
    Box := FindBankBox(Box);
  endif
  

  
  if(Box.isa(POLCLASS_CONTAINER))
    return GetObjProperty(Box, "AccountAmount");
  endif 

  return 0;
  
endfunction

function SpendBankAccountAmount(Box, amount)

	if(IsPlayer(Box))
		Box := FindBankBox(Box);
	endif
	
	if(HaveBankAccount(Box) == error)
		return 0;
	endif
	
	if(Box.isa(POLCLASS_CONTAINER))

		var currentAmount := GetBankAccountAmount(Box);
		
		if(amount > currentAmount)
			return 0;
		endif
		
		SetObjProperty(Box, "AccountAmount", currentAmount - amount);
		return 1;
		
	endif
	
	return 0;
		
endfunction
//-----------------------------------------------------------------------------

