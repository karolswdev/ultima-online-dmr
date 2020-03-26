use uo;
use os;
use cfgfile;

function GetSettingsCfgFile()

	var cfg := ReadConfigFile( ":webApi:settings" );
	if( cfg.errortext )
		SysLog( "Error::GetSettingsCfgFile() - Unable to open [:webApi:settings.cfg] ->"+cfg.errortext );
	endif

	return cfg;
endfunction

function GetSettingsCfgElem( elem_name, byref cfg_file:=0 )

	if( !cfg_file )
		cfg_file := GetSettingsCfgFile();
	endif
	
	var elem := cfg_file[elem_name];

	if( elem.errortext )
		SysLog( "Error::GetSettingsCfgElem() - Unable to find elem ["+elem_name+"] ->"+elem.errortext );
	endif

	return elem;
endfunction
