#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"


#define CE GPIO_PIN_3 // Port C
#define CSN GPIO_PIN_4 // Port C
#define SCK GPIO_PIN_5 // Port C
#define MOSI GPIO_PIN_6 // Port C
#define MISO GPIO_PIN_7 // Port C

#define IRQ GPIO_PIN_2 // Port D





static unsigned long int next = 1;
int rand(void) // RAND_MAX assumed to be 32767
{
        next = next * 1103515245 + 12345;
        return (unsigned int)(next/65536) % 32768;
}


static void delay(uint32_t t)
{
  while(t--);
}



uint8_t nrf24l01p_spi_rw(uint8_t value) {
	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET) {
	}
	SPI_SendData(value);
	while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) {
	}
	return SPI_ReceiveData();
}

int main( void )
{
  CLK_DeInit();
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  
  GPIO_Init(GPIOD, GPIO_PIN_4 | GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
 
  
  GPIO_Init(GPIOC, MOSI, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, MISO, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC, CSN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, CE, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, SCK, GPIO_MODE_OUT_PP_LOW_FAST);
  
  SPI_DeInit();
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_32, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0);
  SPI_Cmd(ENABLE);
  
  uint8_t val = nrf24l01p_spi_rw(0);
  
  val++;
  
  while(1){
    delay(20000);
    GPIO_WriteReverse(GPIOD, (GPIO_PIN_3 | GPIO_PIN_4) & rand());
   
    
    
  }
  
  
  return 0;
}
