#include "stm8s.h"
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"
#include "stm8s_exti.h"
#include "stm8s_uart1.h"
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



uint8_t nrf24l01p_spi_rw(uint8_t value) {
	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET) {
	}
	SPI_SendData(value);
	while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET) {
	}
	return SPI_ReceiveData();
}

void nrf24l01p_ce_high(){ GPIO_WriteHigh(GPIOC, CE); }
void nrf24l01p_ce_low(){ GPIO_WriteLow(GPIOC, CE); }

void nrf24l01p_csn_high(){ GPIO_WriteHigh(GPIOC, CSN); }

void nrf24l01p_csn_low(){ GPIO_WriteLow(GPIOC, CSN); }
void nrf24l01p_irq_enable(){ /*enableInterrupts();*/ }
void nrf24l01p_irq_disable(){ /*disableInterrupts();*/ }




INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
    GPIO_WriteReverse(GPIOD, (GPIO_PIN_4 ) );
    
    uint8_t status = 0, fifo_status = 0;
    status = nrf24l01p_read(FIFO_STATUS, &fifo_status, 1);
    
    if ((fifo_status & (1 << RX_FIFO_EMPTY)) == 0){
      int pipe = (status  >> RX_P_NO) & 7;
    
      uint8_t buff[32] = {0};
      uint8_t pipe_size = get_dyn_packet_size(); // sizeof(buff); // 

       nrf24l01p_read(R_RX_PAYLOAD, buff, pipe_size);
       
       for (int i = 0; i < pipe_size; i++){
        UART1_SendData8(buff[i]);
        while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
       }    
    }
    
    nrf24l01p_write_byte(W_REGISTER | STATUS, status & 0x70); // clear status
    nrf24l01p_read_reg(STATUS, &status, 1);
    status++;
    
}


int main( void )
{
  CLK_DeInit();
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  //CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
  
  
  GPIO_Init(GPIOD, GPIO_PIN_4 | GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
 
  
  GPIO_Init(GPIOC, MOSI, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, MISO, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC, CSN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, CE, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, SCK, GPIO_MODE_OUT_PP_LOW_FAST);
  
  GPIO_Init(GPIOD, IRQ, GPIO_MODE_IN_FL_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_LOW);
  
  
  
  SPI_DeInit();
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_32, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 7);
  SPI_Cmd(ENABLE);
  
  disableInterrupts();
  
  nrf24l01p_ce_low();
  power_down();
  
  nrf24l01p_write(FLUSH_TX, 0, 0);
  nrf24l01p_write(FLUSH_RX, 0, 0);
  
  nrf24l01p_write_byte(W_REGISTER | STATUS, nrf24l01p_read_byte(R_REGISTER | STATUS) & 0x70); // clear status
  
  
  pipe_enable(0);
  //dyn_size_enable();
  set_pipe_size(0,4);
  pipe_dyn_size_enable(0);
  set_channel(125);
  enable_irqs();
  set_rx_mode();
  set_1mbps_speed();
  set_address_width(1);
  //set_tx_mode();
  delay(2000);
  nrf24l01p_ce_high();
  power_up();
  
  
  
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);
  
  UART1_DeInit();
  UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  UART1_Cmd(ENABLE);
  
  
  
  enableInterrupts();
  {
  
     uint8_t addr[5] = {0};
     uint8_t status = 0;
    uint8_t ch = get_channel();
     nrf24l01p_read(TX_ADDR, addr, sizeof(addr));
     nrf24l01p_read_reg(STATUS, &status, 1);
     ch++;
     status++;
    
  }
  
  uint8_t data[] = { 0x31, 0x32, 0x33, 0x34};
  
  while(1){
    delay(2000);
    GPIO_WriteReverse(GPIOD, (GPIO_PIN_3 ) & rand());
   
     //power_up();
  
  

  //nrf24l01p_write(FLUSH_TX, 0, 0);
  //nrf24l01p_write(W_TX_PAYLOAD, (uint8_t*)&data, sizeof(data));
    
    
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