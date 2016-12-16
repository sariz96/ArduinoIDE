/* -- Zoltan Sari
   -- Keyboard matrix on Arduino Micro
   -- Library was created: 2016.09.28.
   --
 -- */

#include "ZKeyMatrix.h"

/* -- for testing .... -- */
char chMintaMatrix[MATRIX_ROWS][MATRIX_COLS] = {
 {'A', 'B', 'C', 'D', 'E', 'F'},
 {'G', 'H', 'I', 'J', 'K', 'L'},
 {'M', 'N', 'O', 'P', 'Q', 'R'},
 {'S', 'T', 'U', 'V', 'X', 'Y'},
 {'a', 'b', 'c', 'd', 'e', 'f'},
 {'g', 'h', 'i', 'j', 'k', 'l'},
 {'m', 'n', 'o', 'p', 'q', 'r'},
 {'s', 't', 'u', 'v', 'x', 'y'}
};

/* --- ZKey --------------------- */

ZKey::ZKey()
{
  m_beallitas = 0;
  m_keyCommand1 = 0;
  m_keyCommand2 = 0;
  
  Init();
}

void ZKey::Init()
{
  m_allapot = 0;
  m_previousMillis = 0; 
}

bool ZKey::GetEngedelyezve()
{
  return ((m_beallitas & ZKEY_BIT_BEALLITAS_ENGEDELY) != 0);
}

void ZKey::SetEngedelyezve(bool argE)
{
  if(argE)
  {
    m_beallitas |= ZKEY_BIT_BEALLITAS_ENGEDELY;
  }
  else
  {
    m_beallitas &= ~ZKEY_BIT_BEALLITAS_ENGEDELY;
  }
}

bool ZKey::GetLHAktiv()
{
  return ((m_beallitas & ZKEY_BIT_BEALLITAS_LHAKTIV) != 0);
}

void ZKey::SetLHAktiv(bool argE)
{
  if(argE)
  {
    m_beallitas |= ZKEY_BIT_BEALLITAS_LHAKTIV;
  }
  else
  {
    m_beallitas &= ~ZKEY_BIT_BEALLITAS_LHAKTIV;
  }
}

bool ZKey::GetHLAktiv()
{
  return ((m_beallitas & ZKEY_BIT_BEALLITAS_HLAKTIV) != 0);
}

void ZKey::SetHLAktiv(bool argE)
{
  if(argE)
  {
    m_beallitas |= ZKEY_BIT_BEALLITAS_HLAKTIV;
  }
  else
  {
    m_beallitas &= ~ZKEY_BIT_BEALLITAS_HLAKTIV;
  }
}

bool ZKey::GetAllapotJelenlegi()
{
  return ((m_allapot & ZKEY_BIT_ALLAPOT_JELENLEGI) != 0);
}

void ZKey::SetAllapotJelenlegi(bool argE)
{
  if(argE)
  {
    m_allapot |= ZKEY_BIT_ALLAPOT_JELENLEGI;
  }
  else
  {
    m_allapot &= ~ZKEY_BIT_ALLAPOT_JELENLEGI;
  }  
}

bool ZKey::GetAllapotLegutobbi()
{
  return ((m_allapot & ZKEY_BIT_ALLAPOT_LEGUTOBBI) != 0);
}

void ZKey::SetAllapotLegutobbi(bool argE)
{
  if(argE)
  {
    m_allapot |= ZKEY_BIT_ALLAPOT_LEGUTOBBI;
  }
  else
  {
    m_allapot &= ~ZKEY_BIT_ALLAPOT_LEGUTOBBI;
  }  
}

bool ZKey::GetAllapotLHTortent()
{
  return ((m_allapot & ZKEY_BIT_ALLAPOT_LHTORTENT) != 0);
}

void ZKey::SetAllapotLHTortent(bool argE)
{
  if(argE)
  {
    m_allapot |= ZKEY_BIT_ALLAPOT_LHTORTENT;
  }
  else
  {
    m_allapot &= ~ZKEY_BIT_ALLAPOT_LHTORTENT;
  }  
}

bool ZKey::GetAllapotHLTortent()
{
  return ((m_allapot & ZKEY_BIT_ALLAPOT_HLTORTENT) != 0);
}

void ZKey::SetAllapotHLTortent(bool argE)
{
  if(argE)
  {
    m_allapot |= ZKEY_BIT_ALLAPOT_HLTORTENT;
  }
  else
  {
    m_allapot &= ~ZKEY_BIT_ALLAPOT_HLTORTENT;
  }  
}

bool ZKey::GetAllapotVarakozas()
{
  return ((m_allapot & ZKEY_BIT_ALLAPOT_VARAKOZAS) != 0);
}

void ZKey::SetAllapotVarakozas(bool argE)
{
  if(argE)
  {
    m_allapot |= ZKEY_BIT_ALLAPOT_VARAKOZAS;
  }
  else
  {
    m_allapot &= ~ZKEY_BIT_ALLAPOT_VARAKOZAS;
  }  
}

unsigned long ZKey::GetPreviousMillis()
{
  return m_previousMillis;
}

void ZKey::SetPreviousMillis(unsigned long argE)
{
  m_previousMillis = argE;
}

void ZKey::SetPreviousMillisNow()
{
  SetPreviousMillis( millis() );
}

unsigned long ZKey::GetCurrentDelay(unsigned long argCurrent)
{
  return (argCurrent - m_previousMillis);
}

bool ZKey::GetDoneDelay(unsigned long argCurrent, unsigned long argWaitMs)
{
  if( GetCurrentDelay(argCurrent) >= argWaitMs )
  {
    return true;
  }
  return false;
}

char ZKey::GetKeyCommand1()
{
  return m_keyCommand1;  
}

void ZKey::SetKeyCommand1(char argC)
{
  m_keyCommand1 = argC;
}

char ZKey::GetKeyCommand2()
{
  return m_keyCommand2;
}

void ZKey::SetKeyCommand2(char argC)
{
  m_keyCommand2 = argC;
}

/* -- ZKeyMatrix ---------------------- */

ZKeyMatrix::ZKeyMatrix(
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
      int argiPinFigyelo7 )
{ 
        m_PinVezerlo[0] = argiPinVezerlo0;
        m_PinVezerlo[1] = argiPinVezerlo1;
        m_PinVezerlo[2] = argiPinVezerlo2;
        m_PinVezerlo[3] = argiPinVezerlo3;
        m_PinVezerlo[4] = argiPinVezerlo4;
        m_PinVezerlo[5] = argiPinVezerlo5;

        m_PinFigyelo[0] = argiPinFigyelo0;
        m_PinFigyelo[1] = argiPinFigyelo1;
        m_PinFigyelo[2] = argiPinFigyelo2;
        m_PinFigyelo[3] = argiPinFigyelo3;
        m_PinFigyelo[4] = argiPinFigyelo4;
        m_PinFigyelo[5] = argiPinFigyelo5;
        m_PinFigyelo[6] = argiPinFigyelo6;
        m_PinFigyelo[7] = argiPinFigyelo7;
}

/* -- default parameters -- */
ZKeyMatrix::ZKeyMatrix()
{
        m_PinVezerlo[0] = ZKEY_MATRIX_PINKBVEZER0;
        m_PinVezerlo[1] = ZKEY_MATRIX_PINKBVEZER1;
        m_PinVezerlo[2] = ZKEY_MATRIX_PINKBVEZER2;
        m_PinVezerlo[3] = ZKEY_MATRIX_PINKBVEZER3;
        m_PinVezerlo[4] = ZKEY_MATRIX_PINKBVEZER4;
        m_PinVezerlo[5] = ZKEY_MATRIX_PINKBVEZER5;

        m_PinFigyelo[0] = ZKEY_MATRIX_PINKBFIGYEL0;
        m_PinFigyelo[1] = ZKEY_MATRIX_PINKBFIGYEL1;
        m_PinFigyelo[2] = ZKEY_MATRIX_PINKBFIGYEL2;
        m_PinFigyelo[3] = ZKEY_MATRIX_PINKBFIGYEL3;
        m_PinFigyelo[4] = ZKEY_MATRIX_PINKBFIGYEL4;
        m_PinFigyelo[5] = ZKEY_MATRIX_PINKBFIGYEL5;
        m_PinFigyelo[6] = ZKEY_MATRIX_PINKBFIGYEL6;
        m_PinFigyelo[7] = ZKEY_MATRIX_PINKBFIGYEL7;
}

ZKey* ZKeyMatrix::GetKeyX(unsigned int argRow, unsigned int argCol)
{
  ZKey* pEredmeny = NULL;
  if((argRow < MATRIX_ROWS) && (argCol < MATRIX_COLS))
  {
    pEredmeny = &(m_keys[argRow][argCol]);
  }
  return pEredmeny;
}

void ZKeyMatrix::Init()
{
  unsigned long currentMillis = millis();
  ZKey* pK;
  
  m_ActColIdx = 0;

#ifdef ZKEY_MATRIX_PINLED
	if(ZKEY_MATRIX_PINLED > 0)
	{
     pinMode(ZKEY_MATRIX_PINLED, OUTPUT);
	}
#endif	

  for(unsigned int i = 0; i < MATRIX_ROWS; i++)
  {
    for(unsigned int j = 0; j < MATRIX_COLS; j++)
    {
      pK = GetKeyX(i,j);
      pK->Init();
      pK->SetPreviousMillis(currentMillis);
    }
  }

  for(int j=0; j < MATRIX_COLS; j++)
  {
    if(m_PinVezerlo[j] > 0)
    {
      pinMode(m_PinVezerlo[j], OUTPUT);
      digitalWrite(m_PinVezerlo[j], HIGH);
    }
  }
  for(int i=0; i < MATRIX_ROWS; i++)
  {
    if(m_PinFigyelo[i] > 0)
    {
      pinMode(m_PinFigyelo[i], INPUT);
    }
  }
}

void ZKeyMatrix::CheckColumn(int argColIdx)
{
  uint8_t bAllapotX;
  ZKey* pK;
  bool bX;
  
  if((argColIdx >= 0) && (argColIdx < MATRIX_COLS))
  {
#ifdef ZKEY_MATRIX_PINLED
	if(ZKEY_MATRIX_PINLED > 0)
	{
     digitalWrite(ZKEY_MATRIX_PINLED, HIGH);
	}
#endif	
    digitalWrite(m_PinVezerlo[argColIdx], LOW);
    delayMicroseconds(50);

    for(int i=0; i < MATRIX_ROWS; i++)
    {
      if(m_PinFigyelo[i] > 0)
      {
        bAllapotX = digitalRead(m_PinFigyelo[i]);
        /* -- alacsonyan aktív !! -- */
        if(bAllapotX == 0)
        {
          bX = true;
        }
        else
        {
          bX = false;
        } 
        pK = GetKeyX(i,argColIdx);
        pK->SetAllapotJelenlegi(bX);
        /* pK->SetPreviousMillisNow(); */
        delayMicroseconds(10);
      }
    }
    
    digitalWrite(m_PinVezerlo[argColIdx], HIGH);
#ifdef ZKEY_MATRIX_PINLED
	if(ZKEY_MATRIX_PINLED > 0)
	{
     digitalWrite(ZKEY_MATRIX_PINLED, LOW);
	}
#endif	
    delayMicroseconds(50);
  }  
}

String ZKeyMatrix::DisplayAllapotok()
{
  ZKey* pK; 
  String s1 = "Allapotok: ";
  String s2 = "Digital: ";
  for(int i=0; i < MATRIX_ROWS; i++)
  {
    for(int j=0; j < MATRIX_COLS; j++)
    {
      pK = GetKeyX(i,j);
      
      if(pK->GetAllapotJelenlegi())
      {
        s2 = s2 + "(sor:" + String(i+1) + " oszlop:" + String(j+1) + ") " + chMintaMatrix[i][j] + ", ";
      }
      else
      {
        s2 = s2 + " , ";
      }
    }
    s2 = s2 + "| ";
  }

  s1 = s1 + "; " + s2;
  return s1;
}

/* -- Csak az aktívakat adja vissza -- */
String ZKeyMatrix::DisplayAktivak()
{
  String s1 = "";
  ZKey* pK;

  for(int i=0; i < MATRIX_ROWS; i++)
  {
    for(int j=0; j < MATRIX_COLS; j++)
    {
      pK = GetKeyX(i,j);
      
      if(pK->GetAllapotJelenlegi())
      {
        s1 = s1 + chMintaMatrix[i][j];
      }
    }
  }
  return s1;
}
