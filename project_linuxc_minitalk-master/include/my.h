/*
** my.h for my in /home/mathie_g/Tek_1_projects/Unix_system/minitalk/PSU_2014_minitalk/include
** 
** Made by Killian Mathieu
** Login   <mathie_g@epitech.net>
** 
** Started on  Mon Aug  3 11:09:43 2015 Killian Mathieu
** Last update Sat Aug  8 17:03:06 2015 Killian Mathieu
*/

#ifndef MY_H_
# define MY_H_

void	my_putnbr(int);
void	my_putchar(char);
void	my_putstr(char *);
void	my_put(char *);
void	client(pid_t, char *);
void	my_puterror(char *);
void	my_aff(int);
void	return1(int);
void	return2(int);
void	error(int);
int	my_server(void);
int	my_getnbr(char *);
int	my_strlen(char *);
int	power2(int);
int	target_kill(char *, pid_t, int);
char	*my_getnbr_bin(int);
char	*my_strcpy(char *);
int	bintodec(int *);
int	*my_revstr(int *);

#endif /* !MY_H_ */
