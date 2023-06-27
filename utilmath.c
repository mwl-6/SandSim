#include "sandsim.h"

int randRange(int range){
	return (rand() % (range + 1));
}
double dist(double x1, double y1, double x2, double y2){
	return sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1));
}