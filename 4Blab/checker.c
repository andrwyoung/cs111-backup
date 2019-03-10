#include <stdio.h>
#include <mraa.h>

#define SENSOR 0

int main(int argc, char* argv[])
{
	mraa_init();

	uint16_t sensor_value;
	mraa_aio_context temp_sensor;

	temp_sensor = mraa_aio_init(SENSOR);

	sensor_value = mraa_aio_read(temp_sensor);
	if(sensor_value == 0)
	{
		fprintf(stderr, "Sensor not working\n");
		return 1;
	}
	return 0;
}

