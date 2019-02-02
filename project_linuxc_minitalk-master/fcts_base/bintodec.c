/*
** bintodec.c for bin to dec in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Fri Aug  7 14:07:20 2015 Killian Mathieu
** Last update Sat Aug  8 15:34:23 2015 Killian Mathieu
*/

int	*my_revstr(int *);

int	power2(int nb)
{
  int	fin;
  int	a;

  fin = 1;
  a = 0;
  while (a != nb)
    {
      fin = fin * 2;
      a++;
    }
  return (fin);
}

char	bintodec(int *num)
{
  int	a;
  char	nb;

  nb = 0;
  a = 0;
  while (a <= 7)
    {
      if (num[a] == 1)
	nb = nb + power2(a);
      a++;
    }
  return (nb);
}
