//--------------------------------------------------------------------------------

include "warcasts";
include "include/objtypes/magic";

//================================================================================
//--------------------------------------------------------------------------------
// War Cast Book
//--------------------------------------------------------------------------------
//================================================================================

const PROP_WCBOOK_IDS        := "WcIds";

//--------------------------------------------------------------------------------

function IsInWarCastBook(Book, wcId)

  var wcIds := GetObjProperty(Book, PROP_WCBOOK_IDS);

  if(!wcIds or !wcIds.size())
    return 0;
  else
    return (wcId in wcIds);
  endif

endfunction

//--------------------------------------------------------------------------------

function AddToWarCastBook(Book, wcId)

  var wcIds := GetObjProperty(Book, PROP_WCBOOK_IDS);

  if(!wcIds or !wcIds.size())
    wcIds := array;
  endif

  if(!(wcId in wcIds))
    wcIds.append(wcId);
    SetObjProperty(Book, PROP_WCBOOK_IDS, wcIds);
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function EraseFromWarCastBook(Book, wcId)

  var wcIds := GetObjProperty(Book, PROP_WCBOOK_IDS);

  if(!wcIds or !wcIds.size())
    return 0;
  endif
  
  var Idx := RetIndex(wcIds, wcId);

  if(Idx)
    wcIds.erase(Idx);
    SetObjProperty(Book, PROP_WCBOOK_IDS, wcIds);
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------
