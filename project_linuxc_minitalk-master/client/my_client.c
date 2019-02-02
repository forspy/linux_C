/*
** my_client.c for my_client in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/client
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Tue Aug  4 12:29:24 2015 Killian Mathieu
** Last update Sat Aug  8 16:51:16 2015 Killian Mathieu
*/

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

char	*my_getnbr_bin(int);
void	error(int);

int	target_kill(char *num, pid_t pid, int b)//ÄãºÃ
{
  if (num[b] == '0')
    {
      if ((kill(pid, SIGUSR1)) == -1)
	{
	  error(1);
	  return (-1);
	}
    }
  else if (num[b] == '1')
    {
      if ((kill(pid, SIGUSR2)) == -1)
	{
	  error(2);
	  return (-1);
	}
    }
  return (0);
}

int	client(pid_t pid, char *str)
{
  int	a;
  int	b;
  char	*num;

  a = 0;
  while (str[a])
    {
      num = my_getnbr_bin(str[a]);
      b = 0;
      while (b < 7)
	{
	  if (target_kill(num, pid, b) == -1)
	    return (-1);
	  b++;
	  usleep(500);
	}
      a++;
    }
  return (0);
}
