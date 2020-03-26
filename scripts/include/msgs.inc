//-----------------------------------------------------------------------------

include "include/client/cids";
include "include/client/fonts";

//-----------------------------------------------------------------------------

const GGM_ABYS := 1;
const GGM_ELA  := 2;
const GGM_AS   := 3;
const GGM_A    := 4;
const GGM_AM   := 8;

//=============================================================================
//-----------------------------------------------------------------------------
//                        Get Gender Message
//-----------------------------------------------------------------------------
// zwraca koncowke zaleznie od plci postaci (ew grafiki dla npeca)
// jesli jako who zostanie przekazana wartosc:
//  0 - zwroc meska odmiane
//  1 - zwroc zenska odmiane
// tak jak who.gender
//=============================================================================

function Ggm(who, type)

  var lFemale := 0;

  if(who == 1)
    lFemale := 1;
  elseif(who != 0)
    if(who.isa(POLCLASS_NPC))
      lFemale := (who.gender == 2) or (who.graphic == CID_HUMAN_FEMALE); // moby maja gender 1==male, 2==female, bo zero oznacza "losuj", oczywiscie jak ma grafe kobiety ludzika, to prop gender nie ma znaczenia
    else
      lFemale := who.gender or (who.graphic == CID_HUMAN_FEMALE); // 1==kobieta, 0==facet
    endif
  endif

  if(lFemale)

     case(type)
      1:  return "abys";
      2:  return "ela";
      3:  return "as";
      4:  return "a";
      5:  return "elas";
      6:  return "a";
      7:  return "nas";
      8:  return "am";
      9:  return "ka";
      10: return "a";
      11: return "";
      12: return "a";
      13: return "la";
      14: return "ka";
      15: return "nam";
      16: return "ja";
      17: return "las";
      18: return "ek";
      19: return "j";
      20: return "j";
      21: return "e";
      22: return "i";
      23: return "a";
      24: return "a";
      25: return "abym";
      26: return "alem";
     endcase

   else

    case(type)
      1:  return "bys";
      2:  return "al";
      3:  return "es";
      4:  return "";
      5:  return "ales";
      6:  return "y";
      7:  return "ienes";
      8:  return "em";
      9:  return "em";
      10: return "ym";
      11: return "e";
      12: return "ego";
      13: return "edl";
      14: return "";
      15: return "ienem";
      16: return "go";
      17: return "edles";
      18: return "ow";
      19: return "go";
      20: return "mu";
      21: return "";
      22: return "";
      23: return "i";
      24: return "m";
      25: return "bym";
      26: return "elam";
    endcase

  endif

  return "";

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
//                        Get Amount Message
//-----------------------------------------------------------------------------
// sufTable[1] = koncowka dla ilosci rownej 1
// sufTable[2] = koncowka dla koncowek 2,3,4 (22, 103, 144) z wylaczeniem 12,13,14 (112, 1314)
// sufTable[3] = koncowka dla pozostalej ilosci
//=============================================================================

const GAM_KE              := 1;
const GAM_YCH             := 2;
const GAM_A               := 3;
const GAM_OW              := 4;
const GAM_O               := 5;
const GAM_I               := 6;
const GAM_E               := 7;
const GAM_EK              := 8;
const GAM_KA              := 9;
const GAM_LAT             := 10;
const GAM_ECY             := 11;
const GAM_EA              := 12;

//-----------------------------------------------------------------------------

function Gam(amt, type)

  var sufTable := array;

  case(type)
    GAM_KE:   sufTable[1] := "ke";
              sufTable[2] := "ki";
              sufTable[3] := "ek";

    GAM_YCH:  sufTable[1] := "a";
              sufTable[2] := "e";
              sufTable[3] := "ych";

    GAM_A:    sufTable[1] := "a";
              sufTable[2] := "y";
              sufTable[3] := "";
              
    GAM_EA:   sufTable[1] := "a";
              sufTable[2] := "a";
              sufTable[3] := "e";

    GAM_OW:   sufTable[1] := "";
              sufTable[2] := "y";
              sufTable[3] := "ow";

    GAM_I:    sufTable[1] := "a";
              sufTable[2] := "e";
              sufTable[3] := "i";

    GAM_O:    sufTable[1] := "o";
              sufTable[2] := "a";
              sufTable[3] := "";

    GAM_EK:   sufTable[1] := "ek";
              sufTable[2] := "ki";
              sufTable[3] := "kow";

    GAM_KA:   sufTable[1] := "ka";
              sufTable[2] := "ki";
              sufTable[3] := "ek";

    GAM_LAT:  sufTable[1] := "rok";
              sufTable[2] := "lata";
              sufTable[3] := "lat";

    GAM_E:    sufTable[1] := "e";
              sufTable[2] := "y";
              sufTable[3] := "";
    
    GAM_ECY:  sufTable[1] := "ac";
              sufTable[2] := "ace";
              sufTable[3] := "ecy";

    default:  return "";
  endcase


  if(amt == 1)
    return sufTable[1];
  endif

  case(amt % 10)
    2:
    3:
    4:         if((amt % 100) >= 11 and (amt % 100) <= 14)
                 return sufTable[3];
               else
                 return sufTable[2];
               endif

    default:   return sufTable[3];
  endcase

endfunction

//-----------------------------------------------------------------------------

function SendBad(who, Msg, Color := COLOR_RED)

  SendSysMessage(who, Msg, FONT_NORMAL, Color);

endfunction

//-----------------------------------------------------------------------------

function SendExcep(who, Msg)

  SendSysMessage(who, Msg, FONT_NORMAL, COLOR_BLUE);

endfunction

//-----------------------------------------------------------------------------

function SendGood(who, Msg, Color := COLOR_GREEN)

  SendSysMessage(who, Msg, FONT_NORMAL, Color);

endfunction

//-----------------------------------------------------------------------------
