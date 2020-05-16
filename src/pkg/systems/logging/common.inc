use os;

include ":logging:seq";
include "include/key";

const LOGTYPE_LOGON := 1;
const LOGTYPE_KILL := 2;
const LOGTYPE_DEATH := 3;
const LOGTYPE_USE_BANKER := 4;
const LOGTYPE_DECODE_MAP := 5;

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

function LogPlayerDecodeMap(parms)

    var player := parms[1];
    var map := parms[3];

    var logObject = struct;

    logObject["AccountName"] := player.acctname;
    logObject["PlayerSerial"] := player.serial;
    logObject["PlayerName"] := player.name;
    logObject["PlayerCoords"] := { player.x, player.y, player.z, player.realm };
    logObject["MapSerial"] := map.serial;
    logObject["MapLevel"] := GetTreasureLvl(map);

    logObject["@mt"] := "{PlayerName} (S: {PlayerSerial}/A: {AccountName}) rozczytal mape (S: {MapSerial} / L: {MapLevel})";

    var res := LogToSeq(logObject);

    return res;

endfunction

