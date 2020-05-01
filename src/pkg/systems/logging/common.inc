use os;
include ":logging:seq";

const LOGTYPE_LOGON := 1;
const LOGTYPE_KILL := 2;
const LOGTYPE_DEATH := 3;
// etc.

function LogPlayerLogon(player)

    var logObject := struct;

    logObject["AccountName"] := player.acctname;
    logObject["PlayerSerial"] := player.serial;
    logObject["PlayerName"] := player.name;
    logObject["PlayerCoords"] := { player.x, player.y, player.z, player.realm };
    logObject["@mt"] := "{PlayerName} (S: {PlayerSerial}/A: {AccountName}) zalogowal sie w {PlayerCoords}.";

    var res := LogToSeq(logObject);

    return res;
endfunction

