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
#include "qspi_ctrl.h"

/************************** Constant Definitions *****************************/



#define QSPI_TEST_NUM		1000000

/*
 * The following constants define the commands which may be sent to the FLASH
 * device.
 */
#define SINGLE_READ_CMD		0x03
#define FAST_READ_CMD		0x0B
#define DUAL_READ_CMD		0x3B
#define QUAD_READ_CMD		0x6B
#define READ_ID_CMD			0x9F

#define WRITE_ENABLE_CMD	0x06
#define WRITE_DISABLE_CMD	0x04
#define BANK_REG_RD			0x16
#define BANK_REG_WR			0x17
/* Bank register is called Extended Address Reg in Micron */
#define EXTADD_REG_RD		0xC8
#define EXTADD_REG_WR		0xC5

#define CONFIG_REG_RD			0x35
#define STATUS_REG_RD			0x05
#define STATUS_REG2_RD			0x07
#define CONFG_REG_WR			0x01
#define AUTO_BOOT_REG_RD		0x14
#define ASP_REG_RD				0x2b

#define QSPI_CONFIG_REG_TEST_NUM	10000
#define QSPI_CONFIG_REG_WRT_WAIT_MAX_NUM	1000000
#define QSPI_WAIT_MAX_NUM					1000000
#define QSPI_WAIT_MAX_NUM2					100


#define COMMAND_OFFSET		0 /* FLASH instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define DATA_OFFSET			4 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		4 /* Dummy byte offset for fast, dual and quad
				     reads */
#define DUMMY_SIZE			1 /* Number of dummy bytes for fast, dual and
						 quad reads */
#define DUMMY_MAX_SIZE			8 /* max Number of dummy bytes for fast, dual and
						 quad reads */
#define RD_ID_SIZE			4 /* Read ID command + 3 bytes ID response */
#define BANK_SEL_SIZE		2 /* BRWR or EARWR command + 1 byte bank value */
#define WRITE_ENABLE_CMD_SIZE	1 /* WE command */
/*
 * The following constants specify the extra bytes which are sent to the
 * FLASH on the QSPI interface, that are not data, but control information
 * which includes the command and address
 */
#define OVERHEAD_SIZE		4

/*
 * The following constants specify the max amount of data and the size of the
 * the buffer required to hold the data and overhead to transfer the data to
 * and from the FLASH.
 */
#define DATA_SIZE		4096



/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/
XQspiPs *QspiInstancePtr=NULL;
extern u32 QspiFlashSize;
extern u32 QspiFlashMake;
extern u32 FlashReadBaseAddress;
extern u8 LinearBootDeviceFlag;


/*
 * The following variables are used to read and write to the eeprom and they
 * are global to avoid having large buffers on the stack
 */
u8 FlashReadBuffer[DATA_SIZE + DATA_OFFSET + DUMMY_SIZE];
u8 FlashWriteBuffer[DATA_OFFSET + DUMMY_SIZE];


u32 QspiFlashReset(void)
{
	u8 u8ResetCmd = { 0xf0 };

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	//if (QspiFlashMake == SPANSION_ID)
	{

		xil_printf( "Reset Spansion flash.\n\r");
		/*
		 * Send the write enable command to the FLASH so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the erase
		 */
		XQspiPs_PolledTransfer(QspiInstancePtr, &u8ResetCmd, NULL,
				  sizeof(u8ResetCmd));

	}
	
	return XST_SUCCESS;
}


u32 QspiFlashStatusShow(void)
{
	u32 Status;
	u8 u8_config=0;

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	//if (QspiFlashMake == SPANSION_ID)
	{

		/*
		 * For testing - Read config register
		 */
		FlashWriteBuffer[COMMAND_OFFSET]   = STATUS_REG_RD;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		u8_config = FlashReadBuffer[1];
		xil_printf( "Spansion QSPI Flash status register: 0x%02x\n\r", u8_config);
		if( 0!= u8_config )
		{
			xil_printf( "\n\r\n\rCaution:Spansion status register is error!!!\n\r\n\r");
		}

	}

	return XST_SUCCESS;
}


u32 QspiFlashStatus2Show(void)
{
	u32 Status;
	u8 u8_config=0;

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	//if (QspiFlashMake == SPANSION_ID)
	{

		/*
		 * For testing - Read config register
		 */
		FlashWriteBuffer[COMMAND_OFFSET]   = STATUS_REG_RD;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		u8_config = FlashReadBuffer[1];
		xil_printf( "Spansion QSPI Flash status2 register: 0x%02x\n\r", u8_config);

	}

	return XST_SUCCESS;
}



u32 QspiFlashBankShow(void)
{
	u32 Status;
	u8 u8_config=0;

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	//if (QspiFlashMake == SPANSION_ID)
	{

		FlashWriteBuffer[COMMAND_OFFSET]   = BANK_REG_RD;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer,
				BANK_SEL_SIZE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		u8_config = FlashReadBuffer[1];
		xil_printf( "Spansion QSPI Flash Bank register: 0x%02x\n\r", u8_config);

	}

	return XST_SUCCESS;
}




u32 QspiFlashAutoBootShow(void)
{
	u32 Status;
	//u8 u8_config=0;

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	//if (QspiFlashMake == SPANSION_ID)
	{

		FlashWriteBuffer[COMMAND_OFFSET]   = AUTO_BOOT_REG_RD;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;
		FlashWriteBuffer[ADDRESS_2_OFFSET] = 0x00;
		FlashWriteBuffer[ADDRESS_3_OFFSET] = 0x00;
		FlashWriteBuffer[4] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer,
				5);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		
		//u8_config = FlashReadBuffer[1];
		xil_printf( "Spansion QSPI Flash auto-boot register: 0x%02x-%02x-%02x-%02x\n\r", 
			FlashReadBuffer[1], FlashReadBuffer[2], 
			FlashReadBuffer[3], FlashReadBuffer[4] );

	}

	return XST_SUCCESS;
}




u32 QspiFlashASPShow(void)
{
	u32 Status;
	//u8 u8_config=0;

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	//if (QspiFlashMake == SPANSION_ID)
	{

		FlashWriteBuffer[COMMAND_OFFSET]   = ASP_REG_RD;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;
		FlashWriteBuffer[ADDRESS_2_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer,
				3);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		
		//u8_config = FlashReadBuffer[1];
		xil_printf( "Spansion QSPI Flash ASP register: 0x%02x-%02x\n\r", 
			FlashReadBuffer[1], FlashReadBuffer[2] );

	}

	return XST_SUCCESS;
}


u8 QspiFlashConfigRead( u32 u32_print_flag )
{
	u32 Status;
	u8 u8_config=0;

	xil_printf("File:%s, function:%s Line:%d. \n\r",
			__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	//if (QspiFlashMake == SPANSION_ID)
	{

		/*
		 * For testing - Read config register
		 */
		FlashWriteBuffer[COMMAND_OFFSET]   = CONFIG_REG_RD;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		u8_config = FlashReadBuffer[1];
		if( 0 != u32_print_flag )
		{
			xil_printf( "Spansion QSPI Flash config register: 0x%02x\n\r", u8_config);
		}

	}

	return u8_config;
}


u32 QspiFlashConfigShow(void)
{

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	QspiFlashConfigRead( 1 );

	return XST_SUCCESS;
}




u32 QspiFlashConfigWrite( u8 u8_set, u8 u8_clear )
{
	u32 Status;
	u8 u8_config=0;
	u32 u32_loop=0;

	XQspiPs *QspiPtr;

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}
	QspiPtr = QspiInstancePtr;

	//if (QspiFlashMake == SPANSION_ID)
	{

		u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
		u8 WriteDisableCmd = { WRITE_DISABLE_CMD };

		/*
		 * For testing - Read config register
		 */
		FlashWriteBuffer[COMMAND_OFFSET]   = CONFIG_REG_RD;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer, 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		u8_config = FlashReadBuffer[1];
		xil_printf( "Spansion QSPI Flash config register old value: 0x%02x\n\r", u8_config);

		/*
		 * Send the write enable command to the FLASH so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the erase
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL,
				  sizeof(WriteEnableCmd));

		FlashWriteBuffer[COMMAND_OFFSET]   = CONFG_REG_WR;
		FlashWriteBuffer[ADDRESS_1_OFFSET] = 0;
		u8_config = (u8_config|u8_set);
		u8_config = (u8_config&(~u8_clear) );
		FlashWriteBuffer[ADDRESS_2_OFFSET] = u8_config;
		xil_printf( "Spansion Flash config register to be written: 0x%02x\n\r", FlashWriteBuffer[ADDRESS_2_OFFSET]);

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		FlashReadBuffer[1] = 0;
		Status = XQspiPs_PolledTransfer(QspiPtr, FlashWriteBuffer, NULL, 3);
		if (Status != XST_SUCCESS) {
			xil_printf( "Failed to write Spansion QSPI Flash config register\n\r");
			return XST_FAILURE;
		}

		for( u32_loop=0; u32_loop<QSPI_CONFIG_REG_WRT_WAIT_MAX_NUM; u32_loop++ )
		{

			/*
			 * For testing - Read config register
			 */
			FlashWriteBuffer[COMMAND_OFFSET]   = STATUS_REG_RD;
			FlashWriteBuffer[ADDRESS_1_OFFSET] = 0x00;

			/*
			 * Send the Extended address register write command
			 * written, no receive buffer required
			 */
			Status = XQspiPs_PolledTransfer(QspiInstancePtr, FlashWriteBuffer, FlashReadBuffer, 2);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			u8_config = FlashReadBuffer[1];
			//xil_printf( "Spansion QSPI Flash status register: 0x%02x\n\r", u8_config);
			if( 0== (u8_config&1) )
			{
				xil_printf( "Spansion QSPI Flash configuration register write operation completed.\n\r");
				break;
			}

		}
		if( u32_loop>=QSPI_CONFIG_REG_WRT_WAIT_MAX_NUM )
		{
			xil_printf( "Spansion QSPI Flash configuration register write operation time-out.\n\r");
		}

		/*
		 * Send the write enable command to the FLASH so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the erase
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteDisableCmd, NULL,
				  sizeof(WriteDisableCmd));


	}

	return XST_SUCCESS;
}

u32 QspiFlashConfigQuadClear( void )
{

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	QspiFlashConfigWrite( 0, 2 );

	return XST_SUCCESS;
}

u32 QspiFlashConfigQuadSet( void )
{

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	QspiFlashConfigWrite( 2, 0 );

	return XST_SUCCESS;
}



u32 QspiFlashConfigQuadCheckSet( void )
{
	u8 u8_config=0;
	//u32 u32_loop=0;

	//for( u32_loop=0; u32_loop<10; u32_loop++ )
	{
		//if (QspiFlashMake == SPANSION_ID)
		{
			xil_printf( "Spansion Flash\n\r");

			u8_config = QspiFlashConfigRead( 0 );
			xil_printf( "u8_config old: 0x%02x\n\r", u8_config);
			
			if ( 0 == (u8_config&2) )
			{
				xil_printf( "Spansion Flash Quad bit is lost. Set it again.\n\r");
				QspiFlashConfigQuadSet( );
			}

			u8_config = QspiFlashConfigRead( 0 );
			xil_printf( "u8_config new: 0x%02x\n\r", u8_config);

		}

	}

	return XST_SUCCESS;
}


u32 QspiFlashConfigLcSet( void )
{

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	QspiFlashConfigWrite( 0xc0, 0 );

	return XST_SUCCESS;
}

u32 QspiFlashConfigLcClear( void )
{

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	QspiFlashConfigWrite( 0, 0xc0 );

	return XST_SUCCESS;
}




u32 QspiFlashConfigCheck( u8 u8_correct_value )
{
	u8 u8_config=0;

	//if (QspiFlashMake == SPANSION_ID)
	{
		//xil_printf( "Spansion Flash\n\r");

		u8_config = QspiFlashConfigRead( 0 );
		//xil_printf( "u8_config old: 0x%02x\n\r", u8_config);
		
		if ( u8_correct_value != u8_config )
		{
			xil_printf( "\n\r\n\rCaution:Spansion Flash configuration is error!!!\n\r\n\r");
		}
	}

	return XST_SUCCESS;
}


u32 QspiFlashAllStatusShow( void )
{

	xil_printf("File:%s, function:%s Line:%d. \n\r",
			__FILE__, __func__, __LINE__ );

	FlashReadID( );
	QspiFlashStatusShow( );
	QspiFlashStatus2Show( );
	QspiFlashBankShow( );
	QspiFlashAutoBootShow( );
	QspiFlashASPShow( );
	QspiFlashConfigShow( );
	QspiFlashStatusShow( );
	QspiFlashConfigShow( );
	usleep(100000);  // 100ms


	return XST_SUCCESS;
}


u32 QspiControllerSet( XQspiPs *QspiPtr )
{

	QspiInstancePtr = QspiPtr;
	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}
	
	return XST_SUCCESS;
}





u32 QspiFlashSpansionInit0619( XQspiPs *QspiPtr )
{

	xil_printf("File:%s, function:%s Line:%d. \n\r",
			__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}
	QspiInstancePtr = QspiPtr;

	//QspiFlashReset( );
	QspiFlashStatusShow( );
	QspiFlashConfigShow( );
	QspiFlashConfigQuadClear( );
	QspiFlashConfigQuadSet( );
	QspiFlashConfigLcSet( );
	QspiFlashConfigLcClear( );
	QspiFlashConfigQuadClear( );
	QspiFlashConfigQuadSet( );
	QspiFlashConfigLcSet( );
	QspiFlashConfigLcClear( );
	QspiFlashConfigCheck( 0x02 );	
	QspiFlashStatusShow( );
	QspiFlashConfigShow( );

	//QspiFlashConfigQuadCheckSet( );

	return XST_SUCCESS;
}



u32 QspiFlashSpansionInit( XQspiPs *QspiPtr )
{

	//xil_printf("File:%s, function:%s Line:%d. \n\r",
	//		__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}
	QspiInstancePtr = QspiPtr;

	//QspiFlashReset( );
	QspiFlashStatusShow( );
	QspiFlashConfigShow( );
	QspiFlashConfigWrite( 0x02, 0xc0 );
	QspiFlashConfigCheck( 0x02 );	
	QspiFlashStatusShow( );
	QspiFlashConfigShow( );

	//QspiFlashConfigQuadCheckSet( );

	return XST_SUCCESS;
}


u32 QspiFlashInitTest( void )
{

	xil_printf("File:%s, function:%s Line:%d. \n\r",
			__FILE__, __func__, __LINE__ );

	if( NULL == QspiInstancePtr )
	{
		xil_printf("Invalid QSPI controller in function:%s Line:%d. \n\r",
				__func__, __LINE__ );
		return XST_FAILURE;
	}

	
	QspiFifoStatusCheck( QspiInstancePtr );

	//QspiFlashReset( );
	QspiFlashStatusShow( );
	QspiFlashConfigShow( );
	//QspiFlashConfigQuadSet( );
	//QspiFlashConfigLcClear( );
	QspiFlashConfigLcSet( );
	QspiFlashStatusShow( );
	QspiFlashConfigShow( );

#if 0
	for( u32_loop=QSPI_TEST_NUM; u32_loop<(QSPI_TEST_NUM+2); u32_loop++ )
	{

		xil_printf( "No.%d loop.\n\r", u32_loop);
		QspiFlashReset( );
		QspiFlashConfigShow( );
		//QspiFlashConfigQuadSet( );
	}
#endif

	//QspiFlashConfigQuadCheckSet( );

	return XST_SUCCESS;
}



#endif

