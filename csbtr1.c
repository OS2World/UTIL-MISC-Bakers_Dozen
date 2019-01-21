/* FROM BTRIEVE DEVELOPERS' JOURNAL, Vol IV No. 4 */

/* Listing 1   Structure definitions to support the Btrieve client/server communication. */


#ifndef CHAT_H
#define CHAT_H

#include "btrapi.h"
#define CS_REQ_CHAT 1000
 
typedef struct 
    {
    BTI_WORD opCode;
    BTI_BYTE posBlk[128];
    BTI_WORD dataLen;
    BTI_BYTE keyBuf[255];
    BTI_SINT keyNum;
    BTI_BYTE clientID[16];
    BTI_CHAR dataBuf[32000];
    } BTRIEVE_PARM_INFO;

struct BTI_POS_BLK
    {
    BTI_BYTE *pb;
    };

#endif
