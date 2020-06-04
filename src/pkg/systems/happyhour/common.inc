
const PROP_HAPPYHOURSTARTED := "HHLaunched";
const PROP_HAPPYHOURAVAILABLE := "HHAvailableTime";

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