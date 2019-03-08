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

#define SENSOR 0
#define BUTTON 62
#define BUFF_SIZE 100

sig_atomic_t volatile run_flag = 1;
int period = 1; //time before each measurement in seconds
char temp_type = 'f'; //celcius or farieheit

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

//loops and takes in stdin
void* pthreader()
{
	int fd = 0;
	char buff[BUFF_SIZE]; //for reading in stdin
	int ret = BUFFSIZE; //return value for read()

	while(run_flag)
	{
		//wait for things to come in from stdin
		poll(&fd, 1, -1);	

		//now handle the events
		while(ret == BUFF_SIZE)
		{
			ret = read(fd, buff, BUFF_SIZE);	
			if(ret == 0)
			{
				fprintf(stderr, "read failed\n");
				exit(1);
			}

			write(1, buff, BUFFSIZE);
		}
	}
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

	while(run_flag)
	{
		sensor_value = mraa_aio_read(temp_sensor);
		print_temp(sensor_value);
		sleep(period);
	}

	mraa_aio_close(temp_sensor);
	mraa_gpio_close(button);
	return 0;
}
