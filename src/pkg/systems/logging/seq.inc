use basic;
use cfgfile;
include "include/clock";

/*
The log events are passed in as structs.
Read about posting RAW events into a Seq sink here -> https://docs.datalust.co/docs/posting-raw-events
*/
function LogToSeq(logStruct, who := 0)

    logStruct["@t"] := "20" + CUnixTime(polcore().systime, RLT_SDATE);

    if(!logStruct["@mt"])
        if(who != 0)
            SendSysMessage(who, "No @mt template passed" );
        endif
        return error { errortext := "Invalid log object. No message template passed!" };
    endif

    var url := ConstructSeqUrl();
    if(!url)
        if(who != 0)
            SendSysMessage(who, "No valid URL provided" );
        endif
        return error { errortext := "No valid URL provided" };
    endif
    var headers := ConstructSeqHeaders();
    if(!headers)
        if(who != 0)
            SendSysMessage(who, "No headers could be constructed. Check URL" );
        endif
        return error { errortext := "Could not consturct headers. Check URL" };
    endif

    var jsonHeaders := PackJson(headers);
    var jsonData := PackJson(logStruct);


    var result := HttpRequest(url, "POST", struct{
        data := jsonData,
        headers := jsonHeaders
    });

    return result;

endfunction

function LoadLoggingFile()

    return ReadConfigFile(":logging:logging");

endfunction

function GetApiKey()

    var cfgFile := LoadLoggingFile();

    if(!cfgFile)
        return error;
    endif

    var securityElem := FindConfigElem(cfgFile, "Security");

    if(!securityElem)
        return error;
    endif

    return GetConfigString(securityElem, "ApiKey");

endfunction

function GetSeqUrl()

    var cfgFile := LoadLoggingFile();

    if(!cfgFile)
        return error;
    endif

    var urlElem := FindConfigElem(cfgFile, "Url");

    if(!urlElem)
        return error;
    endif

    return GetConfigString(urlElem, "SeqUrl");

endfunction

function ConstructSeqHeaders()

    var apiKey := GetApiKey();

    if(!apiKey)
        return error;
    endif

    var header := struct;

    header["Content-Type"] := "application/json";
    if(Len(apiKey) > 2)
        header["X-Seq-ApiKey"] := apiKey;
    endif

    return header;

endfunction

function ConstructSeqUrl()

    var url := GetSeqUrl();

    if(!url)
        return error;
    endif

    var trailingSlash := "";

    if(url[Len(url)] != "/")
        trailingSlash := "/";
    endif

    url += trailingSlash + "api/events/raw?clef";

    return url;

endfunction
