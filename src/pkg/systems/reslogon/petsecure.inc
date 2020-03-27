
const EVID_PET_SECURE   := 0x0001;

//----------------------------------------------------------

const PET_SECURE_UPTIME_LIMIT := 180;
const PET_SECURE_TIME_LIMIT   := 86400;

//----------------------------------------------------------

const PET_SECURE_LOC_X := 5144;
const PET_SECURE_LOC_Y := 1214;
const PET_SECURE_LOC_Z := 0;
const PET_SECURE_LOC_R := _DEFAULT_REALM;

//----------------------------------------------------------

const PROP_SECURED_PETS := "SecuredPets";
const PROP_PET_SECURE_LOC := "SecurePetLoc";
const PROP_PET_SECURED := "PetSecured";

//----------------------------------------------------------
//----------------------------------------------------------

function SendGlobPetSecureRequest(Pet)

  var _ev        := struct;
  _ev.+type      := EVID_PET_SECURE;
  _ev.+pet       := Pet;

  var Proc   := GetGlobCtrlProc(":reslogon:petsecurecore");
  if(Proc)
    return Proc.sendevent(ev);
  endif

  return RetError("Brak Procesu");

endfunction

//----------------------------------------------------------

function SaveSecuredPet(Pet)
  
  var Master := GetMaster(Pet);
  
  if(!Master or !IsPlayer(Master))
    LogError("SecureMobile: Nie ma Mastera, albo Master nie jest graczem: " + Master.serial, LOGF_SCRIPTS);
    return;
  endif

  var Mobs := GetObjProperty(Master, PROP_SECURED_PETS);
  
  if(!Mobs)
    Mobs := array;
  endif

  Mobs.append(Pet.serial);
  SetObjProperty(Master, PROP_SECURED_PETS, Mobs);

endfunction

//----------------------------------------------------------

function UnSecurePets(Master)

  var Pets := GetObjProperty(Master, PROP_SECURED_PETS);
  
  if(!Pets)
    return;
  endif

  foreach Pet in Pets
    var PetObj := FindNpcBySerial(Pet);
    if(!PetObj)
      LogError("SecureMobile: Nie ma peta: " + Pet + " gracza: " + Master.serial, LOGF_SCRIPTS);
      continue;
    endif
    
    SendChEvent(Pet, Master, EVID_MOUNT_LOGON);


  endforeach

  EraseObjProperty(Master, PROP_SECURED_PETS);

endfunction 

//----------------------------------------------------------

function CheckSecurePet(Pet)
    
  var Master := GetMaster(Pet);

  if(!Master or !IsPlayer(Master) or IsStaff(Master))
    return;
  endif

  if(!GetObjProperty(me,"TempHappinessCounter"))
    SetObjProperty(me,"TempHappinessCounter",GetHappinessCounter(Me) - ReadGameClock());
  endif

  if(!GetObjProperty(Pet, PROP_PET_SECURED)) 
    
    if(!GetGlobalProperty(PROP_RESLOGON))
      return;
    endif

    sleepms(300+RandomInt(200)+RandomInt(5));
    var Ret := SendGlobPetSecureRequest(Pet);
    if(! Ret )
       LogError("SecureMobile: Pet: " + Ret.errortext, LOGF_SCRIPTS);
      return;
    endif
    SetObjProperty(Pet, PROP_PET_SECURED, ReadGameClock());
    var Loc := struct;
    Loc.+x := Pet.x;
    Loc.+y := Pet.y;
    Loc.+z := Pet.z;
    Loc.+realm := Pet.realm;
    SetObjProperty(Pet, PROP_PET_SECURE_LOC, Loc);
    MoveObject(Pet, PET_SECURE_LOC_X, PET_SECURE_LOC_Y, PET_SECURE_LOC_Z, PET_SECURE_LOC_R, MOVECHAR_FORCELOCATION); 
    Pet.hidden := 1;
  endif

  var Time := GetObjProperty(Pet, PROP_PET_SECURED);
  

  while(ReadGameClock() < Time + PET_SECURE_TIME_LIMIT)
    
    if(IsLoggedIn(Master))
      break;
    endif

    var Event := wait_for_event(60);

    if(Event)
      if(Event.type == EVID_MOUNT_LOGON)
        break;
      endif
    endif

  endwhile

  var Loc := GetObjProperty(Pet, PROP_PET_SECURE_LOC);

  if(!Loc)
    LogError("SecureMobile: Pet: " + Pet.serial + " gracza: " + Master.serial + " nie ma lokacji powrotu", LOGF_SCRIPTS);
    return;
  endif

  if(!MoveObject(Pet, Loc.x, Loc.y, Loc.z, Loc.realm, MOVECHAR_FORCELOCATION))
    LogError("SecureMobile: Pet: " + Pet + " gracza: " + Master.serial + " nie moze zostac przeniesiony do: " + Loc.x + " " + Loc.y + " " + Loc.z + " " + Loc.realm, LOGF_SCRIPTS);
    return;
  endif

  EraseObjProperty(Pet, PROP_PET_SECURED);
  EraseObjProperty(Pet, PROP_PET_SECURE_LOC);

  Pet.hidden := 0;

  if(GetObjProperty(me,"TempHappinessCounter"))
    SetHappinessCounter(Me,ReadGameClock() + CInt(GetObjProperty(me,"TempHappinessCounter")));
    EraseObjProperty(me,"TempHappinessCounter");
  endif

endfunction

//----------------------------------------------------------
