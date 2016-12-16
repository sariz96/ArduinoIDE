/* -- Zoltan Sari
   -- ESP8266 <--> Arduino Nano I2C Commands
   -- Library was created: 2016.11.30.
   --
	
 -- */

 #include "ZWireCommands.h"

/* -- ZBuffer08 -- */ 
ZBuffer08::ZBuffer08(uint8_t argSize)
{
	if((argSize <= 0) || (argSize > ZWIRECOMM_MAX_BUFFER_LENGTH))
	{
		argSize = ZWIRECOMM_MAX_BUFFER_LENGTH;
	}

	this->m_size = argSize;
	this->m_usedLength = 0;
	this->ClearBuffer(this->m_size);
}

void ZBuffer08::ClearBuffer()
{
	this->ClearBuffer(this->m_size);
}

void ZBuffer08::ClearBuffer(int argLen)
{
	this->ClearBuffer((uint8_t)argLen);
}

void ZBuffer08::ClearBuffer(uint8_t argLen)
{
	if((argLen <= 0) || (argLen > this->m_size))
	{
		argLen = this->m_size;
	}
	memset(this->m_buffer, 0, argLen);	
}

uint8_t *ZBuffer08::GetBuffer()
{
	return (uint8_t *)this->m_buffer;
}

uint8_t ZBuffer08::GetBufferLength()
{
	return this->m_size;	
}

uint8_t ZBuffer08::GetUsedBufferLength()
{
	return this->m_usedLength;
}

void ZBuffer08::SetClientByte(uint8_t argIdx, uint8_t argValueByte)
{
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		this->m_buffer[argIdx] = argValueByte;
	}	
}

void ZBuffer08::SetClientWord(uint8_t argIdx, uint16_t argValueWord)
{
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		this->m_buffer[argIdx++] = highByte(argValueWord);
		this->m_buffer[argIdx] = lowByte(argValueWord);
	}		
}

void ZBuffer08::SetClientDWord(uint8_t argIdx, uint32_t argValueDWord)
{
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		this->m_buffer[argIdx++] = (uint8_t)((argValueDWord >> 24) & 0xff);
		this->m_buffer[argIdx++] = (uint8_t)((argValueDWord >> 16) & 0xff);
		this->m_buffer[argIdx++] = (uint8_t)((argValueDWord >> 8) & 0xff);
		this->m_buffer[argIdx++] = (uint8_t)(argValueDWord & 0xff);
	}
}

void ZBuffer08::SetClientFloat(uint8_t argIdx, float argValueFloat)
{
	this->m_dw2 = *((uint32_t *)(&argValueFloat));
	this->SetClientDWord(argIdx, this->m_dw2);
}


uint8_t ZBuffer08::GetClientByte(uint8_t argIdx)
{
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		return this->m_buffer[argIdx];
	}
	return 0;
}

uint16_t ZBuffer08::GetClientWord(uint8_t argIdx)
{
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		return (uint16_t)word(this->m_buffer[argIdx], this->m_buffer[argIdx + 1]);
	}
	return 0;	
}

uint32_t ZBuffer08::GetClientDWord(uint8_t argIdx)
{
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		this->m_dw1 = (uint32_t)(this->m_buffer[argIdx+3]);
		this->m_dw1 |= ((uint32_t)(this->m_buffer[argIdx+2]) << 8);
		this->m_dw1 |= ((uint32_t)(this->m_buffer[argIdx+1]) << 16);
		this->m_dw1 |= ((uint32_t)(this->m_buffer[argIdx]) << 24);
		return this->m_dw1;
	}
	return 0;	
}

float ZBuffer08::GetClientFloat(uint8_t argIdx)
{
	this->m_dw2 = this->GetClientDWord(argIdx);
	return *((float *)(&this->m_dw2));
}

/* --
[0]:	szöveg hossza,
[1..]:	szöveg karakterei
Return: bemásolt hasznos karakterek száma lezáró 0-val együtt
-- */
uint8_t ZBuffer08::SetClientTxt(uint8_t argIdx, char *argTxt, uint8_t argLength)
{
	this->m_bTmp1 = 0;	/* -- hasznos karakterek száma -- */
	this->m_bTmp2 = 0;	/* -- használható buffer méret kezdő helyzetben -- */
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		this->m_bTmp2 = (this->m_size - 2 - argIdx);
		if(this->m_bTmp2 > (argLength + 2))
		{
			this->m_bTmp2 = argLength + 2;
		}
		this->m_ptrCh1 = argTxt;
		this->m_ptrCh2 = (char *)(this->m_buffer + argIdx + 1);
		for(this->m_bTmp3 = 1; (this->m_bTmp3 < this->m_bTmp2) && (*this->m_ptrCh1 != 0); this->m_bTmp3++)
		{			
			*this->m_ptrCh2++ = *this->m_ptrCh1++;
		}
		*this->m_ptrCh2 = 0;
		this->m_bTmp1 = this->m_bTmp3;
		this->m_buffer[argIdx] = this->m_bTmp1;
	}	
	return this->m_bTmp1;
}

/* --
[0]:	szöveg hossza,
[1..]:	szöveg karakterei
Return: bemásolt hasznos karakterek száma lezáró 0-val együtt
-- */
uint8_t ZBuffer08::GetClientTxt(uint8_t argIdx, char *argTxt, uint8_t argLength)
{
	this->m_bTmp1 = 0;	/* -- hasznos karakterek száma -- */
	this->m_bTmp2 = 0;	/* -- használható buffer méret kezdő helyzetben -- */
	if((argIdx >= 0) && (argIdx < this->m_size))
	{
		this->m_bTmp2 = (this->m_size - 2 - argIdx);
		if(this->m_bTmp2 >= (argLength + 2))
		{
			this->m_bTmp2 = argLength + 2;
		}
		this->m_bTmp1 = this->m_buffer[argIdx];
		this->m_ptrCh1 = argTxt;
		this->m_ptrCh2 = (char *)(this->m_buffer + argIdx + 1);
		for(this->m_bTmp3 = 0; (this->m_bTmp3 < this->m_bTmp1) && (*this->m_ptrCh2 != 0); this->m_bTmp3++)
		{			
			*this->m_ptrCh1++ = *this->m_ptrCh2++;
		}
		*this->m_ptrCh1 = 0;
	}	
	return this->m_bTmp1;
}


/* -- ZWireCommands -- */ 
 
ZWireCommands::ZWireCommands() : ZBuffer08(ZWIRECOMM_MAX_BUFFER_LENGTH)
{
	this->m_uLastReceivedCommand = 0;
}

void ZWireCommands::SetCommand(uint16_t argCmd)
{
	this->SetCommand(argCmd, ZWIRECOMM_MAX_BUFFER_AVG_LENGTH);
}

void ZWireCommands::SetCommand(uint16_t argCmd, uint8_t argClearedBuffLen)
{
	this->ClearBuffer(argClearedBuffLen);
	this->m_uLastReceivedCommand = 0;
	this->m_buffer[0] = highByte(argCmd);
	this->m_buffer[1] = lowByte(argCmd);
}

/* -- Get the received command code on client side -- */
uint16_t ZWireCommands::GetCommand()
{
	this->m_uLastReceivedCommand = (uint16_t)word(this->m_buffer[0], this->m_buffer[1]);
	return this->m_uLastReceivedCommand;
}

void ZWireCommands::SetByte(uint8_t argIdx, uint8_t argValueByte)
{
	argIdx += 2;
	this->SetClientByte(argIdx, argValueByte);
}

void ZWireCommands::SetWord(uint8_t argIdx, uint16_t argValueWord)
{
	argIdx += 2;
	this->SetClientWord(argIdx, argValueWord);
}

void ZWireCommands::SetDWord(uint8_t argIdx, uint32_t argValueDWord)
{
	argIdx += 2;
	this->SetClientDWord(argIdx, argValueDWord);
}

void ZWireCommands::SetFloat(uint8_t argIdx, float argValueFloat)
{
	argIdx += 2;
	this->SetClientFloat(argIdx, argValueFloat);
}

uint8_t ZWireCommands::GetByte(uint8_t argIdx)
{
	argIdx += 2;
	return this->GetClientByte(argIdx);
}

uint16_t ZWireCommands::GetWord(uint8_t argIdx)
{
	argIdx += 2;
	return this->GetClientWord(argIdx);
}

uint32_t ZWireCommands::GetDWord(uint8_t argIdx)
{
	argIdx += 2;
	return this->GetClientDWord(argIdx);
}

float ZWireCommands::GetFloat(uint8_t argIdx)
{
	argIdx += 2;
	return this->GetClientFloat(argIdx);	
}

 /* --- Commands -- */

uint16_t ZWireCommands::GetLastReceivedCommand()
{
	return this->m_uLastReceivedCommand;
}

void ZWireCommands::ClearLastReceivedCommand()
{
	this->m_uLastReceivedCommand = 0;
}

uint8_t ZWireCommands::SetTxt(uint8_t argIdx, char *argTxt, uint8_t argLength)
{
	argIdx += 2;
	return this->SetClientTxt(argIdx, argTxt, argLength);
}

uint8_t ZWireCommands::GetTxt(uint8_t argIdx, char *argTxt, uint8_t argLength)
{
	argIdx += 2;
	return this->GetTxt(argIdx, argTxt, argLength);
}
