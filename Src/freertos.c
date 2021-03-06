/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
//#include "stm32f4xx_hal.h"
#include "event.h"
#include "encoder.h"
#include "adc.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
typedef enum {
    UP = 0,
    UPDOWN,
    DOWN,
    DOWNUP,
    SEQ,
    RND
} arp_mode_type
;
struct encoder *enc1;
struct encoder *enc2;
struct encoder *enc3;
struct encoder *enc4;
uint16_t adc1_values[2];
arp_mode_type arp_mode;

xQueueHandle event_queue;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);

extern void MX_FATFS_Init(void);
extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void vFrontPanelInputHandler( void *pvParameters );
void vKeyboardMode( void *pvParameters );

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
	/* init code for FATFS */
  //MX_FATFS_Init();
  /* init code for USB_DEVICE */
  //MX_USB_DEVICE_Init();
	/* Initialize all external devices */
	
	/* Initialize encoders */
	enc1 = encoder_init(1, ENC_1A_GPIO_Port, ENC_1A_Pin, ENC_1B_GPIO_Port, ENC_1B_Pin, 
											ENC1_SW_GPIO_Port, ENC1_SW_Pin);
	enc2 = encoder_init(1, ENC_2A_GPIO_Port, ENC_2A_Pin, ENC_2B_GPIO_Port, ENC_2B_Pin, 
											ENC2_SW_GPIO_Port, ENC2_SW_Pin);
	enc3 = encoder_init(1, ENC_3A_GPIO_Port, ENC_3A_Pin, ENC_3B_GPIO_Port, ENC_3B_Pin, 
											ENC3_SW_GPIO_Port, ENC3_SW_Pin);
	enc4 = encoder_init(1, ENC_4A_GPIO_Port, ENC_4A_Pin, ENC_4B_GPIO_Port, ENC_4B_Pin, 
											ENC4_SW_GPIO_Port, ENC4_SW_Pin);
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  xTaskCreate(vFrontPanelInputHandler, "IH_FPI", configMINIMAL_STACK_SIZE, NULL, 5, NULL ); 
	xTaskCreate(vKeyboardMode, "MODE_KBRD", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	event_queue = xQueueCreate( 10, sizeof(struct event));
	if (event_queue == NULL) 
	{ 
		printf("Insufficient heap space available to create queue 'event_queue.'\r\n");
	}
	else
	{
		printf("Successfully created queue 'event_queue' with handle %d.\r\n", event_queue);
	}
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  //MX_FATFS_Init();

  /* init code for USB_DEVICE */
  //MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
void vFrontPanelInputHandler( void *pvParameters )
{
	TickType_t xLastExecutionTime;
	xLastExecutionTime = xTaskGetTickCount();
	static uint32_t fpi_bs_new = 0;
	static uint32_t fpi_bs_old = 0;
	static struct event *ev = NULL;
	const portTickType ticks_to_wait = 1/portTICK_RATE_MS;
	uint8_t enc_state;
	portBASE_TYPE xStatus;
	
	if(ev == NULL){ev = event_create(ALL_EVENTS);}
	for( ;; )
	{
		/* Start ADC Conversion */
		//HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc1_values, 2);
		/* Read button states */
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN1_GPIO_Port,  BTN1_Pin)  == 1) ? 1UL : 0UL)     ;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN2_GPIO_Port,  BTN2_Pin)  == 1) ? 1UL : 0UL) << 1;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN3_GPIO_Port,  BTN3_Pin)  == 1) ? 1UL : 0UL) << 2;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN4_GPIO_Port,  BTN4_Pin)  == 1) ? 1UL : 0UL) << 3;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN5_GPIO_Port,  BTN5_Pin)  == 1) ? 1UL : 0UL) << 4;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN6_GPIO_Port,  BTN6_Pin)  == 1) ? 1UL : 0UL) << 5;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN7_GPIO_Port,  BTN7_Pin)  == 1) ? 1UL : 0UL) << 6;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN8_GPIO_Port,  BTN8_Pin)  == 1) ? 1UL : 0UL) << 7;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN9_GPIO_Port,  BTN9_Pin)  == 1) ? 1UL : 0UL) << 8;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN10_GPIO_Port, BTN10_Pin) == 1) ? 1UL : 0UL) << 9;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN11_GPIO_Port, BTN11_Pin) == 1) ? 1UL : 0UL) << 10;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN12_GPIO_Port, BTN12_Pin) == 1) ? 1UL : 0UL) << 11;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN13_GPIO_Port, BTN13_Pin) == 1) ? 1UL : 0UL) << 12;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN14_GPIO_Port, BTN14_Pin) == 1) ? 1UL : 0UL) << 13;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN15_GPIO_Port, BTN15_Pin) == 1) ? 1UL : 0UL) << 14;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN16_GPIO_Port, BTN16_Pin) == 1) ? 1UL : 0UL) << 15;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(BTN17_GPIO_Port, BTN17_Pin) == 1) ? 1UL : 0UL) << 16;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(ENC1_SW_GPIO_Port, ENC1_SW_Pin) == 1) ? 1UL : 0UL) << 17;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(ENC2_SW_GPIO_Port, ENC2_SW_Pin) == 1) ? 1UL : 0UL) << 18;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(ENC3_SW_GPIO_Port, ENC3_SW_Pin) == 1) ? 1UL : 0UL) << 19;
		fpi_bs_new |= ((HAL_GPIO_ReadPin(ENC4_SW_GPIO_Port, ENC4_SW_Pin) == 1) ? 1UL : 0UL) << 20;
		/*Read 2-position switch */
		fpi_bs_new |= ((HAL_GPIO_ReadPin(SW_2POS_GPIO_Port, SW_2POS_Pin) == 1) ? 1UL : 0UL) << 21;
		if(fpi_bs_new != fpi_bs_old)
		{
			for(uint8_t i = 0; i < 23; i++)
			{
				if((fpi_bs_new & (1UL << (i))) != (fpi_bs_old &(1UL << (i))))
				{
					//Generate button press/release event. Add to event queue.
					event_set_type(ev, BUTTON_INPUT);
					event_add_data(event_wrap_data(EVENT_UINT8_T, (void *)i), 0, ev); /* Add button number to event data array */
					event_add_data(event_wrap_data(EVENT_UINT8_T,(void *)(fpi_bs_new & (1UL << i))), 1, ev); /* Add button state to event data array */
					printf("Writing to queue 'event_queue' with handle %d.\r\n",event_queue);
					xStatus = xQueueSend(event_queue, ev, ticks_to_wait);
					if (xStatus != pdPASS)
					{
						printf("Could not add event to queue 'event_queue.' \r\n");
					}
				}
			}
		}
		/* Read encoder values */
		enc_state = encoder_read(enc1);
		if(enc_state != 0)
		{
			event_set_type(ev, ENC_INPUT);
			event_add_data(event_wrap_data(EVENT_UINT8_T, (void *)1) , 0, ev);
			event_add_data(event_wrap_data(EVENT_INT8_T, (void *)enc_state) , 1, ev);
			xQueueSend(event_queue, ev, ticks_to_wait);
		}
		enc_state = encoder_read(enc2);
		if(enc_state != 0)
		{
			event_set_type(ev, ENC_INPUT);
			event_add_data(event_wrap_data(EVENT_UINT8_T, (void *)2) , 0, ev);
			event_add_data(event_wrap_data(EVENT_INT8_T, (void *)enc_state) , 1, ev);
			xQueueSend(event_queue, ev, ticks_to_wait);
		}
		enc_state = encoder_read(enc3);
		if(enc_state != 0)
		{
			event_set_type(ev, ENC_INPUT);
			event_add_data(event_wrap_data(EVENT_UINT8_T, (void *)3) , 0, ev);
			event_add_data(event_wrap_data(EVENT_INT8_T, (void *)enc_state) , 1, ev);
			xQueueSend(event_queue, ev, ticks_to_wait);
		}
		enc_state = encoder_read(enc4);
		if(enc_state != 0)
		{
			event_set_type(ev, ENC_INPUT);
			event_add_data(event_wrap_data(EVENT_UINT8_T, (void *)4) , 0, ev);
			event_add_data(event_wrap_data(EVENT_INT8_T, (void *)enc_state) , 1, ev);
			xQueueSend(event_queue, ev, ticks_to_wait);
		}
		
		/*Read slide potentiometer */
//		static uint8_t index;
//		for(index = 0; index<2; index++){
//			printf("\r\n ADC value on ch%d = %d\r\n", index, adc1_values[index]);
//		}
		
		
		/*Read rotary potentiometer */
		
		
		
		
		vTaskDelayUntil( &xLastExecutionTime, 20/portTICK_RATE_MS); // ~50Hz refresh rate
		// Perform action here.
	}
	vTaskDelete( NULL ); /* Normal execution should never reach this line */
}

void vKeyboardMode ( void *pvParameters )
{
	struct event n_event;
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 1 / portTICK_RATE_MS;
	
	for( ;; )
	{
		/* Check for new events */
		xStatus = xQueueReceive(event_queue, &n_event, xTicksToWait);
			if(xStatus == pdPASS)
			{
				switch (n_event.type)
				{
					case ALL_EVENTS:
						printf("\r\nUninitialized event received on event queue.\r\n");
					case BUTTON_INPUT:
						taskENTER_CRITICAL();
						printf("\r\nReceived event of type ");
						printf("BUTTON_INPUT.\r\n");
					  printf("\tData Payload:\r\n");
						printf("\t\tButton: %d\r\n", (uint8_t)n_event.data[0].data.uint8_v);
					  printf("\t\tAction: %s\r\n", (n_event.data[1].data.uint8_v == 1 ? "PRESS" : "RELEASE"));
						taskEXIT_CRITICAL();
						break;
					case ENC_INPUT:
						taskENTER_CRITICAL();
						printf("\r\nReceived event of type ");
						printf("ENC_INPUT.\r\n");
					  printf("\tData Payload:\r\n");
						printf("\t\tEncoder: %d\r\n", (uint8_t)n_event.data[0].data.uint8_v);
					  printf("\t\tAction: %s\r\n", (n_event.data[1].data.int8_v == 1 ? "LEFT" : "RIGHT"));
						taskEXIT_CRITICAL();
						break;
					default:
						printf("Unrecognized event type received on event queue.\r\n");
						break;
				}
			}
			else
			{
				// Event buffer is currently empty.
			}
	}
	vTaskDelete( NULL ); /* Normal execution should never reach this line */
}
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
