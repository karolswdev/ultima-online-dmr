//-----------------------------------------------------------------------------

const ISETCTRL_DATA          := ":itemsets:control";
const ISC_ELEM_GLOBAL        := "Global";

const PROP_ISET_CTRL_ID      := "IsetCtrlId";

//-----------------------------------------------------------------------------

function GetIsetCtrlElem(Id)

  var Data := GetIsetCtrlData();

  return Data.FindElement(CStr(Id));

endfunction

//-----------------------------------------------------------------------------

function GetIsetCtrlData()

  var Data := OpenDataFile(ISETCTRL_DATA);

  if(!Data)
    Data := CreateDataFile(ISETCTRL_DATA);
  endif

  if(!Data)
    LogError("GetTipsData: Nie udalo sie stworzyc ItemSetsCtrlData ["+ISETCTRL_DATA+"]");
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------
