
#include <math.h>

#ifndef SOILSENSOR_H_
#define SOILSENSOR_H_


#define AVG_MESUREMENTS  10
#define MAX_SENSORS 5
#define CH_S1       3
#define CH_S2       6
#define CH_S3       7
#define CH_S4       1
#define CH_S5       2





void initSoilSensor(void);
double SoilSensorGetSoilHumidity(void);
double average(double a[]);
double SoilreadRaw(void);





#endif /* SOILSENSOR_H_ */