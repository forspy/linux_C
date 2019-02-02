/*
** my_strcpy.c for my_strcpy in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/fcts_base
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Tue Aug  4 15:24:39 2015 Killian Mathieu
** Last update Tue Aug  4 15:35:14 2015 Killian Mathieu
*/

#include <stdlib.h>

int	my_strlen(char *);

char	*my_strcpy(char *src)
{
  char	*dest;
  int	a;

  a = 0;
  dest = malloc(sizeof(char) * my_strlen(src));
  while (src[a])
    {
      dest[a] = src[a];
      a++;
    }
  dest[a] = '\0';
  return (dest);
}
