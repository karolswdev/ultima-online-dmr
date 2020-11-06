use os;

include ":logging:seq";
include "include/key";

const LOGTYPE_LOGON := 1;
const LOGTYPE_KILL := 2;
const LOGTYPE_DEATH := 3;
const LOGTYPE_USE_BANKER := 4;
const LOGTYPE_DECODE_MAP := 5;
const LOGTYPE_CHEST_NO_GUARDIANS_CFG := 6;
const LOGTYPE_CHEST_NO_NPC_SPAWN := 7;
// when happy hour gets assigned to all chars
const LOGTYPE_HAPPYHOUR_DISTRIBUTED := 8;
const LOGTYPE_NO_DEFINED_ORES := 9;
const LOGTYPE_HH_START_STOP := 10;
const LOGTYPE_HH_HAS_ENDED := 11;
const LOGTYPE_REPLACE_FIRE_INGOT := 12;
const LOGTYPE_NO_REPLACE_FIRE_INGOT := 13;

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

    var logObject := struct;

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

function LogNoGuardiansForChest(parms)

    var player := parms[1];
    var chest := parms[3];

    var logObject := struct;

    logObject["AccountName"] := player.acctname;
    logObject["PlayerSerial"] := player.serial;
    logObject["PlayerName"] := player.name;
    logObject["PlayerCoords"] := { player.x, player.y, player.z, player.realm };
    logObject["ChestSerial"] := chest.serial;
    logObject["ChestCoords"] := { chest.x, chest.y, chest.z, chest.realm };

    logObject["@mt"] := "[TreasureChest] Nie znaleziono cfg (guardians) - dla chesta (S: {ChestSerial} / C: {ChestCoords}) - dla gracza {PlayerName} (S: {PlayerSerial}/A: {AccountName})";

    var res := LogToSeq(logObject);
    return res;

endfunction

function LogNoChestSpawnForChest(parms)

    var chest := parms[1];
    var template := parms[3];

    var logObject := struct;


    logObject["ChestSerial"] := chest.serial;
    logObject["ChestCoords"] := { chest.x, chest.y, chest.z, chest.realm };
    logObject["NpcTemplate"] := template;

    logObject["@mt"] := "Skrzynia skarbow (S: {ChestSerial} / C: {ChestCoords}) - nie mozna bylo zespawnowac {NpcTemplate}";
    var res := LogToSeq(logObject);
    return res;

endfunction

function LogHappyHourDistribution(parms)

    var numberOfCharactersAffected := parms[1];

    var logObject := struct;

    logObject["HappyHourCount"] := numberOfCharactersAffected;
    logObject["@mt"] := "Happy Hour rozdano HH {HappyHourCount} postaciom.";

    var res := LogToSeq(logObject);
    return res;

endfunction

function LogNoDefinedOresInRegion(parms)

    var player := parms[1];

    var logObject := struct;

    logObject["AccountName"] := player.acctname;
    logObject["PlayerSerial"] := player.serial;
    logObject["PlayerName"] := player.name;
    logObject["PlayerCoords"] := { player.x, player.y, player.z, player.realm };

    logObject["@mt"] := "{PlayerName} (S: {PlayerSerial}/A: {AccountName}) Brak zdefiniowanych rud dla lokacji: (s: {PlayerCoords})";
    var res :=LogToSeq(logObject);
    return res;

endfunction


function LogHappyHourStartStop(parms)
    var player := parms[1];
    var text := parms [3];
    var availableTimeInSeconds := RealTime(parms [4]) ;

    var logObject := struct;

    logObject["AccountName"] := player.acctname;
    logObject["PlayerSerial"] := player.serial;
    logObject["PlayerName"] := player.name;
    logObject["PlayerText"] := text;
    logObject["HHTime"] := availableTimeInSeconds;

    logObject["@mt"] := "{PlayerName} (S: {PlayerSerial}/A: {AccountName}) uruchomił funkcje Happy Hour : {PlayerText} (Pozostało czasu:{HHTime})";
    var res :=LogToSeq(logObject);
    return res;
endfunction 


function LogHappyHourTimeEnd(parms)

    var player := parms[1];
    var available := RealTime(parms [3]) ;

    var logObject := struct;

    logObject["AccountName"] := player.acctname;
    logObject["PlayerSerial"] := player.serial;
    logObject["PlayerName"] := player.name;
    logObject["HHTime"] := available;

    logObject["@mt"] := "{PlayerName} (S: {PlayerSerial}/A: {AccountName}) Czas Happy Hour dobiegł końca. Skrypt się zatrzymał (Pozostało czasu:{HHTime})";
    var res :=LogToSeq(logObject);
    return res;

endfunction


function LogFireIngotReplace(parms)

    var Ingot := parms[1];
    var Amount := parms[3];
    var container := parms[4];
    

    var logObject := struct;

    logObject["Ingot"] := Ingot;
    logObject["ContainerSerial"] := container.serial;
    logObject["Amount"] := Amount;
    

    logObject["@mt"] := "{Ingot} (S: ContainerSerial/ {ContainerSerial}) wymieniono {Amount} sztab ";
    var res :=LogToSeq(logObject);
    return res;

endfunction

function LogNoFireIngotReplace(parms)

     var Ingot := parms[1];
    var Amount := parms[3];
    var container := parms[4];
    

    var logObject := struct;

    logObject["Ingot"] := Ingot;
    logObject["ContainerSerial"] := container.serial;
    logObject["Amount"] := Amount;
    

    logObject["@mt"] := "{Ingot} (S: ContainerSerial/ {ContainerSerial}) Nie udalo sie wymienic {Amount} sztab ";
    var res :=LogToSeq(logObject);
    return res;

endfunction


