
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

struct semaphore emergency_right;
struct semaphore emergency_left; 
struct semaphore normal_right; 
struct semaphore normal_left; 

unsigned int num_emergency_right=0;
unsigned int num_emergency_left=0;
unsigned int num_normal_right=0;
unsigned int num_normal_left=0;

enum car_direction direction_on_bridge;

unsigned int count_on_bridge;

unsigned int count_block_emergency;

unsigned int Sum_all_cars()
{
	return num_emergency_right+num_emergency_left+num_normal_right+num_normal_left+count_on_bridge;
}


void Count_free_cars(enum car_priority prio, enum car_direction dir)
{
	num_normal_left+=!(prio || dir);
	num_emergency_right+=(prio && dir);
	num_normal_right+=(!prio && dir);
	num_emergency_left+=(prio && !dir);
}


// Called before test. Can initialize some synchronization objects.
void narrow_bridge_init(void)
{
	sema_init(&emergency_right, 0);
	sema_init(&emergency_left, 0);
	sema_init(&normal_right, 0);
	sema_init(&normal_left, 0);
}

void arrive_bridge(enum car_priority prio, enum car_direction dir)
{
	unsigned int sum_all_cars = Sum_all_cars();
	Count_free_cars(prio,dir);
	if (sum_all_cars==0)
	{
		num_normal_left-=!(prio || dir);
		num_emergency_right-=(prio && dir);
		num_normal_right-=(!prio && dir);
		num_emergency_left-=(prio && !dir);
		direction_on_bridge=dir;
		count_on_bridge++;
	}
	else if (sum_all_cars==1)
	{
		if (direction_on_bridge==dir)
		{
			num_normal_left-=!(prio || dir);
			num_emergency_right-=(prio && dir);
			num_normal_right-=(!prio && dir);
			num_emergency_left-=(prio && !dir);
			count_on_bridge++;
		}
		else
		{
			if (!(prio || dir)) sema_down(&normal_left);
			if ((prio && dir)) sema_down(&emergency_right);
			if (!prio && dir) sema_down(&normal_right);
			if (prio && !dir) sema_down(&emergency_left);
		
		}
	}
	else if (sum_all_cars>1)
	{
		if (count_on_bridge==1)
		{
			if (dir==0)
			{
				if (dir==direction_on_bridge && (num_emergency_right==0 || prio==1))
				{
					num_normal_left-=!(prio || dir);
					num_emergency_left-=(prio && !dir);
					count_on_bridge++;
				}
				else if (dir==direction_on_bridge && num_emergency_right>0 && prio==0) sema_down(&normal_left);
				else if (dir!=direction_on_bridge)
				{
					if (!(prio || dir)) sema_down(&normal_left);
					if (prio && !dir) sema_down(&emergency_left);
				}
			}
			if (dir==1)
			{
				if (dir==direction_on_bridge && (num_emergency_left==0 || prio==1))
				{
					num_emergency_right-=(prio && dir);
					num_normal_right-=(!prio && dir);
					count_on_bridge++;
				}
				else if (dir==direction_on_bridge && num_emergency_left>0 && prio==0) sema_down(&normal_right);
				else if (dir!=direction_on_bridge)
				{
					if (!(prio || dir)) sema_down(&normal_right);
					if (prio && !dir) sema_down(&emergency_right);
				}
			}
		}
		else
		{
			if (!(prio || dir)) sema_down(&normal_left);
			if ((prio && dir)) sema_down(&emergency_right);
			if (!prio && dir) sema_down(&normal_right);
			if (prio && !dir) sema_down(&emergency_left);
		}

	}

}

void exit_bridge(enum car_priority prio, enum car_direction dir)
{
	count_on_bridge--;
	if (count_on_bridge==1)
	{
		if (direction_on_bridge==0)
		{
			if (num_emergency_right==0)
			{
				if (num_emergency_left>0)
				{
					sema_up(&emergency_left);
					num_emergency_left--;
					count_on_bridge++;
					
				}
				else 
				{
					if (num_normal_left>0)
					{
						sema_up(&normal_left);
						num_normal_left--;
						count_on_bridge++;
					}

				}
				
			}
			else
			{
				if (num_emergency_left>0)
				{
					sema_up(&emergency_left);
					num_emergency_left--;
					count_on_bridge++;
				}
			}
		}
		if (direction_on_bridge==1)
		{
			if (num_emergency_left==0)
			{
				if (num_emergency_right>0)
				{
					sema_up(&emergency_right);
					num_emergency_right--;
					count_on_bridge++;
					
				}
				else 
				{
					if (num_normal_right>0)
					{
						sema_up(&normal_right);
						num_normal_right--;
						count_on_bridge++;
					}
				}
			}
			else
			{
				if (num_emergency_right>0)
				{
					sema_up(&emergency_right);
					num_emergency_right--;
					count_on_bridge++;
				}
			}
		}
	}
	if (count_on_bridge==0)
	{
		if (dir==0)
		{
			if (num_emergency_right>0) 
			{
				direction_on_bridge=!direction_on_bridge;
				sema_up(&emergency_right);
				num_emergency_right--;
				count_on_bridge++;
				if (num_emergency_right>0) 
				{
					sema_up(&emergency_right);
					num_emergency_right--;
					count_on_bridge++;
				}
				else if (num_emergency_left==0 && num_emergency_right==0 && num_normal_right>0)
				{
					sema_up(&normal_right);
					num_normal_right--;
					count_on_bridge++;
				}
			}
			else
			{
				if (num_emergency_left==0) 
				{
					if (num_normal_right>0)
					{
						direction_on_bridge=!direction_on_bridge;
						sema_up(&normal_right);
						num_normal_right--;
						count_on_bridge++;
						if (num_normal_right>0)
						{
							sema_up(&normal_right);
							num_normal_right--;
							count_on_bridge++;
						}
					}
				}
				else
				{
					sema_up(&emergency_left);
					num_emergency_left--;
					count_on_bridge++;
					if (num_emergency_left>0)
					{
						sema_up(&emergency_left);
						num_emergency_left--;
						count_on_bridge++;
					}
					else
					{
						if (num_normal_left>0)
						{
							sema_up(&normal_left);
							num_normal_left--;
							count_on_bridge++;
						}
					}
				}
			}
		}
		if (dir==1)  
		{
			if (num_emergency_left>0) 
			{
				direction_on_bridge=!direction_on_bridge;
				sema_up(&emergency_left);
				num_emergency_left--;
				count_on_bridge++;
				if (num_emergency_left>0) 
				{
					sema_up(&emergency_left);
					num_emergency_left--;
					count_on_bridge++;
				}
				else if (num_emergency_right==0 && num_emergency_left==0 && num_normal_left>0)
				{
					sema_up(&normal_left);
					num_normal_left--;
					count_on_bridge++;
				}
			}
			else
			{
				if (num_emergency_right==0) 
				{
					if (num_normal_left>0)
					{
						direction_on_bridge=!direction_on_bridge;
						sema_up(&normal_left);
						num_normal_left--;
						count_on_bridge++;
						if (num_normal_left>0)
						{
							sema_up(&normal_left);
							num_normal_left--;
							count_on_bridge++;
						}
					}
				}
				else
				{
					sema_up(&emergency_right);
					num_emergency_right--;
					count_on_bridge++;
					if (num_emergency_right>0)
					{
						sema_up(&emergency_right);
						num_emergency_right--;
						count_on_bridge++;
					}
					else
					{
						if (num_normal_right>0)
						{
							sema_up(&normal_right);
							num_normal_right--;
							count_on_bridge++;
						}
					}
				}
			}
		}
	}
}
