//-----------------------------------------------------------------------------

const DEFAULT_MAXUSES := 1500;

//-----------------------------------------------------------------------------

function UpdateBookName(runebook)

  var name   := GetObjProperty(runebook, "name");
  var charge := GetObjProperty(runebook, "charges");
  var Lname  := lower(name);
  if(!name or lname["napisal"])
    name := "Runebook";
  endif
  var Except := GetObjProperty(runebook,"Exp");
  if(!Except)
    Except := "";
  else
    Except := " " + Except;
  endif
  SetName(runebook,name + " [ladunki: " + charge + "]"+Except);
  
endfunction

//-----------------------------------------------------------------------------

function GetRuneBookMaxCharges(book)

  var maxcharges := GetObjProperty(book, "maxcharges");

  if(!CInt(maxcharges))
    maxcharges := 5;
    SetObjProperty(book,"maxcharges", maxcharges);
  endif

  return maxcharges;

endfunction

//-----------------------------------------------------------------------------

function GetRuneBookCharges(book)

  var charges := GetObjProperty(book, "charges");

  if(charges == error or charges < 0)
    charges := 0;
    SetObjProperty(book,"charges", charges);
  endif

  return CInt(charges);

endfunction

//-----------------------------------------------------------------------------

function GetRuneBookUses(book)

  var uses := GetObjProperty(book, "Uses");

  if(!CInt(uses))
    uses := 0;
    SetObjProperty(book, "Uses", uses);
  endif

  return uses;

endfunction

//-----------------------------------------------------------------------------

function GetRuneBookMaxUses(book)

  var maxuses := GetObjProperty(book,"MaxUses");

  if(!CInt(maxuses))
    maxuses := DEFAULT_MAXUSES;
    SetObjProperty(book,"MaxUses",maxuses);
  endif

  return maxuses;

endfunction

//-----------------------------------------------------------------------------
