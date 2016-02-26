
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

static double toRad(double x) {
	return x * M_PI / 180.0;
}

static double haversine(const double lat1, 
						const double lon1, 
						const double lat2, 
						const double lon2) {
	double R = 6371000.0;
	double x1 = lat2 - lat1;
	double dLat = toRad(x1);
	double x2 = lon2 - lon1;
	double dLon = toRad(x2);
	double a = sin(dLat / 2) * sin(dLat / 2) +
		cos(toRad(lat1)) * cos(toRad(lat2)) *
		sin(dLon / 2) * sin(dLon / 2);
	double c = 2 * atan2(sqrt(a), sqrt(1.0 - a));
	double d = R * c;

	return d;
}