//-----------------------------------------------------------------------------

const UOBJ_DONATE_DEATHRUNE := 0xAA43;
const UOBJ_DONATE_ROBE      := 0xAA43;
const UOBJ_DONATE_CLOAK     := 0xAA43;

//-----------------------------------------------------------------------------

function AddDonationCoinsFromReferal(who, Amount)

  var Acc := who.acct;

  amount := Cint(amount);

  var CurRoyals := Acc.GetProp("Royals");
  if(!CurRoyals)
    CurRoyals := dictionary;
  endif

  if(!CurRoyals["coins"])
    CurRoyals["coins"] := 0;
  endif
  
  if(!CurRoyals["refs"])
    CurRoyals["refs"] := 0;
  endif

  CurRoyals["coins"]    += Amount;
  CurRoyals["refs"]     += Amount;
  Acc.SetProp("Royals", CurRoyals);

  return 1;

endfunction

//-----------------------------------------------------------------------------