/*----------------------------------------------------------------------------
 *      ECE254 Lab Task Management
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX_ext.C
 *      Purpose: Interface functions for blocking 
 *               fixed memory block management system
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code is extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 *      Includes
 *---------------------------------------------------------------------------*/
 
#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_Task.h"       
#include "rt_List.h"
#include "rt_MemBox.h"
#include "rt_HAL_CM.h"
#include "rt_MemBox_ext.h" 
#include <stdio.h>

/* ECE254 Lab Comment: You may need to include more header files */

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/
// Waiting task list
struct OS_XCB waitList = {0,0,0,0,0,0};

P_XCB mem_wait_list = &waitList;
/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

/*==========================================================================*/
/*  The following are added for ECE254 Lab Task Management Assignmet       */
/*==========================================================================*/

/*---------------- rt_alloc_box_s, task blocks when out of memory-----------*/

/**  
   @brief: Blocking memory allocation routine.
 */
void *rt_alloc_box_s (void *p_mpool) {
	/* Add your own code here. Change the following line accordingly */
    void* return_mem_location;
	P_TCB current_task;
    current_task = os_tsk.run;
    return_mem_location = rt_alloc_box(p_mpool);
    
    if (return_mem_location == 0) {
        rt_put_prio(mem_wait_list, current_task);
        printf("blocking task %d\n", rt_tsk_self());
        rt_block(0xffff, WAIT_MEM);
		return return_mem_location;
    }
	else
	{
		return return_mem_location;
	}
}


/*----------- rt_free_box_s, pair with _s memory allocators ----------------*/
/**
 * @brief: free memory pointed by ptr, it will unblock a task that is waiting
 *         for memory.
 * @return: OS_R_OK on success and OS_R_NOK if ptr does not belong to gp_mpool 
 */
OS_RESULT rt_free_box_s (void *p_mpool, void *box) {
	/* Add your own code here. Change the following line accordingly */
    P_TCB next_waiting_task;
    next_waiting_task = rt_get_first(mem_wait_list);
	
	if ( next_waiting_task ) 
    {   
        next_waiting_task->ret_val = (U32)box;
        rt_dispatch(next_waiting_task);
        return (OS_R_OK);
    }
    else 
    {
        if (!rt_free_box(p_mpool, box))
        {
            return (OS_R_OK); 
        }
        return (OS_R_NOK);
    }
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
