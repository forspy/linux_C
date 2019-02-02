/*
** my_server.c for my_server in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/server
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Tue Aug  4 12:37:05 2015 Killian Mathieu
** Last update Sat Aug  8 17:11:18 2015 Killian Mathieu
*/

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "my.h"
//USER1 0 
//USER2 1 
//表示使用两个singal表示1/0来存储字符上的位数，进而显示字符
void		my_aff(int bin)
{
  static int	a;
  static int	*str;
  int		b;

  if (str == 0)
    str = malloc(sizeof(int) * 7);
  if (a < 7)
    str[a++] = bin;
  if (a == 7)
    {
      b = bintodec(str);
      my_putchar(b);
      a = 0;
    }
}

int	my_server()
{
  pid_t	pid;

  pid = getpid();
  my_putstr("Mon pid est le ");
  my_putnbr(pid);
  write(1, "\n", 1);
  while (42)
  {
     if ((signal(SIGUSR1, return1)) == SIG_ERR)
	{
	  error(3);
	  return (-1);
	}
      else if ((signal(SIGUSR2, return2)) == SIG_ERR)
	{
	  error(4);
	  return (-1);
	}
  }
  return (0);
}
