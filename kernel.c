#include "hw.h"
#include "uart.h"

int kmain ( void ) {
  init_hw();
  uart_init();
  char buffer[32];
  //audio_test();  // pour l'audio
  while(1) { 
  	uart_send_str("ton nom ?\n"); 
	// A tester sur raspberry, qemu merde ?
  	uart_receive_str(buffer, 32); 
  	uart_send_str('\n'); 
  	uart_send_str(buffer); 
  }
  return 0;
}
