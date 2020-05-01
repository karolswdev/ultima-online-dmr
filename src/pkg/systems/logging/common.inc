use os;
include ":logging:seq";

const LOGTYPE_LOGON := 1;
const LOGTYPE_KILL := 2;
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

function GetIso8601TimeStamp(who := 0)

  var source := CUnixTime(polcore().systime);

  //20-05-01 05:52:57


  var timeStamp := "";
  var year := "20" + source[1, 2];
  var month := source[4, 2];
  var day := source[7, 2];
  var hours := source[9, 2];
  var minutes := source[11, 2];
  var seconds := source[13, 2];

  if(who)
    SendSysMessage(who, "Year:" + year);
    SendSysMessage(who, "Month:" + month);
    SendSysMessage(who, "Day: " + day);
    SendSysMessage(who, "Hours: " + hours);
    SendSysMessage(who, "Minutes:" + minutes);
    SendSysMessage(who, "Seconds" + seconds);
  endif

  timeStamp := year + "-" + month + "-" + day + "T" + hours + ":" + minutes + ":" + seconds;
  return timeStamp;

endfunction