/* RXCRCDLL.H */

#define  RETSTR_INVALID  40
#define  RETSTR_OK        0

RexxFunctionHandler RxGetCrc;
unsigned long CrcGetCrc( FILE *fp, unsigned long* crcTable );
void CrcGenTable( unsigned long* crcTable );
