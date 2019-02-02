/*
** my_puts.c for my_puts in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/client/fcts
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Tue Aug  4 09:27:00 2015 Killian Mathieu
** Last update Sat Aug  8 17:01:33 2015 Killian Mathieu
*/

#include <unistd.h>
#include <stdlib.h>

int	my_strlen(char *);

void	my_puterror(char *str)
{
  write(2, str, my_strlen(str));
}

void	my_putchar(char a)
{
  write(1, &a, 1);
}

void	my_putstr(char *str)
{
  write(1, str, my_strlen(str));
}

int	*my_revstr(int *str)
{
  int	a;
  int	b;
  int	*dest;

  a = 6;
  b = 0;
  dest = malloc(sizeof(int) * 7);
  while (a != -1)
    {
      dest[b] = str[a];
      b++;
      a--;
    }
  return (dest);
}

void	error(int a)
{
  if (a == 0)
    my_puterror("Error: missing arguments or corrupted parameters\n");
  if (a == 1)
    my_puterror("Error: SIGUSR1 could not be sent\n");
  if (a == 2)
    my_puterror("Error: SIGUSR2 could not be sent\n");
  if (a == 3)
    my_puterror("Error: signal from SIGUSR1 did not arrived to the server\n");
  if (a == 4)
    my_puterror("Error: signal from SIGUSR2 did not arrived to the server\n");
}
