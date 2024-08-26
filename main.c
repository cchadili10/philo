/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchadili <hchadili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 19:40:07 by hchadili          #+#    #+#             */
/*   Updated: 2024/08/26 16:21:49 by hchadili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}


int	ft_usleep(size_t milliseconds)
{
	size_t	start;

	start = get_current_time();
	while ((get_current_time() - start) < milliseconds)
		usleep(500);
	return (0);
}

int	ft_atoi(const char *str)
{
	int					x;
	int					sin;
	unsigned long long	res;

	x = 0;
	sin = 1;
	res = 0;
	while ((str[x] >= 9 && str[x] <= 13) || str[x] == 32)
		x++;
	if (str[x] == '-' || str[x] == '+' )
	{
		if (str[x] == '-')
			sin *= -1;
		x++;
	}
	while (str[x] >= '0' && str[x] <= '9')
	{
		res = res * 10 + str[x++] - 48;
		if (res > LONG_MAX && sin == 1)
			return (-1);
		else if (res > LONG_MAX && sin == -1)
			return (0);
	}
	return (res * sin);
}
void eat(t_philo *philo, t_program *pro)
{
	size_t curt_time;
	
	pthread_mutex_lock(&pro->dead);
	if (pro->dead_flag == 1)
	{
		pthread_mutex_unlock(&pro->dead);
		return;
	}
	pthread_mutex_unlock(&pro->dead);
	curt_time = get_current_time() - philo->program->start_time;
	pthread_mutex_lock(&philo->program->meal_lock);
	philo->last_meal = curt_time;
	pthread_mutex_unlock(&philo->program->meal_lock);
	printf("%zu %d is eating\n",curt_time, philo->philo_id);
	ft_usleep(philo->time_to_eat);
}
void ft_time_sleep(t_philo *philo, t_program *pro)
{
	pthread_mutex_lock(&pro->dead);
	if (pro->dead_flag == 1)
	{
		pthread_mutex_unlock(&pro->dead);
		return;
	}
	pthread_mutex_unlock(&pro->dead);
	printf("%zu %d is sleeping\n",(get_current_time() - philo->program->start_time), philo->philo_id);
	ft_usleep(philo->time_to_sleep);
}
void	think_philo(t_program *pro, t_philo *philo)
{
	pthread_mutex_lock(&pro->dead);
	if (pro->dead_flag == 1)
	{
		pthread_mutex_unlock(&pro->dead);
		return;
	}
	pthread_mutex_unlock(&pro->dead);
	printf("%zu %d is thinking\n", (get_current_time() - pro->start_time), philo->philo_id);
}
void	pick_up_fork(t_program *pro, t_philo *philo, int r_l)
{
	pthread_mutex_lock(&pro->dead);
	if (pro->dead_flag == 1)
	{
		pthread_mutex_unlock(&pro->dead);
		return ;
	}
	pthread_mutex_unlock(&pro->dead);
	pthread_mutex_lock(&pro->fork[r_l]);
	printf("%zu %d has taken a fork\n", (get_current_time() - pro->start_time), philo->philo_id);
}
void *ft_routing(void *t)
{
	t_philo *philo = (t_philo *)t;
	t_program *pro = philo->program;
	
	int left_fork = philo->philo_id - 1;
    int right_fork = philo->philo_id % pro->num_philo;
	if (philo->philo_id % 2 == 0)
		ft_usleep(pro->time_to_eat);
	philo->cont = 0;
	while (1)
	{
		pthread_mutex_lock(&pro->dead);
		if(pro->dead_flag == 1)
		{
			pthread_mutex_unlock(&pro->dead);
			break ;
		}	
		pthread_mutex_unlock(&pro->dead);
		
		think_philo(pro, philo);
		pick_up_fork(pro, philo, left_fork);
		pick_up_fork(pro, philo, right_fork);
		eat(philo, pro);
		pthread_mutex_unlock(&pro->fork[left_fork]);
		pthread_mutex_unlock(&pro->fork[right_fork]);
		ft_time_sleep(philo, pro);
		philo->cont++;
		if(philo->cont == pro->how_many_meals && pro->how_many_meals != 0)
			break;
	}
	printf("%d lwa9t %zu\n", philo->philo_id,(get_current_time() - pro->start_time));
	return NULL;
}
void insert_to_philos(t_philo *philo)
{
	philo->time_to_die = philo->program->time_to_die;
	philo->time_to_eat = philo->program->time_to_eat;
	philo->time_to_sleep = philo->program->time_to_sleep;
}
void *ft_hadi(void *t)
{
	t_program *pro = (t_program *)t;
	t_philo *philo = pro->philos;
	while (1)
	{
		for (int i = 0; i < pro->num_philo ; i++)
		{
			pthread_mutex_lock(&pro->meal_lock);
			if ((get_current_time() - pro->start_time) - pro->philos[i].last_meal >= (size_t)pro->time_to_die)
			{
				printf("%zu %d died\n", (get_current_time() - pro->start_time), pro->philos[i].philo_id);
				pthread_mutex_lock(&pro->dead);
				pro->dead_flag = 1;
				pthread_mutex_unlock(&pro->dead);
				pthread_mutex_unlock(&pro->meal_lock);
				break;
			}
			pthread_mutex_unlock(&pro->meal_lock);
		}
		pthread_mutex_lock(&pro->dead);
		if(pro->dead_flag == 1)
		{
			pthread_mutex_unlock(&pro->dead);
			break;
		}
		pthread_mutex_unlock(&pro->dead);
	}
	return NULL;
}
int main(int argc, char const *argv[])
{
	
	t_program t;
	int i = -1;
	if(argc != 6 && argc != 5)
		return (1);
	t.num_philo = ft_atoi(argv[1]);
	t.time_to_die = ft_atoi(argv[2]);
	t.time_to_eat = ft_atoi(argv[3]);
	t.time_to_sleep = ft_atoi(argv[4]);
	if(argc == 6)
		t.how_many_meals =  ft_atoi(argv[5]);
	t.philos = malloc(sizeof(t_philo) * t.num_philo);
	t.fork = malloc(sizeof(pthread_mutex_t) * t.num_philo);
	t.start_time = (size_t)get_current_time();
	for (int x = 0; x < t.num_philo; x++) {
        pthread_mutex_init(&t.fork[x], NULL);
    }
	pthread_mutex_init(&t.meal_lock, NULL);
	pthread_mutex_init(&t.dead, NULL);
	while (++i < t.num_philo)
	{
		t.philos[i].philo_id = i + 1;
		t.philos[i].program = &t;
		t.philos[i].last_meal=0;
		t.dead_flag=0;
		insert_to_philos(&(t.philos[i]));
	}
	pthread_create(&t.prog, NULL, ft_hadi, &t);
	i = -1;
	while (++i < t.num_philo )
		pthread_create(&(t.philos[i].thread), NULL, ft_routing, &t.philos[i]);
	i = -1;
	while (++i < t.num_philo )
		pthread_join((t.philos[i].thread),NULL);
	pthread_join(t.prog,NULL);
	
	return (0);
}
