

const INSTANCE_STATUS_RUNNING := 1;
const INSTANCE_STATUS_READY := 2;
const INSTANCE_STATUS_COOLDOWN := 3;
const INSTANCE_STATUS_UNKNOWN := 4;

const INSTANCE_DIFFICULTY_NORMAL := 1;
const INSTANCE_DIFFICULTY_HEROIC := 2;
const INSTANCE_DIFFICULTY_EPIC := 3;

const RUNE := 0xA300;
const CHEST := 0xA400;
const TELEPORTER := 0x6200;

include ":discord:outbound";

function GetStatusString(stone)

	var status := GetInstanceStatus(stone);

	var statusString := "Nieznany";
	
	case(status)
		INSTANCE_STATUS_RUNNING: statusString := "W trakcie"; break;
		INSTANCE_STATUS_READY: statusString := "Gotowy"; break;
		INSTANCE_STATUS_COOLDOWN: statusString := "Cooldown"; break;
		INSTANCE_STATUS_UNKNOWN: break;
	endcase
	
	return statusString;
	
endfunction

function GetInstanceStoneSerials()

	return GetGlobalProperty("InstanceStones");

endfunction

function GetInstanceDifficulty(stone)

	return CInt(GetObjProperty(stone, "CurrentDifficulty"));
	
endfunction

function SetInstanceDifficulty(stone, difficulty)

	SetObjProperty(stone, "CurrentDifficulty", difficulty);

endfunction

function GetInstanceStatus(stone)

	if(!GetObjProperty(stone, "InstanceStatus"))
		return INSTANCE_STATUS_UNKNOWN;
	endif
	
	var status := CInt(GetObjProperty(stone, "InstanceStatus"));
	
	if(status == INSTANCE_STATUS_COOLDOWN)
		return INSTANCE_STATUS_COOLDOWN;
	elseif(status == INSTANCE_STATUS_READY)
		return INSTANCE_STATUS_READY;
	elseif(status == INSTANCE_STATUS_RUNNING)
		return INSTANCE_STATUS_RUNNING;
	elseif(status == INSTANCE_STATUS_UNKNOWN)
		return INSTANCE_STATUS_UNKNOWN;
	else
		return INSTANCE_STATUS_UNKNOWN;
	endif
	
endfunction

function GetCurrentNumberOfSpawns(stone)

	var spawnNum := 0;
	
	foreach rune in GetInstanceRunes(stone)
		var runeObject := SystemFindObjectBySerial(rune);
		spawnNum += CheckExistingObjects(runeObject).size();
	endforeach
	
	return spawnNum;

endfunction

function GetMaxNumberOfSpawns(stone)

	var spawnNum := 0;
	
	foreach rune in GetInstanceRunes(stone)
	
		var runeObject := SystemFindObjectBySerial(rune);
		spawnNum += GetRuneMaxAmount(runeObject);
		
	endforeach
	
	return spawnNum;

endfunction

function IsCleared(stone)

	if(GetInstanceStatus(stone) != INSTANCE_STATUS_RUNNING)
		return 0;
	endif
	
	var num := GetCurrentNumberOfSpawns(stone);
	
	if(num)
		return 0;
	endif
	
	return 1;

endfunction

function SetInstanceStatus(stone, status)

	SetObjProperty(stone, "InstanceStatus", status);

endfunction

function GetInstanceName(stone)

	var name := "Unknown";
	
	if(GetObjProperty(stone, "InstanceName"))
		name := GetObjProperty(stone, "InstanceName");
	endif
	
	return name;
	
endfunction

function IsLeader(stone, who);

	if(who.serial == CInt(GetObjProperty(stone, "InstanceLeader")))
		return 1;
	endif
	
	return 0;

endfunction

function IsParticipant(stone, who)

	if(who.serial in GetParticipants(stone))
		return 1;
	endif
	
	return 0;

endfunction

function GetParticipants(stone)

	if(GetObjProperty(stone, "Participants") == error)
		SetObjProperty(stone, "Participants", {});
		return {};
	endif
	
	return GetObjProperty(stone, "Participants");
	
endfunction

function GetInstanceLeader(stone)

	return SystemFindObjectBySerial(CInt(GetObjProperty(stone, "InstanceLeader")));
	
endfunction

function AddParticipant(stone, adding_person, who)

	if(GetObjProperty(stone, "Participants").size() == 0)
		
			if(!CheckInstanceBan(adding_person))
				SendSysMessage(adding_person, "Nie mozesz jeszcze uczestniczyc w wyprawie.", FONT_NORMAL, COLOR_RED);
				return;
			endif
			
			SetObjProperty(stone, "Participants", {});
			SetObjProperty(stone, "InstanceLeader", adding_person.serial);
			SendSysMessage(adding_person, "Jestes pierwsza osoba, ktora dodaje sie do wyprawy. Zostal" + GGm(adding_person, 3) + " liderem wyprawy.", FONT_NORMAL, COLOR_GREEN);
			PrintTextAbove(adding_person, "Lider wyprawy", FONT_NORMAL, COLOR_GREEN);
			var first := GetObjProperty(stone, "Participants");
			first.append(adding_person.serial);
			SetObjProperty(stone, "Participants", first);
			SetObjProperty(adding_person, "LastInstanceEntry", ReadGameClock());
		else
		
			if(!IsLeader(stone, adding_person))
				SendSysMessage(adding_person, "Nie mozesz dodawac uczestnikow.", FONT_NORMAL, COLOR_RED);
				return;
			endif
			
			var participants := GetObjProperty(stone, "Participants");
			if(who.serial in participants)
				SendSysMessage(who, "Juz jestes uczestnikiem wyprawy.", FONT_NORMAL, COLOR_RED);
				SendSysMessage(adding_person, "Juz jest uczestnikiem wyprawy.", FONT_NORMAL, COLOR_RED);
				return;
			else
				if(!CheckInstanceBan(who))								
					SendSysMessage(who, "Nie mozesz jeszcze uczestniczyc w wyprawie.", FONT_NORMAL, COLOR_RED);
					PrintTextAbove(who, "*nie moze uczestniczyc*", FONT_NORMAL, COLOR_RED);
					return;
				endif
				
				participants.append(who.serial);
				SendSysMessage(who, "Zostal" + GGm(who, 3) + " dodany do wyprawy.", FONT_NORMAL, COLOR_GREEN);
				PrintTextAbove(who, "*Moc wyprawy*");
				SetObjProperty(stone, "Participants", participants);
				SetObjProperty(who, "LastInstanceEntry", ReadGameClock());
			endif
			
		endif
			

endfunction

function CheckInstanceBan(who)

	if(!GetObjProperty(who, "LastInstanceEntry"))
		return 1;
	else
		var time := CInt(GetObjProperty(who, "LastInstanceEntry"));
		if(ReadGameClock() - time < 43200)
			return 0;
		else
			return 1;
		endif
		
	endif
	
	return 1;
	
endfunction

function SetInstanceName(stone, name)

	SetObjProperty(stone, "InstanceName", name);

endfunction

function GetInstanceRunes(stone)

	return GetObjProperty(stone, "LinkedRunes");

endfunction

function AddInstanceChest(stone, chest)

	if(!GetObjProperty(stone, "LinkedChests"))
		SetObjProperty(stone, "LinkedChests", array{});
	endif

	var chests := GetObjProperty(stone, "LinkedChests");

	if(chest.serial in chests)
		return 0;
	else
		if(chest.objtype != CHEST)
			return 0;
		endif

		SetObjProperty(chest, "InstanceStoneSerialNumber", stone.serial);
		chests.append(chest.serial);
		SetObjProperty(stone, "LinkedChests", chests);
		return 1;
	endif

	return 1;

endfunction

function AddInstanceRune(stone, rune)

	if(!GetObjProperty(stone, "LinkedRunes"))
		SetObjProperty(stone, "LinkedRunes", array{});
	endif
	
	var runes := GetObjProperty(stone, "LinkedRunes");
	
	if(rune.serial in runes)
		return 0;
	else
		if(rune.objtype != RUNE)
			return 0;
		endif
		
		SetObjProperty(rune, "InstanceStoneSerialNumber", stone.serial);
		runes.append(rune.serial);
		SetObjProperty(stone, "LinkedRunes", runes);
		return 1;
	endif
	
	return 1;
	
endfunction

// time is in hours.
function SetInstanceTimeout(stone, time)

	SetObjProperty(stone, "TimeoutHours", time);

endfunction

function GetInstanceTimeout(stone);

	return CInt(GetObjProperty(stone, "TimeoutHours"));

endfunction

function SetBasePoints(stone, points)

	SetObjProperty(stone, "BasePoints", points);
	
endfunction

function GetBasePoints(stone)

	return CInt(GetObjProperty(stone, "BasePoints"));

endfunction

function GivePrize(stone)

	var basePoints := GetBasePoints(stone);
	
	var points := 0;
	
	var multiplier := 1;
	
	if(GetInstanceDifficulty(stone) == INSTANCE_DIFFICULTY_HEROIC)
		multiplier := 2;
	elseif(GetInstanceDifficulty(stone) == INSTANCE_DIFFICULTY_EPIC)
		multiplier := 5;
	endif
	
	points := basePoints * multiplier;
	
	var participants := GetParticipants(stone);
	var pointsPerParticipant := CInt(points/participants.size());
	
	foreach part in participants
		var participantObject := SystemFindObjectBySerial(part, 1);
		if(participantObject)
			SendSysMessage(participantObject, "Instancje: Zdobyl" + GGm(participantObject, 3) + " " + pointsPerParticipant + " punktow.", FONT_NORMAL, COLOR_BLUE);
			var points := CInt(GetObjProperty(participantObject, "HonorPoints"));
			SetObjProperty(participantObject, "HonorPoints", points + pointsPerParticipant);
			start_script(":instances:teleport_gump", {participantObject, GetInstanceName(stone)});

		endif
	endforeach
				Broadcast("Instancja: " + GetInstanceName(stone) + " zostala wyczyszczona. " + points + " punktow zostanie rozdzielonych na smialkow.", FONT_NORMAL, COLOR_BLUE);
				PresetDiscordMessage("Instancja: " + GetInstanceName(stone) + " zostala wyczyszczona. " + points + " punktow zostanie rozdzielonych na smialkow.");

endfunction

function DoCheck(stone)

	if(!GetGlobalProperty("InstanceStones"))
		SetGlobalProperty("InstanceStones", {});
	endif
	
	var globalStones := GetGlobalProperty("InstanceStones");
	if(!(stone.serial in globalStones) && (GetInstanceName(stone) != "Unknown") )
		globalStones.append(stone.serial);
		SetGlobalProperty("InstanceStones", globalStones);
	endif

	// we don't check for the first 3 minutes... problem is that sometimes spawnnet takes 
	var lastStart := CInt(GetObjProperty(stone, "LastRun"));
	if(ReadGameClock() - lastStart < 160)
		return;
	endif
	
	if(GetInstanceStatus(stone) == INSTANCE_STATUS_COOLDOWN)
		var currentTime := ReadGameClock();
		
		var delay := CInt(GetObjProperty(stone, "TimeoutHours"));
		var lastRunTime := CInt(GetObjProperty(stone, "LastRun"));
		
		// goqsane: we no longer need a cooldown period on instances
		delay := 0;
		
		if(lastRunTime + (delay * 3600) <= ReadGameClock())
			SetInstanceStatus(stone, INSTANCE_STATUS_READY);
			start_script("instances:notify", {"Instancja " + GetInstanceName(stone) + " jest gotowa na przyjecie smialkow."});
			//Broadcast("Instancja " + GetInstanceName(stone) + " jest gotowa na przyjecie smialkow.", FONT_NORMAL, COLOR_BLUE);
		endif
	endif
	
	if(GetInstanceStatus(stone) == INSTANCE_STATUS_RUNNING)
		if(IsCleared(stone))
			GivePrize(stone);
			DoStop(stone);
		else
			if(ReadGameClock() - CInt(GetObjProperty(stone, "LastRun")) > 86400)
				Broadcast("Instancja " + GetInstanceName(stone) + " wygasla. Smialkowie nie wyczyscili jej.", FONT_NORMAL, COLOR_RED);
				DoStop(stone);
				PresetDiscordMessage("Instancja " + GetInstanceName(stone) + " wygasla. Smialkowie nie wyczyscili jej.");

			endif
		endif
	endif
		
	// TODO: This needs to be fixed.
	
endfunction
		
function DoStart(stone)

	if(GetInstanceStatus(stone) == INSTANCE_STATUS_RUNNING)
		syslog("Instances: Trying to run an instance stone when already running");
		return 0;
	endif
	
	if(GetInstanceName(stone) == INSTANCE_STATUS_COOLDOWN)
		syslog("Instances: Trying to run an instance when on cooldown");
		return 0;
	endif
	
	if(GetInstanceStatus(stone) == INSTANCE_STATUS_UNKNOWN)
		syslog("Instances: !? Status was unknown... but tried to start");
		PrintTextAbove(stone, "?! Cos sie tu zepsulo...");
		return 0;
	endif
	
	SetInstanceStatus(stone, INSTANCE_STATUS_RUNNING);
	SetObjProperty(stone, "LastRun", ReadGameClock());
	
	var runes := GetObjProperty(stone, "LinkedRunes");

	var chests := GetObjProperty(stone, "LinkedChests");
	
	foreach rune in runes
		var runeObject := SystemFindObjectBySerial(rune);
		if(runeObject)
			PrintTextAbove(runeObject, "Forcing spawns!", FONT_NORMAL, COLOR_RED);
			if(GetRuneRespawnTime(runeObject) < 4320)
				SetRuneRespawnTime(runeObject, 4320);
			endif
			SetRunning(runeObject);
			ForceSpawn(runeObject);
		endif
	endforeach

	foreach chest in chests
		var chestObject := SystemFindObjectBySerial(chest);
		if(chestObject)
			PrintTextAbove(chestObject, "Forcing spawns!", FONT_NORMAL, COLOR_RED);
			SetRunning(chestObject);
			ForceSpawn(chestObject);
		endif
	endforeach
	syslog("Instances: Starting instance " + GetInstanceName(stone));
	
	PlayStationaryEff(stone.x,stone.y,stone.z, FX_FLAMESTRIKE, 0x0a, 0x1e, 0, stone.realm );
	PrintTextAbove(stone, GetInstanceName(stone) + " jest gotow na smialkow!");
	
endfunction

function DoStop(stone)

	if(GetInstanceName(stone) == INSTANCE_STATUS_COOLDOWN)
		syslog("Instances: Trying to stop an instance when already on cooldown");
		return 0;
	endif
	
	if(GetInstanceStatus(stone) == INSTANCE_STATUS_UNKNOWN)
		syslog("Instances: !? Status was unknown... but tried to stop");
		PrintTextAbove(stone, "?! Cos sie tu zepsulo...");
		return 0;
	endif
	
	if(GetInstanceStatus(stone) == INSTANCE_STATUS_RUNNING)
		EraseObjProperty(stone, "Participants");
		EraseObjProperty(stone, "InstanceLeader");
		var runes := GetObjProperty(stone, "LinkedRunes");
		var chests := GetObjProperty(stone, "LinkedChests");
	
		foreach rune in runes
			var runeObject := SystemFindObjectBySerial(rune);
			if(runeObject)
				PrintTextAbove(runeObject, "About to stop!", FONT_NORMAL, COLOR_RED);
				AnnihilateSpawns(runeObject);
				SetStopped(runeObject);
			endif
		endforeach

		foreach chest in chests
			var chestObject := SystemFindObjectBySerial(chest);
			if(chestObject)
				PrintTextAbove(chestObject, "About to stop!", FONT_NORMAL, COLOR_RED);
				SetStopped(chestObject);
			endif
		endforeach
	
	endif
	
	SetInstanceStatus(stone, INSTANCE_STATUS_COOLDOWN);
	SetObjProperty(stone, "CooldownTime", ReadGameClock());
		
endfunction

function AddTeleporter(stone, teleporter)

	if(teleporter.objtype != TELEPORTER)
		return 0;
	else
		if(!GetObjProperty(stone, "LinkedTeleporters"))
			SetObjProperty(stone, "LinkedTeleporters", {});
			AddTeleporter(stone, teleporter);
		else
			var teleporters := GetObjProperty(stone, "LinkedTeleporters");
			teleporters.append(teleporter.serial);
			SetObjProperty(teleporter, "LinkedInstanceStone", stone.serial);
			SetObjProperty(stone, "LinkedTeleporters", teleporters);
			return 1;
		endif
	endif
	
	return 1;
	
endfunction