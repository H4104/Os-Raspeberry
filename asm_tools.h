#ifndef ASM_TOOLS_H
#define ASM_TOOLS_H

// Permet d'écrire une valeur de 32 bits
// add : adresse mémoire ou le mot doit-etre écrite
// val : valeur à écrire
void set32(unsigned int add, int val);

// Permet de lire une valeur en mémoire
// add : adresse mémoire ou se trouve le mot
// Renvoie : la valeur lue
int get32(unsigned int add);

// Attent c cycles (environ)
void delay(unsigned int c);


#endif
