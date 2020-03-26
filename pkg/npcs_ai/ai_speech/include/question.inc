//-----------------------------------------------------------------------------
// przetwarzanie zdarzenia pytania - mozliwe jest pytanie nawet jak mob jest zajety
//-----------------------------------------------------------------------------

function AISys_Speech_ProcQuestion(Event)

  if(Event.question and lower(TypeOf(Event.answers)) == "dictionary" and Event.script.pid and Event.source.isa(POLCLASS_MOBILE))

    // oops, zajete!
    if(_AI_Speech.QuestionData)
    
      var retEvent := struct;
      retEvent.+type := EVID_QUESTION;
      retEvent.+source := Event.source;
      retEvent.+result := EVRES_FAILED;

      Event.script.SendEvent(retEvent);
      return EVRES_SUCCESS; // przetworzylismy? przetworzylismy!

    endif

    // trzeba zwrocic uwage npeca
    AI_Attention();

    _AI_Speech.QuestionData := Event;
    _AI_Speech.QuestionData.+clock := ReadGameClock();

    AI_Log("Speech", "Question: "+Event.question);

    AISys_Speech_JustSay(Event.source, Event.question);

    return EVRES_SUCCESS;

  else
    AI_LogError("Speech", "Nieprawidlowe dane w zdarzeniu Question!");
  endif

  return EVRES_UNKNOWN;

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_QuestionAnswer(Event)

  if(_AI_Speech.QuestionData.script.pid)
    
    if(_AI_Speech.QuestionData.clock + AI_SPEECH_QUESTION_WAIT_TIME > ReadGameClock())

     if(Event.source == _AI_Speech.QuestionData.source)
      
        var lowerText := lower(Event.text);

        foreach Answer in (_AI_Speech.QuestionData.answers)
          
          if(lowerText[_Answer_iter])
            AISys_Speech_SendAnswer(Answer);
            return 1;
          endif

        endforeach
        
        AISys_Speech_Say(Event.source, AI_SPID_QUESTION_WRONG_ANSWER);

      else

        AISys_Speech_Say(Event.source, AI_SPID_QUESTION_WRONG_DUDE);

      endif

    endif

    return 1;

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_QuestionCheck()

  if(_AI_Speech.QuestionData.script.pid)
  
    if(!CanNpcSpeekWith(Self(), _AI_Speech.QuestionData.source))
      AISys_Speech_SendAnswer(EVRES_FAILED);
    else
      if(_AI_Speech.QuestionData.clock + AI_SPEECH_QUESTION_WAIT_TIME < ReadGameClock())
        AISys_Speech_Say(_AI_Speech.QuestionData.source, AI_SPID_NO_LONGER_WAIT);
        AISys_Speech_SendAnswer(EVRES_FAILED);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_SendAnswer(Res)

  if(_AI_Speech.QuestionData.script.pid)
    
    AI_Log("Speech", "Send question answer: "+Res);

    var Event := struct;
    Event.+type := EVID_QUESTION;
    Event.+source := _AI_Speech.QuestionData.source;
    Event.+result := Res;

    _AI_Speech.QuestionData.script.SendEvent(Event);

  endif

  _AI_Speech.QuestionData := 0;

endfunction

//-----------------------------------------------------------------------------
