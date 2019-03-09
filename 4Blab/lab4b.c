//NAME: Andrew Yong
//EMAIL: yong.andrew11@gmail.com
//ID: 604905807
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include <mraa.h>
#include <signal.h>
#include <math.h> //for log
#include <time.h> //for localtime
#include <poll.h>
#include <pthread.h>

#define SENSOR 0
#define BUTTON 60
#define BUFF_SIZE 100

sig_atomic_t volatile run_flag = 1;
int period = 1; //time before each measurement in seconds
char temp_type = 'f'; //celcius or farieheit
pthread_mutex_t mutexd;

void handler()
{
	//get time
	char time_buff[80];
	long curr_time = time(NULL);
	struct tm ts = *localtime(&curr_time);
	strftime(time_buff, sizeof(time_buff), "%H:%M:%S", &ts);

	write(1, time_buff, 8);
	write(1, " SHUTDOWN\n", 10);

	//doesn't need to be atomic since all threads are interruped
	run_flag = 0;
}

int stoi(char* string)
{
	unsigned int i;
	for(i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
		{
			fprintf(stderr, "not a number\n");
			return -1;
		}
	}
	return atoi(string);
}

int chin(char file[])
{
	int fd = open(file, O_WRONLY);
	if(fd >= 0)
	{
		close(1);
		dup(fd);
		close(fd);
		return 0;

	}
	else
	{
		fprintf(stderr,  "failed to open %s\n", file); 
		return 1;

	}
}

void print_temp(uint16_t value)
{
	int B = 4275; //B value for thermistor?
	int R0 = 100000;

	//convert value to temperature
	float R = 1023.0/((float) value) - 1.0;
	R *= R0;

	float temp = 1.0/(log(R/R0)/B+1/298.15)-273.15;
	if(temp_type == 'f') temp = (temp * 9 / 5) + 32;

	//get time
	char time_buff[80];
	long curr_time = time(NULL);
	struct tm ts = *localtime(&curr_time);
	strftime(time_buff, sizeof(time_buff), "%H:%M:%S", &ts);

	dprintf(1, "%s %0.1f\n", time_buff, temp);
}

//loops and takes in stdin
void* pthreader()
{
	struct pollfd fds[1];
	fds[1].fd = 0;
	fds[1].events = POLLIN;
	char buff[BUFF_SIZE]; //for reading in stdin
	int ret = BUFF_SIZE; //return value for read()
	
	char* command = malloc(sizeof(char));
	int count;

	while(run_flag)
	{
		//wait for things to come in from stdin
		poll(fds, 1, -1);	

		//read in the stuff
		do
		{
			ret = read(0, buff, BUFF_SIZE);	
			if(ret < 0)
			{
				fprintf(stderr, "read failed\n");
				exit(1);
			}

			fprintf(stderr, "got %d bytes!\n", ret);
		} while(ret == BUFF_SIZE);

		//now handle the events
		//carefully to prevent race conditions

		if(strncmp("SCALE=", buff, 6) == 0 && ret == 8)
		{
			fprintf(stderr, "Scale\n");
		}
		else if(strncmp("PERIOD=", buff, 7) == 0)
		{
			//check for 
			fprintf(stderr, "Period\n");
		}
		else if(strncmp("STOP", buff, 4) == 0 && ret == 5)
		{
			fprintf(stderr, "STOPping\n");
		}
		else if(strncmp("START", buff, 5) == 0 && ret == 6)
		{
			fprintf(stderr, "Starting\n");
		}
		else if(strncmp("LOG ", buff, 4) == 0)
		{
			fprintf(stderr, "logging\n");
		}
		else if(strncmp("OFF", buff, 3) == 0 && ret == 4)
		{
			fprintf(stderr, "Offing...\n");
		}
		//fprintf(stderr, "hey\n");
	}
	
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) 
{
	int c; //return value of that option
	int option_index = 0;
	static struct option long_options[] = 
	{
		{"scale",	required_argument,	0, 'k'},
		{"period",	required_argument,	0, 'p'},
		{"log",		required_argument,	0, 'l'},
		{0, 0, 0, 0,}
	};
	
	while(1)
	{
		c = getopt_long(argc, argv, "", long_options, &option_index); 
		if(c == -1)
			break;

		switch(c)
		{
			case 'k':
				switch(optarg[0])
				{
					case 'F':
						temp_type = 'f';
						break;
					case 'C':
						temp_type = 'c';
						break;
					default:
						fprintf(stderr, "SCALE: invalid argument (F or C)\n");
						exit(1);
				}
				break;
			case 'p':
				{
					int num = stoi(optarg);
					if (num <= 0)
					{
						fprintf(stderr, "PEROID: argument must be number\n");
						exit(1);
					}

					period = num;
				}
				break;
			case 'l':
				fprintf(stderr, "logging? do a funtion?\n");
				chin(optarg);
				break;
			case '?':
				fprintf(stderr, "ERROR: invalid argument\n");
				exit(1);
			default:
				fprintf(stderr, "FATAL\n");
			//mark
		}
	}

	//initializing all components
	signal(SIGINT, handler);
	mraa_init();

	uint16_t sensor_value;
	mraa_aio_context temp_sensor;

	temp_sensor = mraa_aio_init(SENSOR);

	mraa_gpio_context button;
	button = mraa_gpio_init(BUTTON);
	mraa_gpio_dir(button, MRAA_GPIO_IN);
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &handler, NULL);

	//pthreading
	pthread_t id;
	if(pthread_create(&id, NULL, &pthreader, NULL) != 0)
	{
		fprintf(stderr, "failed to create thread\n");
		exit(1);
	}

	while(run_flag)
	{
		sensor_value = mraa_aio_read(temp_sensor);
		print_temp(sensor_value);
		sleep(period);
	}

	//join thread
	pthread_cancel(id);
	
	mraa_aio_close(temp_sensor);
	mraa_gpio_close(button)