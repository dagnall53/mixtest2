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


#define _Ch2_ON   //
//#define _Ch3_ON   //Directive to allow quick explores of what happens if we have 4, 3 channels or 2 or 1
//#define _Ch4_ON    //

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
#ifdef _Ch2_ON
AudioGeneratorWAV *wav2;
#endif

#ifdef _Ch3_ON
AudioGeneratorWAV *wav3;
#endif
#ifdef _Ch4_ON
AudioGeneratorWAV *wav4;
#endif

AudioFileSourceSPIFFS *file1;
#ifdef _Ch2_ON
AudioFileSourceSPIFFS *file2;
#endif
#ifdef _Ch3_ON
AudioFileSourceSPIFFS *file3;
#endif
#ifdef _Ch4_ON
AudioFileSourceSPIFFS *file4;
#endif

//AudioOutputI2SDAC *out; // test
AudioOutputI2SNoDAC *out;   


AudioMixerInBuffer *channel1;
#ifdef _Ch2_ON
AudioMixerInBuffer *channel2;
#endif
#ifdef _Ch3_ON
AudioMixerInBuffer *channel3;
#endif
#ifdef _Ch4_ON
AudioMixerInBuffer *channel4;
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
  #ifdef _Ch2_ON
  file2 = new AudioFileSourceSPIFFS( "/silence.wav"); // start with ony 1 ch
  #endif
  #ifdef _Ch3_ON
   file3 = new AudioFileSourceSPIFFS( "/silence.wav"); // start with ony 1 ch
#endif
 #ifdef _Ch4_ON
   file4 = new AudioFileSourceSPIFFS( "/silence.wav"); // start with ony 1 ch
#endif


 // out = new AudioOutputI2SDAC(); //test
  out = new AudioOutputI2SNoDAC();
  // Adjust Volume of Output
  out->SetGain(0.2);

  // out2 = new AudioOutputI2S();
  // out2->SetGain(0.2);
  // out2->SetPinout(26, 25, 33); // the original loibrary uses pin 22 rather than 33 for DIN
  
  wav1 = new AudioGeneratorWAV();
  #ifdef _Ch2_ON
  wav2 = new AudioGeneratorWAV();
#endif
#ifdef _Ch3_ON   
  wav3 = new AudioGeneratorWAV();
#endif
#ifdef _Ch4_ON   
  wav4 = new AudioGeneratorWAV();
#endif
  
  Serial.println("Define MainOut of Mixer");
  // new with Mixer, connect mainOut to the output
  mainOut = new AudioMixerOutBuffer( 8, out );
  
  mainOut->SetRate(44100);
  mainOut->SetChannels(2); // 2=Stereo, 1=Mono
  mainOut->SetBitsPerSample(16); // 16 Bit

  Serial.println("Begin MainOut");
  mainOut->begin();

  // define channel1 of the Mixer and strip it to mainOut as Strip 1
  // defined with buffersize==8, sink==mainout, channelnumber==1
  
  Serial.println("Define Channel1 of Mixer");
  channel1 = new AudioMixerInBuffer( 8, mainOut, 1 );
  channel1->begin();
 #ifdef _Ch2_ON 
  // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel2 of Mixer");  
  channel2 = new AudioMixerInBuffer( 8, mainOut, 2 );
  channel2->begin();
  #endif
#ifdef _Ch3_ON
   // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel3 of Mixer");  
  channel3 = new AudioMixerInBuffer( 8, mainOut, 3 );
  channel3->begin();
#endif
#ifdef _Ch4_ON
   // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel4 of Mixer");  
  channel4 = new AudioMixerInBuffer( 8, mainOut, 4 );
  channel4->begin();
#endif

  Serial.println("Begin Wav1");
  wav1->begin(file1, channel1);
  #ifdef _Ch2_ON
  wav2->begin(file2, channel2);
  #endif
  #ifdef _Ch3_ON 
  wav3->begin(file3, channel3);
  #endif
  #ifdef _Ch4_ON 
  wav4->begin(file4, channel4);
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
         if  ( soundplays <= 30 ){file1 = new AudioFileSourceSPIFFS( "/piz44.wav" );Serial.println("ch 1 : 14 secs Pizzicato ");}
          else{file1 = new AudioFileSourceSPIFFS( "/silence.wav" );Serial.println(" CH1 silence ");}
        wav1->begin(file1, channel1); // channel1
      }  
    }  
  } 
  
#ifdef _Ch2_ON
  if (wav2->isRunning()  ) {
    if (!wav2->loop() ) {
      soundplays =soundplays + 1;
      wav2->stop(); 
    //  channel2->stop();
  //    yield();
      if ( soundplays <= 40 ){
        if  ( soundplays >=25 ){ 
        file2 = new AudioFileSourceSPIFFS( "/viochrom44.wav" );Serial.println(" ch 2 playing bowed arpeggio");}
        else{file2 = new AudioFileSourceSPIFFS( "/silence.wav" );Serial.println("ch 2 :4 sec silence");}
        wav2->begin(file2, channel2 );
      }  
    }
  } 
  #endif
#ifdef _Ch3_ON
 if (wav3->isRunning()  ) {
    if (!wav3->loop() ) {
      soundplays =soundplays + 1;
      wav3->stop(); 
   //   channel1->stop();
   //   yield();
      if ( soundplays <= 40 ){
         if  ( soundplays <= 20 ){file3 = new AudioFileSourceSPIFFS( "/silence.wav" );Serial.println("ch 3 :4 sec silence");}
          else{file3 = new AudioFileSourceSPIFFS( "/piz44.wav" );Serial.println(" ch 3 playing pizzicato");}
        wav3->begin(file3, channel3); // channel1
      }  
    }  
  } 
#endif

#ifdef _Ch4_ON
 if (wav4->isRunning()  ) {
    if (!wav4->loop() ) {
      soundplays =soundplays + 1;
      wav4->stop(); 
   //   channel4->stop();
   //   yield();
      if ( soundplays <= 40 ){
         if  ( soundplays <= 10 ){file4 = new AudioFileSourceSPIFFS( "/silence.wav" );Serial.println("ch 4 :4 sec silence");}
            else{file4 = new AudioFileSourceSPIFFS( "/piz44.wav" );Serial.println(" ch 4 playing pizzicato");}
        wav4->begin(file4, channel4); // channel1
      }  
    }  
  } 
#endif


  if ( soundplays > 50 ){
    // Stop
    soundplays = 0;
    }

#ifdef _Ch3_ON  
  // Stop the Output if all is done ..
  if ( soundplays >=20 && soundplays < 27) {
    out->stop();
  //   out2->stop();
  }
  #endif
  
}

