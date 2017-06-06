/**
 * @brief: ECE254 Keil ARM RL-RTX Task Management Lab starter file that calls os_tsk_get()
 * @file: main_task_exp.c
 * @date: 2015/09/13
 */
/* NOTE: This release is for students to use */

#include <LPC17xx.h>
#include "uart_polling.h" 
#include <RTL.h>
#include "../../RTX_CM3/INC/RTL_ext.h" /* extended interface header file */
#include <stdio.h>
#include <string.h>

#define NUM_FNAMES 7
#define NUM_MEM_BLOCKS 1
#define MEM_BLOCK_SIZE 4

struct func_info {
  void (*p)();      /* function pointer */
  char name[16];    /* name of the function */
};

extern void os_idle_demon(void);
__task void task1(void);
__task void print_task(void);
__task void alloc_task(void);
__task void alloc_task2(void);
__task void free_task(void);
__task void init (void);
 
char *state2str(unsigned char state, char *str);
char *fp2name(void (*p)(), char *str);


// Memory constants for lab2
void *allocated_memory[NUM_MEM_BLOCKS];
int cur_mem_index;

OS_MUT g_mut_uart;
OS_TID g_tid = 255;

int  g_counter = 0;  // a global counter
char g_str[16];
char g_tsk_name[16];

_declare_box( p_mpool, MEM_BLOCK_SIZE, NUM_MEM_BLOCKS );


struct func_info g_task_map[NUM_FNAMES] = \
{
  /* os_idle_demon function ptr to be initialized in main */
  {NULL,  "os_idle_demon"}, \
  {task1, "task1\t"},   \
  {print_task, "print_task"},   \
  {alloc_task, "alloc_task"},   \
  {alloc_task2, "alloc_task2"},   \
  {free_task, "free_task"},   \
  {init,  "init\t" }
};

/* no local variables defined, use one global var */
__task void task1(void)
{
	for (;;) {
		g_counter++;
	}
}


/*--------------------------- print_task -----------------------------------*/
/* checking states of all tasks in the system                          */
/*---------------------------------------------------------------------*/

__task void print_task(void)
{
	U8 i=1;
	RL_TASK_INFO task_info;
	
  
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("TID\tNAME\t\t\tPRIO\tSTATE   \t%%STACK\n");
	os_mut_release(g_mut_uart);
    while (1) 
    { 
        for(i = 0; i < os_tsk_count_get(); i++) { 
            if (os_tsk_get(i+1, &task_info) == OS_R_OK) {
                os_mut_wait(g_mut_uart, 0xFFFF);  
                printf("%d\t%s\t\t%d\t%s\t%d%%\n", \
                       task_info.task_id, \
                       fp2name(task_info.ptask, g_tsk_name), \
                       task_info.prio, \
                       state2str(task_info.state, g_str),  \
                       task_info.stack_usage);
                os_mut_release(g_mut_uart);
            } 
        }
        
        if (os_tsk_get(0xFF, &task_info) == OS_R_OK) {
            os_mut_wait(g_mut_uart, 0xFFFF);  
            printf("%d\t%s\t\t%d\t%s\t%d%%\n", \
                   task_info.task_id, \
                   fp2name(task_info.ptask, g_tsk_name), \
                   task_info.prio, \
                   state2str(task_info.state, g_str),  \
                   task_info.stack_usage);
            os_mut_release(g_mut_uart);
        } 
        
        os_dly_wait(30);
    }
}


/*--------------------------- alloc_task -----------------------------------*/
/* Fills up our memory pool                                         	*/
/*---------------------------------------------------------------------*/
__task void alloc_task(void){

	for (;cur_mem_index < NUM_MEM_BLOCKS+2; cur_mem_index++) {
		allocated_memory[cur_mem_index] = os_mem_alloc(p_mpool);
		printf("Allocated memory block\n");
	}

}

/*--------------------------- alloc_task2 -----------------------------------*/
/* Tries to fill up memory pool then gets blocked                            */
/*---------------------------------------------------------------------*/
__task void alloc_task2(void){
	int i;
	for (i = 0; i < NUM_MEM_BLOCKS+2; i++) {
		allocated_memory[cur_mem_index++] = os_mem_alloc(p_mpool);
		printf("Allocated memory block\n");
	}
    printf("Allocation task complete");
}


/*--------------------------- free_task -----------------------------------*/
/* Frees memory spaces                                                */
/*---------------------------------------------------------------------*/

__task void free_task(void){
	int i;
	for (i = 0; i < NUM_MEM_BLOCKS+2; i++) {
		printf("Freeing memory location\n");
		os_mem_free(p_mpool, allocated_memory[--cur_mem_index]);
		os_dly_wait(10);
	}	
}



/*--------------------------- init ------------------------------------*/
/* initialize system resources and create other tasks                  */
/*---------------------------------------------------------------------*/
__task void init(void)
{
    // Initializing memory pool 
    _init_box( p_mpool, sizeof(p_mpool), MEM_BLOCK_SIZE );
	
	// Initializing mutex
	os_mut_init(&g_mut_uart);
    
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: TID = %d\n", os_tsk_self());
	os_mut_release(g_mut_uart);
	


	// First allocation task to fill up memory pool and get blocked
	g_tid = os_tsk_create(alloc_task, 2);  
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: created alloc_task with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	// Second allocation task to get blocked, has higher priority
	g_tid = os_tsk_create(alloc_task, 4); 
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: created alloc_task2 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	// Task to free up some memory
	g_tid = os_tsk_create(free_task, 1); 
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: created free_task with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	// Printing task status task
	g_tid = os_tsk_create(print_task, 5);
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: created print_task with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
  
	os_tsk_delete_self();     /* task MUST delete itself before exiting */
}

/**
 * @brief: convert state numerical value to string represenation      
 * @param: state numerical value (macro) of the task state
 * @param: str   buffer to save the string representation, 
 *               buffer to be allocated by the caller
 * @return:the string starting address
 */
char *state2str(unsigned char state, char *str)
{
	switch (state) {
	case INACTIVE:
		strcpy(str, "INACTIVE");
		break;
	case READY:
		strcpy(str, "READY   ");
		break;
	case RUNNING:
		strcpy(str, "RUNNING ");
		break;
	case WAIT_DLY:
		strcpy(str, "WAIT_DLY");
		break;
	case WAIT_ITV:
		strcpy(str, "WAIT_ITV");
		break;
	case WAIT_OR:
		strcpy(str, "WAIT_OR");
		break;
	case WAIT_AND:
		strcpy(str, "WAIT_AND");
		break;
	case WAIT_SEM:
		strcpy(str, "WAIT_SEM");
		break;
	case WAIT_MBX:
		strcpy(str, "WAIT_MBX");
		break;
	case WAIT_MUT:
		strcpy(str, "WAIT_MUT");
		break;
    case WAIT_MEM:
        strcpy(str, "WAIT_MEM");
        break;
	default:
		strcpy(str, "UNKNOWN");    
	}
	return str;
}

/** 
 * @brief: get function name by function pointer
 * @param: p the entry point of a function (i.e. function pointer)
 * @param: str the buffer to return the function name
 * @return: the function name string starting address
 */
char *fp2name(void (*p)(), char *str)
{
	int i;
	unsigned char is_found = 0;
  
	for ( i = 0; i < NUM_FNAMES; i++) {
		if (g_task_map[i].p == p) {
			str = strcpy(str, g_task_map[i].name);  
			is_found = 1;
			break;
		}
	}
	if (is_found == 0) {
		strcpy(str, "ghost");
	}
	return str;
}

int main(void)
{
	SystemInit();         /* initialize the LPC17xx MCU */
	uart0_init();         /* initilize the first UART */
   
  
	/* fill the fname map with os_idle_demon entry point */
	g_task_map[0].p = os_idle_demon;
  
	printf("Calling os_sys_init()...\n");
	os_sys_init(init);    /* initilize the OS and start the first task */
}
