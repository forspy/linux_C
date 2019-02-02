/*
** my_getnbr_bin.c for getnbr bin in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/fcts_base
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Thu Aug  6 20:26:15 2015 Killian Mathieu
** Last update Sat Aug  8 07:10:04 2015 Killian Mathieu
*/

#include <stdlib.h>

char	*my_revstr(char *);
int	my_strlen(char *);

char	*my_getnbr_bin(int let)
{
  int	a;
  char	*letter;

  a = 0;
  letter = malloc(sizeof(char) * 9);
  while (let > 0)
    {
      letter[a] = (let % 2) + 48;
      let = let / 2;
      a++;
    }
  letter[a] = '\0';
  if (my_strlen(letter) != 7)
    {
      while (a != 7)
	{
	  letter[a] = '0';
	  a++;
	}
      letter[a] = '\0';
    }
  return (letter);
}
