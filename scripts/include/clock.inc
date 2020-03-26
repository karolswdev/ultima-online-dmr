//-----------------------------------------------------------------------------

use uo;
use cfgfile;

//-----------------------------------------------------------------------------

include "include/misc";
include "include/str";
include "include/bits";

//================================================================================
//--------------------------------------------------------------------------------
// Game Time, Real Time & Sys Time
//--------------------------------------------------------------------------------
//================================================================================

const DAY_LENGTH                       := 86400;
const YEAR_LENGTH_S                    := 31536000;
const YEAR_LENGTH_L                    := 31622400;

const RLT_SDATETIME                    := 0;
const RLT_SDATE                        := 1;
const RLT_DAYTIME                      := 2;
const RLT_STIME                        := 3;
const RLT_SDAY                         := 4;
const RLT_MONTH                        := 5;

const DAYS_CFG                         := ":mcfg:days";

//-----------------------------------------------------------------------------

function RealTime(Time, Type := 0)

  var Days,RetStr := "";

  if(Time <= 0)
    RetStr := "00";
  else

    Days := CInt(Time / 86400);
    
    if(Days >= 1)
      case(Days)
        1:       RetStr := "1 dzien";
        default: RetStr := Days + " dni";
      endcase
      Time := Time - (Days * 86400);
    endif

    if(Time)
      if(RetStr["d"])
        RetStr += " ";
      endif

      Days := CInt(Time / 3600);

      if(Days >= 1)
        RetStr := RetStr + ZeroFill(Days, 2) + ":";
        Time   := Time - (Days * 3600);
      elseif(Days == 0 and (RetStr["d"] or Type == RLT_STIME))
        RetStr := RetStr + "00:";
      endif
      
      Days := CInt(Time / 60);

      if(Days >= 1)
        RetStr := RetStr + ZeroFill(Days, 2) + ":";
        Time   := Time - (Days * 60);
      elseif(Days == 0 and (RetStr[":"] or Type == RLT_STIME))
        RetStr := RetStr + "00:";
      endif

      if(Time >= 0)
        RetStr := RetStr + ZeroFill(Time, 2);
      endif
    endif
    
  endif
  
  return RetStr;
  
endfunction

//-----------------------------------------------------------------------------

function ShortRealTime(Time)

  var Days,RetStr := "";

  if(Time <= 0)
    RetStr := "00";
  else

    Days := CInt(Time / 86400);

    if(Days >= 1)
      RetStr := Days+"d";
      Time := Time - (Days * 86400);
    endif

    if(Time)
      if(RetStr["d"])
        RetStr += " ";
      endif

      Days := CInt(Time / 3600);

      if(Days >= 1)
        RetStr := RetStr + ZeroFill(Days, 2) + ":";
        Time   := Time - (Days * 3600);
      elseif(Days == 0 and RetStr["d"])
        RetStr := RetStr + "00:";
      endif

      Days := CInt(Time / 60);

      if(Days >= 1)
        RetStr := RetStr + ZeroFill(Days, 2) + ":";
        Time   := Time - (Days * 60);
      elseif(Days == 0 and RetStr[":"])
        RetStr := RetStr + "00:";
      endif

      if(Time >= 0)
        RetStr := RetStr + ZeroFill(Time, 2);
      endif
    endif

  endif

  return RetStr;

endfunction

//-----------------------------------------------------------------------------

const GTC_WEEKS      := 0x01;
const GTC_NO_MINUTES := 0x02;
const GTC_REAL       := 0x04;

//-----------------------------------------------------------------------------

function GameTimeComponents(Time, Flags := 0)

  // IMPORTANT NOTE!!!
  // Modulo kurwa nie dziala! Przez glupiego pola stracilem godzine, bo duren nie wywala bledu na modulo, a zwraca zawsze ta sama liczbe.
  // dla pola 100%30 jest 100!

  Time := Max(Time, 0);

  var Components := struct;
  Components.+Weeks := 0;
  Components.+Days := 0;
  Components.+Hours := 0;
  Components.+Minutes := 0;
  Components.+Seconds := 0;
  
  Components.+Time := Time;

  if(Time)

    var DLen, HourLen, MinLen;

    if(Flags & GTC_REAL)
      DLen := 86400.0;
      HourLen := 3600.0;
      MinLen := 60.0;
    else
      DLen  := GetDayLength();
      HourLen := CDBl(DLen / 24.0);
      MinLen  := CDBl(DLen / (24.0 * 60.0));
    endif

    if(Flags & GTC_NO_MINUTES)
      // wycinamy minuty z czasu, i jesli sa dodajemy pelna godzine
      // zostawiamy minuty tylko, gdy czas jest mniejszy niz 1 godzina

      var Hours := CInt(Time / HourLen);
      var TimeLeft := Time - Hours*HourLen;

      if(TimeLeft > 0 and Time >= HourLen)
        Time := (Time - TimeLeft) + HourLen; 
      endif
    endif

    if(Flags & GTC_WEEKS)
      var WLen  := DLen*7;
      Components.Weeks := CInt(Time / WLen);
      Time := Time - (Components.Weeks*WLen);
    endif

    Components.Days  := CInt(Time / DLen); 
    Time := Time - (Components.Days*DLen);

    Components.Hours := CInt(Time / HourLen);
    Time := Time - (Components.Hours*HourLen);
    Components.Minutes := CInt(Time / MinLen);
    Components.Seconds := (Time - (Components.Minutes*MinLen)) / (DLen/86400.0);
  endif

  return Components;

endfunction

//-----------------------------------------------------------------------------

function GameTime(Time)

  var RetStr := "";
  var Components := GameTimeComponents(Time);

  if(Time <= 0)
    RetStr := "Brak";
  else

    if(Components.Days)
      case(Components.Days)
        1:       RetStr := Components.Days + " dzien";
        default: RetStr := Components.Days + " dni";
      endcase
    endif

    if(Components.Minutes or Components.Hours)
      if(RetStr)
        RetStr := RetStr + " ";
      endif
      RetStr := RetStr + ZeroFill(Components.Hours, 2) + ":" + ZeroFill(Components.Minutes, 2);
    elseif(Components.Seconds and !RetStr)
      RetStr := Components.Seconds + "s";
    endif

  endif

  return RetStr;

endfunction

//-----------------------------------------------------------------------------

function DescGameTime(Time)
  
  return DescTime(Time, GTC_NO_MINUTES);

endfunction

//-----------------------------------------------------------------------------

function DescRealTime(Time)

  return DescTime(Time, GTC_REAL);

endfunction 

//-----------------------------------------------------------------------------

function DescTime(Time, Flags)

  if(Time <= 0)
    return "Brak czasu";
  endif

  var Components := GameTimeComponents(Time, GTC_WEEKS | Flags);
  var TimeStr := "";

  if(Components.Weeks)
    if(Components.Weeks == 1)
      TimeStr := Components.Weeks + " tydzien";
    else
      case(Modulo(Components.Weeks, 10))
        2:       
        3:
        4:       TimeStr := Components.Weeks + " tygodnie";
        default: TimeStr := Components.Weeks + " tygodni";
      endcase
    endif
  endif

  if(Components.Days)
    if(TimeStr)
      TimeStr := TimeStr + ", "; 
    endif

    case(Components.Days)
      1:       TimeStr := TimeStr + "1 dzien";
      default: TimeStr := TimeStr + Components.Days+" dni";
    endcase
  endif

  if(Components.Hours)
    if(TimeStr)
      TimeStr := TimeStr + ", " + Components.Hours + " godzin"+Gam(Components.Hours, GAM_A);
    else
      TimeStr := Components.Hours + " godzin"+Gam(Components.Hours, GAM_A);
    endif
  endif

  if(!TimeStr or !IsSetFlag(Flags, GTC_NO_MINUTES))
    if(Components.Minutes)
      if(TimeStr)
        TimeStr := TimeStr + " ";
      endif
      TimeStr += Components.Minutes + " minut" + Gam(Components.Minutes, GAM_A);
    elseif(!TimeStr)
      TimeStr := "Mniej niz minuta";
    endif
  endif

  return TimeStr;

endfunction

//-----------------------------------------------------------------------------

function RealHourTime(Time)

  var Days,RetStr := "";

  if(Time <= 0)
    RetStr := "0 s";
  else
    Days := CInt(Time / 3600);
    if(Days >= 1)
      RetStr := Days + " h";
    else
      Days := CInt(Time / 60);
      if(Days >= 1)
        RetStr := Days + " min";
      else
        if(Time >= 0)
          RetStr := Time + " s";
        endif
      endif
    endif
  endif

  return RetStr;

endfunction

//-----------------------------------------------------------------------------

function GetDayLength()
  var DCfg := ReadConfigFile(DAYS_CFG);
  return CInt(DCfg["WholeDay"].Length) * 60;
endfunction

//-----------------------------------------------------------------------------

function GetTime(Time := 0)

  var DLen  := GetDayLength();
  if(!Time)
    Time  := ReadGameClock();
  endif
  
  return Time - (DLen * CInt(Time / DLen));

endfunction

//-----------------------------------------------------------------------------

function GetTimeofDay(Time := 0)

  var LTime     := GetTime(Time);
  var DLen      := GetDayLength();
  var Hours     := CInt(LTime / CDBl(DLen / 24.0));
  LTime         := LTime - (Hours * CDBl(DLen / 24.0));
  
  var Minutes   := CInt(LTime / (CDbl(DLen / (24.0 * 60.0))));
  
  return ZeroFill(Hours, 2) + ":" + ZeroFill(Minutes, 2);

endfunction

//-----------------------------------------------------------------------------

function GetHourOfDay()

  return CInt(GetTime() / 24);

endfunction

//-----------------------------------------------------------------------------

function GetHour()

  var LTime     := GetTime();
  var DLen      := GetDayLength();
  
  return CInt(LTime / CDBl(DLen / 24.0));

endfunction

//-----------------------------------------------------------------------------

function ConvertToTime(Val)

  Val := Val % 24;

  if(Val < 0)
    Val := Val + 24;
  endif
  
  return Val;
  
endfunction

//-----------------------------------------------------------------------------

function GetMonths()

  var DCfg   := ReadConfigFile(DAYS_CFG);
  return GetConfigStringArray(DCfg["Months"],"Month");

endfunction

//-----------------------------------------------------------------------------

function GetDays()

  var DCfg   := ReadConfigFile(DAYS_CFG);
  return GetConfigStringArray(DCfg["Days"],"Day");

endfunction

//-----------------------------------------------------------------------------

function GetYearDays()

  var DCfg   := ReadConfigFile(DAYS_CFG);
  return GetConfigInt(DCfg["Year"],"Days");

endfunction

//-----------------------------------------------------------------------------

function GetDayNameByTime(Time := 0)

  if(!Time)
    Time := ReadGameClock();
  endif

  var AllDay := GetDays();
  var ADays  := CInt(Time / GetDayLength());
  var Weeks  := CInt(ADays / 7);
  var LDay   := ADays - (Weeks * 7) + 1;
  
  return AllDay[LDay];

endfunction

//-----------------------------------------------------------------------------

function GetDayName(Num)

  var AllDay := GetDays();

  if(Num >= 1 and Num <= AllDay.size())
    return AllDay[Num];
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetYear(Time := 0)

  if(!Time)
    Time := ReadGameClock();
  endif
  var DLen   := GetDayLength();
  return CInt(Time / (GetYearDays() * DLen)) + 1;

endfunction

//-----------------------------------------------------------------------------

function GetDay(Time := 0)

  if(!Time)
    Time := ReadGameClock();
  endif

  var DLen   := GetDayLength();
  var YDays  := GetYearDays();

  var Year   := CInt(Time / (YDays * DLen));
  var LTime  := Time - (Year * (YDays * DLen));

  return CInt(LTime / DLen) + 1;

endfunction

//-----------------------------------------------------------------------------

function GetMonth(Time := 0)

  var DCfg := ReadConfigFile(DAYS_CFG);
  var Day  := GetDay(Time);
  var Month;

  foreach Mon in GetMonths()
    if(Day > CInt(DCfg[Mon].Days))
      Day := Day - CInt(DCfg[Mon].Days);
    else
      Month := Mon;
      break;
    endif
  endforeach

  return CInt(DCfg[Month].Nr);

endfunction

//-----------------------------------------------------------------------------

function GetMonthDay(Time := 0)

  var DCfg := ReadConfigFile(DAYS_CFG);
  var Day  := GetDay(Time);

  foreach Mon in GetMonths()
    if(Day > CInt(DCfg[Mon].Days))
      Day := Day - CInt(DCfg[Mon].Days);
    else
      break;
    endif
  endforeach

  return Day;

endfunction

//-----------------------------------------------------------------------------

function GetMonthName(Time := 0)

  var DCfg := ReadConfigFile(DAYS_CFG);
  return CStr(DCfg[GetMonths()[GetMonth(Time)]].Desc);

endfunction

//-----------------------------------------------------------------------------

function GetDate(Time := 0)

  return ZeroFill(GetMonthDay(Time), 2)+"."+ZeroFill(GetMonth(Time), 2)+"."+ZeroFill(GetYear(Time), 2);

endfunction

//-----------------------------------------------------------------------------

function GetDescDate(Time := 0)

  var Desc := GetYear(Time) + " rok, "+GetMonthName(Time)+", "+GetMonthDay(Time)+" dzien, "+GetDayNameByTime(Time);

  if(GetTime(Time) > 0)
    Desc := Desc + ", " +GetTimeOfDay(Time);
  endif

  return Desc;

endfunction

//-----------------------------------------------------------------------------

function GetFullDate(Time := 0, WithTime := 1)

  var Desc := GetYear(Time) + " rok, "+GetMonthName(Time)+", "+GetMonthDay(Time)+" dzien, "+GetDayNameByTime(Time);

  if(WithTime)
    Desc += ", " +GetTimeOfDay(Time);
  endif

  return Desc;

endfunction

//-----------------------------------------------------------------------------

function CUnixTime(Time := 0,Type := 0)

  if(!Time)
    Time := polcore().systime;
  endif

  var DCfg    := ReadConfigFile(DAYS_CFG);
  var tMove   := CInt(DCfg["RealTime"].MoveTime);
  Time        := Time + tMove;

  var WeekDay := ((((Time / DAY_LENGTH) % 7) + 3) % 7) + 1;

  var Year    := 1970;
  var Month   := 1;
  var Day     := 0;
  var Change  := 2;
  var YearLen := YEAR_LENGTH_S;
  
  while(Time >= YearLen)
    Time := Time - YearLen;
    Year := Year + 1;
    
    Change := Change + 1;
    if(Change == 4)
      YearLen := YEAR_LENGTH_L;
      Change  := 0;
    else
      YearLen := YEAR_LENGTH_S;
    endif
  endwhile

  var MonthLen := GetConfigInt(DCfg["RealTime"],"MonLen"+Month);
  
  while(Time >= (MonthLen * DAY_LENGTH))
    Time  := Time - (MonthLen * DAY_LENGTH);
    Month := Month + 1;
    MonthLen := GetConfigInt(DCfg["RealTime"],"MonLen"+Month);
    if(Change == 0 and Month == 2)
      MonthLen := MonthLen + 1;
    endif
  endwhile
  
  Day  := 1 + (Time / DAY_LENGTH);
  Time := Time % DAY_LENGTH;

  if(Year >= 2000)
    Change := 0;

    if(Month > 3 and Month < 10)
      Change := 1;
    elseif(Month == 3 and Day > MonthLen-7)
      if(WeekDay == 7 and Time >= 7200 or MonthLen-Day < 7-WeekDay)
        Change := 1;
      endif
    elseif(Month == 10)
      if(!(Day > MonthLen-7 and (WeekDay == 7 and Time >= 7200 or MonthLen-Day < 7-WeekDay)))
        Change := 1;
      endif
    endif

    if(Change)
      Time  += 3600;
      Day   += Time / DAY_LENGTH;
      Month += Day  / (MonthLen+1);
      Time  := Time % DAY_LENGTH;
      Day   := ((Day-1) % MonthLen) + 1;
    endif
  endif
  
  case(Type)
    RLT_SDATE:      return ZeroFill(Year-2000, 2) + "-" +ZeroFill(Month, 2)+ "-" +ZeroFill(Day, 2);
    RLT_DAYTIME:    return Time;
    RLT_STIME:      return RealTime(Time);
    RLT_MONTH:      return ZeroFill(Month, 2)+ "-" +ZeroFill(Day, 2)+" "+RealTime(Time,RLT_STIME);
    RLT_SDAY:       return ZeroFill(Day, 2)+"d "+RealTime(Time,RLT_STIME);
    default:        return ZeroFill(Year-2000, 2) + "-" +ZeroFill(Month, 2)+ "-" +ZeroFill(Day, 2)+" "+RealTime(Time,RLT_STIME);
  endcase

endfunction

//-----------------------------------------------------------------------------

function MakeUnixTimeByDate(Date)

  var Split := SplitWords(Date, "-");

  if(Split.size() != 3)
    return error;
  endif

  var Year  := CInt(UnZeroFill(Split[1]));
  var Month := CInt(UnZeroFill(Split[2]));
  var Day   := CInt(UnZeroFill(Split[3]));

  if(len(Split[1]) == 2)
    Year += 2000;
  endif

  if(Year < 2000 or Year > 2100 or Month < 1 or Month > 12 or Day < 1 or Day > 31)
    return error;
  endif

  return MakeUnixTime(Year, Month, Day);

endfunction

//-----------------------------------------------------------------------------

function MakeUnixTime(Year, Month, Day, Hour := 0, Mins := 0, Sec := 0)

  var DCfg    := ReadConfigFile(DAYS_CFG);
  var tMove   := CInt(DCfg["RealTime"].MoveTime);
  var Time    := tMove * -1;

  var cYear   := 1970;
  var cMonth  := 1;
  var Change  := 2;
  var YearLen := YEAR_LENGTH_S;
  
  while(cYear < Year)
    Time := Time + YearLen;
    cYear := cYear + 1;

    Change := Change + 1;
    if(Change == 4)
      YearLen := YEAR_LENGTH_L;
      Change  := 0;
    elseif(Change == 1)
      YearLen := YEAR_LENGTH_S * 3;
      if(cYear + 2 < Year)
        Change := Change + 2;
        cYear  := cYear + 2;
      else
        YearLen := YEAR_LENGTH_S;
      endif
    endif
  endwhile
  
  var MonthLen := GetConfigInt(DCfg["RealTime"],"MonLen"+cMonth);
  
  while(cMonth < Month)
    Time  := Time + (MonthLen * DAY_LENGTH);
    cMonth := cMonth + 1;
    MonthLen := GetConfigInt(DCfg["RealTime"],"MonLen"+cMonth);
    if(Change == 0 and cMonth == 2)
      MonthLen := MonthLen + 1;
    endif
  endwhile
  
  Time := Time + ((Day-1) * DAY_LENGTH) + (Hour * 3600) + (Mins * 60) + Sec;

  if(Year >= 2000)
    var WeekDay := (((((Time+tMove) / DAY_LENGTH) % 7) + 3) % 7) + 1;

    Change := 0;

    if(Month > 3 and Month < 10)
      Change := 1;
    elseif(Month == 3 and Day > MonthLen-7)
      if(WeekDay == 7 and Hour >= 3 or MonthLen-Day < 7-WeekDay)
        Change := 1;
      endif
    elseif(Month == 10)
      if(!(Day > MonthLen-7 and (WeekDay == 7 and Hour >= 2 or MonthLen-Day < 7-WeekDay)))
        Change := 1;
      endif
    endif

    if(Change)
      Time  -= 3600;
    endif
  endif

  return Time;

endfunction

//-----------------------------------------------------------------------------

function ExtractTimeComponents(Time)

  var DCfg    := ReadConfigFile(DAYS_CFG);
  var tShift  := CInt(DCfg["RealTime"].ZoneShift);
  var Year    := 1970;
  var Month   := 1;
  var Day     := 0;
  var Change  := 2;
  var YearLen := YEAR_LENGTH_S;

  Time += tShift;

  var WeekDay := ((((Time / DAY_LENGTH) % 7) + 3) % 7) + 1;

  while(Time >= YearLen)
    Time := Time - YearLen;
    Year := Year + 1;
    
    Change := Change + 1;
    if(Change == 4)
      YearLen := YEAR_LENGTH_L;
      Change  := 0;
    else
      YearLen := YEAR_LENGTH_S;
    endif
  endwhile

  var MonthLen := GetConfigInt(DCfg["RealTime"],"MonLen"+Month);
  
  while(Time >= (MonthLen * DAY_LENGTH))
    Time  := Time - (MonthLen * DAY_LENGTH);
    Month := Month + 1;
    MonthLen := GetConfigInt(DCfg["RealTime"],"MonLen"+Month);
    if(Change == 0 and Month == 2)
      MonthLen := MonthLen + 1;
    endif
  endwhile
  
  Day  := 1 + (Time / DAY_LENGTH);

  Time := Time % DAY_LENGTH;

  var bSummerTime := 0;

  if(Year >= 2000)

    var Hour := CInt(Time / 3600);

    if(Month > 3 and Month < 10)
      bSummerTime := 1;
    elseif(Month == 3 and Day > MonthLen-7)
      if((WeekDay == 7 and Hour >= 3) or MonthLen-Day < 7-WeekDay)
        bSummerTime := 1;
      endif
    elseif(Month == 10)
      if(!(Day > MonthLen-7 and (WeekDay == 7 and Hour >= 2 or MonthLen-Day < 7-WeekDay)))
        bSummerTime := 1;
      endif
    endif

    if(bSummerTime)
      tShift += 3600;
      Time   += 3600;

      // trzeba zaktualizowac dzien, miech i czas
      Day    += CInt(Time / DAY_LENGTH);
      Month  += Day  / (MonthLen+1);
      Time   := Time % DAY_LENGTH;
      Day    := ((Day-1) % MonthLen) + 1;
    endif

  endif

  var sComps := struct;

  sComps.+year     := Year;
  sComps.+month    := Month;
  sComps.+day      := Day;
  sComps.+weekday  := WeekDay;

  sComps.+hour     := CInt(Time / 3600);
  sComps.+minute   := CInt((Time % 3600) / 60);
  sComps.+second   := CInt(Time % 60);
  sComps.+daytime  := Time;
  sComps.+shift    := tShift;
  
  return sComps;

endfunction

//-----------------------------------------------------------------------------

function GetTimeFromString(Str)

  if(Str[3] == ":" and len(Str) == 5)
    Str := SplitWords(Str, ":");
    return CInt(Str[1])*60 + CInt(Str[2]);
  endif

  return RetError("Nieprawidlowy format czasu!");

endfunction

//-----------------------------------------------------------------------------

function GetStringFromTime(Time)

  if(Time < 0)
    Time := 0;
  endif

  var Hour := CInt(Time / 60);
  var Minute := Modulo(Time, 60);

  if(Hour < 10) 
    Hour := "0" + Hour;
  elseif(Hour > 99)
    Hour := 99;
  endif
  
  if(Minute < 10) 
    Minute := "0" + Minute;
  elseif(Minute > 59)
    Minute := 59;
  endif

  return Hour+":"+Minute;

endfunction

//-----------------------------------------------------------------------------
