//-----------------------------------------------------------------------------

include "ai/include/npcspeech";
include "../pkg/systems/bulks/bulks";

//-----------------------------------------------------------------------------

function CheckMySpeech(who, text)

  text := lower(text);

  if(GetNpcConfig(me).MercType)

    if(text["restock"] and IsGM(who))
   
      Say("*hmm*");
      MercRestock();

    elseif(CheckSpokenName(text))

      if(text["kupi"])
        if(CanNpcSpeakWith(who))
          sleepms(1000);
          MercSendBuy(who);
        endif
      elseif(text["sprzeda"])
        if(CanNpcSpeakWith(who))
          sleepms(1000);
          if(!Sell(who))
            return 0;
          endif
        endif
      endif
 
    elseif(text["kupi"])
    
      if(len(text) < 10)
        Say("Nie rozumiem, co tam mowisz?");
      elseif(CanNpcSpeakWith(who))
        Buy(who);
      endif
     
    elseif(text["sprzedac wszystko z pojemnika"])

      if(CanNpcSpeakWith(who))
        SellAllCont(who);
      endif

    elseif(text["sprzedac wszystko"] and text != "sprzedac wszystko")

      if(CanNpcSpeakWith(who))
        SellAll(who);
      endif

    elseif(text["sprzeda"])
  
      if(len(text) < 12)
        Say("Nie rozumiem, co tam mowisz?");
      elseif(CanNpcSpeakWith(who))
        Sell(who);
      endif

    endif
  endif
  
  if(text["ucz"])
    
    TurnToward(who);
    if(len(text) < 8)
      Say("Nie rozumiem, co tam mowisz?");
    elseif(CanNpcSpeakWith(who))
      TrainSkill(me, who);
    endif

  elseif(text["napraw"])
    TurnToward(who);
    if(len(text) < 8)
      Say("Nie rozumiem, co tam mowisz?");
    elseif(CanNpcSpeakWith(who))
      if(text["wszystk"])
        rapairAll(who);
      else
        rapair(who);
      endif
    endif
  elseif(text["wycen"])
    TurnToward(who);
    if(len(text) < 8)
      Say("Nie rozumiem, co tam mowisz?");
    elseif(CanNpcSpeakWith(who))
      if(text["wszystk"])
        rapairAll(who,1);
      else
        rapair(who,1);
      endif
    endif
  else
    return 0;
  endif

  return 1;
   
endfunction

//-----------------------------------------------------------------------------
