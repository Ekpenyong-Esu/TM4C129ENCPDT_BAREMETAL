/*One of the most well-known examples is the dilemma of the Dining philosopher.
They are all seated around a table with a large bowl of rice in the middle.
 In the middle of each philosopher is a chopstick To eat rice, a philosopher requires two chopsticks.
Philosophy's way of life includes periods of thinking and eating.
While eating, the philosopher reaches for the chopsticks on the table nearby.
In the event that she manages to acquire two chopsticks, she eats for some time before putting them down
 and continuing to ponder her thoughts. Because there are only so many chopsticks to go around,
 there are only so many philosophers to go around, which means that each philosopher can only use one at a time.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "drivers/pinout.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

#define NUM_OF_PHIL 5
#define WAITING 2
#define THINKING 1
#define EATING 0
#define LEFT (phil_index + 4) % NUM_OF_PHIL
#define RIGHT (phil_index + 1) % NUM_OF_PHIL



SemaphoreHandle_t xSemaphores[NUM_OF_PHIL];
SemaphoreHandle_t xMutex[NUM_OF_PHIL];

uint8_t phil[NUM_OF_PHIL] = {0,1,2,3,4};
volatile uint8_t currentState[] = {WAITING,WAITING,WAITING,WAITING,WAITING};

void ConfigureUART(void);

void Eat(int phil_index)
{
    // check if current philosopher is thinking
    // and if left and right neighbours are not eating
    if (currentState[phil_index] == THINKING
        && currentState[LEFT] != EATING
        && currentState[RIGHT] != EATING) {
        // assign current philosopher state to eating
        currentState[phil_index] = EATING;
        // delay for 2ms
        vTaskDelay(2);
        //print
        UARTprintf("Philosopher %d takes chopstick %d and %d\n",
                   phil_index + 1, LEFT + 1, phil_index + 1);

        UARTprintf("Philosopher %d is Eating\n", phil_index + 1);
        // release binary semaphore
        // wake up hungry philosophers
        xSemaphoreGive(xSemaphores[phil_index]);
    }
}

void chopStickTake(uint8_t phil_index)
{
    // Critical section; Take mutex to lock so no 2 philosopher tasks can have access
    // at same time
    if( xSemaphoreTake(xMutex[phil_index], ( TickType_t ) 10 ) == pdTRUE )
    {
        // assign current philosopher state to thinking
        if(currentState[phil_index] != EATING){
            currentState[phil_index] = THINKING;
            UARTprintf("Philosopher %d is Thinking\n", phil_index+1);
        }
        // Try eating if neighbours are not eating
        Eat(phil_index);

        //Release the mutex semaphore
        xSemaphoreGive(xMutex[phil_index]);

    }

    // Eat if enabled
    xSemaphoreTake(xSemaphores[phil_index], portMAX_DELAY);

    // delay for 1ms
    vTaskDelay(1);

}

void chopStickGive(uint8_t phil_index)
{
    // Critical section; Take mutex to lock so no 2 philosopher tasks can have access
    // at same time

    if( xSemaphoreTake( xMutex[phil_index], ( TickType_t ) 10 ) == pdTRUE )
    {
        // Philosopher finishes eating, relinquish its resources
        UARTprintf("Philosopher %d putting chopstick %d and %d down\n",
               phil_index + 1, LEFT + 1, phil_index + 1);
        // initialize state to default
        currentState[phil_index] = WAITING;

        // release waiting neighbours
        //  Let philosopher on left eat if possible
        Eat(LEFT);
        // Let philosopher on right eat if possible
        Eat(RIGHT);

        //Release the mutex semaphore
        xSemaphoreGive(xMutex[phil_index]);

    }


}

void Philosopher(void *pvParam)
{
    while(1){
        uint32_t *phil_index = (uint32_t*)pvParam;
        vTaskDelay(2);
        chopStickTake(*phil_index);
        vTaskDelay(1);
        chopStickGive(*phil_index);
    }
}

int main()
{
    // Configure the UART
    ConfigureUART();
    int i;
    // Create a mutex type semaphore.
    for(i = 0; i < NUM_OF_PHIL; i++){
        xMutex[i] = xSemaphoreCreateMutex();
    }
    // Create binary type semaphore.

    for(i = 0; i < NUM_OF_PHIL; i++){
        xSemaphores[i] = xSemaphoreCreateBinary();
    }
    // Create tasks

    xTaskCreate(Philosopher,"Philosoper 1",configMINIMAL_STACK_SIZE,(void *)&phil[0],0,NULL);
    xTaskCreate(Philosopher,"Philosoper 2",configMINIMAL_STACK_SIZE,(void *)&phil[1],0,NULL);
    xTaskCreate(Philosopher,"Philosoper 3",configMINIMAL_STACK_SIZE,(void *)&phil[2],0,NULL);
    xTaskCreate(Philosopher,"Philosoper 4",configMINIMAL_STACK_SIZE,(void *)&phil[3],0,NULL);
    xTaskCreate(Philosopher,"Philosoper 5",configMINIMAL_STACK_SIZE,(void *)&phil[4],0,NULL);
    // Schedule Tasks
    vTaskStartScheduler();

    while(1);
}

// Configure the UART.
void ConfigureUART(void){
    // Enable Port for GPIO peripheral used by UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable UART0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Configure UART mode
    GPIOPinConfigure(GPIO_PA0_U0RX); //for receive
    GPIOPinConfigure(GPIO_PA1_U0TX); // for transmit
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // initialize UART for I/O
    UARTStdioConfig(0, 115200, 16000000);
}
