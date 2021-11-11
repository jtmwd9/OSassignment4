#ifndef SHMSEGMENT_H
#define SHMSEGMENT_H
#include <sstream>
#include <string>
#include <iostream>
using namespace std;
struct shm {
	int clock [2];
	int processTable [18][6]; //PID, CPUtime, systemTime, priority, isBlocked, cpu/io
};

void initProcessTable (shm* segment) {
	for (int i = 0; i < 18; i ++) {
		segment->processTable [i][0] = -1;
	}
}

void updateClock (int nanosec, shm* segment) {
	segment->clock[1] += nanosec;
	if (segment->clock[1] >= 1000000000){
		segment->clock[0] += 1;
		segment->clock[1] -= 1000000000;
	}
}

string clockToString (shm* segment) {
	stringstream sec, nan;
	sec << segment->clock[0];
	nan << segment->clock[1];
	string out = sec.str() + ":" + nan.str();
	return out;	
}

void addProcess (int * process, shm* segment) {
	for (int i = 0; i < 18; i++) {
		if (segment->processTable[i][0] == -1) {
			for (int o = 0; o < 6; o++) {
				segment->processTable[i][o] = process[o];
			}
			break;
		}
	}
}

#endif
