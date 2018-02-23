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
 *  modified for EASY wav and nodac testing 
 *  changed to viola sounds as it is easier to hear faults etc,  data for spiffs tests using wavs cut to 44k or 11k from http://www.openairlib.net/sites/default/files/anechoic/data/helena-daffern/baroque-viola/mono/vla-bar-chrom-ed.wav 
 *  
 *  DAGNALL
*/
//#define _NoDAC  //COMMENT OUT TO USE WITH I2SDAC  

//#define AudioDebug   // see debug from the mixer? ...does not seem to show anything...
#define _Ch1_ON  //always on?, 
//#define _Ch2_ON   //
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

#ifdef _NoDAC
#include "AudioOutputI2SNoDAC.h" //
#else
#include "AudioOutputI2SDAC.h"   //test
#endif

#include "AudioMixerOutBuffer.h"
#include "AudioMixerInBuffer.h"
  

#ifdef _Ch1_ON
AudioGeneratorWAV *wav1;
#endif
#ifdef _Ch2_ON
AudioGeneratorWAV *wav2;
#endif

#ifdef _Ch3_ON
AudioGeneratorWAV *wav3;
#endif
#ifdef _Ch4_ON
AudioGeneratorWAV *wav4;
#endif
#ifdef _Ch1_ON
AudioFileSourceSPIFFS *file1;
#endif
#ifdef _Ch2_ON
AudioFileSourceSPIFFS *file2;
#endif
#ifdef _Ch3_ON
AudioFileSourceSPIFFS *file3;
#endif
#ifdef _Ch4_ON
AudioFileSourceSPIFFS *file4;
#endif

#ifdef _NoDAC
AudioOutputI2SNoDAC *out; //
#else
AudioOutputI2SDAC *out; // test
#endif
//
   


#ifdef _Ch1_ON
AudioMixerInBuffer *channel1;
#endif
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

uint32_t TIMER1;  
uint32_t TIMER2;  
uint32_t TIMER3;  
uint32_t TIMER4;  
void setup()
{
  soundplays = 0;
  WiFi.mode(WIFI_OFF);

  Serial.begin(115200);
  delay(1000);
//need to set new file to something in setup , because for houskeeping later all new files will be preceeded by Delete 
// may be best to set this to a small silent file..
 #ifdef _Ch1_ON
  file1 = new AudioFileSourceSPIFFS( "/silence11.wav"); // start with 11khz bitrate silence?
  #endif
  #ifdef _Ch2_ON
  file2 = new AudioFileSourceSPIFFS( "/silence11.wav"); // 
  #endif
  #ifdef _Ch3_ON
   file3 = new AudioFileSourceSPIFFS( "/silence11.wav"); // 
#endif
 #ifdef _Ch4_ON
   file4 = new AudioFileSourceSPIFFS( "/silence11.wav"); // 
#endif

#ifdef _NoDAC
  out = new AudioOutputI2SNoDAC();Serial.println("-----------Using NoDAC---------");
#else
 out = new AudioOutputI2SDAC();Serial.println("-----------Using I2S DAC---------"); //test
#endif


  // Adjust Volume etc of Output
  out->SetGain(0.5);

 
#ifdef _Ch1_ON  
  wav1 = new AudioGeneratorWAV();
  #endif
  #ifdef _Ch2_ON
  wav2 = new AudioGeneratorWAV();
#endif
#ifdef _Ch3_ON   
  wav3 = new AudioGeneratorWAV();
#endif
#ifdef _Ch4_ON   
  wav4 = new AudioGeneratorWAV();
#endif

  int buffersize;   // just to make changing /exploring buffer size easier in development 
  buffersize=128;
  int buffersizeM;
  buffersizeM=buffersize;
  
  Serial.println("Define MainOut of Mixer");
  // new with Mixer, connect mainOut to the output
  mainOut = new AudioMixerOutBuffer( buffersize, out );
  mainOut->SetChannels(1); // 2=Stereo, 1=Mono
  mainOut->SetBitsPerSample(16); // 16 Bit

  Serial.println("Begin MainOut");
  mainOut->begin();

  // define channel1 of the Mixer and strip it to mainOut as Strip 1
  // defined with buffersize==xx, sink==mainout, channelnumber==yy
#ifdef _Ch1_ON  
  Serial.println("Define Channel1 of Mixer");
  channel1 = new AudioMixerInBuffer( buffersizeM, mainOut, 1 );  // ch range from 0..15
  channel1->begin();  
  #endif
 #ifdef _Ch2_ON 
  // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel2 of Mixer");  
  channel2 = new AudioMixerInBuffer( buffersizeM, mainOut, 2 );
  channel2->begin();
  #endif
#ifdef _Ch3_ON
   // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel3 of Mixer");  
  channel3 = new AudioMixerInBuffer( buffersizeM, mainOut, 3 );
  channel3->begin();
#endif
#ifdef _Ch4_ON
   // define channel2 of the Mixer and strip it to mainOut as Strip 2
  Serial.println("Define Channel4 of Mixer");  
  channel4 = new AudioMixerInBuffer( buffersizeM, mainOut, 4 );
  channel4->begin();
#endif
/*
  Serial.println("Begin Wavs");
  #ifdef _Ch1_ON
//  wav1->begin(file1, channel1);
  #endif
  #ifdef _Ch2_ON
//  wav2->begin(file2, channel2);
  #endif
  #ifdef _Ch3_ON 
  wav3->begin(file3, channel3);
  #endif
  #ifdef _Ch4_ON 
  wav4->begin(file4, channel4);
  #endif
  */

  // set initial start times for the sounds
 TIMER1=millis()+500; 
  TIMER2=millis()+2000; 
   TIMER3=millis()+5000; 
    TIMER4=millis()+3000; 
 }

void loop()
{
//delay(30);
#ifdef _Ch1_ON  
 // if (!wav1->isRunning()){Serial.print(".");}
if ((millis()>= TIMER1)&&(!wav1->isRunning())){ //do not try and interrupt        
        delete file1;    //housekeeping? .. delete last file   
        //  file1 = new AudioFileSourceSPIFFS( "/viochrom44.wav" ); // long duration
        Serial.print("PLAY ");
        Serial.println(soundplays);
        if (soundplays<=5){
        file1 = new AudioFileSourceSPIFFS( "/piz44.wav" ); Serial.println("Initiating ch 1 : 7 secs 44k Pizzicato ");// long duration 
        }else {
          file1 = new AudioFileSourceSPIFFS( "/piz11.wav" ); Serial.println("Initiating ch 1 : 7 secs 11k Pizzicato ");// long duration 
        }
                  wav1->begin(file1, channel1); 
                  soundplays =soundplays + 1;
     TIMER1=millis()+10000;
   }
   #endif
   
#ifdef _Ch2_ON  
if ((millis()>= TIMER2)&&(!wav2->isRunning())){ //do not try and interrupt      
      delete file2;    //housekeeping? .. delete last file  
      //file2 = new AudioFileSourceSPIFFS( "/viochrom44.wav" ); Serial.println("Initiating ch 2 : bowed Arpeggio ");// long duration
          file2 = new AudioFileSourceSPIFFS( "/piz44.wav" ); Serial.println("Initiating ch 2 : 7 secs 44k Pizzicato ");// long duration 
        wav2->begin(file2, channel2);
     soundplays =soundplays + 1;
     TIMER2=millis()+5000;
   }
   #endif
 #ifdef _Ch3_ON  
  if (millis()>= TIMER3){
    if (wav3->isRunning()){ wav3->stop();  }// force close of old file if new one requested
      if (soundplays <= 5){ delete file3; //housekeeping
      file3 = new AudioFileSourceSPIFFS( "/viochrom44.wav" ); Serial.println("Initiating ch 3 : bowed Arpeggio ");// long duration
      }else{  delete file3; //housekeeping; 
      file3 = new AudioFileSourceSPIFFS( "/piz44.wav" ); Serial.println("Initiating ch 3 : 7 secs Pizzicato ");// long duration 
      }
      wav3->begin(file3, channel3);
     soundplays =soundplays + 1;
     TIMER3=millis()+3000;
   }
   #endif
   #ifdef _Ch4_ON  
if (millis()>= TIMER4){
    if (wav4->isRunning()){ wav4->stop();  }// force close of old file if new one requested
      if (soundplays <= 10){ delete file4; //housekeeping
      file4 = new AudioFileSourceSPIFFS( "/viochrom44.wav" ); Serial.println("Initiating ch 4 : bowed Arpeggio ");// long duration
      }else{  delete file4; //housekeeping; 
      file4 = new AudioFileSourceSPIFFS( "/piz44.wav" ); Serial.println("Initiating ch 4 : 7 secs Pizzicato ");// long duration 
      }
      wav4->begin(file4, channel4);
     soundplays =soundplays + 1;
     TIMER4=millis()+4000;
   }
   #endif 




//delay(100);
 
 #ifdef _Ch1_ON
  if (wav1->isRunning()  ) {
    if (!wav1->loop() ) {
      wav1->stop(); 
     channel1->stop();
      yield();
      } } 
  #endif
  
#ifdef _Ch2_ON
    if (wav2->isRunning()  ) {
    if (!wav2->loop() ) {
      wav2->stop(); 
      channel2->stop();
      yield();
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
         }  } 
#endif

#ifdef _Ch4_ON
 if (wav4->isRunning()  ) {
    if (!wav4->loop() ) {
      soundplays =soundplays + 1;
      wav4->stop(); 
   //   channel4->stop();
   //   yield();
        }   } 
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

