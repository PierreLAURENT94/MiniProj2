// Atr.h: interface for the CAtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATR_H__D6E314F1_1248_4738_BDB5_F38DCB059D31__INCLUDED_)
#define AFX_ATR_H__D6E314F1_1248_4738_BDB5_F38DCB059D31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



/* Return values */
#define ATR_OK			0	/* ATR could be parsed and data returned */
#define ATR_NOT_FOUND	1	/* Data not present in ATR */
#define ATR_MALFORMED	2	/* ATR could not be parsed */
#define ATR_IO_ERROR	3	/* I/O stream error */

/* Paramenters */
#define ATR_MAX_SIZE 			33	/* Maximum size of ATR byte array */
#define ATR_MAX_HISTORICAL		15	/* Maximum number of historical bytes */
#define ATR_MAX_PROTOCOLS		7	/* Maximun number of protocols */
#define ATR_MAX_IB				4	/* Maximum number of interface bytes per protocol */
#define ATR_CONVENTION_DIRECT	0	/* Direct convention */
#define ATR_CONVENTION_INVERSE	1	/* Inverse convention */
#define ATR_PROTOCOL_TYPE_T0	0	/* Protocol type T=0 */
#define ATR_PROTOCOL_TYPE_T1	1	/* Protocol type T=1 */
#define ATR_PROTOCOL_TYPE_T2	2	/* Protocol type T=2 */
#define ATR_PROTOCOL_TYPE_T3	3	/* Protocol type T=3 */
#define ATR_PROTOCOL_TYPE_T14	14	/* Protocol type T=14 */
#define ATR_INTERFACE_BYTE_TA	0	/* Interface byte TAi */
#define ATR_INTERFACE_BYTE_TB	1	/* Interface byte TBi */
#define ATR_INTERFACE_BYTE_TC	2	/* Interface byte TCi */
#define ATR_INTERFACE_BYTE_TD	3	/* Interface byte TDi */
#define ATR_PARAMETER_F			0	/* Parameter F */
#define ATR_PARAMETER_D			1	/* Parameter D */
#define ATR_PARAMETER_I			2	/* Parameter I */
#define ATR_PARAMETER_P			3	/* Parameter P */
#define ATR_PARAMETER_N			4	/* Parameter N */
#define ATR_INTEGER_VALUE_FI	0	/* Integer value FI */
#define ATR_INTEGER_VALUE_DI	1	/* Integer value DI */
#define ATR_INTEGER_VALUE_II	2	/* Integer value II */
#define ATR_INTEGER_VALUE_PI1	3	/* Integer value PI1 */
#define ATR_INTEGER_VALUE_N		4	/* Integer value N */
#define ATR_INTEGER_VALUE_PI2	5	/* Integer value PI2 */

/* Default values for paramenters */
#define ATR_DEFAULT_F	372
#define ATR_DEFAULT_D	1
#define ATR_DEFAULT_I 	50
#define ATR_DEFAULT_N	0
#define ATR_DEFAULT_P	5


typedef struct _T_ATR
{
	unsigned length;
	BYTE TS;
	BYTE T0;
	struct
	{
		BYTE value;
		bool present;
	}
	ib[ATR_MAX_PROTOCOLS][ATR_MAX_IB], TCK;
	unsigned pn;
	BYTE hb[ATR_MAX_HISTORICAL];
	unsigned hbn;
} ATR, *PATR;

//////////////////////////////////////////////////////////////////////////

/* Invert order of bits in a byte: b7->b0, b0->b7 */
#ifndef INVERT_BYTE
#define INVERT_BYTE(a)		((((a) << 7) & 0x80) | \
				(((a) << 5) & 0x40) | \
				(((a) << 3) & 0x20) | \
				(((a) << 1) & 0x10) | \
				(((a) >> 1) & 0x08) | \
				(((a) >> 3) & 0x04) | \
				(((a) >> 5) & 0x02) | \
(((a) >> 7) & 0x01))
#endif

typedef int ifd_t;

//////////////////////////////////////////////////////////////////////////
class CAtr  
{
public:
	CAtr(unsigned char *bcdATR, unsigned int length);
	virtual ~CAtr();
		
	INT ATR_GetFsMax (unsigned long *fsmax);
	INT ATR_GetCheckByte (BYTE * check_byte);
	INT ATR_GetRaw (BYTE buffer[ATR_MAX_SIZE], unsigned *length);
	INT ATR_GetHistoricalBytes (BYTE hist[ATR_MAX_HISTORICAL], unsigned *length);
	INT ATR_GetParameter (int name, double *parameter);
	INT ATR_GetIntegerValue (int name, BYTE * value);
	INT ATR_GetInterfaceByte (unsigned number, int character, BYTE * value);
	INT ATR_GetProtocolType (unsigned number_protocol, BYTE *protocol_type);
	INT ATR_GetNumberOfProtocols (unsigned *number_protocols);
	INT ATR_GetSize (unsigned *size);
	INT ATR_GetConvention (int *convention);
	INT ATR_InitFromArray (BYTE atr_buffer[ATR_MAX_SIZE], unsigned length);
	
public:
	ATR	m_ATR;
};

#endif // !defined(AFX_ATR_H__D6E314F1_1248_4738_BDB5_F38DCB059D31__INCLUDED_)
