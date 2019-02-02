/*
** main.c for main in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Mon Aug  3 11:07:28 2015 Killian Mathieu
** Last update Sat Aug  8 16:43:57 2015 Killian Mathieu
*/

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int	my_getnbr(char *);
int	client(int, char *);
void	error(int);

int	main(int argc, char **argv)
{
  if (argc < 3 || argv[1][0] == '\0' || argv[2][0] == '\0')
    error(0);
  else
    {
      if (client(my_getnbr(argv[1]), argv[2]) == -1)
	return (-1);
    }
  return (0);
}
