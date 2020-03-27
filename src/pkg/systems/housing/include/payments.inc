//-----------------------------------------------------------------------------

include "common";
include ":banking:gold";

include ":mail:mail";

//-----------------------------------------------------------------------------

const HOUSEPAY_PERIODS       := 4;
const HOUSEPAY_LOCK_PERIODS  := 2;
const HOUSEPAY_PERIOD_TIME   := 2419200; // 28 dni // to ile ma miec jeden okres platnosci 
const HOUSEPAY_COST_TIME     := 2419200; // 28 dni // dla jakiego czasu liczona jest kasa
const HOUSEPAY_MAX_PERIODS   := 8;

const HOUSEPAY_MIN_PERCENT   := 0.009;
const HOUSEPAY_MAX_PERCENT   := 0.018;
const HOUSEPAY_CITY_DISTMAX  := 300; // odleglosc max od miasta - wtedy najnizszy podatek.

const HOUSEPAY_GUILD         := 3000;

//-----------------------------------------------------------------------------

function IsHousePaymentLocked(House)

  if(GetNotPayedPeriods(House) >= HOUSEPAY_LOCK_PERIODS)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHousePayment(House)

  if(!House.isa(POLCLASS_MULTI))
    return 0;
  endif

  EraseObjProperty(House, "PaymentAmount");
  return CalcHousePaymentAmount(House);

endfunction

//-----------------------------------------------------------------------------

function GetHousePaymentTime(House)

  return ReadGameClock() + HOUSEPAY_PERIOD_TIME;

  var Time := GetObjProperty(House, "PaymentLastTime");

  if(!Time)
    Time := ReadGameClock();
    Time := Time - Modulo(Time, HOUSEPAY_PERIOD_TIME);
    SetObjProperty(House, "PaymentLastTime", Time);
  else
    Time := Time - Modulo(Time, HOUSEPAY_PERIOD_TIME);
  endif

  return Time;

endfunction

//-----------------------------------------------------------------------------

function MakeHousePayment(who, House)

  if(IsStaff(who) and !IsAdmin(who))
    var Owner := GetHouseOwner(House);
    if(!IsStaff(Owner))
      SendBad(who, "Co ty sobie wyobrazasz? Oplacac dom gracza?!");
      SendBad(who, "Ta karygodna akcja zostala zalogowana.");
      LogNotice("[HousePayments] "+GetRealName(who)+" ["+who.acctname+"/"+who.serial+"] probowal oplacic dom gracza "+GetRealName(Owner)+" ["+Owner.acctname+"/"+Owner.serial+"]");
      return;
    endif
  endif

  var Amount := CalcHousePaymentAmount(House);

  if(SpendGold(who, Amount))

    var Time := GetHousePaymentTime(House) + HOUSEPAY_PERIOD_TIME;
    SetObjProperty(House, "PaymentLastTime", Time);
    SetObjProperty(House, "InformedPeriods", GetNotPayedPeriods(House));

    SendSysMessage(who, "Wykonal"+ggm(who,3)+" oplate za dom w wysokosci "+Amount+"gp.", FONT_NORMAL, COLOR_BLUE);

  else

    SendSysMessage(who, "Potrzebujesz "+Amount+"gp na oplate domu. Nie masz takiej kwoty przy sobie.", FONT_NORMAL, COLOR_RED);

  endif

endfunction

//-----------------------------------------------------------------------------

function DecreaseHousePayment(who, House)

  var Time := Max(0, GetHousePaymentTime(House) - HOUSEPAY_PERIOD_TIME);
  SetObjProperty(House, "PaymentLastTime", Time);
  SetObjProperty(House, "InformedPeriods", GetNotPayedPeriods(House));

  SendSysMessage(who, "Cofn"+ggm(who,5)+" oplate za dom o "+(HOUSEPAY_PERIOD_TIME/86400)+" dni.", FONT_NORMAL, COLOR_ORANGE);

endfunction

//-----------------------------------------------------------------------------

function CalcHousePaymentAmount(House)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var DeedObjType := GetDeedObjTypeByHouseType(House.objtype);
  var Price := 0;

  if(DeedObjType)
    var Elem := ItemCfg[DeedObjType];

    if(Elem)
      var CityDistance := CalcHouseCityDistance(House);
      var DistPerc := MinMax(CDbl(CityDistance / HOUSEPAY_CITY_DISTMAX), 0.0, 1.0);
      var RealPerc := HOUSEPAY_MIN_PERCENT + ((HOUSEPAY_MAX_PERCENT - HOUSEPAY_MIN_PERCENT) * DistPerc);

      Price := CInt(CDbl(Elem.VendorSellsFor) * RealPerc * (HOUSEPAY_PERIOD_TIME / HOUSEPAY_COST_TIME));

      if(HaveHouseGuild(House))
        Price := Price + HOUSEPAY_GUILD;
      endif
    endif
  endif

  return Price;

endfunction

//-----------------------------------------------------------------------------

function GetNotPayedPeriods(House)

  var Time := GetHousePaymentTime(House);
  var CurTime := ReadGameClock();
  var Delay := CurTime - Time;
  
  if(Delay > 0)
    return CInt(Delay / HOUSEPAY_PERIOD_TIME);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanPayForHouse(House)

  var Time := GetHousePaymentTime(House);
  var CurTime := ReadGameClock();

  CurTime := CurTime - Modulo(CurTime, HOUSEPAY_PERIOD_TIME);

  if(Time - (HOUSEPAY_PERIOD_TIME * HOUSEPAY_PERIODS) < CurTime)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CalcHouseCityDistance(House)

  var Res := FindNearestCity(House.x, House.y, House.realm);
  return CDbl(HOUSEPAY_CITY_DISTMAX - MinMax(Res.dist, 0, HOUSEPAY_CITY_DISTMAX));

endfunction

//-----------------------------------------------------------------------------

function InformAboutPayments(House)

  var Periods := GetNotPayedPeriods(House);
  var InfPeriods := CInt(GetObjProperty(House, "InformedPeriods"));

  if(Periods > 0 and InfPeriods < Periods)
    var Owner := GetHouseOwner(House);
    var CoOwners := GetHouseCoOwners(House);
    var resCity := FindNearestCity(House.x, House.y, House.realm);
    var fromTitle;

    if(!resCity)
      fromTitle := "Lord British";
    else
      fromTitle := "Miasto "+resCity.city;
    endif

    var CoOwner, Text;
    var NotPayedTime := Periods * HOUSEPAY_PERIOD_TIME;

    if(Periods >= HOUSEPAY_LOCK_PERIODS)
      Text := array("Kolejna oplata nie zostala uiszczona za dom", "znajdujacy sie w", "lokacji: "+ToSextant(House.x, House.y, House.realm)+"!",
                    "Zostalismy zmuszeni do przejecia domu oraz", "zamkniecia go do odwolania.",
                    "Prosimy o niezwloczne oplacenie podatku,", "w celu ponownego otwarcia domu.");
    else
      Text := array("Chcielismy poinformowac, iz oplata nie", "zostala uiszczona za dom znajdujacy sie w", "lokacji: "+ToSextant(House.x, House.y, House.realm),
                    "Prosimy o niezwloczne oplacenie podatku.", "W przeciwnym wypadku bedziemy zmuszeni zamknac dom.");
    endif

    Text += array("", "Wlasciciel domu: "+GetName(Owner));
    Text += array("Zalegle oplaty od: "+DescGameTime(NotPayedTime));

    var Res := PostTextMail(Owner, fromTitle, "Nieuiszczona oplata za dom", Text);

    if(!Res)
      LogError("Nie udalo sie wyslac maila paymentu ["+Owner.serial+"/"+GetName(Owner)+" "+hex(House.serial)+"]: "+Res.errortext);
    endif

    foreach Serial in CoOwners
      CoOwner := FindPlayerBySerial(Serial);
      PostTextMail(CoOwner, fromTitle, "Nieuiszczona oplata za dom", Text);    
    endforeach
    
    SetObjProperty(House, "InformedPeriods", Periods);

  endif

endfunction

//-----------------------------------------------------------------------------
