#ifndef ZKeyMatrix_h
#define ZKeyMatrix_h

/* -- Zoltan Sari
   -- Keyboard matrix on Arduino Micro
   -- Library was created: 2016.09.28.
   --
 -- */
#include <inttypes.h>

#if ARDUINO >= 100
#include "Arduino.h"       // for delayMicroseconds, digitalPinToBitMask, etc
#endif 

/* -- Sorok száma -- */
#ifndef MATRIX_ROWS
#define MATRIX_ROWS 8
#endif 

/* -- Oszlopok száma -- */
#ifndef MATRIX_COLS
#define MATRIX_COLS 6
#endif 

#define ZKEY_BIT_BEALLITAS_ENGEDELY ((uint8_t)0x01)
#define ZKEY_BIT_BEALLITAS_LHAKTIV ((uint8_t)0x02)
#define ZKEY_BIT_BEALLITAS_HLAKTIV ((uint8_t)0x04)

#define ZKEY_BIT_ALLAPOT_JELENLEGI ((uint8_t)0x01)
#define ZKEY_BIT_ALLAPOT_LEGUTOBBI ((uint8_t)0x02)
#define ZKEY_BIT_ALLAPOT_LHTORTENT ((uint8_t)0x03)
#define ZKEY_BIT_ALLAPOT_HLTORTENT ((uint8_t)0x04)
#define ZKEY_BIT_ALLAPOT_VARAKOZAS ((uint8_t)0x05)

#define ZKEY_MATRIX_PINKBVEZER0 ((int)7)
#define ZKEY_MATRIX_PINKBVEZER1 ((int)8)
#define ZKEY_MATRIX_PINKBVEZER2 ((int)9)
#define ZKEY_MATRIX_PINKBVEZER3 ((int)10)
#define ZKEY_MATRIX_PINKBVEZER4 ((int)11)
#define ZKEY_MATRIX_PINKBVEZER5 ((int)12)

#define ZKEY_MATRIX_PINKBFIGYEL0 (A0)
#define ZKEY_MATRIX_PINKBFIGYEL1 (A1)
#define ZKEY_MATRIX_PINKBFIGYEL2 (A2)
#define ZKEY_MATRIX_PINKBFIGYEL3 (A3)
#define ZKEY_MATRIX_PINKBFIGYEL4 (A4)
#define ZKEY_MATRIX_PINKBFIGYEL5 (A5)
#define ZKEY_MATRIX_PINKBFIGYEL6 (A6)
#define ZKEY_MATRIX_PINKBFIGYEL7 (A7)

#define ZKEY_MATRIX_PINLED (13)

extern char chMintaMatrix[MATRIX_ROWS][MATRIX_COLS];

class ZKey
{
  public:
    ZKey();
    void Init();
    
    bool GetEngedelyezve();
    void SetEngedelyezve(bool argE);
    bool GetLHAktiv();
    void SetLHAktiv(bool argE);
    bool GetHLAktiv();
    void SetHLAktiv(bool argE);

    bool GetAllapotJelenlegi();
    void SetAllapotJelenlegi(bool argE);
    bool GetAllapotLegutobbi();
    void SetAllapotLegutobbi(bool argE);
    bool GetAllapotLHTortent();
    void SetAllapotLHTortent(bool argE);
    bool GetAllapotHLTortent();
    void SetAllapotHLTortent(bool argE);
    bool GetAllapotVarakozas();
    void SetAllapotVarakozas(bool argE);
    unsigned long GetPreviousMillis();
    void SetPreviousMillis(unsigned long argE);
    void SetPreviousMillisNow();
    /* -- Mennyi idő telt el millisec-ben ? -- */
    unsigned long GetCurrentDelay(unsigned long argCurrent);
    /* -- Eltelt-e már adott idő millisec-ben ? -- */
    bool GetDoneDelay(unsigned long argCurrent, unsigned long argWaitMs);
    char GetKeyCommand1();
    void SetKeyCommand1(char argC);
    char GetKeyCommand2();
    void SetKeyCommand2(char argC);

  private:
    uint8_t m_beallitas;
    uint8_t m_allapot;
    unsigned long m_previousMillis;
    char m_keyCommand1;
    char m_keyCommand2;
};

class ZKeyMatrix
{
  public:
  /* -- default parameters -- */
    ZKeyMatrix();
  /* -- user parameters -- */
    ZKeyMatrix(
      int argiPinVezerlo0,
      int argiPinVezerlo1,
      int argiPinVezerlo2,
      int argiPinVezerlo3,
      int argiPinVezerlo4,
      int argiPinVezerlo5,       
      int argiPinFigyelo0,
      int argiPinFigyelo1,
      int argiPinFigyelo2,
      int argiPinFigyelo3,
      int argiPinFigyelo4,
      int argiPinFigyelo5,
      int argiPinFigyelo6,
      int argiPinFigyelo7
      );
     void Init();
     void CheckColumn(int argColIdx);
     String DisplayAllapotok();
     String DisplayAktivak();
     ZKey* GetKeyX(unsigned int argRow, unsigned int argCol);

  private:
     int m_PinVezerlo[MATRIX_COLS];
     int m_PinFigyelo[MATRIX_ROWS];
     ZKey m_keys[MATRIX_ROWS][MATRIX_COLS];
     int m_ActColIdx;
     
};


#endif
