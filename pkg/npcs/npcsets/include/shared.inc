//-----------------------------------------------------------------------------

const NSETCTRL_DATA          := ":npcsets:control";
const NSC_ELEM_GLOBAL        := "Global";

const PROP_NSET_CTRL_ID      := "NsetCtrlId";

//-----------------------------------------------------------------------------

function GetNsetCtrlElem(Id)

  var Data := GetNsetCtrlData();

  return Data.FindElement(CStr(Id));

endfunction

//-----------------------------------------------------------------------------

function GetNsetCtrlData()

  var Data := OpenDataFile(NSETCTRL_DATA);

  if(!Data)
    Data := CreateDataFile(NSETCTRL_DATA);
  endif

  if(!Data)
    LogError("GetTipsData: Nie udalo sie stworzyc NSETCTRL_DATA ["+NSETCTRL_DATA+"]");
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------
