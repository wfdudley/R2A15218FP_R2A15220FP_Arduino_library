#include "r2a15220fp.h"
#include <Arduino.h>

#define VOLDATA 25
#define VOLCLK 25
#define AMUTE 17

/* NOTE: You should mute the audio output (outside and independent of
 * the R2A15220FP/R2A15218FP chip) for about 400msec when switching
 * inputs lest you get a speaker or ear damaging POP.  Or so says one
 * guy on the internet.
 */

R2A15220FP audiofp(VOLDATA, VOLCLK);

int16_t Fvolume;
int16_t Bass;
int16_t Treble;
uint8_t main_input = 0;
uint8_t sub_input = 0;
bool muted;

void amute(bool);

void setup(void) {
  Serial.begin(115200);
  delay(500);
  pinMode(AMUTE, OUTPUT);
  amute(muted = 1);
  Serial.println("\nR2A15220FP R2A15218FP demo");
  Serial.println("\nSetting initial conditions");
  Fvolume = -20;
  Bass = -2;
  Treble = +4;
  main_input = 1;
  sub_input = 2;
  audiofp.set_main_input(main_input);
  audiofp.set_sub_input(sub_input);
  audiofp.set_front_vol(Fvolume);
  audiofp.set_bass(Bass);
  audiofp.set_treble(Treble);
  audiofp.dump(1);
  audiofp.dump(2);
  audiofp.dump(3);
  delay(400);
  amute(muted = 0);
}

void amute(bool mute) {
  digitalWrite(AMUTE, mute);
}

/* possible functions to test:
set_center_vol(int16_t v);
set_subwoof_vol(int16_t v);
set_centch_vol(int16_t v);
set_surround_vol(int16_t v);
set_LR_vol_input (uint8_t v);
set_SLSR_vol_input (uint8_t v);
set_att_out_level (uint8_t v);
set_tone_bypass (bool v);
set_mix_plus_tone_bypass (bool v);
set_cent_subw_vol_input (bool v);
set_rec_input_main_sub (bool v);
set_rec_i_o_bypass (bool v);
*/

void usage(void) {
  Serial.println("nD - dump register n");
  Serial.println("nm - set main input to n");
  Serial.println("ns - set sub input to n");
  Serial.println("V  - increase volume control");
  Serial.println("v  - decrease volume control");
  Serial.println("B  - increase bass tone control");
  Serial.println("b  - decrease bass tone control");
  Serial.println("T  - increase treble tone control");
  Serial.println("t  - decrease treble tone control");
  Serial.println("?  - this help");
  Serial.println("0-9,-  - input number n");
}

void loop(void) {
static int n;
  if(Serial.available()) {
    char c = Serial.read();
    switch(c) {
      case '0' : case '1' : case '2' : case '3' : case '4' :
      case '5' : case '6' : case '7' : case '8' : case '9' :
        n = (n * 10) + c - '0';
        break;
      case '-' :        // minus
        n = 0 - n;
        break;
      case 'V' :        // front volume up
        Fvolume++;
        goto SetVolume;
      case 'v' :        // front volume down
        Fvolume--;
SetVolume: Serial.printf("setting front volume to %d\n", Fvolume);
        audiofp.set_front_vol(Fvolume);
        break;
      case 'B' :        // bass up
        Bass++;
        if(Bass > 14) Bass = 14;
        goto SetBass;
      case 'b' :        // bass down
        Bass--;
        if(Bass < -14) Bass = -14;
SetBass: Serial.printf("setting bass tone to %d\n", Bass);
        audiofp.set_bass(Bass);
        break;
      case 'T' :        // treble up
        Treble++;
        if(Treble > 14) Treble = 14;
        goto SetTreble;
      case 't' :        // treble down
        Treble--;
        if(Treble < -14) Treble = -14;
SetTreble: Serial.printf("setting treble tone to %d\n", Treble);
        audiofp.set_treble(Treble);
        break;
      case 'M' :        // toggle Mute
        amute(muted ^= 1);
        Serial.printf("audio %s muted\n", muted ? "is" : "is not");
        break;
      case 'm' :        // main input
        if(n) {
          main_input = n & 0x0f;
          Serial.printf("setting main input to %d (with mute)\n", main_input);
          amute(muted = 1);
          audiofp.set_main_input(main_input);
          delay(400);
          amute(muted = 0);
          n = 0;
        }
        break;
      case 's' :        // sub input
        if(n) {
          sub_input = n & 0x0f;
          Serial.printf("setting sub input to %d (with mute)\n", sub_input);
          amute(muted = 1);
          audiofp.set_sub_input(sub_input);
          delay(400);
          amute(muted = 0);
          n = 0;
        }
        break;
      case 'd' :        // Dump
        if(n) {
          audiofp.dump(n);
          n = 0;
        }
        break;
      case '?' :        // help
        usage();
        break;
    }
  }
}
