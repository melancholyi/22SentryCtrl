#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "sys.h"


void SPI1_Init(void);
u8 SPI_ReadWriteByte(u8 TxData);
#ifdef __cplusplus
}
#endif
#endif

