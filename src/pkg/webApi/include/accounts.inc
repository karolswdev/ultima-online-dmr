use uo;
use os;

CONST MAX_CHARACTERS	:= 5;

function GetCharacters( account )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	var mobile_list := array;
	for i:=1 to MAX_CHARACTERS
		var mobile := account.GetCharacter(i);
		if( mobile )
			mobile_list.Append( mobile );
		endif
		SleepMS(2);
	endfor

	return mobile_list;
endfunction

function GetNumCharacters( account )

	if( TypeOf( account ) != "AccountRef" )
		account := FindAccount( account );
		if( account.errortext )
			return account;
		endif
	endif

	var count := 0;
	for i:=1 to MAX_CHARACTERS
		if( account.GetCharacter( i ))
			count := count+1;
		endif
		SleepMS(2);
	endfor

	return count;
endfunction