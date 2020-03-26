//-----------------------------------------------------------------------------

function SendWaitGump()

  GInit(50, 50);
  GNoClose();
  GNoDispose();
  GResPic(0, 20, 0xA28, 330, 120);
  GGumpPic(280, 10, 0x589);
  GGumpPic(291, 20, 0x15E8);
  GTilePic(30, 65, 0x1810);

  GTextLine(80, 60, 1420, "Trwa zapis stanu swiata");
  GTextLine(120, 80, 999, "Prosze czekac...");

  foreach Person in EnumerateOnlineCharacters()
	Malloc(person);
    GPacketSend(Person);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function CloseWaitGump()

  foreach Person in EnumerateOnlineCharacters()
    CloseGumpEx(Person, GetPid());
  endforeach

endfunction

//-----------------------------------------------------------------------------
