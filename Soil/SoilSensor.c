
#include "SoilSensor.h"
#include "adc.h"



void initSoilSensor(void){
	initAdc();
	
}

double SoilreadRaw(void){
	double sensor=0;
	double temp[AVG_MESUREMENTS];

		for(int j=0;j<AVG_MESUREMENTS;j++)
		temp[j]=readAdc();  //Take AVG_MESUREMENTS mesurements
		
		sensor=average(temp);         //Average for each sensor
	
	return sensor;	
}

double SoilSensorGetSoilHumidity(void){
	uint8_t humidity = 0;
	double sensor = SoilreadRaw();
	double val = (sensor /1024 )*100;
	
	humidity = (uint8_t) val;
	printf("val sensor: %d \n", humidity);
	
	return humidity;
}


double average(double a[]){
	int i;
	double avg, sum=0.0;
	
	for(i=0;i<AVG_MESUREMENTS;i++){
		sum+=a[i];
	}
	avg =(sum/AVG_MESUREMENTS);
	return avg;
}

