
//------------------------------------------------------------------------------------------------//
// includy                                                                                        //
//------------------------------------------------------------------------------------------------//

use os;
use uo;
use util;
use vitals;

//-----------------------------------------------------------------

include "include/misc";
include "include/effects";
include "include/gumpboxes";
include "include/client/fonts";
include "include/checks/stdchecks";
include "include/const/attributes";

//-----------------------------------------------------------------

//------------------------------------------------------------------------------------------------//
// Stale                                                                                          //
//------------------------------------------------------------------------------------------------//


//ObjType 
const BA_STONE    := 0xBA00;  //kamien areny
const BA_BORDER   := 0xBA01;  //ogrodzenie areny
const BA_WALL     := 0xBA02;  //niezniszczalna sciana
const BA_BLOCK    := 0xBA03;  //sciana, ktora mozna zniszczyc bomba
const BA_T1    := 6126; 
const BA_T2    := 6153; 

//CProps
////Lokacja: tablica(2 elementowa) zawierajaca struktore (x,y,realm) NW i SE areny.
  const BA_A_LOC      := "BALocalization";

////Tablica zawierajaca w 1 elemencie tablice seriali miejsca koncowego, w 2 miejsce startowe 1, w 3 miejsce startowe 2.
  const BA_A_T1       := "BAArrayTeam1";
  const BA_A_T2       := "BAArrayTeam2";

////Tablica zawierajaca w 1 elemencie tablice seriali miejsca koncowego, w 2 miejsce startowe 1, w 3 miejsce startowe 2.
  const BA_P_T1       := "BAPlayerTeam1";
  const BA_P_T2       := "BAPlayerTeam2";

////Tablice elementow(seriali) na arenie
  const BA_A_BORDER :=  "BAArrayBorder";
  const BA_A_WALL   :=  "BAArrayWall";
  const BA_A_BLOCK  :=  "BAArrayBlock";


////Jesli klocki sa ukryte
  const BA_H_BORDER :=  "BAHiddenBorder";
  const BA_H_WALL   :=  "BAHiddenWall";
  const BA_H_BLOCK  :=  "BAHiddenBlock";

////Czasy
  const BA_GAME_TIME  := "BAGameTime";    //Max czas gry
  const BA_START_TIME := "BAStartTime";   //Czas rozpoczecia gry

////CanDestroy
  const BA_CND        := "BACanNotDestroy";

////StoneInUse
  const BA_SIU        := "BAStoneInUse";

////GameScript
  const BA_GAME_SCRIPT := "BAGameScript";

////Lista stworzony obiektow podczas gry
  const BA_A_ITEM := "BAArrayItem";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Nazwy druzyn
const  BA_T1_NAME := "Lotherianie";
const  BA_T2_NAME := "Garwezianie";

//CZAS WALKI

const   BA_MIN_TIME := 5;
const   BA_DEFAULT_TIME := 10;
const   BA_MAX_TIME := 30;

//Sprawdzanie czy istnieje

const   BA_C_LOC    := 0x0001;
const   BA_C_BORDER := 0x0002; //raczej nie przypdatne
const   BA_C_WALL   := 0x0004;
const   BA_C_BLOCK  := 0x0008;


//Stala dla funkcji CreateIt 
const BA_M_Z := 0xFFFF; // Jesli item ma zostac wbudowany na najnizszej mozliwej pozycji

//Stala dla funkcji ChangeVisible 
const BA_HOP := 0xFFFF; // zmien widocznosc na przeciwna



//------------------------------------------------------------------------------------------------//
// Funkcje                                                                                        //
//------------------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------//
////  Checki, czy istnieja poszczegolne skladowe areny                                            //
//------------------------------------------------------------------------------------------------//

// array of params
function Check(stone, params, who := 0)

  if (!params.exists(1))  //jesli params nie jest tablica
    params := {params};
  endif
  foreach what in params
    if(!GetObjProperty(stone,what))
      if(who)
        SendSysMessage(who, GetCheckText(what), FONT_NORMAL, COLOR_RED);
      endif
      return 0;
    endif
  endforeach

  return 1;

endfunction

function GetCheckText (what)

    case (what)
      BA_A_LOC:         return "Nie zdefiniowano lokalizacji areny";
      BA_A_BORDER:      return "Nie zdefiniowano krawedzi areny";
      BA_A_WALL:        return "Nie zdefiniowano lokalizacji scian na arenie";
      BA_A_BLOCK:       return "Nie zdefiniowano lokalizacji blokow na arenie";
      BA_H_BORDER:      return "Plot nieukryty";
      BA_H_WALL:        return "Sciany nieukryte";
      BA_H_BLOCK:       return "Bloki nie ukryte";
      BA_A_T1:          return "Nie zdefiniowano lokacji druzyny 1";
      BA_A_T2:          return "Nie zdefiniowano lokacji druzyny 2";
      default:          return "Bledny parametr funkcji sprawdzajacej " + what;
    endcase

endfunction