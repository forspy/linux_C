/*
** my_getnbr.c for getnbr in /home/mathie_g/Tek_1_projects/C_prog_elem/CPE_2014_allum1/basics
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Mon Jun 29 16:37:00 2015 Killian Mathieu
** Last update Mon Aug  3 18:59:47 2015 Killian Mathieu
*/

int	my_getnbr(char *str)
{
  int	a;
  int	b;
  int	nb;

  a = (b = (nb = 0));
  while (str[b] != '\0')
    {
      if (str[b] != '-' && (str[b] < '0' || str[b] > '9'))
	return (nb);
      else if (str[b] == '-')
	a++;
      else if (str[b] >= '0' && str[b] <= '9')
	{
	  nb = (nb * 10) + (str[b] - 48);
	  if (str[b + 1] < '0' || str[b + 1] > '9')
	    str[b + 1] = '\0';
	}
      b++;
    }
  if (a % 2 == 1)
    nb = nb * (-1);
  return (nb);
}
