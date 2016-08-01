/******************************************************************************
*
* Copyright (C) 2012 - 2014 Xilinx, Inc.  All rights reserved.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal 
* in the Software without restriction, including without limitation the rights 
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in 
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications: 
* (a) running on a Xilinx device, or 
* (b) that interact with a Xilinx device through a bus or interconnect.  
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
* XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in 
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file qspi.c
*
* Contains code for the QSPI FLASH functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	01/10/10 Initial release
* 3.00a mb  25/06/12 InitQspi, data is read first and required config bits
*                    are set
* 4.00a sg	02/28/13 Cleanup
* 					 Removed LPBK_DLY_ADJ register setting code as we use
* 					 divisor 8
* 5.00a sgd	05/17/13 Added Flash Size > 128Mbit support
* 					 Dual Stack support
*					 Fix for CR:721674 - FSBL- Failed to boot from Dual
*					                     stacked QSPI
* 6.00a kc  08/30/13 Fix for CR#722979 - Provide customer-friendly
*                                        changelogs in FSBL
*                    Fix for CR#739711 - FSBL not able to read Large QSPI
*                    					 (512M) in IO Mode
* 7.00a kc  10/25/13 Fix for CR#739968 - FSBL should do the QSPI config
*                    					 settings for Dual parallel
*                    					 configuration in IO mode
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef QSPI_CTRL_H		/* prevent circular inclusions */
#define QSPI_CTRL_H		/* by using protection macros */
	
#ifdef __cplusplus
	extern "C" {
#endif

/***************************** Include Files *********************************/

#include "qspi.h"
#include "xqspips.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/




void QspiRegContentDump( void );
u32 QspiFifoStatusCheck( XQspiPs *QspiPtr );
u32 QspiDisableSlaveSelect( void );


#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */


