#include "input.h"

#ifdef _WIN32
#include <conio.h>

int capturar_tecla(void)
{
  return getch();
}
#else
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int capturar_tecla(void)
{
  struct termios oldt, newt;
  int ch;
  // Pega as configurações atuais do terminal
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  // Desativa o modo canônico (Buffer de linha) e o Eco
  newt.c_lflag &= ~(ICANON | ECHO);
  // Aplica as novas configurações imediatamente
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);  
  ch = getchar();

  // Restaura as configurações originais do terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}
#endif