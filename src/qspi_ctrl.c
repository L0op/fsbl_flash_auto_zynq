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

/***************************** Include Files *********************************/

#include "xparameters.h"	/* SDK generated parameters */

#ifdef XPAR_PS7_QSPI_LINEAR_0_S_AXI_BASEADDR

#include "xqspips.h"

#include "sleep.h"
#include "dbg_print.h"

/************************** Constant Definitions *****************************/



/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/
#define QSPI_WAIT_MAX_NUM					1000000
#define QSPI_WAIT_MAX_NUM2					100


/************************** Function Prototypes ******************************/
int XQspiPs_DisableSlaveSelect(XQspiPs *InstancePtr);


/************************** Variable Definitions *****************************/
extern XQspiPs *QspiInstancePtr;



void QspiRegContentDump( void )
{

	xil_printf("\n\r\n\rDump QSPI controller registers \n\r");
	dbg_mem_word_dump( (u32 *)0xe000d000, 8*4 );
	dbg_mem_word_dump( (u32 *)0xe000d024, 8*4 );
	dbg_mem_word_dump( (u32 *)0xe000d080, 4*4 );
	dbg_mem_word_dump( (u32 *)0xe000d0a0, 4*4 );
	xil_printf("\n\rDump QSPI Flash contents \n\r");
	dbg_mem_word_dump( (u32 *)0xfc000000, 0x100 );

}


/******************************************************************************
*
* This functions selects the current bank
*
* @param	BankSel is the bank to be selected in the flash device(s).
*
* @return	XST_SUCCESS if bank selected
*			XST_FAILURE if selection failed
* @note		None.
*

*/

u32 QspiFifoStatusCheck( XQspiPs *QspiPtr )
{
	XQspiPs *InstancePtr;
	
	u32 StatusReg;
	u32 Data;
	u32 u32_loop;

	if( NULL == QspiPtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}
	InstancePtr = QspiPtr;
	if( NULL == QspiInstancePtr )
	{
		QspiInstancePtr = QspiPtr;
	}
	
	Data = XQspiPs_ReadReg(
			InstancePtr->Config.BaseAddress,
			XQSPIPS_RXWR_OFFSET);
	xil_printf( "QSPI controller rx threshhold:%d.\n\r", Data);
	
	Data = XQspiPs_ReadReg(
			InstancePtr->Config.BaseAddress,
			XQSPIPS_TXWR_OFFSET);
	xil_printf( "QSPI controller tx threshhold:%d.\n\r", Data);

	/*
	 * Check RX FIFO empty.
	 */
	for( u32_loop=0; u32_loop<QSPI_WAIT_MAX_NUM; u32_loop++)
	{
		StatusReg = XQspiPs_ReadReg(
				InstancePtr->Config.BaseAddress,
				XQSPIPS_SR_OFFSET);

		if(1 == (StatusReg & XQSPIPS_IXR_RXNEMPTY_MASK) )
		{
			xil_printf( "QSPI controller has rx data.\n\r");
		}
		else
		{
			// No data.
			break;
		}
	} 

#if 0
	/*
	 * Check TX FIFO empty.
	 */
	for( u32_loop=0; u32_loop<QSPI_WAIT_MAX_NUM2; u32_loop++)
	{
		StatusReg = XQspiPs_ReadReg(
				InstancePtr->Config.BaseAddress,
				XQSPIPS_SR_OFFSET);

		if(1 != (StatusReg & XQSPIPS_IXR_TXOW_MASK) )
		{
			usleep(1000);
			xil_printf( "QSPI controller has tx data, status register: 0x%08x.\n\r", StatusReg);
		}
		else
		{
			// No data.
			break;
		}
	} 
#endif

	return XST_SUCCESS;
}


/******************************************************************************
*
* This functions selects the current bank
*
* @param	BankSel is the bank to be selected in the flash device(s).
*
* @return	XST_SUCCESS if bank selected
*			XST_FAILURE if selection failed
* @note		None.
*
******************************************************************************/
u32 QspiDisableSlaveSelect( void )
{
	xil_printf("File:%s, function:%s Line:%d. \n\r",
			__FILE__, __func__, __LINE__ );
	
	if( NULL != QspiInstancePtr )
	{
		xil_printf("File:%s, function:%s Line:%d. \n\r",
				__FILE__, __func__, __LINE__ );
		XQspiPs_DisableSlaveSelect( QspiInstancePtr );
	}

	return XST_SUCCESS;
}


#endif

