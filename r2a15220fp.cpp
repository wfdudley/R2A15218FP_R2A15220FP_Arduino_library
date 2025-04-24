// #include <cstdint>
// #include <cstdio>
/* Copyright 2024 William F. Dudley Jr.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "r2a15220fp.h"
#include <Arduino.h>

#define BIT_DELAY 2

static const uint8_t tone_lookup[16] = { 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0,
			          0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0 };

// it's all bit-banged, so you can choose almost any pins you like.
R2A15220FP::R2A15220FP(uint8_t data, uint8_t clock) {
    this->_d = data;
    this->_c = clock;
    this->_front_balance = 0;
    this->Aregister.DA.address     = 1;
    this->Bregister.DB.address     = 2;
    this->Cregister.DC.address     = 3;
    this->Dregister.DD.address     = 4;
    this->Eregister.DE.address     = 5;
    this->Fregister.DF.address     = 6;
    this->Gregister.DG.address     = 7;
#if 1
    pinMode(this->_c, OUTPUT);
    pinMode(this->_d, OUTPUT);
    digitalWrite(this->_c, LOW);
    digitalWrite(this->_d, LOW);
#endif
}

/*
 * to send:
 * for each 23 bits:
 * present data bit
 * clock hi
 * set data lo
 * clock lo

 * for the 24th bit:
 * present data bit
 * clock hi
 * AND THEN, set data hi
 * clock lo
 */

void R2A15220FP::dump (uint8_t address) {
  uint32_t ui32;
  switch(address) {
    case 1 :
        ui32 = Aregister.UI1.ui;
        break;
    case 2 :
        ui32 = Bregister.UI1.ui;
        break;
    case 3 :
        ui32 = Cregister.UI1.ui;
        break;
    case 4 :
        ui32 = Dregister.UI1.ui;
        break;
    case 5 :
        ui32 = Eregister.UI1.ui;
        break;
    case 6 :
        ui32 = Fregister.UI1.ui;
        break;
    case 7 :
        ui32 = Gregister.UI1.ui;
        break;
  }
  Serial.printf("%c: ", ('A' - 1) + address);
  for(int i = 0 ; i < 24 ; i++) {
    Serial.printf("%1d ", (unsigned int)((ui32 & 0x800000) >> 23));
    if(!((i+1) % 8)) Serial.printf(" ");
    ui32 <<= 1;
  }
  Serial.printf("\n");
}

void R2A15220FP::send (uint32_t s) {
#if 1
  for(int i = 0 ; i < 23 ; i++) {
    digitalWrite(_d, (s & 0x800000) >> 23);
    delayMicroseconds(BIT_DELAY);
    digitalWrite(_c, HIGH);
    delayMicroseconds(BIT_DELAY);
    digitalWrite(_d, LOW);
    delayMicroseconds(BIT_DELAY);
    digitalWrite(_c, LOW);
    delayMicroseconds(BIT_DELAY);
    s <<= 1;
  }
  digitalWrite(_d, (s & 0x800000) >> 23);
  delayMicroseconds(BIT_DELAY);
  digitalWrite(_c, HIGH);
  delayMicroseconds(BIT_DELAY);
  digitalWrite(_d, HIGH);
  delayMicroseconds(BIT_DELAY);
  digitalWrite(_c, LOW);
  delayMicroseconds(BIT_DELAY);
  digitalWrite(_d, LOW);
#endif
}

// v is in 1/2 dB increments
void R2A15220FP::set_front_vol(int16_t v) {
  int16_t lv = v + (_front_balance >> 1);
  int16_t rv = v - (_front_balance >> 1);
  if(lv <= 0) {
    Bregister.DB.flch_vol_hi = 0; 
    if(lv < -190) { lv = -190; };
    Bregister.DB.flch_vol_lo = 0 - lv; 
  }
  else {
    Bregister.DB.flch_vol_lo = 0; 
    if(lv > 84) { lv = 84; }
    Bregister.DB.flch_vol_hi = lv; 
  }
  if(rv <= 0) {
    Cregister.DC.frch_vol_hi = 0; 
    if(rv < -190) { rv = -190; };
    Cregister.DC.frch_vol_lo = 0 - rv; 
  }
  else {
    Cregister.DC.frch_vol_lo = 0; 
    if(rv > 84) { rv = 84; }
    Cregister.DC.frch_vol_hi = rv; 
  }
  send(Bregister.UI1.ui);
  send(Cregister.UI1.ui);
}

// v is in 1/2 dB increments
void R2A15220FP::set_subwoof_vol(int16_t v) {
  if(v <= 0) {
    if(v < -190) { v = -190; };
    Dregister.DD.swch_vol = 0 - v; 
  }
  else {
    if(v > 84) { v = 84; }
    Dregister.DD.swch_vol = v + 384; 
  }
  send(Dregister.UI1.ui);
}

// v is in 1/2 dB increments
void R2A15220FP::set_center_vol(int16_t v) {
  if(v <= 0) {
    if(v < -190) { v = -190; };
    Dregister.DD.cch_vol = 0 - v; 
  }
  else {
    if(v > 84) { v = 84; }
    Dregister.DD.cch_vol = v + 384; 
  }
  send(Dregister.UI1.ui);
}

// v is in 1/2 dB increments
void R2A15220FP::set_surround_vol(int16_t v) {
  if(v <= 0) {
    if(v < -190) { v = -190; };
    Eregister.DE.srch_vol = 0 - v; 
    Eregister.DE.slch_vol = 0 - v; 
  }
  else {
    if(v > 84) { v = 84; }
    Eregister.DE.srch_vol = v + 384; 
    Eregister.DE.slch_vol = v + 384; 
  }
  send(Eregister.UI1.ui);
}

// which is 1 to 16, value in register is 0 to 15
void R2A15220FP::set_main_input (uint8_t which) {
  Aregister.DA.input_main = --which;
  Aregister.DA.rec_i_o = (which == 0x0b || which == 0x0c) ? 1 : 0;
  send(Aregister.UI1.ui);
}

// this is direct - no volume control ?
/* this is mostly guess work:
 * 3 = SLCIN & SRCIN
 * 2 = SBLCIN & SBRCIN - tested
 * 1 = SBLIN1 & SBRIN1
 * 0 = SLCIN  & SRCIN
 */
void R2A15220FP::set_SBLSBR_vol_input (uint8_t v) {
  Aregister.DA.sblsbr_vol_in = v;
  send(Aregister.UI1.ui);
}

// this is after front volume control, ignores surround volume control
void R2A15220FP::set_front_to_surround_input (bool b) {
  Aregister.DA.frt_to_sur = b;
  send(Aregister.UI1.ui);
}

// 2 bits
void R2A15220FP::set_mystery_A_v(uint8_t v) {
  Aregister.DA.fillerA = v;
  send(Aregister.UI1.ui);
}

// one bit
void R2A15220FP::set_enable_rec_out(bool b) {
  Aregister.DA.enable_rec_out = b;
  send(Aregister.UI1.ui);
}

// which is 1 to 16, value in register is 0 to 15
void R2A15220FP::set_sub_input (uint8_t which) {
  Aregister.DA.input_sub = --which;
  Aregister.DA.rec_i_o = (which == 0x0b || which == 0x0c) ? 1 : 0;
  send(Aregister.UI1.ui);
}

// arg is boost or cut in dB, 2 dB increments
void R2A15220FP::set_bass (int16_t bv) {
  if(bv > 14) { bv = 14; }
  if(bv < -14) { bv = -14; }
  bv += 14;
  Aregister.DA.bass = tone_lookup[bv/2];
  send(Aregister.UI1.ui);
}

// arg is boost or cut in dB, 2 dB increments
void R2A15220FP::set_treble (int16_t tv) {
  if(tv > 14) { tv = 14; }
  if(tv < -14) { tv = -14; }
  tv += 14;
  Bregister.DB.treble = tone_lookup[tv/2];
  send(Bregister.UI1.ui);
}

// two bits
void R2A15220FP::set_mystery_B_v(uint8_t v) {
  Bregister.DB.fillerA = v;
  send(Bregister.UI1.ui);
}

void R2A15220FP::set_LR_vol_input (uint8_t v) {
  v &= 0x03;
  if(v == 2) return;
  Aregister.DA.l_r_vol_in = v;
  send(Aregister.UI1.ui);
}

void R2A15220FP::set_SLSR_vol_input (uint8_t v) {
  v &= 0x03;
  Eregister.DE.sl_sr_vol_i = v;
  send(Eregister.UI1.ui);
}

void R2A15220FP::set_att_out_level (uint8_t v) {
  v &= 0x03;
  Dregister.DD.ATT = v;
  send(Dregister.UI1.ui);
}

void R2A15220FP::set_tone_bypass (bool b) {
  Fregister.DF.bypass = b;
  send(Fregister.UI1.ui);
}

void R2A15220FP::set_mix_plus_tone_bypass (bool b) {
  Fregister.DF.mix = b;
  send(Fregister.UI1.ui);
}

// 18 bits
void R2A15220FP::set_mystery_F_v(uint8_t v) {
  Fregister.DF.fillerA = v;
  send(Fregister.UI1.ui);
}

// 1 bits
void R2A15220FP::set_mystery_F_b(bool b) {
  Fregister.DF.fillerB = b;
  send(Fregister.UI1.ui);
}

// 21 bits
void R2A15220FP::set_mystery_G_v(uint8_t v) {
  send(Gregister.UI1.ui);
}

void R2A15220FP::set_cent_subw_vol_input (bool b) {
  Dregister.DD.c_sw_vol = b;
  send(Dregister.UI1.ui);
}

void R2A15220FP::set_rec_input_main_sub (bool v) {
  Cregister.DC.recm_s = v;
  send(Cregister.UI1.ui);
}

void R2A15220FP::set_recN (uint8_t n, bool v) {
  switch(n) {
    case 1 :
      Cregister.DC.rec1 = v;
      break;
    case 2 :
      Cregister.DC.rec2 = v;
      break;
    case 3 :
      Cregister.DC.rec3 = v;
      break;
    case 4 :
      Cregister.DC.rec4 = v;
      break;
    case 5 :
      Cregister.DC.rec5 = v;
      break;
  }
  send(Cregister.UI1.ui);
}

void R2A15220FP::set_rec_i_o_bypass (bool v) {
  Aregister.DA.rec_i_o = v;
  send(Aregister.UI1.ui);
}

void R2A15220FP::set_front_balance(int v) {
  _front_balance = v;
}
