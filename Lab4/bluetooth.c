#define ARM_MATH_CM4
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "stm32f4xx_hal.h"
#include "arm_math.h"
#include "math.h"
#include "interthread.h"

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */

#define SPI_FLAG_TIMEOUT         ((uint32_t)0x1000)
__IO uint32_t  SPITimeout = SPI_FLAG_TIMEOUT;

#define READWRITE_CMD              ((uint8_t)0x80)
/* Multiple byte read/write command */
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

SPI_HandleTypeDef    Spi2Handle;
void SPI_Write(uint8_t* pBuffer, uint16_t NumByteToWrite);


/**
  * @brief  SPI Interface pins
  */


uint8_t SPI_SendByte(uint8_t byte);
void SPI_Init(void);
void SPI_Write(uint8_t* pBuffer, uint16_t NumByteToWrite);
void SPI_Read(uint8_t* pBuffer, uint16_t NumByteToRead);
uint8_t SPI_ReceiveData(SPI_HandleTypeDef *hspi);
// Variables + function names

osThreadId tid_Thread_Bluetooth;
void Thread_Bluetooth(void const *argument);
osThreadDef(Thread_Bluetooth, osPriorityRealtime, 1, 0);


/*if(__HAL_SPI_GET_FLAG(&SpiHandle_2, SPI_FLAG_RXNE) == SET){
//			if(HAL_GPIO_ReadPin(NUCLEO_SPI_GPIO_PORT, NUCLEO_SPI_CS_PIN) == GPIO_PIN_RESET){*/
#define data_ready_flag 1
		osMutexId  disp_mutex; 
osMutexDef (disp_mutex);
/**
  * @brief  Start bluetooth thread
  * @param  None
  * @retval int
  */
int start_Thread_Bluetooth	(void){
	SPI_Init();
	disp_mutex = osMutexCreate(osMutex(disp_mutex)); 
	tid_Thread_Bluetooth = osThreadCreate(osThread(Thread_Bluetooth), NULL); // Start Bluetooth
  if (!tid_Thread_Bluetooth) return(-1); 
  return(0);
}
void SPI_Write(uint8_t* pBuffer, uint16_t NumByteToWrite);
/**
  * @brief  Bluetooth thread
  * @param  argument
  * @retval None
  */
uint8_t empty[3];
int SPItim=1000;

void disable_all_irq(){
HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
}
void enable_all_irq(){
HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}
void Thread_Bluetooth(void const *argument){
	while(1){
		 // 1 ms 
		// Talk to SPI
		// Send accelerometer, temperature
		// Get angles
		
		uint8_t testBytesArray[12] = {15,7,3,1,2,1,1,1,3,1,1,1};
		//DELAY
		osDelay(1000);	

	
		/*HAL_SPI_Receive(&Spi2Handle,empty,3,10000)*/HAL_SPI_Transmit(&Spi2Handle, testBytesArray, 12, 1000);
		//SPI_Read(empty,3) ;
		printf("values: %d %d %d %d, emp:%d %d %d\n", testBytesArray[0],testBytesArray[1],testBytesArray[2],testBytesArray[3],empty[0],empty[1],empty[2]);
		//SPI_Write(testBytesArray,12);
		
		
	}
}

void SPI_Init(void)
{
  /* Configure the low level interface ---------------------------------------*/
	  /* SPI configuration -------------------------------------------------------*/
	__HAL_RCC_SPI2_CLK_ENABLE();
	
  HAL_SPI_DeInit(&Spi2Handle);
  Spi2Handle.Instance 							  = SPI2;
  Spi2Handle.Init.Direction 					= SPI_DIRECTION_2LINES;
  Spi2Handle.Init.CLKPhase 						= SPI_PHASE_1EDGE;
  Spi2Handle.Init.CLKPolarity 				= SPI_POLARITY_LOW;
  Spi2Handle.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLED;
  Spi2Handle.Init.CRCPolynomial 			= 7;
  Spi2Handle.Init.DataSize 						= SPI_DATASIZE_8BIT;
  Spi2Handle.Init.FirstBit 						= SPI_FIRSTBIT_MSB;
  Spi2Handle.Init.NSS 								= SPI_NSS_HARD_INPUT;
  Spi2Handle.Init.TIMode 							= SPI_TIMODE_DISABLED;
  Spi2Handle.Init.Mode 								= SPI_MODE_SLAVE;
	
	if (HAL_SPI_Init(&Spi2Handle) != HAL_OK) {printf ("ERROR: Error in initialising SPI2 \n");};
  
	__HAL_SPI_ENABLE(&Spi2Handle);
		HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}


void SPI_Write(uint8_t* pBuffer, uint16_t NumByteToWrite)
{


  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    SPI_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }


}

void SPI_Read(uint8_t* pBuffer, uint16_t NumByteToRead)
{

disable_all_irq();
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to LIS3DSH (Slave device) */
    SPITimeout = SPI_FLAG_TIMEOUT;
 while (__HAL_SPI_GET_FLAG(&Spi2Handle, SPI_FLAG_RXNE) == RESET)
  {
    if((SPITimeout--) == 0) {
		 printf("timed out\n");
			return;
		}
  }
	*pBuffer=SPI2->DR;
    NumByteToRead--;
    pBuffer++;
  }
	enable_all_irq();

}



/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
uint8_t SPI_SendByte(uint8_t byte)
{empty[0]=1;//Spi2Handle.Instance->DR;
  /* Loop while DR register in not empty */
  SPITimeout = SPI_FLAG_TIMEOUT;
  while (__HAL_SPI_GET_FLAG(&Spi2Handle, SPI_FLAG_TXE) == RESET)
  {
    if((SPITimeout--) == 0) return 0; // Timeout
  }

  /* Send a Byte through the SPI peripheral */
  Spi2Handle.Instance->DR = byte;

//  /* Wait to receive a Byte */
//  SPITimeout = SPI_FLAG_TIMEOUT;
//  while (__HAL_SPI_GET_FLAG(&Spi2Handle, SPI_FLAG_RXNE) == RESET)
//  {
//    if((SPITimeout--) == 0) {
//			return 0; // Timeout
//		}
//  }

  /* Return the Byte read from the SPI bus */ 
  return Spi2Handle.Instance->DR;
}

