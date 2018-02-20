/* Notes:
 *  This INO sketch was created to test the classes for playing a WAV-Sample, 
 *  Mix different samples into one main-output and hand it over to an
 *  I2S-DAC.
 *  Finally after playing some samples in an unspecified loop the system 
 *  should stop playing new samples and should switch off the output without to much noise.
 *  I have tested to play 4 sounds out of progmem at the same time without any bigger issues.
 *  
 *  The purpose is to use this to create Your own homegrown music-sampleplayer, loop-playback, 
 *  e-drum, e-percussion, sound-art-installation or 
 *  an midi-instrument.
 *  
 *  I wanted to get the library to the point to play 4 sounds indipendently like a drummer could do.
 *  the sounds were sampled from an softsynth by myself. I used only 2 sounds, one short and one with a longer duration 
 *  to be able to test overlapping durations.
 *  
 *  Erich Heinemann
 *  
 *  added mods...dagnall
 *  modified for wav and nodac testing 
 *  changed to viola sound from earles tests, as it is easier to hear, and added data for spiffs tests using wavs cut to 44k from http://www.openairlib.net/sites/default/files/anechoic/data/helena-daffern/baroque-viola/mono/vla-bar-chrom-ed.wav 
 *  
 *  DAGNALL
*/
#define _2CH
//#define _3CH   //Directive to allow quick explores of what happens if we have 3 channels or 2 or 1

#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

//#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"


// #include "AudioOutputI2Sesp32.h"
// #include "AudioOutputI2S.h"
//#include "AudioOutputI2SDAC.h"   //test
#include "AudioOutputI2SNoDAC.h"

#include "AudioMixerOutBuffer.h"
#include "AudioMixerInBuffer.h"
  

// Sample, 
//#include "angels.h"
//#include "arabicbass.h"
//#include "viola.h"

AudioGeneratorWAV *wav1;
#ifdef _2CH
AudioGeneratorWAV *wav2;
#endif

#ifdef _3CH
AudioGeneratorWAV *wav3;
#endif


AudioFileSourceSPIFFS *file1;
#ifdef _2CH
AudioFileSourceSPIFFS *file2;
#endif
#ifdef _3CH
AudioFileSourceSPIFFS *file3;
#endif


//AudioOutputI2SDAC *out; // test
AudioOutputI2SNoDAC *out;   


AudioMixerInBuffer *channel1;
#ifdef _2CH
AudioMixerInBuffer *channel2;
#endif
#ifdef _3CH
AudioMixerInBuffer *channel3;
#endif

AudioMixerOutBuffer *mainOut;


  int soundplays = 0;

  
void setup()
{
  soundplays = 0;
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(1000);
 // file1 = new AudioFileSourceSPIFFS( "/viochrom44.wav" ); // long duration
 
  file1 = new AudioFileSourceSPIFFS( "/piz44.wav" ); // long duration 
  #ifdef _2CH
  file2 = new AudioFileSourceSPIFFS( "/silence.wav"); // start with ony 1 ch
  #endif
  #ifdef _3CH
   file3 = new AudioFileSourceSPIFFS( "/silence.wav"); // start with ony 1 ch
#endif
 


 // out = new AudioOutputI2SDAC(); //test
  out = new AudioOutputI2SNoDAC();
  // Adjust Volume of Output
  out->SetGain(0.2);

  // out2 = new AudioOutputI2S();
  // out2->SetGain(0.2);
  // out2->SetPinout(26, 25, 33); // the original loibrary uses pin 22 rather than 33 for DIN
  
  wav1 = new AudioGeneratorWAV();
  #ifdef _2CH
  wav2 = new AudioGeneratorWAV();
#endif
#ifdef _3CH   
  wav3 = new AudioGeneratorWAV();
#endif
  
  Serial.println("Define MainOut of Mixer");
  // new with Mixer, connect mainOut to the output
  mainOut = new AudioMixerOutBuffer( 8, out );
  
  mainOut->SetRate(44100);
  mainOut->SetChannels(1); // 2=Stereo, 1=Mono
  mainOut->SetBitsPerSample(16); // 16 Bit

  Serial.println("Begin MainOut");
  mainOut->begin();

  // define channel1 of the Mixer and strip it to mainOut as Strip 1
  // defined with buffersize==8, sink==mainout, channelnumber==1
  
  Serial.println("Define Channel1 of Mixer");
  channel1 = new AudioMixerInBuffer( 8, mainOut, 1 );
  channel1->begin();
 #ifdef _2CH 
  // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel2 of Mixer");  
  channel2 = new AudioMixerInBuffer( 8, mainOut, 2 );
  channel2->begin();
  #endif
#ifdef _3CH
   // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel3 of Mixer");  
  channel3 = new AudioMixerInBuffer( 8, mainOut, 3 );
  channel3->begin();
#endif


  Serial.println("Begin Wav1");
  wav1->begin(file1, channel1);
  #ifdef _2CH
  wav2->begin(file2, channel2);
#endif
#ifdef _3CH 
wav3->begin(file3, channel3);
#endif
 
}

void loop()
{
 
  if (wav1->isRunning()  ) {
    if (!wav1->loop() ) {
      soundplays =soundplays + 1;
      wav1->stop(); 
   //   channel1->stop();
   //   yield();
      if ( soundplays <= 40 ){
         if  ( soundplays <= 30 ){file1 = new AudioFileSourceSPIFFS( "/piz44.wav" );Serial.println("ch 1 : 14 secs scales");}
          else{file1 = new AudioFileSourceSPIFFS( "/silence.wav" );Serial.println(" CH1 silence ");}
        wav1->begin(file1, channel1); // channel1
      }  
    }  
  } 
  
#ifdef _2CH
  if (wav2->isRunning()  ) {
    if (!wav2->loop() ) {
      soundplays =soundplays + 1;
      wav2->stop(); 
    //  channel2->stop();
  //    yield();
      if ( soundplays <= 40 ){
        if  ( soundplays >=25 ){ 
        file2 = new AudioFileSourceSPIFFS( "/viochrom44.wav" );Serial.println(" ch 2 playing arpeggio");}
        else{file2 = new AudioFileSourceSPIFFS( "/silence.wav" );Serial.println("ch 2 :4 sec silence");}
        wav2->begin(file2, channel2 );
      }  
    }
  } 
  #endif
#ifdef _3CH
 if (wav3->isRunning()  ) {
    if (!wav3->loop() ) {
      soundplays =soundplays + 1;
      wav3->stop(); 
   //   channel1->stop();
   //   yield();
      if ( soundplays <= 40 ){
         if  ( soundplays <= 20 ){file3 = new AudioFileSourceSPIFFS( "/silence.wav" );Serial.println("ch 3 :4 sec silence");}
          else{file3 = new AudioFileSourceSPIFFS( "/piz44.wav" );Serial.println(" ch 3 playing arpegions");}
        wav3->begin(file3, channel3); // channel1
      }  
    }  
  } 
#endif



  if ( soundplays > 50 ){
    // Stop
    soundplays = 0;
    }

#ifdef _3CH  
  // Stop the Output if all is done ..
  if ( soundplays >=20 && soundplays < 27) {
    out->stop();
  //   out2->stop();
  }
  #endif
  
}

