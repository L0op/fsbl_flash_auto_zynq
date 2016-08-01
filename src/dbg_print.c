/******************************************************************************
*
* (c) Copyright 2013 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file dbg_print.c
*
* This file contains a design example using the MicroBlaze exception features.
*
* @note		None.
*
* <pre>
*
* MODIFICATION HISTORY:
*
* Ver       Who       Date               Changes
* -----   ----     --------    ---------------------------------------------
* 1.00a    Hank     03/10/14       First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/




#include "dbg_print.h"






/************************** Constant Definitions *****************************/



/**************************** Type Definitions *******************************/



/***************** Macros (Inline Functions) Definitions *********************/




/************************** Variable Definitions *****************************/


/*   */
#ifdef DBG_RUNTIME_PRINT_ENABLE
u8        gu8_dbg_flag=0;
#endif		/* DBG_RUNTIME_PRINT_ENABLE  */

#ifdef DBG_RUNTIME_MOD_PRINT_ENABLE
u8        gu8_mod_dbg_flag[DBG_MOD_NUM]={};
#endif		/* DBG_RUNTIME_MOD_PRINT_ENABLE  */

#ifdef FAST_PRINT_ENABLE

u32     gu32_fast_print_serial_num=0;
fast_print_info_st  g_fast_print_info[FAST_PRINT_INFO_NUM];

/* for debug */
DECLARE_FILE_NAME;
unsigned int g_ui_line_begin_fast_print_test_dbg = 0;
unsigned int g_ui_line_end_fast_print_test_dbg = 0;

#endif		/* FAST_PRINT_ENABLE  */






/************************** Function Prototypes ******************************/




/************************** Function Implementation ******************************/

#ifdef DBG_RUNTIME_PRINT_ENABLE

void dbg_flag_set( u8 u8_dbg_flag )
{

    gu8_dbg_flag = u8_dbg_flag;

}

u8 dbg_flag_get( void )
{

    return gu8_dbg_flag;

}

#endif		/* DBG_RUNTIME_PRINT_ENABLE  */



#ifdef DBG_RUNTIME_MOD_PRINT_ENABLE

void dbg_mod_flag_set( u8 u8_dbg_mod, u8 u8_mod_dbg_flag )
{
    if( u8_dbg_mod >= DBG_MOD_NUM )
    {
        return;
    }

    gu8_mod_dbg_flag[u8_dbg_mod] = u8_mod_dbg_flag;

}

u8 dbg_mod_flag_get( u8 u8_dbg_mod )
{
    if( u8_dbg_mod >= DBG_MOD_NUM )
    {
        return 0;
    }

    return gu8_mod_dbg_flag[u8_dbg_mod];

}
#endif		/* DBG_RUNTIME_MOD_PRINT_ENABLE  */


#ifdef FAST_PRINT_ENABLE



/*

 const char * file_name:: 
 The pointer 'file_name' can be modified. 
 But the buffer pointed to by the pointer 'file_name' could not be modified.

 char * const file_name:: 
 The pointer 'file_name' can not be modified. 
 But the buffer pointed to by the pointer 'file_name' could be modified.

 */

void fast_print_buf
(
    const char * file_name,
    const void * p_func,
    const unsigned int ui_line_num, 
    const unsigned int ui_mod_num, 
    const unsigned int ui_level, 
    const char *fmt, ...
)
{
    va_list args;
    unsigned int ui_arg=0; 
    unsigned int ui_info_num=0; 
    fast_print_info_st  *p_fast_print_info=NULL;

    va_start(args, fmt);

    FAST_PRINT_LOCK();
    ui_info_num = gu32_fast_print_serial_num%FAST_PRINT_INFO_NUM;
    gu32_fast_print_serial_num++;
    FAST_PRINT_UNLOCK();
    
    p_fast_print_info = &g_fast_print_info[ui_info_num];

    /* Location information about message */
    p_fast_print_info->ui_mod_num = ui_mod_num;
    p_fast_print_info->p_file_name = file_name;
    p_fast_print_info->p_func = p_func;
    p_fast_print_info->ui_line_num = ui_line_num;
    p_fast_print_info->ui_level = ui_level;
    
    /* Time information about message */
    p_fast_print_info->ui_serial_num = gu32_fast_print_serial_num; /* used begin with 1 */
    XTime_GetTime( &p_fast_print_info->ull_time );

    /* Information format */
    p_fast_print_info->p_format = fmt;
    
    ui_arg=va_arg(args, int); 
    p_fast_print_info->ui_arg0= ui_arg;
    
    ui_arg=va_arg(args, int); 
    p_fast_print_info->ui_arg1= ui_arg;
    
    ui_arg=va_arg(args, int); 
    p_fast_print_info->ui_arg2= ui_arg;
    
    ui_arg=va_arg(args, int); 
    p_fast_print_info->ui_arg3= ui_arg;
    
    ui_arg=va_arg(args, int); 
    p_fast_print_info->ui_arg4= ui_arg;
    
    ui_arg=va_arg(args, int); 
    p_fast_print_info->ui_arg5= ui_arg;
    
    ui_arg=va_arg(args, int); 
    p_fast_print_info->ui_arg6= ui_arg;

    va_end(args);

}
 
 
 
/*
  Not tested yet.
  Save arguments of printing only.
  Generate string later.
  Purporse: reduce time.
*/
 int fast_print_out
(
    const char *file_name,                   // not used, set as 0 (NULL)   
    const void *p_func,                      // not used, set as 0 (NULL)   
    const unsigned int ui_line_num_min,      // not used, set as 0
    const unsigned int ui_line_num_max,      // not used, set as 0xffffffff
    const unsigned int ui_mod_num,           // not used, set as 0
    const unsigned int ui_level_min          // not used, set as 0
)
{
    unsigned int ui_loop=0; 
    unsigned int ui_print_flag=1; 
    unsigned int ui_info_num_begin=0; 
    unsigned int ui_info_num=0; 
    unsigned int ui_printed_num=0; 
    fast_print_info_st  *p_fast_print_info=NULL;

    FAST_PRINT_LOCK();
    ui_info_num_begin = (gu32_fast_print_serial_num+1)%FAST_PRINT_INFO_NUM;
    FAST_PRINT_UNLOCK();

    ui_printed_num = 0;
    for( ui_loop =0; ui_loop<FAST_PRINT_INFO_NUM; ui_loop++)
    {
        ui_print_flag=1; 
        
        ui_info_num = (ui_info_num_begin+ui_loop)%FAST_PRINT_INFO_NUM;        
        p_fast_print_info = &g_fast_print_info[ui_info_num];

        if( NULL != file_name )
        {
            if( p_fast_print_info->p_file_name != file_name )
            {
                ui_print_flag=0; 
            }
        }
        
        if( NULL != p_func )
        {
            if( p_fast_print_info->p_func != p_func )
            {
                ui_print_flag=0; 
            }
        }

        if(  ( p_fast_print_info->ui_line_num < ui_line_num_min )
            || ( p_fast_print_info->ui_line_num > ui_line_num_max ) )
        {
            ui_print_flag=0; 
        }

        if( 0 != ui_mod_num )
        {
            if( p_fast_print_info->ui_mod_num != ui_mod_num )
            {
                ui_print_flag=0; 
            }
        }

        if( 0 != ui_level_min )
        {
            if( p_fast_print_info->ui_level < ui_level_min )
            {
                ui_print_flag=0; 
            }
        }

        if( 0 == p_fast_print_info->ui_serial_num )
        {
            ui_print_flag=0; 
        }
        

        if( 0 != ui_print_flag )
        {
            printf( "Message from module:%d, file: %s function: 0x%08x at line:%d, level:%d, serial number:%d, time:%llu-0x%llx: ",
                p_fast_print_info->ui_mod_num,
                p_fast_print_info->p_file_name, 
                (unsigned int)p_fast_print_info->p_func,
                p_fast_print_info->ui_line_num, 
                p_fast_print_info->ui_level, 
                p_fast_print_info->ui_serial_num,
                p_fast_print_info->ull_time,
                p_fast_print_info->ull_time );
        
            printf( p_fast_print_info->p_format, p_fast_print_info->ui_arg0,
                p_fast_print_info->ui_arg1, p_fast_print_info->ui_arg2,
                p_fast_print_info->ui_arg3, p_fast_print_info->ui_arg4,
                p_fast_print_info->ui_arg5, p_fast_print_info->ui_arg6 );
            
            ui_printed_num++;
        }
    }

    printf( "\n\rPrinted %d message\n\rDone.\n\r", ui_printed_num);
    
    return   ui_printed_num++;
    
}



#ifdef DBG_PRINT_TEST

void fast_print_test_fatal( void )
{
    unsigned int ui_arg0=0x0000; 
    unsigned int ui_arg1=0x1000; 
    unsigned int ui_arg2=0x2000; 
    unsigned int ui_arg3=0x3000; 
    unsigned int ui_arg4=0x4000; 
    unsigned long long ull_arg5=0x6000; 

    ui_arg0=0x0000; 
    ui_arg1=0x1001; 
    ui_arg2=0x2002; 
    ui_arg3=0x3003; 
    ui_arg4=0x4004; 
    ull_arg5=0x112345785005; 
    fast_print(10, FAST_PRINT_FATAL, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx fatal message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0101; 
    ui_arg1=0x1101; 
    ui_arg2=0x2102; 
    ui_arg3=0x3103; 
    ui_arg4=0x4104; 
    ull_arg5=0x112345785105; 
    fast_print(11, FAST_PRINT_FATAL, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx fatal message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_FATAL_TEMP, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx fatal message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

}



void fast_print_test_critical( void )
{
    unsigned int ui_arg0=0x0000; 
    unsigned int ui_arg1=0x1000; 
    unsigned int ui_arg2=0x2000; 
    unsigned int ui_arg3=0x3000; 
    unsigned int ui_arg4=0x4000; 
    unsigned long long ull_arg5=0x6000; 

    ui_arg0=0x0000; 
    ui_arg1=0x1001; 
    ui_arg2=0x2002; 
    ui_arg3=0x3003; 
    ui_arg4=0x4004; 
    ull_arg5=0x112345785005; 
    fast_print(10, FAST_PRINT_CRITICAL, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx llx-critical message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0101; 
    ui_arg1=0x1101; 
    ui_arg2=0x2102; 
    ui_arg3=0x3103; 
    ui_arg4=0x4104; 
    ull_arg5=0x112345785105; 
    fast_print(11, FAST_PRINT_CRITICAL, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx critical message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_CRITICAL_TEMP, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx critical message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

}




void fast_print_test_warn( void )
{
    unsigned int ui_arg0=0x0000; 
    unsigned int ui_arg1=0x1000; 
    unsigned int ui_arg2=0x2000; 
    unsigned int ui_arg3=0x3000; 
    unsigned int ui_arg4=0x4000; 
    unsigned long long ull_arg5=0x6000; 

    ui_arg0=0x0000; 
    ui_arg1=0x1001; 
    ui_arg2=0x2002; 
    ui_arg3=0x3003; 
    ui_arg4=0x4004; 
    ull_arg5=0x112345785005; 
    fast_print(10, FAST_PRINT_WARN, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx warn message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0101; 
    ui_arg1=0x1101; 
    ui_arg2=0x2102; 
    ui_arg3=0x3103; 
    ui_arg4=0x4104; 
    ull_arg5=0x112345785105; 
    fast_print(11, FAST_PRINT_WARN, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx warn message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_WARN_TEMP, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx warn message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

}




void fast_print_test_note( void )
{
    unsigned int ui_arg0=0x0000; 
    unsigned int ui_arg1=0x1000; 
    unsigned int ui_arg2=0x2000; 
    unsigned int ui_arg3=0x3000; 
    unsigned int ui_arg4=0x4000; 
    unsigned long long ull_arg5=0x6000; 

    ui_arg0=0x0000; 
    ui_arg1=0x1001; 
    ui_arg2=0x2002; 
    ui_arg3=0x3003; 
    ui_arg4=0x4004; 
    ull_arg5=0x112345785005; 
    fast_print(10, FAST_PRINT_NOTE, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx note message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0101; 
    ui_arg1=0x1101; 
    ui_arg2=0x2102; 
    ui_arg3=0x3103; 
    ui_arg4=0x4104; 
    ull_arg5=0x112345785105; 
    fast_print(11, FAST_PRINT_NOTE, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx note message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_NOTE_TEMP, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx note message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

}


void fast_print_test_dbg( void )
{
    unsigned int ui_arg0=0x0000; 
    unsigned int ui_arg1=0x1000; 
    unsigned int ui_arg2=0x2000; 
    unsigned int ui_arg3=0x3000; 
    unsigned int ui_arg4=0x4000; 
    unsigned long long ull_arg5=0x6000; 
    
    g_ui_line_begin_fast_print_test_dbg = __LINE__; 

    ui_arg0=0x0000; 
    ui_arg1=0x1001; 
    ui_arg2=0x2002; 
    ui_arg3=0x3003; 
    ui_arg4=0x4004; 
    ull_arg5=0x112345785005; 
    fast_print(10, FAST_PRINT_DBG, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0101; 
    ui_arg1=0x1101; 
    ui_arg2=0x2102; 
    ui_arg3=0x3103; 
    ui_arg4=0x4104; 
    ull_arg5=0x112345785105; 
    fast_print(11, FAST_PRINT_DBG, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_DBG_TEMP, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_DBG_PROGRAM, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_DBG_PROJECT, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_DBG_TEAM, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_DBG_FILE, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_DBG_FUNC, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );

    ui_arg0=0x0202; 
    ui_arg1=0x1201; 
    ui_arg2=0x2202; 
    ui_arg3=0x3203; 
    ui_arg4=0x4204; 
    ull_arg5=0x112345785205; 
    fast_print(12, FAST_PRINT_DBG_BLOCK, "No.%d-%d-%d-0x%08x-0x%08x-0x%llx dbg message\n\r", 
                ui_arg0, ui_arg1, ui_arg2, ui_arg3, ui_arg4, ull_arg5 );
    
    g_ui_line_end_fast_print_test_dbg = __LINE__; 

}




void fast_print_out_test_level( const unsigned int ui_mod_num )
{

    printf( "\n\r\n\r\n\r\n\rPrint all message from module: %d\n\r", ui_mod_num);
    fast_print_out( NULL, NULL, 0, 0xffffffff, ui_mod_num, 0 );

    printf( "\n\r\n\r\n\r\n\rPrint all fatal message from module: %d\n\r", ui_mod_num);
    fast_print_out( NULL, NULL, 0, 0xffffffff, ui_mod_num, FAST_PRINT_FATAL_TEMP );

    printf( "\n\r\n\r\n\r\n\rPrint all critical and above message from module: %d\n\r", ui_mod_num);
    fast_print_out( NULL, NULL, 0, 0xffffffff, ui_mod_num, FAST_PRINT_CRITICAL_TEMP );

    printf( "\n\r\n\r\n\r\n\rPrint all warn and above message from module: %d\n\r", ui_mod_num);
    fast_print_out( NULL, NULL, 0, 0xffffffff, ui_mod_num, FAST_PRINT_WARN_TEMP );

    printf( "\n\r\n\r\n\r\n\rPrint all note and above message from module: %d\n\r", ui_mod_num);
    fast_print_out( NULL, NULL, 0, 0xffffffff, ui_mod_num, FAST_PRINT_NOTE_TEMP );

    printf( "\n\r\n\r\n\r\n\rPrint all dbg_team and above message from module: %d\n\r", ui_mod_num);
    fast_print_out( NULL, NULL, 0, 0xffffffff, ui_mod_num, FAST_PRINT_DBG_TEAM );

    printf( "\n\r\n\r\n\r\n\rPrint all message from module: %d\n\r", ui_mod_num);
    fast_print_out( NULL, NULL, 0, 0xffffffff, ui_mod_num, FAST_PRINT_DBG_TEMP );

}


#endif		/* DBG_PRINT_TEST  */


#endif		/* FAST_PRINT_ENABLE  */


#ifdef DBG_PRINT_TEST


#ifdef DBG_PARAM_CHECK_ENABLE
void dbg_param_check_print_test( void )
{
    int  i_check = 1000;
    static int  i_check_static = 1000;
    int  *pi_null = NULL;
    int  *pi_check_static = &i_check_static ;

    dbg_print("dbg_print_test begin.\n\r");

    dbg_print_param_min_check(i_check, 100, RTN_VOID);
    dbg_print_param_min_check(i_check, 10000, RTN_VOID);
    dbg_print_param_max_check(i_check, 100, RTN_VOID);
    dbg_print_param_max_check(i_check, 10000, RTN_VOID);
    dbg_print_param_limit_check(i_check, 100, 200-1, RTN_VOID);
    dbg_print_param_limit_check(i_check, 100, 10000-1, RTN_VOID);
    dbg_print_param_limit_check(i_check, 10000, 20000-1, RTN_VOID);
    
    dbg_print_null_pointer_check(pi_null, RTN_VOID);
    dbg_print_null_pointer_check(i_check, RTN_VOID);
    
    dbg_ptr_limit_check(i_check, RTN_VOID);
    ptr_limit_check(i_check, RTN_VOID);
    dbg_ptr_limit_check(pi_check_static, RTN_VOID);
    ptr_limit_check(pi_check_static, RTN_VOID);
}
#endif  /* DBG_PARAM_CHECK_ENABLE  */



void dbg_print_test( void )
{

    dbg_print("dbg_print_test begin.\n\r");

#ifdef DBG_PARAM_CHECK_ENABLE
    dbg_param_check_print_test( );
#endif  /* DBG_PARAM_CHECK_ENABLE  */

    
#ifdef DBG_RUNTIME_PRINT_ENABLE
    dbg_flag_set( 5 );
    dbg_rt_print(4, "dbg_rt_print at level 4, should not be printed out.\n\r");
    dbg_rt_print(5, "dbg_rt_print at level 5, should not be printed out.\n\r");
    dbg_rt_print(6, "dbg_rt_print at level 6, should be printed out.\n\r");
#endif  /* DBG_RUNTIME_PRINT_ENABLE  */

    
#ifdef DBG_RUNTIME_MOD_PRINT_ENABLE
    dbg_mod_flag_set( 0, 5 );
    dbg_rt_mod_print(0, 4, "dbg_rt_print at level 4, should not be printed out.\n\r");
    dbg_rt_mod_print(0, 5, "dbg_rt_print at level 5, should not be printed out.\n\r");
    dbg_rt_mod_print(0, 6, "dbg_rt_print at level 6, should be printed out.\n\r");
#endif  /* DBG_RUNTIME_MOD_PRINT_ENABLE  */


#ifdef FAST_PRINT_ENABLE
    {
        dbg_print_func_begin_addr(fast_print_test_fatal);
        dbg_print_func_begin_addr(fast_print_test_critical);
        dbg_print_func_begin_addr(fast_print_test_warn);
        dbg_print_func_begin_addr(fast_print_test_note);
        dbg_print_func_begin_addr(fast_print_test_dbg);
        dbg_print_func_begin_addr(fast_print_out_test_level);
        
        fast_print_test_fatal( );
        fast_print_test_critical( );
        fast_print_test_warn( );
        fast_print_test_note( );
        fast_print_test_dbg( );

        fast_print_out_test_level( 10 );
        fast_print_out_test_level( 11 );
        fast_print_out_test_level( 12 );
        
        fast_print_out_test_level( 0 );

        printf( "\n\r\n\r\n\r\n\rPrint all message between line: %d and  line: %d (function:fast_print_test_dbg)\n\r", 
                    g_ui_line_begin_fast_print_test_dbg, 
                    g_ui_line_end_fast_print_test_dbg);        
        fast_print_out( NULL, NULL, g_ui_line_begin_fast_print_test_dbg, 
            g_ui_line_end_fast_print_test_dbg, 0, 0 );
        
    }
#endif		/* FAST_PRINT_ENABLE  */

}




#endif  /* DBG_PRINT_TEST  */


// u32_byte_num: number of bytes
void dbg_mem_word_dump( u32 *pu32_mem_base, u32 u32_byte_num )
{
    u32 u32_loop;
    
    for( u32_loop=0; u32_loop<(u32_byte_num/sizeof(u32)); u32_loop++ )
    {
        if( 0 == (u32_loop%4) )
        {
            xil_printf("\n\r0x%08x-%08x:", (unsigned int)(&pu32_mem_base[u32_loop]), u32_loop*4);
        }
        xil_printf(" %08x", pu32_mem_base[u32_loop]);
    }
    xil_printf("\n\r");
}



// u32_byte_num: number of bytes
// No memory address is printed out.
void dbg_mem_word_dump2( u32 *pu32_mem_base, u32 u32_byte_num )
{
    u32 u32_loop;

    for( u32_loop=0; u32_loop<(u32_byte_num/sizeof(u32)); u32_loop++ )
    {
        if( 0 == (u32_loop%4) )
        {
            xil_printf("\n\r0x%08x:", (unsigned int)(&pu32_mem_base[u32_loop]) );
        }
        xil_printf(" %08x", pu32_mem_base[u32_loop]);
    }
    xil_printf("\n\r");
}


void dbg_mem_short_dump( u16 *pu16_mem_base, u32 u32_len )
{
    u32 u32_loop;
    
    for( u32_loop=0; u32_loop<(u32_len/sizeof(u16)); u32_loop++ )
    {
        if( 0 == (u32_loop%16) )
        {
            xil_printf("\n\r0x%08x:", (unsigned int)(&pu16_mem_base[u32_loop]) );
        }
        xil_printf(" %04x", pu16_mem_base[u32_loop]);
    }
    xil_printf("\n\r");
}


// u32_byte_num: number of bytes
void dbg_mem_byte_dump( u8 *pu8_mem_base, u32 u32_byte_num )
{
    u32 u32_loop;

    for( u32_loop=0; u32_loop<(u32_byte_num/sizeof(u8)); u32_loop++ )
    {
        if( 0 == (u32_loop%16) )
        {
            xil_printf("\n\r0x%08x:", (unsigned int)(&pu8_mem_base[u32_loop]));
        }
        xil_printf(" %02x", pu8_mem_base[u32_loop]);
    }
    xil_printf("\n\r");
}


