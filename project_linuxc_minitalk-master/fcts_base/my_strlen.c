/*
** my_strlen.c for my strlen in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/client/fcts
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Mon Aug  3 19:06:55 2015 Killian Mathieu
** Last update Mon Aug  3 19:07:59 2015 Killian Mathieu
*/

int	my_strlen(char *str)
{
  int	a;

  a = 0;
  while (str[a])
    a++;
  return (a);
}
