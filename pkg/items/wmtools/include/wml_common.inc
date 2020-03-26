//-----------------------------------------------------------------------------

const WMULTIS_DATA           := ":wmtools:multis";

//-----------------------------------------------------------------------------

function GetWmlGroup(wmlId)

  if(wmlId == error)
    return RetError("Brak wmlId: "+wmlId);
  endif

  var Data := GetWmlData();

  var Res := Data.FindElement(CStr(CInt(wmlId)));

  if(!Res)
    return RetError("Nie znaleziono elementu WML: "+wmlId);
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function EraseWmlGroup(wmlId)

  var Data := GetWmlData();

  var Res := Data.DeleteElement(CStr(CInt(wmlId)));

  if(!Res)
    return RetError("Nie znaleziono elementu WML: "+wmlId);
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function GetWmlData()

  var Data := OpenDataFile(WMULTIS_DATA);

  if(!Data)
    Data := CreateDataFile(WMULTIS_DATA);
  endif

  if(!Data)
    LogError("GetWmlData: Nie udalo sie stworzyc WMULTIS_DATA ["+WMULTIS_DATA+"]: "+Data.errortext);
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------
