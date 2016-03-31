#include "cube_hal.h"

#include "osal.h"
#include "sensor_service.h"
#include "debug.h"
#include "stm32_bluenrg_ble.h"
#include "bluenrg_utils.h"

#include <string.h>
#include <stdio.h>
#define SPI_FLAG_TIMEOUT         ((uint32_t)0x1000)
__IO uint32_t  SPITimeout = SPI_FLAG_TIMEOUT;
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
SPI_HandleTypeDef    DiscoverySpiHandle;

/**
  * @brief  SPI Interface pins
  */
#define SPI                       SPI2
//#define SPI_CLK                   RCC_APB2Periph_SPI1

#define SPI_SCK_PIN               GPIO_PIN_13                  /* PA.05 */
#define SPI_SCK_GPIO_PORT         GPIOB                       /* GPIOA */
//#define SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define SPI_SCK_SOURCE            GPIO_PinSource13
#define SPI_SCK_AF                GPIO_AF5_SPI2

#define SPI_MISO_PIN              GPIO_PIN_14                  /* PA.6 */
#define SPI_MISO_GPIO_PORT        GPIOB                       /* GPIOA */
//#define SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define SPI_MISO_SOURCE           GPIO_PinSource14
#define SPI_MISO_AF               GPIO_AF5_SPI2

#define SPI_MOSI_PIN              GPIO_PIN_15                  /* PA.7 */
#define SPI_MOSI_GPIO_PORT        GPIOB                       /* GPIOA */
//#define SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define SPI_MOSI_SOURCE           GPIO_PinSource15
#define SPI_MOSI_AF               GPIO_AF5_SPI2

#define SPI_CS_PIN                GPIO_PIN_12                  /* PE.03 */
#define SPI_CS_GPIO_PORT          GPIOB                       /* GPIOE */
//#define SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOE

#define SPI_INT1_PIN              GPIO_PIN_1                  /* PE.00 */
#define SPI_INT1_GPIO_PORT        GPIOB                       /* GPIOE */
#define SPI_INT1_EXTI_LINE        EXTI_Line1
#define SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource1
#define SPI_INT1_EXTI_IRQn        EXTI1_IRQn

#define SPI_IRQ_PIN

// IRQ: PA.0
//#define BNRG_SPI_IRQ_PIN            GPIO_PIN_0
//#define BNRG_SPI_IRQ_MODE           GPIO_MODE_IT_RISING
//#define BNRG_SPI_IRQ_PULL           GPIO_NOPULL
//#define BNRG_SPI_IRQ_SPEED          GPIO_SPEED_HIGH
//#define BNRG_SPI_IRQ_ALTERNATE      0
//#define BNRG_SPI_IRQ_PORT           GPIOA
//#define BNRG_SPI_IRQ_CLK_ENABLE()   __GPIOA_CLK_ENABLE()

//// EXTI External Interrupt for SPI
//// NOTE: if you change the IRQ pin remember to implement a corresponding handler
//// function like EXTI0_IRQHandler() in the user project
//#define BNRG_SPI_EXTI_IRQn          EXTI0_IRQn
//#define BNRG_SPI_EXTI_IRQHandler    EXTI0_IRQHandler
//#define BNRG_SPI_EXTI_PIN           BNRG_SPI_IRQ_PIN
//#define BNRG_SPI_EXTI_PORT          BNRG_SPI_IRQ_PORT
//#define RTC_WAKEUP_IRQHandler       RTC_WKUP_IRQHandler

#define CS_LOW()       HAL_GPIO_WritePin(SPI_CS_GPIO_PORT, SPI_CS_PIN, GPIO_PIN_RESET)
#define CS_HIGH()      HAL_GPIO_WritePin(SPI_CS_GPIO_PORT, SPI_CS_PIN, GPIO_PIN_SET)



void SPI_Init(void)
{
  /* Configure the low level interface ---------------------------------------*/
	  /* SPI configuration -------------------------------------------------------*/
	__HAL_RCC_SPI2_CLK_ENABLE();
	
  HAL_SPI_DeInit(&DiscoverySpiHandle);
  DiscoverySpiHandle.Instance 							  = SPI2;
  DiscoverySpiHandle.Init.BaudRatePrescaler 	= SPI_BAUDRATEPRESCALER_4;
  DiscoverySpiHandle.Init.Direction 					= SPI_DIRECTION_2LINES;
  DiscoverySpiHandle.Init.CLKPhase 					= SPI_PHASE_1EDGE;
  DiscoverySpiHandle.Init.CLKPolarity 				= SPI_POLARITY_LOW;
  DiscoverySpiHandle.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLED;
  DiscoverySpiHandle.Init.CRCPolynomial 			= 7;
  DiscoverySpiHandle.Init.DataSize 					= SPI_DATASIZE_8BIT;
  DiscoverySpiHandle.Init.FirstBit 					= SPI_FIRSTBIT_MSB;
  DiscoverySpiHandle.Init.NSS 								= SPI_NSS_SOFT;
  DiscoverySpiHandle.Init.TIMode 						= SPI_TIMODE_DISABLED;
  DiscoverySpiHandle.Init.Mode 							= SPI_MODE_MASTER;
	if (HAL_SPI_Init(&DiscoverySpiHandle) != HAL_OK) {printf ("ERROR: Error in initialising SPI1 \n");};
  
	__HAL_SPI_ENABLE(&DiscoverySpiHandle);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi){
	GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the SPI periph */
  __SPI1_CLK_ENABLE();

  /* Enable SCK, MOSI and MISO GPIO clocks */
  __GPIOB_CLK_ENABLE();

  /* Enable CS, INT1, INT2  GPIO clock */
//__GPIOE_CLK_ENABLE();

  //GPIO_PinAFConfig(SPI_SCK_GPIO_PORT, SPI_SCK_SOURCE, SPI_SCK_AF);
  //GPIO_PinAFConfig(SPI_MISO_GPIO_PORT, SPI_MISO_SOURCE, SPI_MISO_AF);
  //GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT, SPI_MOSI_SOURCE, SPI_MOSI_AF);

  GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
  GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.Pin = SPI_SCK_PIN;
  HAL_GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.Pin =  SPI_MOSI_PIN;
  HAL_GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.Pin = SPI_MISO_PIN;
  HAL_GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin   = SPI_CS_PIN;
  GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  HAL_GPIO_WritePin(SPI_CS_GPIO_PORT, SPI_CS_PIN, GPIO_PIN_SET);

//  /* Configure GPIO PINs to detect Interrupts */
//  GPIO_InitStructure.Pin   = SPI_INT1_PIN;
//  GPIO_InitStructure.Mode  = GPIO_MODE_IT_FALLING;
//  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
//  HAL_GPIO_Init(SPI_INT1_GPIO_PORT, &GPIO_InitStructure);


}

void SPI_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit:
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
  }
  /* Set chip select Low at the start of the transmission */ 
  CS_LOW();

  /* Send the Address of the indexed register */
  //SPI_SendByte(WriteAddr);
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    //SPI_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  CS_HIGH();
}