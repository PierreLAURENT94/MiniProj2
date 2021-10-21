// Atr.cpp: 
//          SmartCard's ATR Processor based on Towitoko Project.
//
//			by Y.Volta Tang (y.volta@gmail.com)
//          2008-01-18 22:06
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Atr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unsigned long 
atr_fs_table[16] =
{ 
	0, 5000000L, 6000000L, 8000000L, 12000000L, 16000000L, 20000000L, 
		0, 0, 5000000L, 7500000L, 10000000L, 15000000L, 20000000L, 0, 0 
};

unsigned
atr_num_ib_table[16] = 
{ 
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 
};

//Exported variables definition
//this is the clock rate adjustment table, 
unsigned
atr_f_table[16] = 
{ 
	0, 372, 558, 744, 1116, 1488, 1860, 0, 0, 512, 768, 1024, 1536, 2048, 0, 0 
};

//this is Baud Rate adjustment table, 
double 
atr_d_table[16] = 
{ 
	0, 1, 2, 4, 8, 16, 0, 0, 0, 0, 0.5, 0.25, 0.125, 0.0625, 0.03125, 0.015625 
};

unsigned 
atr_i_table[4] = 
{ 
	25, 50, 100, 0 
};

//////////////////////////////////////////////////////////////////////////
// Create ATR object from an ATR stream.
CAtr::CAtr(BYTE *byATR, unsigned int length)
{
	BYTE		szHex[ATR_MAX_SIZE+1] = {0};
	INT			nLen=length/2+1;

	ATR_InitFromArray(byATR, length);
}


CAtr::~CAtr()
{

}

//
INT CAtr::ATR_InitFromArray(BYTE atr_buffer[], unsigned int length)
{
	BYTE TDi;
	BYTE buffer[ATR_MAX_SIZE] = {0};
	unsigned pointer = 0, pn = 0;
	
	/* Check size of buffer */
	if (length < 2)
		return (ATR_MALFORMED);
	
	/* Check if ATR is from a inverse convention card */
	if (atr_buffer[0] == 0x03)
	{
		for (pointer = 0; pointer < length; pointer++)
			buffer[pointer] = ~(INVERT_BYTE (atr_buffer[pointer]));
	}
	else
	{
		RtlCopyMemory (buffer, atr_buffer, length);
	}
	/* Store T0 and TS */
	m_ATR.TS = buffer[0];
	
	m_ATR.T0 = TDi = buffer[1];
	pointer = 1;
	
	/* Store number of historical bytes */
	m_ATR.hbn = TDi & 0x0F;
	
	/* TCK is not present by default */
	(m_ATR.TCK).present = FALSE;
	
	/* Extract interface bytes */
	while (pointer < length)
	{
		/* Check buffer is long enough */
		if (pointer + atr_num_ib_table[(0xF0 & TDi) >> 4] >= length)
		{
			return (ATR_MALFORMED);
		}
		/* Check TAi is present */
		if ((TDi | 0xEF) == 0xFF)
		{
			pointer++;
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TA].value = buffer[pointer];
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TA].present = TRUE;
		}
		else
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TA].present = FALSE;
		/* Check TBi is present */
		if ((TDi | 0xDF) == 0xFF)
		{
			pointer++;
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TB].value = buffer[pointer];
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TB].present = TRUE;
		}
		else
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TB].present = FALSE;
		
		/* Check TCi is present */
		if ((TDi | 0xBF) == 0xFF)
		{
			pointer++;
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TC].value = buffer[pointer];
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TC].present = TRUE;
		}
		else
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TC].present = FALSE;
		
		/* Read TDi if present */
		if ((TDi | 0x7F) == 0xFF)
		{
			pointer++;
			TDi = m_ATR.ib[pn][ATR_INTERFACE_BYTE_TD].value = buffer[pointer];
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TD].present = TRUE;
			(m_ATR.TCK).present = ((TDi & 0x0F) != ATR_PROTOCOL_TYPE_T0);
			if (pn >= ATR_MAX_PROTOCOLS)
				return (ATR_MALFORMED);
			pn++;
		}
		else
		{
			m_ATR.ib[pn][ATR_INTERFACE_BYTE_TD].present = FALSE;
			break;
		}
	}
	
	/* Store number of protocols */
	m_ATR.pn = pn + 1;
	
	/* Store historical bytes */
	if (pointer + m_ATR.hbn >= length)
		return (ATR_MALFORMED);
	
	RtlCopyMemory (m_ATR.hb, buffer + pointer + 1, m_ATR.hbn);
	pointer += (m_ATR.hbn);
	
	/* Store TCK  */
	if ((m_ATR.TCK).present)
	{
		
		if (pointer + 1 >= length)
			return (ATR_MALFORMED);
		
		pointer++;
		
		(m_ATR.TCK).value = buffer[pointer];
	}
	
	m_ATR.length = pointer + 1;
	return (ATR_OK);
}

// For 'TS'
INT CAtr::ATR_GetConvention(int *convention)
{
  if (m_ATR.TS == 0x3B)					// Direct Card
    (*convention) = ATR_CONVENTION_DIRECT;
  else if (m_ATR.TS == 0x3F)			// Inverse Card
    (*convention) = ATR_CONVENTION_INVERSE;
  else
    return (ATR_MALFORMED);				// Unknown!!!
  return (ATR_OK);
}

// Length of ATR.
INT CAtr::ATR_GetSize(unsigned int *size)
{
  (*size) = m_ATR.length;
  return (ATR_OK);
}

// How many protocals supported by this Card
INT CAtr::ATR_GetNumberOfProtocols(unsigned int *number_protocols)
{
  (*number_protocols) = m_ATR.pn;
  return (ATR_OK);
}

// T0, T1, RAW?
INT CAtr::ATR_GetProtocolType(unsigned int number_protocol, BYTE *protocol_type)
{
  if ((number_protocol > m_ATR.pn) || number_protocol < 2)
    return ATR_NOT_FOUND;

  if (m_ATR.ib[number_protocol - 2][ATR_INTERFACE_BYTE_TD].present)
    (*protocol_type) =
      (m_ATR.ib[number_protocol - 2][ATR_INTERFACE_BYTE_TD].value & 0x0F);
  else
    (*protocol_type) = ATR_PROTOCOL_TYPE_T0;

  return (ATR_OK);
}

// (TA)i, (TB)i, (TC)i, (TD)i 
INT CAtr::ATR_GetInterfaceByte(unsigned int number, int character, BYTE *value)
{
  if (number > m_ATR.pn || number < 1)
    return (ATR_NOT_FOUND);

  if (m_ATR.ib[number - 1][character].present &&
      (character == ATR_INTERFACE_BYTE_TA ||
       character == ATR_INTERFACE_BYTE_TB ||
       character == ATR_INTERFACE_BYTE_TC ||
       character == ATR_INTERFACE_BYTE_TD))
    (*value) = m_ATR.ib[number - 1][character].value;
  else
    return (ATR_NOT_FOUND);

  return (ATR_OK);

}


INT CAtr::ATR_GetIntegerValue(int name, BYTE *value)
{
  int ret;

  if (name == ATR_INTEGER_VALUE_FI)
    {
      if (m_ATR.ib[0][ATR_INTERFACE_BYTE_TA].present)
	{
	  (*value) = (m_ATR.ib[0][ATR_INTERFACE_BYTE_TA].value & 0xF0) >> 4;
	  ret = ATR_OK;
	}
      else
	ret = ATR_NOT_FOUND;
    }

  else if (name == ATR_INTEGER_VALUE_DI)
    {
      if (m_ATR.ib[0][ATR_INTERFACE_BYTE_TA].present)
	{
	  (*value) = (m_ATR.ib[0][ATR_INTERFACE_BYTE_TA].value & 0x0F);
	  ret = ATR_OK;
	}
      else
	ret = ATR_NOT_FOUND;
    }

  else if (name == ATR_INTEGER_VALUE_II)
    {
      if (m_ATR.ib[0][ATR_INTERFACE_BYTE_TB].present)
	{
	  (*value) = (m_ATR.ib[0][ATR_INTERFACE_BYTE_TB].value & 0x60) >> 5;
	  ret = ATR_OK;
	}
      else
	ret = ATR_NOT_FOUND;
    }

  else if (name == ATR_INTEGER_VALUE_PI1)
    {
      if (m_ATR.ib[0][ATR_INTERFACE_BYTE_TB].present)
	{
	  (*value) = (m_ATR.ib[0][ATR_INTERFACE_BYTE_TB].value & 0x1F);
	  ret = ATR_OK;
	}
      else
	ret = ATR_NOT_FOUND;
    }

  else if (name == ATR_INTEGER_VALUE_PI2)
    {
      if (m_ATR.ib[1][ATR_INTERFACE_BYTE_TB].present)
	{
	  (*value) = m_ATR.ib[1][ATR_INTERFACE_BYTE_TB].value;
	  ret = ATR_OK;
	}
      else
	ret = ATR_NOT_FOUND;
    }

  else if (name == ATR_INTEGER_VALUE_N)
    {
      if (m_ATR.ib[0][ATR_INTERFACE_BYTE_TC].present)
	{
	  (*value) = m_ATR.ib[0][ATR_INTERFACE_BYTE_TC].value;
	  ret = ATR_OK;
	}
      else
	ret = ATR_NOT_FOUND;
    }
  else
    ret = ATR_NOT_FOUND;

  return ret;
}

//
INT CAtr::ATR_GetParameter(int name, double *parameter)
{
  BYTE FI, DI, II, PI1, PI2, N;

  if (name == ATR_PARAMETER_F)
    {
      if (ATR_GetIntegerValue (ATR_INTEGER_VALUE_FI, &FI) == ATR_OK)
	(*parameter) = (double) (atr_f_table[FI]);
      else
	(*parameter) = (double) ATR_DEFAULT_F;
      return (ATR_OK);
    }

  else if (name == ATR_PARAMETER_D)
    {
      if (ATR_GetIntegerValue (ATR_INTEGER_VALUE_DI, &DI) == ATR_OK)
	(*parameter) = (double) (atr_d_table[DI]);
      else
	(*parameter) = (double) ATR_DEFAULT_D;
      return (ATR_OK);
    }

  else if (name == ATR_PARAMETER_I)
    {
      if (ATR_GetIntegerValue (ATR_INTEGER_VALUE_II, &II) == ATR_OK)
	(*parameter) = (double) (atr_i_table[II]);
      else
	(*parameter) = ATR_DEFAULT_I;
      return (ATR_OK);
    }

  else if (name == ATR_PARAMETER_P)
    {
      if (ATR_GetIntegerValue (ATR_INTEGER_VALUE_PI2, &PI2) == ATR_OK)
	(*parameter) = (double) PI2;
      else if (ATR_GetIntegerValue (ATR_INTEGER_VALUE_PI1, &PI1) == ATR_OK)
	(*parameter) = (double) PI1;
      else
	(*parameter) = (double) ATR_DEFAULT_P;
      return (ATR_OK);
    }

  else if (name == ATR_PARAMETER_N)
    {
      if (ATR_GetIntegerValue (ATR_INTEGER_VALUE_N, &N) == ATR_OK)
	(*parameter) = (double) N;
      else
	(*parameter) = (double) ATR_DEFAULT_N;
      return (ATR_OK);
    }

  return (ATR_NOT_FOUND);
}

// Get history bytes.
INT CAtr::ATR_GetHistoricalBytes(BYTE hist[], unsigned int *length)
{
  if (m_ATR.hbn == 0)
    return (ATR_NOT_FOUND);

  (*length) = m_ATR.hbn;
  RtlCopyMemory (hist, m_ATR.hb, m_ATR.hbn);
  return (ATR_OK);
}

//
INT CAtr::ATR_GetRaw(BYTE buffer[], unsigned int *length)
{
  unsigned i, j;

  buffer[0] = m_ATR.TS;
  buffer[1] = m_ATR.T0;

  j = 2;

  for (i = 0; i < m_ATR.pn; i++)
    {
      if (m_ATR.ib[i][ATR_INTERFACE_BYTE_TA].present)
	buffer[j++] = m_ATR.ib[i][ATR_INTERFACE_BYTE_TA].value;

      if (m_ATR.ib[i][ATR_INTERFACE_BYTE_TB].present)
	buffer[j++] = m_ATR.ib[i][ATR_INTERFACE_BYTE_TB].value;

      if (m_ATR.ib[i][ATR_INTERFACE_BYTE_TC].present)
	buffer[j++] = m_ATR.ib[i][ATR_INTERFACE_BYTE_TC].value;

      if (m_ATR.ib[i][ATR_INTERFACE_BYTE_TD].present)
	buffer[j++] = m_ATR.ib[i][ATR_INTERFACE_BYTE_TD].value;
    }

  if (m_ATR.hbn > 0)
    {
      RtlCopyMemory (&(buffer[j]), m_ATR.hb, m_ATR.hbn);
      j += m_ATR.hbn;
    }

  if ((m_ATR.TCK).present)
    buffer[j++] = (m_ATR.TCK).value;

  (*length) = j;

  return ATR_OK;
}


INT CAtr::ATR_GetCheckByte(BYTE *check_byte)
{
  if (!((m_ATR.TCK).present))
    return (ATR_NOT_FOUND);

  (*check_byte) = (m_ATR.TCK).value;
  return (ATR_OK);
}

//
INT CAtr::ATR_GetFsMax(unsigned long *fsmax)
{
 BYTE FI;

  if (ATR_GetIntegerValue (ATR_INTEGER_VALUE_FI, &FI) == ATR_OK)
    (*fsmax) = atr_fs_table[FI];
  else
    (*fsmax) = atr_fs_table[1];

  return (ATR_OK);
}


// end-of-file
//////////////////////////////////////////////////////////////////////////