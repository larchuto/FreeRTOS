/*
    FreeRTOS V7.4.2 - Copyright (C) 2013 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.

    >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details. You should have received a copy of the GNU General Public License
    and the FreeRTOS license exception along with FreeRTOS; if not it can be
    viewed here: http://www.freertos.org/a00114.html and also obtained by
    writing to Real Time Engineers Ltd., contact details for whom are available
    on the FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, books, training, latest versions, 
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, and our new
    fully thread aware and reentrant UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High 
    Integrity Systems, who sell the code with commercial support, 
    indemnification and middleware, under the OpenRTOS brand.
    
    http://www.SafeRTOS.com - High Integrity Systems also provide a safety 
    engineered and independently SIL3 certified version for use in safety and 
    mission critical applications that require provable dependability.
*/

/*
 *******************************************************************************
 * -NOTE- The Win32 port is a simulation (or is that emulation?) only!  Do not
 * expect to get real time behaviour from the Win32 port or this demo
 * application.  It is provided as a convenient development and demonstration
 * test bed only.  This was tested using Windows XP on a dual core laptop.
 *
 * In this example, one simulated millisecond will take approximately 40ms to
 * execute, and Windows will not be running the FreeRTOS simulator threads
 * continuously, so the timing information in the FreeRTOS+Trace logs have no
 * meaningful units.  See the documentation page for the Windows simulator for
 * an explanation of the slow timing:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 * - READ THE WEB DOCUMENTATION FOR THIS PORT FOR MORE INFORMATION ON USING IT -
 *******************************************************************************
 *
 * main() creates all the demo application tasks, then starts the scheduler.  
 * The web documentation provides more details of the standard demo application 
 * tasks, which provide no particular functionality but do provide a good 
 * example of how to use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five seconds but has a high priority
 * to ensure it gets processor time.  Its main function is to check that all the
 * standard demo tasks are still operational.  While no errors have been
 * discovered the check task will print out "OK" and the current simulated tick
 * time.  If an error is discovered in the execution of a task then the check
 * task will print out an appropriate error message.
 *
 */


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

/* Kernel includes. */
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Standard demo includes. */
#include "BlockQ.h"
#include "integer.h"
#include "semtest.h"
#include "PollQ.h"
#include "GenQTest.h"
#include "QPeek.h"
#include "recmutex.h"
#include "flop.h"
#include "TimerDemo.h"
#include "countsem.h"
#include "death.h"
#include "dynamic.h"
#include "QueueSet.h"
#include "QueueOverwrite.h"

/* Priorities at which the tasks are created. */
#define mainCHECK_TASK_PRIORITY			( configMAX_PRIORITIES - 1 )
#define mainQUEUE_POLL_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainSEM_TEST_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainFLASH_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainuIP_TASK_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainINTEGER_TASK_PRIORITY		( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY		( tskIDLE_PRIORITY )
#define mainFLOP_TASK_PRIORITY			( tskIDLE_PRIORITY )
#define mainQUEUE_OVERWRITE_PRIORITY	( tskIDLE_PRIORITY )

#define mainTIMER_TEST_PERIOD			( 50 )

/* Task function prototypes. */
static void prvCheckTask( void *pvParameters );

/* A task that is created from the idle task to test the functionality of 
eTaskStateGet(). */
static void prvTestTask( void *pvParameters );

/*
 * Writes trace data to a disk file when the trace recording is stopped.
 * This function will simply overwrite any trace files that already exist.
 */
static void prvSaveTraceFile( void );

/*
 * Called from the idle task hook function to demonstrate a few utility
 * functions that are not demonstrated by any of the standard demo tasks.
 */
static void prvDemonstrateTaskStateAndHandleGetFunctions( void );

/*-----------------------------------------------------------*/

/* The variable into which error messages are latched. */
static char *pcStatusMessage = "OK";

/* This semaphore is created purely to test using the vSemaphoreDelete() and
semaphore tracing API functions.  It has no other purpose. */
static xSemaphoreHandle xMutexToDelete = NULL;

/* The user trace event posted to the trace recording on each tick interrupt.
Note tick events will not appear in the trace recording with regular period
because this project runs in a Windows simulator, and does not therefore
exhibit deterministic behaviour. */
traceLabel xTickTraceUserEvent;

/*-----------------------------------------------------------*/

int main( void )
{
	/* Initialise the trace recorder and create the label used to post user
	events to the trace recording on each tick interrupt. */
	vTraceInitTraceData();
	xTickTraceUserEvent = xTraceOpenLabel( "tick" );

	/* Start the check task as described at the top of this file. */
	xTaskCreate( prvCheckTask, ( signed char * ) "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

	/* Create the standard demo tasks. */
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
	vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartIntegerMathTasks( mainINTEGER_TASK_PRIORITY );
	vStartGenericQueueTasks( mainGEN_QUEUE_TASK_PRIORITY );
	vStartQueuePeekTasks();
	vStartMathTasks( mainFLOP_TASK_PRIORITY );
	vStartRecursiveMutexTasks();
	vStartTimerDemoTask( mainTIMER_TEST_PERIOD );
	vStartCountingSemaphoreTasks();
	vStartDynamicPriorityTasks();
	vStartQueueSetTasks();
	vStartQueueOverwriteTask( mainQUEUE_OVERWRITE_PRIORITY );

	/* The suicide tasks must be created last as they need to know how many
	tasks were running prior to their creation.  This then allows them to 
	ascertain whether or not the correct/expected number of tasks are running at 
	any given time. */
	vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

	/* Create the semaphore that will be deleted in the idle task hook.  This
	is done purely to test the use of vSemaphoreDelete(). */
	xMutexToDelete = xSemaphoreCreateMutex();

	/* Start the trace recording - the recording is written to a file if
	configASSERT() is called. */
	printf( "\r\nTrace started.  Hit a key to dump trace file to disk.\r\n" );
	uiTraceStart();

	/* Start the scheduler itself. */
	vTaskStartScheduler();

    /* Should never get here unless there was not enough heap space to create 
	the idle and other system tasks. */
    return 0;
}
/*-----------------------------------------------------------*/

static void prvCheckTask( void *pvParameters )
{
portTickType xNextWakeTime;
const portTickType xCycleFrequency = 1000 / portTICK_RATE_MS;

	/* Just to remove compiler warning. */
	( void ) pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, xCycleFrequency );

		/* Check the standard demo tasks are running without error. */
		if( xAreTimerDemoTasksStillRunning( xCycleFrequency ) != pdTRUE )
		{
			pcStatusMessage = "Error: TimerDemo";
		}
	    else if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: IntMath";
	    }	
		else if( xAreGenericQueueTasksStillRunning() != pdTRUE )
		{			
			pcStatusMessage = "Error: GenQueue";
		}
		else if( xAreQueuePeekTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: QueuePeek";
		}
		else if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: BlockQueue";
		}
	    else if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: SemTest";
	    }
	    else if( xArePollingQueuesStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: PollQueue";
	    }
		else if( xAreMathsTaskStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Flop";
		}
	    else if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: RecMutex";
		}
		else if( xAreCountingSemaphoreTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: CountSem";
		}
		else if( xIsCreateTaskStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: Death";
		}
		else if( xAreDynamicPriorityTasksStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Dynamic\r\n";
		}
		else if( xAreQueueSetTasksStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Queue set\r\n";
		}
		else if( xIsQueueOverwriteTaskStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Queue overwrite\r\n";
		}

		/* This is the only task that uses stdout so its ok to call printf() 
		directly. */
		printf( "%s - %d\r\n", pcStatusMessage, xTaskGetTickCount() );
	}
}
/*-----------------------------------------------------------*/

static void prvTestTask( void *pvParameters )
{
const unsigned long ulMSToSleep = 5;

	/* Just to remove compiler warnings. */
	( void ) pvParameters;

	/* This task is just used to test the eTaskStateGet() function.  It
	does not have anything to do. */
	for( ;; )
	{
		/* Sleep to reduce CPU load, but don't sleep indefinitely in case there are
		tasks waiting to be terminated by the idle task. */
		Sleep( ulMSToSleep );
	}
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
const unsigned long ulMSToSleep = 15;
const unsigned char ucConstQueueNumber = 0xaaU;
void *pvAllocated;
static portBASE_TYPE xTraceRunning = pdTRUE;

/* These three functions are only meant for use by trace code, and not for
direct use from application code, hence their prototypes are not in queue.h. */
extern void vQueueSetQueueNumber( xQueueHandle pxQueue, unsigned char ucQueueNumber );
extern unsigned char ucQueueGetQueueNumber( xQueueHandle pxQueue );
extern unsigned char ucQueueGetQueueType( xQueueHandle pxQueue );
extern void vTaskSetTaskNumber( xTaskHandle xTask, unsigned portBASE_TYPE uxHandle );
extern unsigned portBASE_TYPE uxTaskGetTaskNumber( xTaskHandle xTask );

	/* Sleep to reduce CPU load, but don't sleep indefinitely in case there are
	tasks waiting to be terminated by the idle task. */
	Sleep( ulMSToSleep );

	/* Demonstrate a few utility functions that are not demonstrated by any of
	the standard demo tasks. */
	prvDemonstrateTaskStateAndHandleGetFunctions();

	/* If xMutexToDelete has not already been deleted, then delete it now.
	This is done purely to demonstrate the use of, and test, the 
	vSemaphoreDelete() macro.  Care must be taken not to delete a semaphore
	that has tasks blocked on it. */
	if( xMutexToDelete != NULL )
	{
		/* Before deleting the semaphore, test the function used to set its
		number.  This would normally only be done from trace software, rather
		than application code. */
		vQueueSetQueueNumber( xMutexToDelete, ucConstQueueNumber );

		/* Before deleting the semaphore, test the functions used to get its
		type and number.  Again, these would normally only be done from trace
		software, rather than application code. */
		configASSERT( ucQueueGetQueueNumber( xMutexToDelete ) == ucConstQueueNumber );
		configASSERT( ucQueueGetQueueType( xMutexToDelete ) == queueQUEUE_TYPE_MUTEX );
		vSemaphoreDelete( xMutexToDelete );
		xMutexToDelete = NULL;
	}

	/* Exercise heap_4 a bit.  The malloc failed hook will trap failed 
	allocations so there is no need to test here. */
	pvAllocated = pvPortMalloc( ( rand() % 100 ) + 1 );
	vPortFree( pvAllocated );

	if( _kbhit() != pdFALSE )
	{
		if( xTraceRunning == pdTRUE )
		{
			prvSaveTraceFile();
			xTraceRunning = pdFALSE;
		}
	}
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	vAssertCalled();
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( void )
{
	/* Can be implemented if required, but not required in this 
	environment and running this demo. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* Call the periodic timer test, which tests the timer API functions that
	can be called from an ISR. */
	vTimerPeriodicISRTests();

	/* Write to a queue that is in use as part of the queue set demo to 
	demonstrate using queue sets from an ISR. */
	vQueueSetAccessQueueSetFromISR();

	/* Write a user event to the trace log.  
	Note tick events will not appear in the trace recording with regular period
	because this project runs in a Windows simulator, and does not therefore
	exhibit deterministic behaviour. */
	vTraceUserEvent( xTickTraceUserEvent );
}
/*-----------------------------------------------------------*/

void vAssertCalled( void )
{
	taskDISABLE_INTERRUPTS();

	/* Stop the trace recording. */
	vTraceStop();
	prvSaveTraceFile();
		
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile( void )
{
FILE* pxOutputFile;

	fopen_s( &pxOutputFile, "Trace.dump", "wb");

	if( pxOutputFile != NULL )
	{
		fwrite( RecorderDataPtr, sizeof( RecorderDataType ), 1, pxOutputFile );
		fclose( pxOutputFile );
		printf( "\r\nTrace output saved to Trace.dump\r\n" );
	}
	else
	{
		printf( "\r\nFailed to create trace dump file\r\n" );
	}
}
/*-----------------------------------------------------------*/

static void prvDemonstrateTaskStateAndHandleGetFunctions( void )
{
xTaskHandle xIdleTaskHandle, xTimerTaskHandle;
const unsigned char ucConstTaskNumber = 0x55U;
signed char *pcTaskName;
static portBASE_TYPE xPerformedOneShotTests = pdFALSE;
xTaskHandle xTestTask;

	/* Demonstrate the use of the xTimerGetTimerDaemonTaskHandle() and 
	xTaskGetIdleTaskHandle() functions.  Also try using the function that sets
	the task number. */
	xIdleTaskHandle = xTaskGetIdleTaskHandle();
	xTimerTaskHandle = xTimerGetTimerDaemonTaskHandle();
	vTaskSetTaskNumber( xIdleTaskHandle, ( unsigned long ) ucConstTaskNumber );
	configASSERT( uxTaskGetTaskNumber( xIdleTaskHandle ) == ucConstTaskNumber );

	/* This is the idle hook, so the current task handle should equal the 
	returned idle task handle. */
	if( xTaskGetCurrentTaskHandle() != xIdleTaskHandle )
	{
		pcStatusMessage = "Error:  Returned idle task handle was incorrect";
	}

	/* Check the timer task handle was returned correctly. */
	pcTaskName = pcTaskGetTaskName( xTimerTaskHandle );
	if( strcmp( pcTaskName, "Tmr Svc" ) != 0 )
	{
		pcStatusMessage = "Error:  Returned timer task handle was incorrect";
	}

	/* This task is running, make sure it's state is returned as running. */
	if( eTaskStateGet( xIdleTaskHandle ) != eRunning )
	{
		pcStatusMessage = "Error:  Returned idle task state was incorrect";
	}

	/* If this task is running, then the timer task must be blocked. */
	if( eTaskStateGet( xTimerTaskHandle ) != eBlocked )
	{
		pcStatusMessage = "Error:  Returned timer task state was incorrect";
	}

	/* Other tests that should only be performed once follow.  The test task
	is not created on each iteration because to do so would cause the death
	task to report an error (too many tasks running). */
	if( xPerformedOneShotTests == pdFALSE )
	{
		/* Don't run this part of the test again. */
		xPerformedOneShotTests = pdTRUE;

		/* Create a test task to use to test other eTaskStateGet() return values. */
		if( xTaskCreate( prvTestTask, "Test", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTestTask ) == pdPASS )
		{
			/* If this task is running, the test task must be in the ready state. */
			if( eTaskStateGet( xTestTask ) != eReady )
			{
				pcStatusMessage = "Error: Returned test task state was incorrect 1";
			}

			/* Now suspend the test task and check its state is reported correctly. */
			vTaskSuspend( xTestTask );
			if( eTaskStateGet( xTestTask ) != eSuspended )
			{
				pcStatusMessage = "Error: Returned test task state was incorrect 2";
			}

			/* Now delete the task and check its state is reported correctly. */
			vTaskDelete( xTestTask );
			if( eTaskStateGet( xTestTask ) != eDeleted )
			{
				pcStatusMessage = "Error: Returned test task state was incorrect 3";
			}
		}
	}
}

