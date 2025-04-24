// R2A15218FP and R2A15220FP library
/* Copyright 2025 William F. Dudley Jr.

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
#ifndef __R2A15220FP__
#define __R2A15220FP__
#include <Arduino.h>

typedef union {
    struct Da {
	unsigned int address     : 3;	// 001
	unsigned int rec_i_o     : 1;
	unsigned int enable_rec_out : 1;
	unsigned int bass        : 4;
	unsigned int frt_to_sur  : 1;
	unsigned int fillerA     : 2;
	unsigned int sblsbr_vol_in : 2;
	unsigned int l_r_vol_in  : 2;
	unsigned int input_sub   : 4;
	unsigned int input_main  : 4;
	unsigned int filler8     : 8;
    } DA ;
    struct ui1 {
	uint32_t ui;
    } UI1 ;
} regA ;

typedef union {
    struct Db {
	unsigned int address     : 3;	// 010
	unsigned int fillerA     : 2;
	unsigned int treble      : 4;
	unsigned int flch_vol_hi : 7;
	unsigned int flch_vol_lo : 8;
	unsigned int filler8     : 8;
    } DB ;
    struct ui1 {
	uint32_t ui;
    } UI1 ;
} regB ;

typedef union {
    struct Dc {
	unsigned int address     : 3;	// 011
	unsigned int recm_s      : 1;
	unsigned int rec5        : 1;
	unsigned int rec4        : 1;
	unsigned int rec3        : 1;
	unsigned int rec2        : 1;
	unsigned int rec1        : 1;
	unsigned int frch_vol_hi : 7;
	unsigned int frch_vol_lo : 8;
	unsigned int filler8     : 8;
    } DC ;
    struct ui1 {
	uint32_t ui;
    } UI1 ;
} regC ;

typedef union {
    struct Dd {
	unsigned int address     : 3;	// 100
	unsigned int ATT         : 2;
	unsigned int c_sw_vol    : 1;
	unsigned int swch_vol    : 9;
	unsigned int cch_vol     : 9;
	unsigned int filler8     : 8;
    } DD ;
    struct ui1 {
	uint32_t ui;
    } UI1 ;
} regD ;

typedef union {
    struct De {
	unsigned int address     : 3;	// 101
	unsigned int fillerA     : 1;
	unsigned int sl_sr_vol_i : 2;
	unsigned int srch_vol    : 9;
	unsigned int slch_vol    : 9;
	unsigned int filler8     : 8;
    } DE ;
    struct ui1 {
	uint32_t ui;
    } UI1 ;
} regE ;

typedef union {
    struct Df {
	unsigned int address     : 3;	// 110
	unsigned int fillerB     : 1;
	unsigned int mix         : 1;
	unsigned int bypass      : 1;
	unsigned int fillerA     : 18;
	unsigned int filler8     : 8;
    } DF ;
    struct ui1 {
	uint32_t ui;
    } UI1 ;
} regF ;

typedef union {
    struct Dg {
	unsigned int address     : 3;	// 111
	unsigned int fillerA     : 21;
	unsigned int filler8     : 8;
    } DG ;
    struct ui1 {
	uint32_t ui;
    } UI1 ;
} regG ;

class R2A15220FP {
    private:
	uint8_t _d;
	uint8_t _c;
  int _front_balance; // left - right volume difference
	regA Aregister;
	regB Bregister;
	regC Cregister;
	regD Dregister;
	regE Eregister;
	regF Fregister;
	regG Gregister;

    public:
	R2A15220FP(uint8_t, uint8_t);
	void dump (uint8_t addr);
	void send (uint32_t s);
	void set_front_vol(int16_t v);
	void set_center_vol(int16_t v);
	void set_subwoof_vol(int16_t v);
	void set_surround_vol(int16_t v);
	void set_main_input (uint8_t which);
	void set_sub_input (uint8_t which);
	void set_bass (int16_t bv);
	void set_treble (int16_t tv);
	void set_LR_vol_input (uint8_t v);
	void set_SLSR_vol_input (uint8_t v);
	void set_att_out_level (uint8_t v);
	void set_tone_bypass (bool v);
	void set_mix_plus_tone_bypass (bool v);
	void set_cent_subw_vol_input (bool v);
	void set_rec_input_main_sub (bool v);
  void set_recN (uint8_t n, bool v);
	void set_rec_i_o_bypass (bool v);
  void set_SBLSBR_vol_input (uint8_t v);
  void set_front_to_surround_input (bool b);
  void set_enable_rec_out(bool b);
  void set_mystery_A_v(uint8_t v);  // 3 bits
  void set_mystery_B_v(uint8_t v);
  void set_mystery_F_v(uint8_t v);  // 18 bits
  void set_mystery_F_b(bool b);
  void set_mystery_G_v(uint8_t v);
  void set_front_balance(int v);
};

#endif
