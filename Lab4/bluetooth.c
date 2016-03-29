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
SPI_HandleTypeDef    SpiHandle;

// Function def
uint8_t SPI_ReceiveData(SPI_HandleTypeDef *hspi);
void SPI_SendData(SPI_HandleTypeDef *hspi, uint16_t Data);
static uint8_t SPI_SendByte(uint8_t byte);
void SPI_Init(void);

// Variables + function names

osThreadId tid_Thread_Bluetooth;
void Thread_Bluetooth(void const *argument);
osThreadDef(Thread_Bluetooth, osPriorityNormal, 1, 0);

#define data_ready_flag 1

/**
  * @brief  Start bluetooth thread
  * @param  None
  * @retval int
  */
int start_Thread_Accelerometer	(void){
	tid_Thread_Bluetooth = osThreadCreate(osThread(Thread_Bluetooth), NULL); // Start Bluetooth
  if (!tid_Thread_Bluetooth) return(-1); 
  return(0);
}

/**
  * @brief  Bluetooth thread
  * @param  argument
  * @retval None
  */
void Thread_Bluetooth(void const *argument){
	while(1){
		osDelay(1); // 1 ms 
		// Talk to SPI
		// Send accelerometer, temperature
		// Get angles
		float roll = getSetValue(1,0,0);
		float pitch = getSetValue(1,0,1);
		// Temperature
		float temp = getSetValue(1,0,2);
		
		// Now how do I transmit these values via SPI?
		// We will need some sort of data encoding on this end:
		// 0 -> roll
		// 1 -> pitch
		// 2 -> temp
		SPI_SendByte(0);
		SPI_SendByte((int)(roll * 100));
		SPI_SendByte(1);
		SPI_SendByte((int)(pitch * 100));
		SPI_SendByte(2);
		SPI_SendByte((int)(temp * 100));
		
	}
}

void SPI_Init(void)
{
  uint8_t ctrl = 0x00;

  /* Configure the low level interface ---------------------------------------*/
	  /* SPI configuration -------------------------------------------------------*/
	__HAL_RCC_SPI1_CLK_ENABLE();
	
  HAL_SPI_DeInit(&SpiHandle);
  SpiHandle.Instance 							  = SPI1;
  SpiHandle.Init.BaudRatePrescaler 	= SPI_BAUDRATEPRESCALER_4;
  SpiHandle.Init.Direction 					= SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase 					= SPI_PHASE_1EDGE;
  SpiHandle.Init.CLKPolarity 				= SPI_POLARITY_LOW;
  SpiHandle.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLED;
  SpiHandle.Init.CRCPolynomial 			= 7;
  SpiHandle.Init.DataSize 					= SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit 					= SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS 								= SPI_NSS_SOFT;
  SpiHandle.Init.TIMode 						= SPI_TIMODE_DISABLED;
  SpiHandle.Init.Mode 							= SPI_MODE_MASTER;
	if (HAL_SPI_Init(&SpiHandle) != HAL_OK) {printf ("ERROR: Error in initialising SPI1 \n");};
  
	__HAL_SPI_ENABLE(&SpiHandle);
  
//	/* Configure MEMS: data rate, update mode and axes */
//  ctrl = (uint8_t) (LIS3DSH_InitStruct->Power_Mode_Output_DataRate | \
//										LIS3DSH_InitStruct->Continous_Update           | \
//										LIS3DSH_InitStruct->Axes_Enable);


//  /* Write value to MEMS CTRL_REG4 regsister */
//  LIS3DSH_Write(&ctrl, LIS3DSH_CTRL_REG4, 1);

//	/* Configure MEMS: Anti-aliasing filter, full scale, self test  */
//	ctrl = (uint8_t) (LIS3DSH_InitStruct->AA_Filter_BW | \
//										LIS3DSH_InitStruct->Full_Scale   | \
//										LIS3DSH_InitStruct->Self_Test);

//	/* Write value to MEMS CTRL_REG5 regsister */
//	LIS3DSH_Write(&ctrl, LIS3DSH_CTRL_REG5, 1);
}

void sendFloatValue(SPI_HandleTypeDef *spi, float value) {
	SPI_SendData(spi, value);
}


/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
static uint8_t SPI_SendByte(uint8_t byte)
{
  /* Loop while DR register in not empty */
  SPITimeout = SPI_FLAG_TIMEOUT;
  while (__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_TXE) == RESET)
  {
    if((SPITimeout--) == 0) return 0; // Timeout
  }

  /* Send a Byte through the SPI peripheral */
  SPI_SendData(&SpiHandle,  byte);

  /* Wait to receive a Byte */
  SPITimeout = SPI_FLAG_TIMEOUT;
  while (__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_RXNE) == RESET)
  {
    if((SPITimeout--) == 0) {
			return 0; // Timeout
		}
  }

  /* Return the Byte read from the SPI bus */ 
  return SPI_ReceiveData(&SpiHandle);
}

/**
  * @brief  Returns the most recent received data by the SPIx/I2Sx peripheral. 
  * @param  *hspi: Pointer to the SPI handle. Its member Instance can point to either SPI1, SPI2 or SPI3 
  * @retval The value of the received data.
  */
uint8_t SPI_ReceiveData(SPI_HandleTypeDef *hspi)
{
  /* Return the data in the DR register */
  return hspi->Instance->DR;
}

/**
  * @brief  Transmits a Data through the SPIx/I2Sx peripheral.
  * @param  *hspi: Pointer to the SPI handle. Its member Instance can point to either SPI1, SPI2 or SPI3 
  * @param  Data: Data to be transmitted.
  * @retval None
  */
void SPI_SendData(SPI_HandleTypeDef *hspi, uint16_t Data)
{ 
  /* Write in the DR register the data to be sent */
  hspi->Instance->DR = Data;
}


