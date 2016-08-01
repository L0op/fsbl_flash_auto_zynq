/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

	 
#ifndef __DBG_PRINT_H_
#define __DBG_PRINT_H_


#include <stdio.h>
#include <stdlib.h>

// Sleect one of the application situation.
#define XILINX_STANDALONE_APP
//#define LINUX_APP
//#define LINUX_KERNEL

#if defined(XILINX_STANDALONE_APP)

// For xilinx Standalone application
/* Xilinx header files */
#include <xil_printf.h>
#include "xil_types.h"

#include "xil_exception.h"
#include "xtime_l.h"

#elif defined(LINUX_APP)

// For Linux  application


#elif defined(LINUX_KERNEL)
// For Linux  kernel

#else   /*    */
#ERROR
#endif /*  XILINX_STANDALONE_APP-LINUX_APP-LINUX_KERNEL  */


/**************************** Type Definitions *******************************/

typedef struct
{
    /* Location information */
    unsigned int ui_mod_num;    // module number
    const char   *p_file_name;   // We might could upload file name, function name to host in advance or get them from ELF file.

    // Pointer to string of 'Function name' or Pointer to 'Function instruction'
    // Pointer to string of 'Function name': ease to use, use __funct__ in all reference
    // Pointer to 'Function instruction': difficult to use, provided manually, more information?
    // Now Pointer to string of 'Function name'
    // Same buffer is used for each 'Function name'
    // So we get same value for the buffer for each function
    const void   *p_func;        // function location
    unsigned int ui_line_num;

    /* debug level,  bit 15-8: fatal/critical/warining/note/debug/
                               bit   7-0:  temp debug levle.  */
    unsigned int ui_level;      
    unsigned int ui_serial_num;   /* serial number for print message */    
    
    //unsigned int ui_time;       // time, 64 bit?
    //unsigned int ui_time_high;  // time, 64 bit? Can we use high 16-bit of ui_level to store this field
    unsigned long long ull_time; // time or system cycle, 64 bit
    
    const  char  *p_format;
    
    unsigned int ui_arg0;   // is 7 args enough?
    unsigned int ui_arg1;
    unsigned int ui_arg2;
    unsigned int ui_arg3;
    unsigned int ui_arg4;
    unsigned int ui_arg5;
    unsigned int ui_arg6;
    //unsigned int ui_arg8;      // is 8 args enough?

    /* Can I operate at 64-bit data? 
            It would be faster.  
            Does args in stack frame always align to 64-bit?
         */
    //long long ll_arg1;                
    //long long ll_arg2;
    //long long ll_arg3;
    //long long ll_arg4;
} fast_print_info_st;


/************************** Constant Definitions *****************************/

#define	FAST_PRINT_INFO_NUM     2

#define	FAST_PRINT_FATAL          0x05f0
#define	FAST_PRINT_FATAL_TEMP     0x0500
#define	FAST_PRINT_CRITICAL       0x04f0
#define	FAST_PRINT_CRITICAL_TEMP  0x0400
#define	FAST_PRINT_WARN           0x03f0
#define	FAST_PRINT_WARN_TEMP      0x0300
#define	FAST_PRINT_NOTE           0x02f0
#define	FAST_PRINT_NOTE_TEMP      0x0200
#define	FAST_PRINT_DBG            0x01f0
#define	FAST_PRINT_DBG_TEMP       0x0100
#define	FAST_PRINT_DBG_MONTH      0x0103
#define	FAST_PRINT_DBG_WEEK       0x0102
#define	FAST_PRINT_DBG_DAY        0x0101
#define	FAST_PRINT_DBG_PROGRAM    0x0106
#define	FAST_PRINT_DBG_PROJECT    0x0105
#define	FAST_PRINT_DBG_TEAM       0x0104
#define	FAST_PRINT_DBG_FILE       0x0103
#define	FAST_PRINT_DBG_FUNC       0x0102
#define	FAST_PRINT_DBG_BLOCK      0x0101
#define	FAST_PRINT_DBG_PERSON     0x0101


#define	DBG_MOD_NUM     64

#define	RTN_VOID

// dbgx_flag has 32 bits, 
// we can enable printing information of 32-moduels indiviually.
#define dbgx_flag 0xffffffff

/***************** Macros (Inline Functions) Definitions *********************/

#define	DBG_CHECK


#define	DBG_PRINT_ENABLE
#define	DBGX_PRINT_ENABLE
#define DBG_PARAM_CHECK_ENABLE
//#define	DBG_RUNTIME_PRINT_ENABLE
//#define	DBG_RUNTIME_MOD_PRINT_ENABLE
//#define 	FAST_PRINT_ENABLE
//#define	DBG_PRINT_TEST




#ifndef STDOUT_BASEADDRESS
#undef DBG_PRINT_ENABLE
#undef DBG_PARAM_CHECK_ENABLE
#undef DBG_RUNTIME_PRINT_ENABLE
#undef DBG_RUNTIME_MOD_PRINT_ENABLE
#undef DBG_PRINT_TEST
#endif

#define STR(s)     #s

#if 0
#define  __ANONYMOUS_STR1(str_info, var, line)  char    var##line[] = str_info
#define  _ANONYMOUS_STR0(str_info, line)  __ANONYMOUS_STR1( str_info, _anonymous, line)
#define  ANONYMOUS_STR( str_info )  _ANONYMOUS_STR0( str_info, __LINE__)
#endif

#define  _GET_FILE_NAME(f)   #f
#define  GET_FILE_NAME(f)    _GET_FILE_NAME(f)
//#define  DECLARE_FILE_NAME2  static char  str_fast_print_file_name_static2[] = GET_FILE_NAME(__FILE__);
#define  DECLARE_FILE_NAME  static char  str_fast_print_file_name_static[] = __FILE__;

/* ============================= */
#if defined(XILINX_STANDALONE_APP)

// For xilinx Standalone application
#define print_func	xil_printf
#define print_flush_func	 fflush(stdout);
#define	FAST_PRINT_LOCK         Xil_ExceptionDisable
#define	FAST_PRINT_UNLOCK       Xil_ExceptionEnable

#elif defined(LINUX_APP)

// For Linux  application
#define print_func	printf
#define print_flush_func	 fflush(stdout);    // For Linux  application
#define	FAST_PRINT_LOCK         
#define	FAST_PRINT_UNLOCK       


#elif defined(LINUX_KERNEL)

// For Linux  kernel
#define print_func	printf
#define print_flush_func	 
#define	FAST_PRINT_LOCK         
#define	FAST_PRINT_UNLOCK       

#else   /*    */
#ERROR
#endif /*  XILINX_STANDALONE_APP-LINUX_APP-LINUX_KERNEL  */


#define fast_print(mod, level, format, args...) \
    do{ fast_print_buf( str_fast_print_file_name_static, __func__, __LINE__,  mod, level, format, ##args); }while(0)


/* print function */
#define print_var(xxx) 						do { print_func( "%s = %d\r\n", #xxx, xxx ); print_flush_func; }while(0)
#define print_var2(xxx) 					do { print_func( "%s: %s-%d: Variable: %s: %d=0x%x.\r\n", __FILE__, __func__, __LINE__, #xxx, xxx, xxx );  print_flush_func;  }while(0)


#define print_var_non_zero(xxx) 			do { if (0 != (xxx) ) { print_func( "%s = %d\r\n", #xxx, xxx ); print_flush_func; } }while(0)
#define print_var_info(info, xxx) 			do { print_func( info" %s = %d\r\n", #xxx, xxx ); print_flush_func; }while(0)
#define print_var_info_non_zero(info, xxx) 	do { if (0 != (xxx) ) { print_func( info" %s = %d\r\n", #xxx, xxx ); print_flush_func; } }while(0)
#define print_var_hex(xxx) 					do { print_func( "%s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; }while(0)
#define print_var_hex_non_zero(xxx) 		do { if (0 != (xxx) ) { print_func( "%s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; } }while(0)
#define print_var_info_hex(info, xxx) 		do { print_func( info" %s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; }while(0)
#define print_var_info_hex_non_zero(info, xxx) 		do { if (0 != xxx ) { print_func( info" %s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; } }while(0)

#define print_var_location(xxx) 			do { print_func( "Address of variable:%s is 0x%08x\r\n", #xxx, (unsigned int)&xxx ); print_flush_func; }while(0)

#define print_pointer_hex_4(pointer) 		do { unsigned int *pui_temp=(unsigned int *)pointer; print_func( "0x%08x:  %08x  %08x  %08x  %08x \r\n", \
						(unsigned int)&(pui_temp[0]), pui_temp[0], pui_temp[1], pui_temp[2], pui_temp[3] ); print_flush_func; }while(0)

#define print_pointer_hex_8(pointer) 		do { print_pointer_hex_4(pointer); print_pointer_hex_4(pointer); }while(0)

#define print_pointer_hex_16(pointer) 		do { print_pointer_hex_8(pointer); print_pointer_hex_8(pointer); }while(0)


#define print_var_old(xxx) 					do { print_func( "Old %s = %d\r\n", #xxx, (unsigned int)xxx ); print_flush_func; }while(0)
#define print_var_old_hex(xxx) 				do { print_func( "Old %s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; }while(0)
#define print_var_old_hex_non_zero(xxx) 	do { if (0 != xxx ) print_func( "Old %s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; }while(0)
#define print_var_new(xxx) 					do { print_func( "New %s = %d\r\n", #xxx, (unsigned int)xxx );  print_flush_func; }while(0)
#define print_var_new_hex(xxx) 				do { print_func( "New %s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; }while(0)
#define print_var_new_hex_non_zero(xxx) 	do { if (0 != xxx ) print_func( "New %s = 0x%08x\r\n", #xxx, (unsigned int)xxx ); print_flush_func; }while(0)

#define ptr_null_check(xxx, rtn_sts) 			do { if ( NULL != (unsigned int)(xxx) ) \
											{ print_func( "Error: %s: %s-%d: pointer: %s is NULL.\r\n", __FILE__, __func__, __LINE__, #xxx ); print_flush_func;  return rtn_sts; } }while(0)

#define ptr_limit_check( ptr, min, max, rtn_sts  )                     \
					    do { if( ( (unsigned int) (ptr) <=    (unsigned int)(min) )           \
					         ||  ( (unsigned int) (ptr) >    (unsigned int)(max) ) )        \
					        {                                               \
						        print_func( "Error: %s : %s-%d : pointer %s is invalid: 0x%08x \r\n", __FILE__, __func__, __LINE__,  #ptr, (unsigned int)ptr );  \
					             print_flush_func;  return rtn_sts;                                                 \
					        } } while(0)
	
#define ptr_limit_check_rw_region( ptr, rtn_sts  )   do { ptr_limit_check( ptr, __rodata_end, _heap_end, rtn_sts  )  } while(0)


#define param_min_check(xxx, min, rtn_sts) 			do { if ( (unsigned int)(xxx)<(unsigned int)(min) ) \
											{ print_func( "Error: %s: %s-%d: parameter: %s: %d is less than %d.\r\n", __FILE__, __func__, __LINE__, #xxx, xxx, min );  print_flush_func; return rtn_sts; } }while(0)
#define param_max_check(xxx, max, rtn_sts) 			do { if ( (unsigned int)(xxx)>(unsigned int)(max) ) \
											{ print_func( "Error: %s: %s-%d: parameter: %s: %d is great than %d.\r\n", __FILE__, __func__, __LINE__, #xxx, xxx, max );  print_flush_func; return rtn_sts; } }while(0)
#define param_limit_check(xxx, min, max, rtn_sts) 	do { if ( ( (unsigned int)(xxx)<(unsigned int)(min) ) || ( (unsigned int)(xxx)>(unsigned int)(max) ) ) \
											{ print_func( "Error: %s: %s-%d: parameter: %s: %d is beyond limit: %d~%d.\r\n", __FILE__, __func__, __LINE__, #xxx, xxx, min, max );  print_flush_func; return rtn_sts; } }while(0)


#ifdef	DBG_PRINT_ENABLE

#define dbg_print_func_begin 					do { print_func( "\r\n%s : begin.\r\n", __func__ ); print_flush_func; }while(0)
#define dbg_print_func_begin_addr(func_addr) 	do { print_func( "\r\n%s : begin at 0x%08x.\r\n", #func_addr, (unsigned int)func_addr ); print_flush_func; }while(0)

#define dbg_print(format,args...) 				do { print_func(format, ##args); print_flush_func; }while(0)
#define dbg_print_line( format,args...) 		do {  print_func("Func: %16s line:%6d: ", __func__, __LINE__);  print_func(format, ##args); print_flush_func; }while(0)

#define dbg_print_var(xxx) 						do { print_var(xxx); }while(0)
#define dbg_print_var_non_zero(xxx) 			do { print_var_non_zero(xxx); }while(0)
#define dbg_print_var_hex(xxx) 					do { print_var_hex(xxx); }while(0)
#define dbg_print_var_hex_non_zero(xxx) 		do { print_var_hex_non_zero(xxx); }while(0)

#define dbg_print_var_info(info, xxx) 			do { print_var_info(xxx); }while(0)
#define dbg_print_var_info_non_zero(info, xxx) 	do { print_var_info_non_zero(xxx); }while(0)
#define dbg_print_var_info_hex(info, xxx) 		do { print_var_info_hex(xxx); }while(0)
#define dbg_print_var_info_hex_non_zero(info, xxx) 		do { print_var_info_hex_non_zero(xxx); }while(0)

#define dbg_print_pointer_hex_4(pointer) 		do { print_pointer_hex_4(pointer); }while(0)
#define dbg_print_pointer_hex_8(pointer) 		do { print_pointer_hex_8(pointer); }while(0)
#define dbg_print_pointer_hex_16(pointer) 		do { print_pointer_hex_16(pointer); }while(0)

#define dbg_print_var_old(xxx) 					do { print_var_old(pointer); }while(0)
#define dbg_print_var_old_hex(xxx) 				do { print_var_old_hex(pointer); }while(0)
#define dbg_print_var_old_hex_non_zero(xxx) 	do { print_var_old_hex_non_zero(pointer); }while(0)

#define dbg_print_var_new(xxx) 					do { print_var_new(pointer); }while(0)
#define dbg_print_var_new_hex(xxx) 				do { print_var_new_hex(pointer); }while(0)
#define dbg_print_var_new_hex_non_zero(xxx) 	do { print_var_new_hex_non_zero(pointer); }while(0)

#define dbg_ptr_null_check(xxx, rtn_sts) 		do { ptr_null_check(xxx, rtn_sts) }while(0)
#define dbg_ptr_limit_check( ptr, min, max, rtn_sts )   do { ptr_limit_check(xxx, min, max, rtn_sts) }while(0)
#define dbg_ptr_limit_check_rw_region(xxx, rtn_sts) 	do { ptr_limit_check_rw_region(xxx, rtn_sts) }while(0)


#define dbg_param_min_check(xxx, min, rtn_sts) 	do { param_min_check(xxx, rtn_sts) }while(0)
#define dbg_param_max_check(xxx, max, rtn_sts) 	do { param_max_check(xxx, rtn_sts) }while(0)
#define dbg_param_limit_check(xxx, min, max, rtn_sts) 	do { param_limit_check(xxx, rtn_sts) }while(0)


#else     	/* DBG_PRINT_ENABLE  */

#define dbg_print_func_begin 							do { ; }while(0)

#define dbg_print(format,args...)						do { ; }while(0)
#define dbg_print_line( format,args...) 				do { ; }while(0)

#define dbg_print_var(xxx)
#define dbg_print_var_non_zero(xxx)						do { ; }while(0)
#define dbg_print_var_hex(xxx)							do { ; }while(0)
#define dbg_print_var_hex_non_zero(xxx)					do { ; }while(0)

#define dbg_print_var_info(info, xxx)					do { ; }while(0)
#define dbg_print_var_info_non_zero(info, xxx)			do { ; }while(0)
#define dbg_print_var_info_hex(info, xxx)				do { ; }while(0)
#define dbg_print_var_info_hex_non_zero(info, xxx)		do { ; }while(0)

#define dbg_print_pointer_hex_4(pointer) 			do { ; }while(0)
#define dbg_print_pointer_hex_8(pointer) 			do { ; }while(0)
#define dbg_print_pointer_hex_16(pointer) 			do { ; }while(0)

#define dbg_print_var_old(xxx)						do { ; }while(0)
#define dbg_print_var_old_hex(xxx)					do { ; }while(0)
#define dbg_print_var_old_hex_non_zero(xxx)			do { ; }while(0)

#define dbg_print_var_new(xxx)						do { ; }while(0)
#define dbg_print_var_new_hex(xxx)					do { ; }while(0)
#define dbg_print_var_new_hex_non_zero(xxx)			do { ; }while(0)

#define dbg_ptr_null_check(xxx, rtn_sts) 		do { ; }while(0)
#define dbg_ptr_limit_check( ptr, min, max, rtn_sts)	do { ; }while(0)
#define dbg_ptr_limit_check_rw_region(xxx, rtn_sts) 	do { ; }while(0)

#define dbg_param_min_check(xxx, min, rtn_sts) 		do { ; }while(0)
#define dbg_param_max_check(xxx, max, rtn_sts) 		do { ; }while(0)
#define dbg_param_limit_check(xxx, max, rtn_sts) 		do { ; }while(0)


#endif		/* DBG_PRINT_ENABLE  */

// dbgx_flag has 32 bits, 
// we can enable printing information of 32-moduels indiviually.
#ifdef DBGX_PRINT_ENABLE
#define dbgx_print(x, format,args...) 		do { if(0!=(x&dbgx_flag))	{	print_func(format, ##args); print_flush_func; } }while(0)
#define dbgx_print_line(x, format,args...) 	do {  if(0!=(x&dbgx_flag))	 {	print_func("Func: %16s line:%6d: ", __func__, __LINE__);  print_func(format, ##args); print_flush_func; } }while(0)
#else	/* DBGX_PRINT_ENABLE */
#define dbgx_print(x, format,args...) 		do { ; }while(0)
#define dbgx_print_line(x, format,args...) 	do { ; }while(0)
#endif	/* DBGX_PRINT_ENABLE */


#ifdef DBG_RUNTIME_PRINT_ENABLE

#define dbg_rt_print(type,...) \
		if (((type) > gu8_dbg_flag))  {print_func (__VA_ARGS__); }
        
#else		/* DBG_RUNTIME_PRINT_ENABLE  */

#define dbg_rt_print(type, ...)

#endif		/* DBG_RUNTIME_PRINT_ENABLE  */

#ifdef DBG_RUNTIME_MOD_PRINT_ENABLE

#define dbg_rt_mod_print(mod, type,...)                \
    if (((mod) < DBG_MOD_NUM ) )                        \
        {                                               \
            if (((type) > gu8_mod_dbg_flag[mod]))      \
            {                                           \
                print_func (__VA_ARGS__);               \
            }                                            \
        }
    
        
#else		/* DBG_RUNTIME_MOD_PRINT_ENABLE  */

#define dbg_rt_mod_print(type, ...)

#endif		/* DBG_RUNTIME_MOD_PRINT_ENABLE  */


#ifdef DBG_CHECK
    
        
#else		/* DBG_CHECK  */


#endif		/* DBG_CHECK  */




/************************** Variable Definitions *****************************/


#ifdef DBG_RUNTIME_PRINT_ENABLE
extern  u8        gu8_dbg_flag;
#endif		/* DBG_RUNTIME_PRINT_ENABLE  */

#ifdef DBG_RUNTIME_MOD_PRINT_ENABLE
extern  u8        gu8_mod_dbg_flag[DBG_MOD_NUM];
#endif		/* DBG_RUNTIME_MOD_PRINT_ENABLE  */

extern int __rodata_start[];
extern int __rodata_end[];
extern int _heap_start[];
extern int _heap_end[];

/************************** Function Prototypes ******************************/

#ifdef DBG_RUNTIME_PRINT_ENABLE
void dbg_flag_set( u8 u8_dbg_flag );
u8 dbg_flag_get( void );
#endif		/* DBG_RUNTIME_PRINT_ENABLE  */

#ifdef DBG_RUNTIME_MOD_PRINT_ENABLE
void dbg_mod_flag_set( u8 u8_dbg_mod, u8 u8_mod_dbg_flag );
u8 dbg_mod_flag_get( u8 u8_dbg_mod );
#endif		/* DBG_RUNTIME_MOD_PRINT_ENABLE  */



#ifdef FAST_PRINT_ENABLE

void fast_print_buf
(
    const char * file_name,
    const void * p_func,
    const unsigned int ui_line_num, 
    const unsigned int ui_mod_num, 
    const unsigned int ui_level, 
    const char *fmt, ...
);


int fast_print_out
(
    const char *file_name,                   // not used, set as 0 (NULL)   
    const void *p_func,                      // not used, set as 0 (NULL)   
    const unsigned int ui_line_num_min,      // not used, set as 0
    const unsigned int ui_line_num_max,      // not used, set as 0xffffffff
    const unsigned int ui_mod_num,           // not used, set as 0
    const unsigned int ui_level_min          // not used, set as 0
);

#endif  /* FAST_PRINT_ENABLE  */


#ifdef DBG_PRINT_TEST
void dbg_print_test( void );
#endif  /* DBG_PRINT_TEST  */

void dbg_mem_word_dump( u32 *pu32_mem_base, u32 u32_byte_num );
void dbg_mem_short_dump( u16 *pu16_mem_base, u32 u32_byte_num );
void dbg_mem_byte_dump( u8 *pu8_mem_base, u32 u32_byte_num );

#endif /* __DBG_PRINT_H_  */

