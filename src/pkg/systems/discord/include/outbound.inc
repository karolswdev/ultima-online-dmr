/* $Id: outbound.inc
 *
 * Purpose
 * Provide a single function to be able to quickly send messages to discord from within the server without external programs.
 * All outbound json data is based on discord structure defined in the discordapp.com developer documents.
 * Which can be found here: https://discordapp.com/developers/docs/resources/webhook
 * 
 * 
 * Created by Dev GIB
 *
 */
use uo;
use os;

/*
 * PresetDiscordMessage( content, presetType )
 *
 * Purpose
 * Sends a message to a discord server based on the presetType defined in the settings.cfg file.
 * 
 *
 * Parameters
 * Content:        The content which will be sent as the body of the discord message.
 * presetType:     The preset message type defined in the settings.cfg file (example presets are defined).
 *
 * Return value
 * Success - Returns value 1.
 * Failure - Returns error message.
 *
 */
function PresetDiscordMessage( content, presetType:= "Default")

	var settings_cfg := ReadConfigFile(":discord:settings");
	if( settings_cfg.errortext )
		SysLog("Error::PresetDiscordMessage() - Unable to open [:discord:settings.cfg] ->"+settings_cfg.errortext);
		return error{"errortext" := "Unable to open [:discord:settings.cfg]"};
	endif
	
	var default_cfg_elem := FindConfigElem( settings_cfg, "Default" );
	var cfg_elem := FindConfigElem( settings_cfg, presetType );
	
	var channel_id := CheckAndGetConfig( cfg_elem, default_cfg_elem, "channel_id" );
	syslog("Discord:: got channel id " + channel_id);
	var token := CheckAndGetConfig( cfg_elem, default_cfg_elem, "token" );
	var username := CheckAndGetConfig( cfg_elem, default_cfg_elem, "username" );
	var avatar_url := CheckAndGetConfig( cfg_elem, default_cfg_elem, "avatar_url" );
	
	PackAndSendMessage(channel_id, token, content, username, avatar_url);
	return 1;
	  
endfunction

function CheckAndGetConfig( cfg, default_cfg, elem )


	print("Running CheckAndGetConfig for Discord: [cfg]:" + cfg + " [default_cfg]: " + default_cfg + " [elem]:" + elem + ".");
	var res := GetConfigString( cfg, elem );
	if( res == error )
		res := GetConfigString( default_cfg, elem );
		if( res == error )
			//LogToFile("::log/discord.log", "Error::PresetDiscordMessage() - "+elem+" can't be found in settings.cfg .", LOG_DATETIME);
		  return error{"errortext" := elem+" can't be found in settings.cfg ."};
		endif
	endif
	
	
	
	print("res is " + res);

	
	if(res == "-1430257618")
		return "518185354700783662";
	else
		return res;
	endif
	
endfunction

function PackAndSendMessage(channel_id, token, content, username, avatar_url)
	
	syslog("Discord: channel_id: " + channel_id);
	syslog("Discord: token: " + token);
	syslog("Sending (1):" + "https://discordapp.com/api/webhooks/"+channel_id+"/"+token);
	
	
	HTTPRequest(
  	"https://discordapp.com/api/webhooks/"+channel_id+"/"+token,
  	"POST", 
  	struct{ 
  data := "{\"content\":\""+content+"\",
  					\"username\":\""+username+"\",
  					\"avatar_url\":\""+avatar_url+"\"  					
  					}", 
  headers := struct{
    "Content-Type" := "application/json"
  }} );
  
endfunction