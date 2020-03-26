//--------------------------------------------------------------------------------

include "include/staff";
include "include/misc";

//--------------------------------------------------------------------------------

const MAKROCHECK_TIME    := 900; //15 min
const MAKRO_REST_TIME    := 120;  //2 min
const MAKRO_MAX_SGCOUNT  := 3;

const MAKRO_WRONG_DELAY  := 21600;
const MAKRO_MISS_DELAY   := 43200;

const MAKRO_MISS_MAXNUM  := 3;
const MAKRO_WRONG_MAXNUM := 5;

const MAKRO_GAIN_BLOCK_TIME := 86400;

const MC_TYPE_LETTERS    := 0;
const MC_TYPE_EQUATION  := 1;
const MC_TYPE_TEXT      := 2;

//--------------------------------------------------------------------------------

function EraseMakroData()

  EraseGlobalProperty("MakroCheckSerials");

endfunction

//--------------------------------------------------------------------------------

function EraseFromMakro(who)

  var MakroCheckSers := GetMakroSers();
  EraseObjProperty(who,"#MakroReported");
  EraseObjProperty(who,"MakroCheckTimer");
  EraseObjProperty(who,"MakroGumpNotPressed");
  EraseObjProperty(who,"MakroLastLetterPressed");
  EraseObjProperty(who,"MakroGumpPressedAfterTime");
  if(who.serial in MakroCheckSers)
    MakroCheckSers.erase(retindex(MakroCheckSers,who.serial));
    SetMakroSers(MakroCheckSers);
    return 1;
  endif
  return 0;
  
endfunction
  
//--------------------------------------------------------------------------------

function ReportMakro(who,Let := 0)

  if(!Let)
    Let := {"Nic","N/A"};
  endif

  AlertSeers(who.name + " podejrzany makrownik. [.makro]",FONT_NORMAL,COLOR_BLUE);

  SetObjProperty(who,"MakroGumpNotPressed",CInt(GetObjProperty(who,"MakroGumpNotPressed")) + 1);
  SetObjProperty(who,"MakroLastLetterPressed",Let);

  if(GetObjProperty(who,"#MakroReported"))
    return;
  endif

  SetObjProperty(who,"#MakroReported",1);
  SetObjProperty(who,"MakroCheckTimer",ReadGameClock());

  var MakroSers := GetMakroSers();
  
  if(!(who.serial in MakroSers))
    MakroSers.append(who.serial);
    SetMakroSers(MakroSers);
  endif

endfunction

//--------------------------------------------------------------------------------

function SendMakrorer(who)

  if(!IsLoggedIn(who))
    return;
  endif
  
  var MakroPid := GetGlobCtrlProc(MAKROCHECK_SCNAME);
  if(MakroPid)
    var ev := array;
    ev.+type   := EVID_REPORTMAKRO;
    ev.+source := who;
    MakroPid.sendevent(ev);
  endif

endfunction

//--------------------------------------------------------------------------------

function GetRandomLetter()

 var Letters := GetMakroLetters();
 var Rand    := RandomDiceRoll("1d"+len(Letters));


 return Letters[Rand];

endfunction

//--------------------------------------------------------------------------------

function ShowMakroLetterGump(who, Text)

  start_script(":makrocheck:timed_close_gump", array(who, GetPid(), MAKRO_REST_TIME));
  
  var Rand1   := RandomInt(150);
  var Rand2   := RandomInt(20);
  var gMoveX  := 0;
  var gMoveY  := 0;
  var Clr     := 0;
  var toRead;
  var Type;
  var toWrite;
  
  var lChance := Max(10, CInt(GetObjProperty(who, "MakroDiff")));

  if(Text)

    if(len(Text) < 4)
      Type    := MC_TYPE_LETTERS;
      
    else
      Type  := MC_TYPE_TEXT;
      
    endif
    toWrite := Text;
    ToRead  := Text;

  elseif(HaveChance(lChance))
  
    Type    := MC_TYPE_EQUATION;
    toRead  := 1 + RandomInt(9);
    toWrite := RandomInt(10);

    if(toWrite > toRead)
      toWrite := toWrite + " - " + abs(toRead-toWrite);
    else
      toWrite := toWrite + " + " + abs(toRead-toWrite);
    endif

  else

    Type    := MC_TYPE_LETTERS;
    toRead  := "";
    for i := 0 to ToMin(RandomInt(6)-3,0)
      toRead  := toRead + GetRandomLetter();
    endfor
    toWrite := toRead;

  endif
  
  GInit(RandomInt(350), 30+RandomInt(350));
  GMode(GMODE_NORMAL, GSMODE_PACKET);
  GNoDispose();
  GNoMove();
  GNoClose();

  var gNr := CInt(GetObjProperty(who, "#LastMakroGumpNr"));
  if(!gNr)
   gNr := RandomInt(9);
   SetObjProperty(who, "#LastMakroGumpNr", gNr);
  endif

  case(gNr)
   1:       GGumpPic(540, 420, 1770); // alchemy
   2:       GGumpPic(530, 450, 1770);
   3:       GGumpPic(500, 460, 1770); // carpentry / tinker_wood / tinker_ingot / blacksmith / tailor
   4:       GGumpPic(400, 220, 1770); // skillwin
   5:       GGumpPic(400, 320, 1770); // skillwin
   6:       GGumpPic(400, 420, 1770); // skillwin
   7:       GGumpPic(400, 520, 1770); // skillwin
   8:       GGumpPic(350, 300, 1770); // inscription
  endcase

  case(RandomInt(10))
   0:          Clr := 0;
   1:          Clr := 1158;
   2:          Clr := 1153;
   3:          Clr := 1111;
   4:          Clr := 999;
   6:          Clr := 1420+RandomInt(4);
   7:          Clr := 1104+RandomInt(4);
   8:          Clr := 1204+RandomInt(4);
   default:    Clr := 1304+RandomInt(4);
  endcase

  GGumpPic( 0, 0, 0x9c4);

  GResPic( gMoveX+21,  gMoveY+17,   2620, 166+Rand2, 90+Rand2 );

  var r1 := RandomInt(100);
  var r2 := RandomInt(100);

  if(r1 < 50)
   GTextLine( gMoveX+40+(Rand2/2), gMoveY+27+(Rand2/2),  650,  "Smiertelniku daj nam");

    if(RandomInt(100) < 40)
     GTextLine(gMoveX+2000+Rand1+RandomInt(30), gMoveX+2000+Rand2+RandomInt(330), Clr, GetRandomLetter());
    endif
  endif

  if(r2 < 50)
    GTextLine( gMoveX+54+(Rand2/2), gMoveY+47+(Rand2/2),  650,  "Bogom znak zycia");
    
     
    if ( type == MC_TYPE_LETTERS and len(toWrite) < 2)
      GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac znak");
    else
      case(type)
      MC_TYPE_TEXT:     GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac tekst");
      MC_TYPE_LETTERS:  GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac znaki");
      MC_TYPE_EQUATION: GTextLine( gMoveX+35+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac wynik wyrazenia");
      endcase;
    endif;
  endif
   
  

  
  case(type)
    MC_TYPE_EQUATION: //GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac wyrazenie");
                      GGumpPic(gMoveX+Rand1, gMoveY,   0x98C);
                      GTextLine(gMoveX+10+Rand1, gMoveY+2, Clr, toWrite);

    MC_TYPE_LETTERS:  //GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac znaki");
                      GGumpPic(gMoveX+Rand1, gMoveY,   0x98C, __ifelse(RandomInt(3),0, RandomInt(400)+650));
                      if(!RandomInt(3))
                        GCheckerTrans(gMoveX+Rand1+6, gMoveY+5, 52, 13);
                      endif
                      GTextLine(gMoveX+15+Rand1+RandomInt(20), gMoveY+2, Clr, toWrite);

    MC_TYPE_TEXT:     //GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac tekst");
                      GGumpPic(gMoveX+10+Rand2, gMoveY,   0x98E);
                      GTextLine(gMoveX+20+Rand2+RandomInt(10), gMoveY+2, Clr, toWrite);
  endcase
  
/*
  if(Text)
    if(len(Text) < 10)
     GGumpPic(gMoveX+50+Rand2, gMoveY,   0x98D);
     GTextLine(gMoveX+60+Rand2+RandomInt(10), gMoveY+2, Clr, toWrite);
    else
     GGumpPic(gMoveX+10+Rand2, gMoveY,   0x98E);
     GTextLine(gMoveX+20+Rand2+RandomInt(10), gMoveY+2, Clr, toWrite);
    endif
  else
    GGumpPic(gMoveX+Rand1, gMoveY,   0x98C, 950+RandomInt(100));
    //  GCheckerTrans( gMoveX+Rand1, gMoveY, 80, 20);
    
    GTextLine(gMoveX+10+Rand1+RandomInt(30), gMoveY+2, Clr, toWrite);
  endif
*/
  if(RandomInt(100) < 30)
    GTextLine(gMoveX+1200+Rand1+RandomInt(30), gMoveX+1000+Rand2+RandomInt(330), Clr, GetRandomLetter());
  endif

  if(r1 >= 40)
    GTextLine( gMoveX+40+(Rand2/2), gMoveY+27+(Rand2/2),  650,  "Smiertelniku daj nam");
  endif

  if(r2 >= 50)
    GTextLine( gMoveX+54+(Rand2/2), gMoveY+47+(Rand2/2),  650,  "Bogom znak zycia");
    
    if ( type == MC_TYPE_LETTERS and len(toWrite) < 2)
      GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac znak");
    else
      case(type)
      MC_TYPE_TEXT:     GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac tekst");
      MC_TYPE_LETTERS:  GTextLine( gMoveX+62+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac znaki");
      MC_TYPE_EQUATION: GTextLine( gMoveX+35+(Rand2/2), gMoveY+67+(Rand2/2),  650,  "Wpisujac wynik wyrazenia");
      endcase;
    endif;
  endif

  if(RandomInt(100) < 30)
    GTextLine(gMoveX+1200+Rand1+RandomInt(30), gMoveX+1000+Rand2+RandomInt(330), Clr, GetRandomLetter());
  endif

  if(RandomInt(100) < 90)
    GTextLine(gMoveX+1000+Rand1+RandomInt(30), gMoveX+1500+Rand2+RandomInt(330), Clr, GetRandomLetter());
  endif

  GGumpPic(gMoveX+28+Rand2,  gMoveY+90+Rand2,   0x98C);
  GTextEntry(gMoveX+43+(Rand2/2), gMoveY+92+Rand2,  55, 20, 0, 2, "");

  if(RandomInt(100) < 50)
   GTextLine(gMoveX+1700+Rand1+RandomInt(30), gMoveX+900+Rand2+RandomInt(330), Clr, GetRandomLetter());
  endif

  GButton( gMoveX+100+Rand2, gMoveY+85+Rand2, 0x47B, 0x47C, 1);

  var Res := GSend(who);

  if(!Res.butid)
    return 0;
  endif

  Res := Res.text[2];

  if(lower(Res) == lower(toRead))
    return toRead;
  elseif(Res)
    return {Res,toWrite};
  elseif(Res == "")
    return {"Nic",toWrite};
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetMakroLetters()

  //return "AbBcCdDEfFgGhHjJkKmMnNpPQrRsStTVxXzZ%!@#&*?";
  return "AbBcCdDE";

endfunction

//--------------------------------------------------------------------------------

function GetRandomString(maxLen)

  var Str  := "", i;
  var sLen := RandomInt(maxLen);
  var Letters := GetMakroLetters();

  for(i:=0;i<=sLen;i:=i+1)
    Str := Str + Letters[RandomInt(Letters.size())+1];
  endfor

  return Str;

endfunction

//--------------------------------------------------------------------------------

function GetMakroSers()

  if(!GetGlobalProperty("MakroCheckSerials") or !GetGlobalProperty("MakroCheckSerials").size())
    SetGlobalProperty("MakroCheckSerials",{});
  endif
  return GetGlobalProperty("MakroCheckSerials");

endfunction

//--------------------------------------------------------------------------------

function SetMakroSers(Sers)

  return SetGlobalProperty("MakroCheckSerials",Sers);

endfunction

//--------------------------------------------------------------------------------

function AddSGWait(who, Id)

  var SGWait := GetObjProperty(who, "#SGumpWait");

  if(!SGWait or !SGWait.size())
    SGWait := dictionary;
  endif

  SGWait[Id] := PolCore().systime;
  SetObjProperty(who, "#SGumpWait", SGWait);
  SetObjProperty(who, "#SGumpLast", PolCore().systime );

endfunction

//--------------------------------------------------------------------------------

function RemSGWait(who, Id, Res)

  var SGWait := GetObjProperty(who, "#SGumpWait");

  if(!SGWait or !SGWait.size())
    SGWait := dictionary;
  else
    if(Res and !Res.size())
      SGWait.erase(Id);
    else
      SGWait[Id] := array(SGWait[Id], Res);
    endif
  endif

  var Cnt := 0;

  foreach Entry in SGWait
    if(!Entry.size())
      Cnt := Cnt+1;
    endif
  endforeach

  if(!Cnt)
    EraseObjProperty(who, "#LastMakroGumpNr");
  endif

  SetObjProperty(who, "#SGumpWait", SGWait);

endfunction

//--------------------------------------------------------------------------------

function GetSGWait(who)

  return GetObjProperty(who, "#SGumpWait");

endfunction

//--------------------------------------------------------------------------------

function GetMakroTimers(who)

  var Timers := GetObjProperty(who, "MakroTimers");

  if(!Timers)
    Timers := struct;
    Timers.+wrongNum := 0;
    Timers.+wrongTimer := 0;
    Timers.+missNum := 0;
    Timers.+missTimer := 0;
    Timers.+missAllNum := 0;
    Timers.+wrongAllNum := 0;
  endif


  var bChanged := 0;
  
  if(Timers.missTimer and Timers.missTimer + MAKRO_MISS_DELAY < ReadGameClock())
    Timers.missNum := 0;
    Timers.missTimer := 0;
    bChanged := 1;
  elseif(!Timers.missTimer and Timers.missNum)
    Timers.missNum := 0;
    bChanged := 1;
  endif
  
  if(Timers.wrongTimer and Timers.wrongTimer  + MAKRO_WRONG_DELAY < ReadGameClock())
    Timers.wrongNum := 0;
    Timers.wrongTimer := 0;
    bChanged := 1;
  elseif(!Timers.wrongTimer and Timers.wrongNum)
    Timers.wrongNum := 0;
    bChanged := 1;
  endif

  if(bChanged)
    SetObjProperty(who, "MakroTimers", Timers);
  endif

  return Timers;

endfunction

//--------------------------------------------------------------------------------

const MAKROF_MISS   := 0x01;
const MAKROF_WRONG  := 0x02;

//--------------------------------------------------------------------------------

function IncreaseMakroTimers(who, byref Timers, Flags)

  if(Flags & MAKROF_MISS)
    Timers.missTimer := ReadGameClock();
    Timers.missNum += 1;
    Timers.missAllNum += 1;
  endif

  if(Flags & MAKROF_WRONG)
    Timers.wrongTimer := ReadGameClock();
    Timers.wrongNum += 1;
    Timers.wrongAllNum += 1;
  endif

  SetObjProperty(who, "MakroTimers", Timers);

endfunction

//--------------------------------------------------------------------------------

function ResetMakroTimers(who, byref Timers, Flags)

  if(Flags & MAKROF_MISS)
    Timers.missTimer := 0;
    Timers.missNum := 0;
  endif

  if(Flags & MAKROF_WRONG)
    Timers.wrongTimer := 0;
    Timers.wrongNum := 0;
  endif

  SetObjProperty(who, "MakroTimers", Timers);

endfunction

//--------------------------------------------------------------------------------

function CheckMakroTimers(who, Flags)

  var Timers := GetMakroTimers(who);
  
  IncreaseMakroTimers(who, Timers, Flags);

  if(Flags & MAKROF_WRONG)
    if(Timers.wrongNum >= MAKRO_WRONG_MAXNUM)
      GainBlockTimed(who, MAKRO_GAIN_BLOCK_TIME);
      ResetMakroTimers(who, Timers, Flags);
      SendMakroInfoGump(who, "Blednie potwierdzil"+ggm(who,3)+" znak zycia po raz "+MAKRO_WRONG_MAXNUM+"!|Przyrost Twoich umiejetnosci oraz statystyk zostal|zablokowany na czas "+DescGameTime(MAKRO_GAIN_BLOCK_TIME)+"! Pilnuj sie!", 1);
    else
      SendMakroInfoGump(who, "Blednie potwierdzil"+ggm(who,3)+" swoj znak zycia!|Jesli to sie powtorzy "+(MAKRO_WRONG_MAXNUM - Timers.wrongNum)+" razy w przeciagu "+DescGameTime(MAKRO_WRONG_DELAY)+",|to nie bedziesz mogl"+ggm(who,4)+" dalej cwiczyc|przez "+DescGameTime(MAKRO_GAIN_BLOCK_TIME)+".", 0);
    endif
  endif
  
  if(Flags & MAKROF_MISS)
    if(Timers.missNum >= MAKRO_MISS_MAXNUM)
      GainBlockTimed(who, MAKRO_GAIN_BLOCK_TIME);
      ResetMakroTimers(who, Timers, Flags);
      SendMakroInfoGump(who, "Nie potwierdzil"+ggm(who,3)+" swojego istnienia|Nie dal"+ggm(who,3)+" znaku zycia po raz "+MAKRO_MISS_MAXNUM+"!|Przyrost Twoich umiejetnosci oraz statystyk zostal zablokowany|na czas "+DescGameTime(MAKRO_GAIN_BLOCK_TIME)+"! Pilnuj sie!", 1);
    else
      SendMakroInfoGump(who, "Nie potwierdzil"+ggm(who,3)+" swojego istnienia|Jesli to sie powtorzy "+(MAKRO_MISS_MAXNUM - Timers.missNum)+" razy w przeciagu "+DescGameTime(MAKRO_MISS_DELAY)+",|to nie bedziesz mogl"+ggm(who,4)+" dalej cwiczyc|przez "+DescGameTime(MAKRO_GAIN_BLOCK_TIME)+".", 0);
    endif
  endif

endfunction

//----------------------------------------------------------------------------

function SendMakroInfoGump(who, Text, isBad := 0)

  if(!IsLoggedIn(who))
    return;
  endif

  GInit(190, 20);
  GNoClose();
  GNoDispose();
  GResPic(0, 20, 0xA28, 400, 200);
  GGumpPic(350, 10, 0x589);
  GGumpPic(361, 20, 0x1580);
  GGumpPic(0, 15, 0x4ED);

  var Lines := SplitWords(Text, "|");
  var i;

  for(i:=1; i<=Lines.size(); i+=1)
    GTextLine(60, __IfElse(i==1, 40, 50)+i*20, __IfElse(i==1, __IfElse(isBad, 1211, 42), 1153), Lines[i]);
  endfor

  GButton(280, 160, 0x867, 0x869, 1);
  GTextLine(313, 163, 999, "Zamknij");

  GPacketSend(who);

endfunction

//--------------------------------------------------------------------------------

function SendMakroGump(char, bywho := 0)

  start_script(":makrocheck:showgump", array(char, bywho));

endfunction

//--------------------------------------------------------------------------------
