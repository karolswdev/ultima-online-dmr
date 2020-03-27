//-----------------------------------------------------------------------------
// funkcja dodajaca uwage na konto gracza
// Typy uwag:
//-----------------------------------------------------------------------------

const NOTETYPE_GM        := "G";
const NOTETYPE_JAIL      := "J";
const NOTETYPE_JAILMOD   := "M";

//-----------------------------------------------------------------------------

function AddPlayerNote(who, Obj, NoteType, NoteText)

  var Targ, TargAcc;
  var Res := error;

  if(IsPlayer(Obj))
    Targ := Obj;
    TargAcc := Targ.acct;
  elseif(Obj.get_member("enabled") != error)
    TargAcc := Obj;
  else
    Res.+errortext := "Bledny obiekt.";
    return Res;
  endif

  if(!NoteText)
    Res.+errortext := "Tekst uwagi jest pusty!";
    return Res;
  endif

  var Notes := TargAcc.GetProp("Notes");

  if(!Notes)
    Notes := {};
  endif

  if(lower(Notes[Notes.size()][3]) == lower(NoteText) and NoteType == NOTETYPE_GM)
    Res.+errortext := "Poprzednia notatka jest taka sama!";
    return Res;
  endif

  var Note := array;

  if(who)
    Note[1] := who.name;
    Note[2] := who.acctname;
  else
    Note[1] := "System";
    Note[2] := "";
  endif

  Note[3] := NoteText;
  Note[4] := PolCore().systime;
  Note[5] := NoteType;

  Notes.append(Note);
  TargAcc.SetProp("Notes", Notes);

  return 1;

endfunction

//-----------------------------------------------------------------------------
