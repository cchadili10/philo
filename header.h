/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchadili <hchadili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/24 19:39:18 by hchadili          #+#    #+#             */
/*   Updated: 2024/08/26 15:13:34 by hchadili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
# define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

typedef struct s_program t_program;
typedef struct s_philo t_philo;


struct s_philo
{
	pthread_t		thread;
	int		philo_id;
	int		num_forks;
	int		time_to_die;
	int		time_to_eat;
	int		time_to_sleep;
	int		how_many_meals;
	int		cont;
	size_t last_meal;
	
	pthread_mutex_t	*r_fork;
	pthread_mutex_t	*l_fork;
	pthread_mutex_t	*write_lock;
	pthread_mutex_t	*dead_lock;
	pthread_mutex_t	*meal_lock;
	struct s_program *program;
	
};

struct s_program
{
	pthread_t		prog;
	pthread_mutex_t	*fork;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	dead;
	int				dead_flag;
	size_t		start_time;
	int		how_many_meals;
	int		num_philo;
	int		time_to_die;
	int		time_to_eat;
	int		time_to_sleep;
	struct s_philo	*philos;
};
#endif

// -lpthread
// pthread_mutex_t	dead_lock;
// pthread_mutex_t	meal_lock;
// pthread_mutex_t	write_lock;