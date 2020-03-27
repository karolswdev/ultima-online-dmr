use util;

const ACPROP_TOPLIST := "TopList";
const TL_DATE_FS := "%d-%m-%Y";

function SetDefTLParam(who)
  var TLparam :={1, "dd-mm-yyyy"};
  who.acct.SetProp(ACPROP_TOPLIST, TLParam);
  return TLparam;
endfunction