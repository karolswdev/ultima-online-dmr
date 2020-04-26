use basic;


/*
The log events are passed in as structs.
Read about posting RAW events into a Seq sink here -> https://docs.datalust.co/docs/posting-raw-events
*/
function LogToSeq(logStruct)

    if(logStruct["@mt"] == error)
        return error "Invalid log object. No message template passed!";
    endif

    var url := GetSeqUrl();
    if(!url)
        return error "No valid URL provided";
    endif
    var headers := ConstructSeqHeaders();
    if(!headers)
        return error "Could not consturct headers. Check URL";
    endif


    var result := HttpRequest(url, "POST", struct{
        data := PackJson(logStruct),
        headers := PackJson(headers);
    });

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

    var urlElem := FindConfigElem(cfgFile, "Url")

    if(!urlElem)
        return error;
    endif

    return GetConfigString(securityElem, "SeqUrl");

endfunction

function ConstructSeqHeaders()

    var apiKey = GetApiKey();

    if(!apiKey)
        return error;
    endif

    var header := struct;

    header["Content-Type"] = "application/json";
    if(Len(apiKey) > 2)
        header["X-Seq-ApiKey"] = apiKey;
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
