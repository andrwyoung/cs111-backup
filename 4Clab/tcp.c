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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SENSOR 0
#define BUTTON 60
#define BUFF_SIZE 1000

sig_atomic_t volatile run_flag = 1;
int period = 1; //time before each measurement in seconds
int temp_type = 1; //0 = celcius or 1 = farieheit
pthread_mutex_t mutexd;
int logfile = -1; //fd for logfile if it exists
int netfd = -1;

int leave = 0;

int stoi(char* string)
{
	unsigned int i;
	for(i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
		{
			return -1;
		}
	}
	return atoi(string);
}

int id_checker(char* string)
{
	if(strlen(string) != 9)
		return 1;
	unsigned int i;
	for(i = 0; i < strlen(string); i++)
	{
		if(!isdigit(string[i]))
		{
			return 1;
		}
	}

	return 0;
}

void print_temp(uint16_t value)
{
	int B = 4275; //B value for thermistor?
	int R0 = 100000;

	//convert value to temperature
	float R = 1023.0/((float) value) - 1.0;
	R *= R0;

	float temp = 1.0/(log(R/R0)/B+1/298.15)-273.15;
	if(temp_type) temp = (temp * 9 / 5) + 32;

	//get time
	char time_buff[80];
	long curr_time = time(NULL);
	struct tm ts = *localtime(&curr_time);
	strftime(time_buff, sizeof(time_buff), "%H:%M:%S", &ts);

	dprintf(logfile, "%s %0.1f\n", time_buff, temp);
	dprintf(netfd , "%s %0.1f\n", time_buff, temp);
}

void process_command(char command[], int count)
{
	if(strncmp("SCALE=", command, 6) == 0)
	{
		switch(command[6])
		{
			case 'C':
				temp_type = 0;
				break;
			case 'F':
				temp_type = 1;
				break;
			default:
				fprintf(stderr, "ERROR: bad scale option\n");
		}
	}
	else if(strncmp("PERIOD=", command, 7) == 0)
	{
		//checking correct number 
		int num = atoi(command + 7);
		if(num > 0)
		{
			period = num;
		}
	}
	else if(strncmp("STOP", command, count) == 0)
	{
		pthread_mutex_lock(&mutexd);	
	}
	else if(strncmp("START", command, count) == 0)
	{
		pthread_mutex_unlock(&mutexd);
	}
	else if(strncmp("LOG ", command, 4) == 0)
	{
	}
	else if(strncmp("OFF", command, count) == 0)
	{
		//get time
		char time_buff[80];
		long curr_time = time(NULL);
		struct tm ts = *localtime(&curr_time);
		strftime(time_buff, sizeof(time_buff), "%H:%M:%S", &ts);

		dprintf(netfd , "%s SHUTDOWN\n", time_buff);
		dprintf(logfile , "%s SHUTDOWN\n", time_buff);
		dprintf(logfile , "%s\n", command);

		run_flag = 0;
		pthread_mutex_unlock(&mutexd);
		return;
	}
	else
	{
		fprintf(stderr, "ERROR: invalid argument\n");
		return;
	}

	dprintf(logfile , "%s\n", command);
}

//loops and takes in stdin
void* pthreader()
{
	char buff[BUFF_SIZE]; //for reading in stdin
	int ret = BUFF_SIZE; //return value for read()

	//assumes command won't be larger than buff_size
	char* command = malloc(sizeof(char) * BUFF_SIZE);
	int count = 0; //current length of command buffer
	
	while(run_flag)
	{
		//read in the stuff
		do
		{
			ret = read(netfd, buff, BUFF_SIZE);	
			if(ret < 0)
			{
				fprintf(stderr, "ERROR: read failed\n");
				exit(1);
			}

			//fprintf(stderr, "got %d bytes!\n", ret);
		} while(ret == BUFF_SIZE);
		
		//now process all those commands
		int i = 0;
		while(i < ret)
		{
			if(buff[i] == '\n')
			{
				process_command(command, count);
				count = 0;
			}
			else
			{
				command[count] = buff[i];
				count++;
			}
			i++;
		}

		bzero(buff, BUFF_SIZE);
	}
	
	free(command);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) 
{
	int mandatory = 7; //mandatory options
	int port = -2; //initializing mandatory port number
	struct hostent* hostname;
	char* id_num;


	int c; //return value of that option
	int option_index = 0;
	static struct option long_options[] = 
	{
		{"scale",	required_argument,	0, 'k'},
		{"period",	required_argument,	0, 'p'},
		{"log",		required_argument,	0, 'l'},
		{"host",	required_argument,	0, 'h'},
		{"id",		required_argument,	0, 'i'},
		{0, 0, 0, 0,}
	};
	
	while(optind < argc)
	{
		//fprintf(stderr, "argv[%d]: %s\n", optind, argv[optind]);

		if(argv[optind][0] == '-' && argv[optind][1] == '-' && 
			(c = getopt_long(argc, argv, "", long_options, &option_index)) != -1 ) { 

			//fprintf(stderr, "processing c: %d\n", c);

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
					logfile = open(optarg, O_WRONLY | O_CREAT | O_APPEND, 0644);
					if(logfile < 0) {
						fprintf(stderr, "LOG: failed to open logfile\n");
						exit(1);
					}
					mandatory ^= 4;
					break;
				case 'h':
					hostname = gethostbyname(optarg);
					if(hostname == NULL) {
						fprintf(stderr, "HOST: failed to get hostbyname\n");
						exit(1);
					}
					//fprintf(stderr, "hostname: %d\n", hostname->h_length);
					mandatory ^= 2;
					break;
				case 'i':
					id_num = optarg;
					if(id_checker(optarg) != 0)
					{
						fprintf(stderr, "ID: invalid 9 digit id\n");
						exit(1);
					}
					mandatory ^= 1;
					break;
				case '?':
					fprintf(stderr, "ERROR: invalid argument\n");
					exit(1);
				default:
					fprintf(stderr, "FATAL\n");
				//mark
			}
		}

		else {
			//fprintf(stderr, "hey\n");

			if(port == -2) {
				port = stoi(argv[optind]);
				if(port < 0) {
					fprintf(stderr, "ERROR: port must be number\n");
					exit(1);
				}

			} else {
				fprintf(stderr, "ERROR: port number already initialized\n");
				exit(1);
			}

			optind++;
		}
	}

	//checking if port was initialized
	if(port == -2) {
		fprintf(stderr, "ERROR: port number must be included as argument (non-opiont)\n");
		exit(1);
	}

	//checking argument validaty
	if(mandatory != 0) {
		fprintf(stderr, "ERROR: --log, --host, --id are mandatory options\n");
		exit(1);
	}

	//initializing all components
	mraa_init();
	uint16_t sensor_value;
	mraa_aio_context temp_sensor;
	temp_sensor = mraa_aio_init(SENSOR);

	pthread_mutex_init(&mutexd, NULL);



	//opening network stuff
	netfd = socket(AF_INET, SOCK_STREAM, 0);
	if(netfd < 1) {
		fprintf(stderr, "ERROR: failed to initiate socket\n");
		exit(1);
	}

	//setting up connection variables
	struct sockaddr_in address;
	bzero((char*) &address, sizeof(address));

	address.sin_family = AF_INET;
	bcopy((char*)hostname->h_addr, (char*)&address.sin_addr.s_addr, hostname->h_length);
	address.sin_port = htons(port);

	//address
	if(connect(netfd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		fprintf(stderr, "ERROR: couldn't connect\n");
		exit(1);
	}
	//initiate
	dprintf(netfd , "ID=%s\n", id_num);
	dprintf(logfile , "ID=%s\n", id_num);



	//pthreading
	pthread_t id;
	if(pthread_create(&id, NULL, &pthreader, NULL) != 0)
	{
		fprintf(stderr, "ERROR: failed to create thread\n");
		exit(1);
	}


	//actually running now
	while(run_flag)
	{
		sensor_value = mraa_aio_read(temp_sensor);
		print_temp(sensor_value);
		sleep(period);

		pthread_mutex_lock(&mutexd);
		pthread_mutex_unlock(&mutexd);
	}

	//closing and freeing everything
	pthread_cancel(id);
	mraa_aio_close(temp_sensor);
	close(netfd);
	close(logfile);
	return 0;
}
