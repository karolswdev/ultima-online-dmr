include ":logging:seq";

function LogPlayerLogon(player)

    var logObject := struct;

    logObject["AccountName"] := player.acctname;
    logObject["PlayerSerial"] := player.serial;
    logObject["PlayerName"] := player.name;
    logObject["PlayerCoords"] := { player.x, player.y, player.z, player.realm };
    logObject["@mt"] := "{PlayerName} (S: {PlayerSerial}/A: {AccountName}) zalogowal sie.";

    return LogToSeq(logObject);

endfunction