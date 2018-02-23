# mixtest2

updated friday: Discovered that just with 1 channel, the mixer does not play correctly...

revised and updated today: 
Changed the way that the samples are initiated. They are now initiated on timers in the main loop. 
This has removed some of the click issues I saw, But:
It has become apparent that the mixer does not start until it has ALL channels started. 
Ie with CH1 and Ch2, the code should play the CH1 almost immediately, then add ch2 at 10 sec. but Ch1 does not play until CH2 is started. 
Ditto with 3 channels. 




