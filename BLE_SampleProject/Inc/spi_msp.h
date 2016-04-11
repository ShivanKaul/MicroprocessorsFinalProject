
#ifndef __Discovery_H
#define __Discovery_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32F4XX_NUCLEO
  * @{
  */

/** @defgroup STM32F4XX_NUCLEO_BLUENRG
  * @{
  */ 

/** @defgroup STM32F4XX_NUCLEO_BLUENRG_Exported_Defines
  * @{
  */
  
/**
* @brief SPI communication details between Nucleo F4 and BlueNRG
*        Expansion Board.
*/
// SPI Instance
#define Disc_SPI_INSTANCE           SPI2
#define Disc_SPI_CLK_ENABLE()       __SPI2_CLK_ENABLE()

// SPI Configuration
#define Disc_SPI_MODE               SPI_MODE_MASTER
#define Disc_SPI_DIRECTION          SPI_DIRECTION_2LINES
#define Disc_SPI_DATASIZE           SPI_DATASIZE_8BIT
#define Disc_SPI_CLKPOLARITY        SPI_POLARITY_LOW
#define Disc_SPI_CLKPHASE           SPI_PHASE_1EDGE
#define Disc_SPI_NSS                SPI_NSS_SOFT
#define Disc_SPI_FIRSTBIT           SPI_FIRSTBIT_MSB
#define Disc_SPI_TIMODE             SPI_TIMODE_DISABLED
#define Disc_SPI_CRCPOLYNOMIAL      7
#define Disc_SPI_BAUDRATEPRESCALER  SPI_BAUDRATEPRESCALER_4
#define Disc_SPI_CRCCALCULATION     SPI_CRCCALCULATION_DISABLED



// SCLK: PB.13
#define Disc_SPI_SCLK_PIN           GPIO_PIN_13
#define Disc_SPI_SCLK_PORT          GPIOB
#define Disc_SPI_SCLK_CLK_ENABLE()  __GPIOB_CLK_ENABLE()

// MISO (Master Input Slave Output): PB.14
#define Disc_SPI_MISO_PIN           GPIO_PIN_14
#define Disc_SPI_MISO_PORT          GPIOB 
#define Disc_SPI_MISO_CLK_ENABLE()  __GPIOB_CLK_ENABLE()

// MOSI (Master Output Slave Input): PB.15
#define Disc_SPI_MOSI_PIN           GPIO_PIN_15
#define Disc_SPI_MOSI_PORT          GPIOB
#define Disc_SPI_MOSI_CLK_ENABLE()  __GPIOB_CLK_ENABLE()

// NSS/CSN/CS: PB.12
#define Disc_SPI_CS_PIN             GPIO_PIN_12
#define Disc_SPI_CS_MODE            GPIO_MODE_OUTPUT_PP
#define Disc_SPI_CS_PULL            GPIO_PULLUP
#define Disc_SPI_CS_SPEED           GPIO_SPEED_HIGH
#define Disc_SPI_CS_ALTERNATE       GPIO_AF5_SPI2
#define Disc_SPI_CS_PORT            GPIOB
#define Disc_SPI_CS_CLK_ENABLE()    __GPIOB_CLK_ENABLE()

// IRQ: PB.1
#define Disc_SPI_IRQ_PIN            GPIO_PIN_1
#define Disc_SPI_IRQ_MODE           GPIO_MODE_IT_RISING
#define Disc_SPI_IRQ_PULL           GPIO_NOPULL
#define Disc_SPI_IRQ_SPEED          GPIO_SPEED_HIGH
#define Disc_SPI_IRQ_ALTERNATE      1
#define Disc_SPI_IRQ_PORT           GPIOB
#define Disc_SPI_IRQ_CLK_ENABLE()   __GPIOA_CLK_ENABLE()

// EXTI External Interrupt for SPI
// NOTE: if you change the IRQ pin remember to implement a corresponding handler
// function like EXTI0_IRQHandler() in the user project
#define Disc_SPI_EXTI_IRQn          EXTI1_IRQn
#define Disc_SPI_EXTI_IRQHandler    EXTI1_IRQHandler
#define Disc_SPI_EXTI_PIN           Disc_SPI_IRQ_PIN
#define Disc_SPI_EXTI_PORT          Disc_SPI_IRQ_PORT
#define RTC_WAKEUP_IRQHandler       RTC_WKUP_IRQHandler


/**
  * @}
  */

/** @defgroup STM32F4XX_NUCLEO_BLUENRG_Exported_Functions
  * @{
  */
  
void Disc_SPI_Init(void);
void spiReadFromDiscovery(void);
//void Clear_SPI_IRQ(void);
//void Clear_SPI_EXTI_Flag(void);

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4XX_NUCLEO_BLUENRG_H */

    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
