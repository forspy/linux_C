/*
** main.c for my server in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/server
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Mon Aug  3 18:01:15 2015 Killian Mathieu
** Last update Sat Aug  8 16:52:28 2015 Killian Mathieu
*/

int	my_server(void);

int	main()
{
  if (my_server() == -1)
    return (-1);
  return (0);
}
