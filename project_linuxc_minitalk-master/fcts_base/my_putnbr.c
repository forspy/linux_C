/*
** my_putnbr.c for my_putnbr in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/server/fcts
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Mon Aug  3 18:03:18 2015 Killian Mathieu
** Last update Sat Aug  8 09:58:10 2015 Killian Mathieu
*/

#include <unistd.h>

void	my_putchar(char a);

void	my_putnbr(int nb)
{
  if (nb < 0)
    {
      write(1, "-", 1);
      my_putnbr(nb * -1);
    }
  else if (nb >= 10)
    {
      my_putnbr(nb / 10);
      my_putnbr(nb % 10);
    }
  else
    my_putchar(nb + 48);
}
