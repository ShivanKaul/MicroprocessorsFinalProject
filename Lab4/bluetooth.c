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





// Function def
uint8_t SPI_ReceiveData(SPI_HandleTypeDef *hspi);
void SPI_SendData(SPI_HandleTypeDef *hspi, uint16_t Data);
static uint8_t SPI_SendByte(uint8_t byte);
void SPI_Init(void);


uint8_t* testBytesArray;
extern float displayed_values[]; 
void test_SPI(void){//(uint8_t*)displayed_values;//
	int last_rec;
		testBytesArray=(uint8_t*)displayed_values;
	
	HAL_SPI_Transmit(&Spi2Handle,testBytesArray , 12, 1);
	//last_rec = Spi2Handle.Instance->DR;
	printf("last byte sent %d last byte received %d\n",testBytesArray[11],last_rec);

		//printf("hi  guys! %d",);
		//printf("values: %d %d %d %d \n", testBytesArray[0],testBytesArray[1],testBytesArray[2],testBytesArray[3]);
		//printf("disp %f %f %f",displayed_values[0],displayed_values[1],displayed_values[2]);		
		//SPI_Write((uint8_t*)displayed_values,12);
}

void SPI_Init(void)
{
  /* Configure the low level interface ---------------------------------------*/
	  /* SPI configuration -------------------------------------------------------*/
	__HAL_RCC_SPI2_CLK_ENABLE();
	
  HAL_SPI_DeInit(&Spi2Handle);
  Spi2Handle.Instance 							  = SPI2;
  Spi2Handle.Init.BaudRatePrescaler 	= SPI_BAUDRATEPRESCALER_4; 
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
	HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);    
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

void SPI_Write(uint8_t* pBuffer, uint16_t NumByteToWrite)
{

  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0)
  {
    printf("receibing %d\n",SPI_SendByte(*pBuffer));
    NumByteToWrite--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
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
  while (__HAL_SPI_GET_FLAG(&Spi2Handle, SPI_FLAG_TXE) == RESET)
  {
    if((SPITimeout--) == 0) return 0; // Timeout
  }

  /* Send a Byte through the SPI peripheral */
  Spi2Handle.Instance->DR = byte;
SPITimeout=10;

  while ((SPITimeout--) == 0)
  {
  }

  /* Return the Byte read from the SPI bus */ 
  return Spi2Handle.Instance->DR;
}




