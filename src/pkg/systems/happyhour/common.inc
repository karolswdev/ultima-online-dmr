
const PROP_HAPPYHOURSTARTED := "HHLaunched";
const PROP_HAPPYHOURAVAILABLE := "HHAvailableTime";
const PROP_GLOBALAPPLIEDTIME := "HHAppliedTime";
const PROP_LAST_HAPPYHOUR_MEASURED := "LastHHMeasured";

function SetAvailableHHTime(who, timeinSeconds)

    SetObjProperty(who, PROP_HAPPYHOURAVAILABLE, timeinSeconds);

endfunction

function GetAvailableHHTime(who)

    if(!GetObjProperty(who, PROP_HAPPYHOURAVAILABLE))
        return error;
    endif

    return CInt(GetObjProperty(who, PROP_HAPPYHOURAVAILABLE));

endfunction

function StartHH(who)

    SetObjProperty(who, PROP_HAPPYHOURSTARTED, ReadGameClock());

endfunction

function StopHH(who)

    EraseObjProperty(who, PROP_HAPPYHOURSTARTED);
    EraseObjProperty(who, PROP_LAST_HAPPYHOUR_MEASURED);

endfunction

function GetStartHHTime(who)

    if(!GetObjProperty(who, PROP_HAPPYHOURSTARTED))
        return error;
    endif

    return CInt(GetObjProperty(who, PROP_HAPPYHOURSTARTED));

endfunction

function IsHHStarted(who)

    return GetStartHHTime(who);

endfunction