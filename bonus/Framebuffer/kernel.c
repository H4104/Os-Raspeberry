#include "hw.h"
// #include "sched.h"
// #include "syscall.h"
#include "fb.h"

// void funcA() {
//   int cptA = 0;
//   while ( 1 ) {
//     cptA ++;
// //     switch_to();
//   }
// }
// 
// void funcB() {
//   sys_reboot();
// }
//------------------------------------------------------------------------
int kmain ( void ) {
  init_hw();
//   start_sched();
//   
//   sys_wait(10);
//   
//   create_process(funcB, NULL);
//   create_process(funcA, NULL);
// 
//   while(1);
  FramebufferInitialize();
  /* Pas atteignable vues nos 2 fonctions */
  draw();
  
  while(1) { drawRed();drawBlue(); }
  return 0;
}