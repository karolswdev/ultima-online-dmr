//-----------------------------------------------------------------------------

function AISys_Speech_ProcEvent(Event)

  case(Event.type)
    
    EVID_SPEECH:             return AISys_Speech_Proc(Event);

    EVID_QUESTION:           return AISys_Speech_ProcQuestion(Event);

    default:                 return EVRES_UNKNOWN;
  endcase

endfunction

//-----------------------------------------------------------------------------
// przetwarzanie zdarzenia mowy
//-----------------------------------------------------------------------------

function AISys_Speech_Proc(Event)

  if(!CanNpcSpeekWith(Self(), Event.source))
    return EVRES_PROCESS;
  endif

  AI_Log("Speech", "Process: '"+Event.text+"'");

  if(AISys_Speech_QuestionAnswer(Event))
    return EVRES_SUCCESS;
  endif

  if(AISys_Handler_IsBusy())
    
    // trzeba zwrocic uwage npeca
    AI_Attention();

    if(_AI_Handler.Current.source == Event.source)
      AISys_Speech_Say(Event.source, AI_SPID_BUSY_SOURCE);
    else
      AISys_Speech_Say(Event.source, AI_SPID_BUSY_OTHER);
    endif

    return EVRES_PROCESS;

  endif

  var Triggers := AISys_Speech_Triggered(Event.source, Event.text);
  var bCanSpeak := error;

  if(Triggers.size())

    // trzeba zwrocic uwage npeca
    AI_Attention();
    TurnToward(Event.source);

    foreach Trigger in Triggers

      if(IsBitOn(Trigger.flags, AI_SPT_FULL_SENTENCE) and len(Event.text) < AI_SPEECH_FULL_SENT_SIZE)
        AISys_Speech_Say(Event.source, AI_SPID_DONT_UNDERSTAND);
        return EVRES_FAILED;
      else

        if(!IsBitOn(Trigger.flags, AI_SPT_NO_CAN_SPEAK_TRIG) and bCanSpeak == error)
          if(AISys_Handler_HaveTrigger(AIHT_CAN_SPEAK_WITH))
            if(!AISys_Handler_Trigger(Event.source, AIHT_CAN_SPEAK_WITH))
              return EVRES_PROCESS;
            endif
          endif
          
          bCanSpeak := 1;
        endif

        if(!AISys_Handler_Start(Event.source, Trigger.script, Trigger.hdlflags))
          return EVRES_FAILED;
        endif
      endif

    endforeach

    return EVRES_SUCCESS;

  endif

  return EVRES_PROCESS;

endfunction

//-----------------------------------------------------------------------------
