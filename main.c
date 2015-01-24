#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"
#include "nrf24l01.h"


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
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_32, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 7);
  SPI_Cmd(ENABLE);
  
  {
  
     uint8_t addr[5] = {0};
     uint8_t status = 0;
    uint8_t ch = get_channel();
     nrf24l01p_read(TX_ADDR, addr, sizeof(addr));
     nrf24l01p_read_reg(STATUS, &status, 1);
     ch++;
     status++;
    
  }
  
  while(1){
    delay(20000);
    GPIO_WriteReverse(GPIOD, (GPIO_PIN_3 | GPIO_PIN_4) & rand());
   
    
    
  }
  
  
  return 0;
}



#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */

u8* file_;
void assert_failed(u8* file, u32 line)
{ 
  file_ = file;
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif