

const PD_PROP := "PdPoints";
const PD_HISTORY := "PdHistory";

function GetPdPoints(who);

	if(GetObjProperty(who, PD_PROP) == error)
		return error;
	endif

	return CInt(GetObjProperty(who, PD_PROP));
	
endfunction

function IncrementPdPoints(who, points)

	if(GetObjProperty(who, PD_PROP) == error)
		SetObjProperty(who, PD_PROP, points);
		return;
	endif
	
	var currentPoints := GetPdPoints(who);
	SetObjProperty(who, PD_PROP, points + currentPoints);

endfunction

function DeductPdPoints(who, points)

	if(GetObjProperty(who, PD_PROP) == error)
		SetObjProperty(who, PD_PROP, points);
		return;
	endif
	
	var currentPoints := GetPdPoints(who);
	
	SetObjProperty(who, PD_PROP, currentPoints - points);

endfunction

function GetPdHistory(who)

	if(GetObjProperty(who, PD_HISTORY) == error)
		SetObjProperty(who, PD_HISTORY, {});
	endif

	return GetObjProperty(who, PD_HISTORY);
	
endfunction

function GrantPd(who, thePlayer, reason)

	var pdObject := struct;
	
	pdObject.+time := ReadGameClock();
	pdObject.+type := "+";
	pdObject.+granter := who.serial;
	pdObject.+reason := reason;
	
	IncrementPdPoints(thePlayer, 1);
	
	var history := GetPdHistory(thePlayer);
	history.append(pdObject);
	SetObjProperty(thePlayer, PD_HISTORY, history);
	
	SendSysMessage(thePlayer, "[PD]: Zdobyto dodatkowy punkt za " + reason, FONT_NORMAL, COLOR_BLUE); 

endfunction

function RevokePd(who, thePlayer, reason)

	var pdObject := struct;
	
	pdObject.+time := ReadGameClock();
	pdObject.+type := "-";
	pdObject.+granter := who.serial;
	pdObject.+reason := reason;
	
	
	DeductPdPoints(thePlayer, 1);
	
	var history := GetPdHistory(thePlayer);
	history.append(pdObject);
	SetObjProperty(thePlayer, PD_HISTORY, history);
	SendSysMessage(thePlayer, "[PD]: Stracono punkt za " + reason, FONT_NORMAL, COLOR_BLUE); 


endfunction

