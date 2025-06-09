# Piano_stm32
stm32, python developing with  buzzer, 4x4Keyboard, Potentiometer, LCD

0-9: 
    Do-high Mi

#: 
    1.Delete LCD output, Store Key
    2.stop the Function A, C, D after pressing their button
    3.Function B continue tune setting

A:
    play 0-9 song 

B:
    setting 0-9 tune

C:
    1.after pressing C select * for looping and playing the song list 0-9
    2.after pressing C select # for randomly playing the song list 0-9

D: 
    after run the python code voice_recognize_test1.py request stm32 instruction, 
    and press the D to activate the python code to use whisper as the voice recognizer,
    and you can say the song name and it will record for 5 second, after that it will start 
    analying the song name if it successfully analyze the song name it will play the song on
    your board
