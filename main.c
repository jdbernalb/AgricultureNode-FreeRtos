/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include "main.h"

/******************************************************************************
 * Private macro definitions.
 ******************************************************************************/

#define idle_TASK_PRIORITY   (tskIDLE_PRIORITY)
#define main_TASK_PRIORITY ( ( unsigned portBASE_TYPE ) 1U )
static uint8_t addTx_start[NRF24L01_ADDRSIZE] = NRF24L01_ADDRTX;
static uint8_t bufferout[8] = {'h','e','l','l','o',' ',' ','\n'}; //test
//uint8_t flag = 0;
#define intqNO_DELAY			( 1 )
/******************************************************************************
 * Private function prototypes.
 ******************************************************************************/


static void vReadDigitalSensor(void* pvParameters);
static void vReadHumidity(void* pvParameters);
static void vReadClockSensor(void* pvParameters);
static void vSend(void* pvParameters);
/******************************************************************************
 * Public function definitions.
 ******************************************************************************/

 static QueueHandle_t xQueueD,xQueueH, xQueueRTC;


/**************************************************************************//**
 * \fn int main(void)
 *
 * \brief Main function.
 *
 * \return
 ******************************************************************************/
int main(void)
{
	initUART();
	printf("init\n");
				uint8_t year = 0;
				uint8_t month = 0;
				uint8_t day = 0;
				uint8_t hour = 0;
				uint8_t minute = 0;
				uint8_t second = 0;
	xQueueD = xQueueCreate( 1, sizeof( struct DigSensorMessage ) );
	xQueueH = xQueueCreate(1, sizeof( struct HumidityMessage) );
	xQueueRTC = xQueueCreate( 1,sizeof( struct I2CMessage ) );
		
    // Create task.
	printf("Start Create : \n");
	xTaskCreate( vReadHumidity, "soil", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
    xTaskCreate( vReadDigitalSensor, "dig", configMINIMAL_STACK_SIZE, NULL,  main_TASK_PRIORITY, NULL   );
	xTaskCreate( vReadClockSensor, "hum", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
    xTaskCreate( vSend, "send", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);


    printf("Init Modules : \n");
	//DDRD |= (1<<PD5); //output LED
	//PORTD |= (1<<PD5); //LED On
    nrf24l01_init();                        //Init Wireless Module
    nrf24l01_settxaddr(addTx_start);       //Setup address to send Data (Next Node)
	//sei();
	ds1307_init();
	
	bmp085_init();
	initSoilSensor();
    printf("Start Scheduler : \n");
	

	ds1307_getdate(&year, &month, &day, &hour, &minute, &second);
	if (year==0)
		ds1307_setdate(17, 04, 26, 02, 10, 35);
    // Start scheduler.
    vTaskStartScheduler();
	
    return 0;
}

/**************************************************************************//**
 * \fn static vApplicationIdleHook(void)
 *
 * \brief
 ******************************************************************************/
void vApplicationIdleHook(void)
{
}

/******************************************************************************
 * Private function definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn static void vReadSoilSensor(void* pvParameters)
 *
 * \brief
 *
 * \param[in]   pvParameters
 ******************************************************************************/
static void vReadHumidity(void* pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 180000/ portTICK_PERIOD_MS;
	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	for ( ;; )
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		struct HumidityMessage c;
		uint8_t SoilHumidity=0;
		uint8_t humidity = 0;
		SoilHumidity =(uint8_t) SoilSensorGetSoilHumidity();
		humidity = (uint8_t) dht11_gethumidity();
		//SoilHumidity = 50;
		//humidity = 80;
		c.data1=SoilHumidity;
		c.data2=humidity;
		xQueueSend( xQueueH,(void*) &c, intqNO_DELAY);
		printf("sleep hum\n");
		
	}
}
/**************************************************************************//**
 * \fn static void vReadDigitalSensor(void* pvParameters)
 *
 * \brief
 *
 * \param[in]   pvParameters
 ******************************************************************************/
static void vReadDigitalSensor(void* pvParameters)
{
		TickType_t xLastWakeTime;
		const TickType_t xFrequency = 420000/ portTICK_PERIOD_MS;
		// Initialise the xLastWakeTime variable with the current time.
		xLastWakeTime = xTaskGetTickCount();
	for ( ;; )
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		struct DigSensorMessage b;
		uint8_t temperature1 = 0;
		uint32_t pressure=0;
		temperature1 = (uint8_t) bmp085_gettemperature();
		pressure = (uint32_t) bmp085_getpressure();
		//temperature1 = (uint8_t) 25.5;
		//pressure = (uint32_t) 704.8;
		b.data1=pressure;
		b.data2=temperature1;
		xQueueSend( xQueueD,(void*)&b, intqNO_DELAY);
		printf("sleep dig\n");
		
	}
}
/**************************************************************************//**
 * \fn static void vReadClockSensor(void* pvParameters)
 *
 * \brief
 *
 * \param[in]   pvParameters
 ******************************************************************************/
static void vReadClockSensor(void* pvParameters)
{
			TickType_t xLastWakeTime;
			const TickType_t xFrequency = 20000/ portTICK_PERIOD_MS;
			// Initialise the xLastWakeTime variable with the current time.
			xLastWakeTime = xTaskGetTickCount();
	for ( ;; )
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		struct I2CMessage a;
		uint8_t year = 0;
		uint8_t month = 0;
		uint8_t day = 0;
		uint8_t hour = 0;
		uint8_t minute = 0;
		uint8_t second = 0;
		ds1307_getdate(&year, &month, &day, &hour, &minute, &second);
		a.data1=hour;
		a.data2=minute;
		xQueueSend( xQueueRTC,  (void*)&a, intqNO_DELAY );
		
	}
}
/**************************************************************************//**
 * \fn static void vSend(void* pvParameters)
 *
 * \brief
 *
 * \param[in]   pvParameters
 ******************************************************************************/
static void vSend(void* pvParameters)
{

	//setup buffer
			TickType_t xLastWakeTime;
			const TickType_t xFrequency = 60000/ portTICK_PERIOD_MS;
			// Initialise the xLastWakeTime variable with the current time.
			xLastWakeTime = xTaskGetTickCount();
	for ( ;; )
	{
		
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		static uint8_t message[10]={':','\x0','\x0','S','\x0','\x0','H','\x0','\x0',':'};
		struct I2CMessage pxRxedMessage;
		struct DigSensorMessage pxRxedSensorMessage;
		struct HumidityMessage pxRxedSHumMessage;
		

		if( xQueueReceive( xQueueRTC, &(pxRxedMessage), intqNO_DELAY) ){
			printf("RTC: %" PRIu8,pxRxedMessage.data1);
			printf(": %" PRIu8 "\n",pxRxedMessage.data2);
			message[1]=pxRxedMessage.data1;
			message[2]=pxRxedMessage.data2;
		}
		//################################### Soil & humidity ####################################
		if( xQueueReceive( xQueueH, &(pxRxedSHumMessage), intqNO_DELAY) ){
			//printf("S: %" PRIu8 "\n",pxRxedSHumMessage.data1);
			//printf("H: %" PRIu8 "\n",pxRxedSHumMessage.data2);
			message[3]='S';
			message[5]=pxRxedSHumMessage.data1;
			message[6]='H';
			message[8]=pxRxedSHumMessage.data2;
			if(nrf24l01_write(message) == 1)
			{	printf("ok \n");
			}
			else{
				if(nrf24l01_writeRT()== 1)
				{printf("ok \n");
				}
			}
			printf("Hum");
			printf("\n");
		}
		////################################################################################
		////################################### Temperature & Pressure ####################################
		if( xQueueReceive( xQueueD, &(pxRxedSensorMessage), intqNO_DELAY) ){
				//printf("P: %" PRIu32 "\n",pxRxedSensorMessage.data1);
				//printf("T: %" PRIu8 "\n",pxRxedSensorMessage.data2);
				message[3]='P';
				message[4]=(uint8_t) ((pxRxedSensorMessage.data1&0xFF00)>>8);
				message[5]=(uint8_t) (pxRxedSensorMessage.data1&0xFF);
				message[6]='T';
				message[8]=pxRxedSensorMessage.data2;
				if(nrf24l01_write(message) == 1)
				{	printf("ok \n");
				}
				else{
					if(nrf24l01_writeRT()== 1)
					{printf("ok \n");
					}
				}
				printf("Dig");
				printf("\n");
		}
		////################################################################################
		printf("sleep send\n");
		
	}
}



