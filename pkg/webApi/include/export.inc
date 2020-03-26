use uo;
use os;
use sql;

include ":webApi:accounts";
include ":webApi:settings";

function ExportChars( db, settings )

            var acct, chars, index;
            
            mysql_select_db(db,settings.DBName);
			
			mysql_query(db,"TRUNCATE TABLE "+settings.CharTable);
			mysql_query(db,"TRUNCATE TABLE "+settings.CharLayers);

			foreach acct_name in ( ListAccounts() )
				acct := FindAccount( acct_name );
				
				if ( GetNumCharacters( acct ) > 0 )
					chars := GetCharacters( acct );
					foreach charRef in ( chars )
						if ( charRef )
                            if ( charRef.cmdlevel <= CInt(settings.CharExportMaxCmdLevel))
                                var race := "";
                                
                                if (charRef.trueobjtype == 0x190 || charRef.trueobjtype == 0x191)
                                    race := "Human";
                                elseif (charRef.trueobjtype == 0x25D || charRef.trueobjtype == 0x25E)
                                    race := "Elf";
                                elseif (charRef.trueobjtype == 0x29A || charRef.trueobjtype == 0x29B)
                                    race := "Gargoyle";
                                else 
                                    race := "Other";
                                endif
                            
                                mysql_query(db,"INSERT INTO "+settings.CharTable+" (char_id, char_name, char_title, char_race, char_body, char_female, char_bodyhue, char_public, char_cmdlevel ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)",array{charRef.serial, charRef.name, charRef.title_prefix, race, charRef.trueobjtype, charRef.gender, charRef.truecolor, 0, charRef.cmdlevel});
                                
                                var x, i;
                                for (i := 1; i <= 24; i := i + 1)
                                    x := GetEquipmentByLayer(charRef, i);
                                    
                                    if ( (i == 0x0b) || (i == 0x10) || (i == 0x15) )
                                        mysql_query(db,"INSERT INTO "+settings.CharLayers+" (char_id, layer_id, item_id, item_hue) VALUES (?, ?, ?, ?)",array{charRef.serial, 0, x.objtype, x.color});
                                    else
                                        mysql_query(db,"INSERT INTO "+settings.CharLayers+" (char_id, layer_id, item_id, item_hue) VALUES (?, ?, ?, ?)",array{charRef.serial, i, x.objtype, x.color});
                                    endif
                                endfor
							endif
						endif

					endforeach
				endif
				
			endforeach
endfunction

function ExportOnline( db, settings )

        mysql_select_db(db,settings.DBName);
        
        mysql_query(db,"TRUNCATE TABLE "+settings.CharOnline);
        
        if( db )
            foreach char in EnumerateOnlineCharacters()
                if(char.cmdlevel  <= CInt(settings.OnlineExportMaxCmdLevel))
                    var query := mysql_query(db,"INSERT INTO "+settings.CharOnline+" (char_id, char_name, login_time, char_cmdlevel ) VALUES (?, ?, ?, ?)", array{char.serial, char.name, POLCore().systime, char.cmdlevel});  
                endif
            endforeach
        else
            Print ("DBError: "+db.errortext);
        endif
endfunction