/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                          STM32F0518-EVAL
*                                         Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : MD
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB          AppTaskStartTCB;
static  OS_TCB          AppTaskLED2TCB;
static  OS_TCB          AppTaskLED3TCB;
static  OS_TCB          AppTaskLED4TCB;

static  CPU_STK         AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  CPU_STK         AppTaskLED2Stk[APP_CFG_TASK_LED2_STK_SIZE];
static  CPU_STK         AppTaskLED3Stk[APP_CFG_TASK_LED3_STK_SIZE];
static  CPU_STK         AppTaskLED4Stk[APP_CFG_TASK_LED4_STK_SIZE];


        OS_TMR          AppLED3Tmr;
        OS_SEM          AppLED3Sem;

        OS_TMR          AppLED4Tmr;
        OS_Q            AppLED4MsgQ;

        CPU_INT08U      AppLEDNbr;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart              (void        *p_arg);
static  void  AppTaskLED2               (void        *p_arg);   /* Toggle timer for LED 2                               */
static  void  AppTaskLED3               (void        *p_arg);   /* Toggle timer for LED 3                               */
static  void  AppTaskLED4               (void        *p_arg);   /* Toggle timer for LED 4                               */
static  void  AppObjCreate              (void);
static  void  AppTaskCreate             (void);
                                                                /* Timer callback functions for LED applications        */
        void  AppLED3TmrCallBack        (OS_TMR *p_tmr,
                                         void   *p_arg);

        void  AppLED4TmrCallBack        (OS_TMR *p_tmr,
                                         void   *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR      err;
#if (CPU_CFG_NAME_ERR == DEF_ENABLED)
    CPU_ERR     cpu_err;
#endif

    CPU_Init();

    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit(&err);                                               /* Initialize "uC/OS-III, The Real-Time Kernel"          */

    App_OS_SetAllHooks();
  
    OSTaskCreate((OS_TCB      *)&AppTaskStartTCB,               /* Create the start task                                */
                 (CPU_CHAR    *)"App Task Start",
                 (OS_TASK_PTR  )AppTaskStart,
                 (void        *)0,
                 (OS_PRIO      )APP_CFG_TASK_START_PRIO,
                 (CPU_STK     *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY   )0,
                 (OS_TICK      )0,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III)   */

    return (1);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;

    OS_ERR      os_err;

    (void)p_arg;                                                /* Note #1                                             */


    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */


    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OSCfg_TickRate_Hz;

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&os_err);                            /* Compute CPU capacity with no task running            */
#endif


    AppObjCreate();                                             /* Create Application Events                             */

    AppTaskCreate();                                            /* Create application tasks                              */


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

    //    BSP_LED_Toggle(0);
        OSTimeDlyHMSM(0, 0, 0, 500,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_err);
    }
}

/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create the application Objects
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
    CPU_INT08U  os_err;


    OSSemCreate((OS_SEM      *)&AppLED3Sem,                     /* Activate semaphore for LED3 task                     */
                (CPU_CHAR    *)"LED4 semaphore",
                (OS_SEM_CTR   ) 0,
                (OS_ERR      *)&os_err);

    OSQCreate((OS_Q          *)&AppLED4MsgQ,                    /* Create queue for LED 4 toggle                        */
              (CPU_CHAR      *)"App LED4 MsgQ",
              (OS_MSG_QTY     ) 10,
              (OS_ERR        *)&os_err);

    OSTmrCreate((OS_TMR              *)&AppLED3Tmr,             /* Create timer event for LED3 semaphore event          */
                (CPU_CHAR            *)"Blink LED3 timer",
                (OS_TICK              ) 4,
                (OS_TICK              ) 200,
                (OS_OPT               ) OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR  ) AppLED3TmrCallBack,
                (void                *) 0,
                (OS_ERR              *)&os_err);

    OSTmrCreate((OS_TMR              *)&AppLED4Tmr,             /* Create timer event for utilizing message queue       */
                (CPU_CHAR            *)"Blink LED 4 timer",
                (OS_TICK              ) 1,
                (OS_TICK              ) 5,
                (OS_OPT               ) OS_OPT_TMR_PERIODIC,
                (OS_TMR_CALLBACK_PTR  ) AppLED4TmrCallBack,
                (void                *)&AppLEDNbr,
                (OS_ERR              *)&os_err);

}


/*
*********************************************************************************************************
*                                      AppTaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  err;

    OSTaskCreate((OS_TCB      *)&AppTaskLED2TCB,                /* Create the Toggle LED2 task                          */
                 (CPU_CHAR    *)"App Task LED2",
                 (OS_TASK_PTR  ) AppTaskLED2,
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_LED2_PRIO,
                 (CPU_STK     *)&AppTaskLED2Stk[0],
                 (CPU_STK_SIZE ) APP_CFG_TASK_LED2_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE ) APP_CFG_TASK_LED2_STK_SIZE,
                 (OS_MSG_QTY   ) 0,
                 (OS_TICK      ) 0,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&err);

    OSTaskCreate((OS_TCB      *)&AppTaskLED3TCB,                /* Create the task for LED3 semaphore blinking          */
                 (CPU_CHAR    *)"App Task LED3",
                 (OS_TASK_PTR  ) AppTaskLED3,
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_LED3_PRIO,
                 (CPU_STK     *)&AppTaskLED3Stk[0],
                 (CPU_STK_SIZE ) APP_CFG_TASK_LED3_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE ) APP_CFG_TASK_LED3_STK_SIZE,
                 (OS_MSG_QTY   ) 0,
                 (OS_TICK      ) 0,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&err);

    OSTaskCreate((OS_TCB      *)&AppTaskLED4TCB,                /* Task creation for LED 4 queue control with LED 3 too */
                 (CPU_CHAR    *)"App Task LED4",
                 (OS_TASK_PTR  ) AppTaskLED4,
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_LED4_PRIO,
                 (CPU_STK     *)&AppTaskLED4Stk[0],
                 (CPU_STK_SIZE ) APP_CFG_TASK_LED4_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE ) APP_CFG_TASK_LED4_STK_SIZE,
                 (OS_MSG_QTY   ) 0,
                 (OS_TICK      ) 0,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&err);

}


/*
*********************************************************************************************************
*                                         AppTaskLED2()
*
* Description : Task to activate and toggle LED2 after timer tick system expires.
*
* Argument(s) : p_arg   is the argument passed to 'AppTaskLED2TCB()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskLED2(void *p_arg)
{
    OS_ERR          os_err;

    (void)p_arg;


    while(DEF_TRUE) {
        BSP_LED_Toggle(2);
        OSTimeDlyHMSM(0, 0, 0, 170,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_err);
    }

}


/*
*********************************************************************************************************
*                                         AppTaskLED3()
*
* Description : Toggle LED3 using semaphore services.
*
* Argument(s) : p_arg   is the argument passed to 'AppTaskLED3TCB()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : Calls the LED function to activate LED 3.
*********************************************************************************************************
*/


static  void  AppTaskLED3(void *p_arg)
{
    CPU_BOOLEAN  status;
    OS_ERR       os_err;


    (void)p_arg;

    while (DEF_TRUE) {
        status = OSTmrStart(&AppLED3Tmr, &os_err);
        if (status != DEF_OK) {
            while (DEF_TRUE) {
                ;
            }
        } else {
            OSSemPend((OS_SEM   *)&AppLED3Sem,
                      (OS_TICK   ) 0,
                      (OS_OPT    ) OS_OPT_PEND_BLOCKING,
                      (CPU_TS   *) 0,
                      (OS_ERR   *)&os_err);
        }

        BSP_LED_Toggle(3);
    }

}

void  AppLED3TmrCallBack (OS_TMR   *p_tmr,
                          void     *p_arg)
{

    OS_ERR  os_err;


    OSSemPost((OS_SEM   *)&AppLED3Sem,
              (OS_OPT    )OS_OPT_POST_ALL,
              (OS_ERR   *)&os_err);

}

/*
*********************************************************************************************************
*                                         AppTaskLED4()
*
* Description : Toggle LED3 and LED4 using a message queue service.
*
* Argument(s) : p_arg   is the argument passed to 'AppTaskLED4()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : Calls the LED function to activate LED 4 after timer expires.
*********************************************************************************************************
*/


static  void  AppTaskLED4(void *p_arg)
{
    CPU_INT32U      *p_msg;
    CPU_INT08U       led_nbr;
    OS_MSG_SIZE      msg_size;
    CPU_BOOLEAN      status;
    OS_ERR           os_err;


    (void)p_arg;

    AppLEDNbr = 2;
    status = OSTmrStart(&AppLED4Tmr, &os_err);
    if (status != DEF_OK) {
        while (DEF_TRUE) {
                ;
        }
    }
    while (DEF_TRUE) {
        if (AppLEDNbr == 5) {
          AppLEDNbr = 3;
        }

        p_msg = (CPU_INT32U *)(OSQPend((OS_Q        *)&AppLED4MsgQ,
                                       (OS_TICK      ) 0,
                                       (OS_OPT       ) OS_OPT_PEND_BLOCKING,
                                       (OS_MSG_SIZE *)&msg_size,
                                       (CPU_TS      *) 0,
                                       (OS_ERR      *)&os_err));

        if (os_err == OS_ERR_NONE) {
            led_nbr = (CPU_INT08U)*p_msg;
        }

    BSP_LED_Toggle(led_nbr);
    AppLEDNbr++;
    }
}

void  AppLED4TmrCallBack(OS_TMR *p_tmr,
                         void   *p_arg)
{

    OS_ERR      os_err;

   OSQPost((OS_Q       *)&AppLED4MsgQ,
           (void       *) p_arg,
           (OS_MSG_SIZE ) sizeof(CPU_INT08U),
           (OS_OPT      ) OS_OPT_POST_FIFO,
           (OS_ERR     *)&os_err);

}
