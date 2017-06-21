/*----------------------------------------------------------------------------
 *      ECE254 Lab Task Management
 *----------------------------------------------------------------------------
 *      Name:    RT_TASK_ext.C
 *      Purpose: Interface functions for extended task management  
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_Task.h"
#include "rt_List.h"
#include "rt_MemBox.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"
#include "rt_Task_ext.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *      Local Functions
 *---------------------------------------------------------------------------*/
/*--------------------------- rt_tsk_count_get ------------------------------*/
int rt_tsk_count_get (void) {
    int active_tasks;
    int i;
    struct OS_TCB current_TCB;
    struct OS_TCB* p;
    
    p = &current_TCB;
    active_tasks = 0;
    
    for(i = 0; i < os_maxtaskrun; i++)
    {
        p = (struct OS_TCB*)os_active_TCB[i];
        if ( p->state != 0 && os_active_TCB[i] != 0)
        {
            active_tasks++;
        }
    }
    
    if(os_idle_TCB.state != 0)
    {
        active_tasks++;
    }

    return active_tasks;

}

/*--------------------------- rt_tsk_get ------------------------------------*/
/* added in ECE254 lab keil_proc */
OS_RESULT rt_tsk_get (OS_TID task_id, RL_TASK_INFO *p_task_info) {
    struct OS_TCB* temp;
	
    temp = (struct OS_TCB*)os_active_TCB[task_id - 1];
    if (temp == NULL) {
        return OS_R_NOK;
    }    
	
	p_task_info->task_id     = task_id;
	p_task_info->state       = temp->state;
	p_task_info->prio        = temp->prio;
	p_task_info->ptask       = temp->ptask;
  
	if (p_task_info->state == RUNNING) 
    {
        p_task_info->stack_usage = ((U16)os_stackinfo - rt_get_PSP())/(*temp->stack);
    }
    else
    {
        p_task_info->stack_usage = ((U16)os_stackinfo - temp->tsk_stack)/(*temp->stack);
    }
	
	
	
	/* Attempted implementation 2
    int tot_stack_size;
	
    temp = (struct OS_TCB*)os_active_TCB[task_id - 1];
    if (temp == NULL) {
        return OS_R_NOK;
    }    
  
    
	p_task_info->task_id     = task_id;
	p_task_info->state       = temp->state;
	p_task_info->prio        = temp->prio;
    p_task_info->ptask       = temp->ptask;
    
    tot_stack_size = (U16)(temp->priv_stack);
    if(tot_stack_size == 0){
        tot_stack_size = (U16)os_stackinfo >> 2;
    }
    p_task_info->stack_usage = 100 - 100*((U32)temp->tsk_stack - (U32)&(temp->stack[0]))/(tot_stack_size);
    
	*/
	
	return OS_R_OK;
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
