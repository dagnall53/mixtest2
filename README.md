# mixtest2
second test of mixer, using spiffs and just two channels
I have only copied the "mixer" files from Erich' libraries. Otherwise I am using the latest Earle Philhower Code library stuff


The example "works" as is, the Files viola and arabicbass left over from when I was testing using progmem. 

I changed to different samples because I was unclear what the percussif sounds in the original test were supposed to sound like, and I also needed to eventually get the code working with SPIFFS. 

This code initially plays a pizzicato scale repeatedly with silence on the other channel,
It then plays a partial bowed arpeggio scale on the other channel at the same time as the scale. and after a few plays turns off the pizzicato scales, just leaving the bowed arpegios. The serial monitor shows what is supposed to be playing...

but: When tested with proper "DAC" output, the Sound quality is very poor, it sounds "wobbly", as though the samples were not being added properly? or were modulated by the mixer?

(Sound quality of the samples was fine if played directly to the DAC output (or NoDAC). (tested with different ino code..))

I have also now tried this mixer code with NoDAC... To me, the NoDAc output is much clearer BUT..
Whilst the The pizzicato scales and bowed arpeggios are audible, there is now a very obvious regular clicking present that is perhaps masked on the DAC output, or is causing the "wobbly" effect? 

My conclusion was that there was something in the mixer adding periodic spikes to the output. 

EXPERIMENTS: carried out on the code: 
--------------------------------------
Working further with the NoDAc variant: 

setting mainOut->SetBitsPerSample(8); /gives a very "wooshy sound.. nasty, but no effect on clicks.. so revert to 16 bit

increasing output buffer size (  mainOut = new AudioMixerOutBuffer( 64, out );) ..No apparent effect 

then also increasing channel buffers (  channel2 = new AudioMixerInBuffer( 32, mainOut, 2 );) .. no apparent effects

Reducing buffer size to "2" for mixer out and channels... Clicks now seem slightly more frequent (perhaps twice as often, 4hz or so..

Reverting to buffersize =8 for all ...clicks again at about 1Hz

next,
-----
Removing (commenting out) yield() from loop "isrunning" code......no effect on playing. still cliks at 1-2 hz

lastly: Commenting out the Channel->stop() in the loops (because as far as I can see, the mixer is running, even if there is no input signal..).. this again has no effect. Clicks are still present and the code works.   

I have now added directives to explore  adding  more "channels" 
With _2CH enabled (but not _3CH) works as described above

with _3CH  enabled, the sounds are much worse,  unintelligable, but no clicks...
Commenting out both _2 and _3 ch: The pizzicato sound is now audible with a sort of quiet background "mush", but NO CLICKS 

I have updated the example ino to this stage of the experiment...







