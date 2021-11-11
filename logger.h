#ifndef LOGGER_H
#define LOGGER_H
#include <fstream>
#include <string>

void log (string out) {
	ofstream myfile ("logfile");
	myfile << "OSS: " << string << endl;
	myfile.close();
}

#endif
