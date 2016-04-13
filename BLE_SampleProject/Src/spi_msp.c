#include "cube_hal.h"

#include "osal.h"
#include "sensor_service.h"
#include "debug.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"
#include "spi_msp.h"
#include <string.h>
#include <stdio.h>
#define Disc_SPI_FLAG_TIMEOUT         ((uint32_t)0x1000)
__IO uint32_t  SPITimeout = Disc_SPI_FLAG_TIMEOUT;


/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x63)

#define SPI_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define CS_LOW()       HAL_GPIO_WritePin(Disc_SPI_CS_PORT, Disc_SPI_CS_PIN, GPIO_PIN_RESET)
#define CS_HIGH()      HAL_GPIO_WritePin(Disc_SPI_CS_PORT, Disc_SPI_CS_PIN, GPIO_PIN_SET)

SPI_HandleTypeDef    DiscoverySpiHandle;
uint8_t SPI_SendByte(uint8_t byte);
void SPI_SendData(SPI_HandleTypeDef *hspi, uint16_t Data);
uint8_t SPI_ReceiveData(SPI_HandleTypeDef *hspi);
void SPI_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void SPI_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

void Disc_SPI_Init(void)
{
  /* Configure the low level interface ---------------------------------------*/
	  /* SPI configuration -------------------------------------------------------*/
	__HAL_RCC_SPI2_CLK_ENABLE();
	
  HAL_SPI_DeInit(&DiscoverySpiHandle);
  DiscoverySpiHandle.Instance 							  = Disc_SPI_INSTANCE;
  DiscoverySpiHandle.Init.BaudRatePrescaler 	= SPI_BAUDRATEPRESCALER_256;
  DiscoverySpiHandle.Init.Direction 					= SPI_DIRECTION_2LINES;
  DiscoverySpiHandle.Init.CLKPhase 						= SPI_PHASE_2EDGE;
  DiscoverySpiHandle.Init.CLKPolarity 				= SPI_POLARITY_HIGH;
  DiscoverySpiHandle.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLED;
  DiscoverySpiHandle.Init.CRCPolynomial 			= 7;
  DiscoverySpiHandle.Init.DataSize 						= SPI_DATASIZE_8BIT;
  DiscoverySpiHandle.Init.FirstBit 						= SPI_FIRSTBIT_MSB;
  DiscoverySpiHandle.Init.NSS 								= SPI_NSS_HARD_OUTPUT;
  DiscoverySpiHandle.Init.TIMode 							= SPI_TIMODE_DISABLED;
  DiscoverySpiHandle.Init.Mode 								= SPI_MODE_MASTER;

	if (HAL_SPI_Init(&DiscoverySpiHandle) != HAL_OK) {printf ("ERROR: Error in initialising SPI2 \n");};
  
	__HAL_SPI_ENABLE(&DiscoverySpiHandle);
}

void Discovery_MSP_init(SPI_HandleTypeDef *hspi){
	GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the SPI periph */
  __SPI2_CLK_ENABLE();

  /* Enable SCK, MOSI and MISO GPIO clocks */
  __GPIOB_CLK_ENABLE();

  GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
  GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.Pin = Disc_SPI_SCLK_PIN;
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(Disc_SPI_SCLK_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.Pin =  Disc_SPI_MOSI_PIN;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(Disc_SPI_MOSI_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.Pin = Disc_SPI_MISO_PIN;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(Disc_SPI_MISO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin   = Disc_SPI_CS_PIN;
  GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Disc_SPI_CS_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  HAL_GPIO_WritePin(Disc_SPI_CS_PORT, Disc_SPI_CS_PIN, GPIO_PIN_SET);

//  /* Configure GPIO PINs to detect Interrupts */
//  GPIO_InitStructure.Pin   = Disc_SPI_INT1_PIN;
//  GPIO_InitStructure.Mode  = GPIO_MODE_IT_FALLING;
//  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
//  HAL_GPIO_Init(Disc_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
}

void spiReadFromDiscovery(void){
	uint8_t a[12],b[]={63,15,101};
	
//	printf("BEFORE READING: \n");
//	printf("First 4 bytes: %d, %d, %d, %d \n",a[0],a[1],a[2],a[3]);
//	printf("Second 4 bytes: %d, %d, %d, %d \n",a[4],a[5],a[6],a[7]);
//	printf("Third 4 bytes: %d, %d, %d, %d \n",a[8],a[9],a[10],a[11]);
//	
	printf("\n\n<----------------------SPI CALL -------------------->\n\n");
	CS_LOW();
	SPI_Read(a, 12, 12);
	CS_HIGH();
	//printf("AFTER READING: STATUS = %d \n", readStatus);
	printf("First 4 bytes: %d, %d, %d, %d \n",a[0],a[1],a[2],a[3]);
	printf("Second 4 bytes: %d, %d, %d, %d \n",a[4],a[5],a[6],a[7]);
	printf("Third 4 bytes: %d, %d, %d, %d %d\n",a[8],a[9],a[10],a[11],a[12]);
}

void SPI_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit:
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */

  /* Set chip select Low at the start of the transmission */ 
  CS_LOW();


  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    SPI_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  CS_HIGH();
}

void SPI_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  

  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to LIS3DSH (Slave device) */
    *pBuffer = SPI_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  CS_HIGH();
}



/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
uint8_t SPI_SendByte(uint8_t byte)
{
  /* Loop while DR register in not empty */
  SPITimeout = SPI_FLAG_TIMEOUT;
  while (__HAL_SPI_GET_FLAG(&DiscoverySpiHandle, SPI_FLAG_TXE) == RESET)
  {
    if((SPITimeout--) == 0) return 0; // Timeout
  }

  /* Send a Byte through the SPI peripheral */
 DiscoverySpiHandle.Instance->DR=  byte;

  /* Wait to receive a Byte */
  SPITimeout = SPI_FLAG_TIMEOUT;
  while (__HAL_SPI_GET_FLAG(&DiscoverySpiHandle, SPI_FLAG_RXNE) == RESET)
  {
    if((SPITimeout--) == 0) {
			return 0; // Timeout
		}
  }

  /* Return the Byte read from the SPI bus */ 
  return DiscoverySpiHandle.Instance->DR;
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
