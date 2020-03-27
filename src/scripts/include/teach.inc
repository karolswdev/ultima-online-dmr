//--------------------------------------------------------------------------------

include "include/client/fonts";
include "include/msgs";

//--------------------------------------------------------------------------------

const MAX_STUDS  := 3;
const TEACH_DIST := 2;
const TMSG_TIME  := 30;

//================================================================================
//--------------------------------------------------------------------------------
// Teaching
//--------------------------------------------------------------------------------
//================================================================================

function GetStuds(who)

  var Studs := GetObjProperty(who,"Studs");
  if(!Studs)
    Studs := {};
  endif

  var RStuds := {};

  foreach Stud in Studs
    Stud := FindPlayerBySerial(Stud);
    if(Distance(Stud,who) <= TEACH_DIST and IsLoggedIn(Stud) and CInt(GetObjProperty(Stud, "Learning")) == who.serial and !Stud.dead)
      RStuds.append(Stud);
    else
      SendSysMessage(Stud,who.name+" przestal cie nauczac.",FONT_NORMAL,COLOR_GREEN);
      SendSysMessage(who,"Przestal"+ggm(who,3)+" nauczac osobe "+Stud.name+".",FONT_NORMAL,COLOR_GREEN);
      EraseObjProperty(Stud,"Learning");
    endif
  endforeach

  Studs := {};

  foreach Stud in RStuds
    Studs.append(Stud.serial);
  endforeach

  SetObjProperty(who,"Studs",Studs);

  return RStuds;

endfunction

//-----------------------------------------------------------------------------

function AddStudent(who,Stud)

  if(IsLearning(who))
    SendSysMessage(who,"Juz ktos cie naucza...",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(IsLearning(Stud))
    SendSysMessage(who,"Juz ktos naucza osobe "+Stud.name+"...",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(IsTeaching(Stud))
    SendSysMessage(who,"Osoba "+Stud.name+" juz kogos naucza...",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Studs := GetStuds(who);

  if(Stud in Studs)
    SendSysMessage(who,"Juz nauczasz ta osobe.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(Studs.size() >= MAX_STUDS)
    SendSysMessage(who,"Mozesz nauczac jednoczescie maksymalnie "+MAX_STUDS+" osoby.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  SendSysMessage(Stud,"Czy chcesz by "+who.name+" cie nauczal"+ggm(who,4)+"?",FONT_NORMAL,COLOR_GREEN);

  if(!YesNo(Stud,"Tak ?"))
    SendSysMessage(who,Stud.name+" nie chce bys go nauczal"+ggm(who,4)+".",FONT_NORMAL,COLOR_RED);
    SendSysMessage(Stud,"Odmowil"+ggm(Stud,3)+" nauki przez osobe "+who.name+".",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(IsLearning(who))
    SendSysMessage(who,"Juz ktos cie naucza...",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(IsLearning(Stud))
    SendSysMessage(who,"Juz ktos naucza osobe "+Stud.name+"...",FONT_NORMAL,COLOR_RED);
    return;
  endif
  
  if(IsTeaching(Stud))
    SendSysMessage(who,"Osoba "+Stud.name+" juz kogos naucza...",FONT_NORMAL,COLOR_RED);
    return;
  endif

  Studs := GetStuds(who);

  if(Studs.size() >= MAX_STUDS)
    SendSysMessage(who,"Mozesz nauczac jednoczescie maksymalnie "+MAX_STUDS+" osoby.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  SendSysMessage(who,"Nauczasz teraz "+Stud.name+"...",FONT_NORMAL,COLOR_GREEN);
  SendSysMessage(Stud,who.name+" bedzie cie teraz uczyc...",FONT_NORMAL,COLOR_GREEN);

  foreach Stud in Studs
    Stud := Stud.serial;
  endforeach

  Studs.append(Stud.serial);
  SetObjProperty(Stud,"Learning",who.serial);
  SetObjProperty(who,"Studs",Studs);

endfunction

//-----------------------------------------------------------------------------

function IsTeaching(who)

  return GetStuds(who).size();

endfunction

//-----------------------------------------------------------------------------

function IsLearning(who)

  var Teacher := GetObjProperty(who,"Learning");

  if(Teacher)
    Teacher := FindPlayerBySerial(Teacher);
    if(Teacher)
      if(Distance(Teacher,who) <= TEACH_DIST and IsLoggedIn(Teacher))
        return 1;
      endif
    endif
    EraseObjProperty(who,"Learning");
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function TeachStuds(who,SkID,Gain,Diff,Elem)

  if(Elem.NoTeach)
    return 0;
  endif

  var Studs := GetStuds(who);

  if(Studs.size())
    var Skill := CInt(GetBaseSkill(who,SkID) / 2);

    if(!Skill)
      return 0;
    endif

    foreach Stud in Studs
      if(GetBaseSkill(Stud,SkID) < Skill)
        SendTeachMsg(Stud,who.name+" naucza cie umiejetnosci "+Elem.name+"...");
        SendTeachMsg(who,"Nauczasz "+Stud.name+" umiejetnosci "+Elem.name+"...");
        AwardPoints(Stud,SkID,Gain,TEACH_AWARD,Diff);
      else
        SendTeachMsg(who,"Nauczyl"+ggm(who,3)+" "+Stud.name+" wszystkiego czego mogl"+ggm(who,3)+" o umiejetnosci "+Elem.name+".");
        SendTeachMsg(Stud,who.name+" nauczyl"+ggm(who,4)+" cie wszystkiego czego mogl"+ggm(who,4)+" o umiejetnosci "+Elem.name+".");
      endif
    endforeach
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendTeachMsg(who,Msg)

  if(CInt(GetObjProperty(who,"#TeachMsg")) + TMSG_TIME < ReadGameClock())
    SendSysMessage(who,Msg,FONT_NORMAL,COLOR_GREEN);
    SetObjProperty(who,"#TeachMsg",ReadGameClock());
  endif

endfunction

//-----------------------------------------------------------------------------
