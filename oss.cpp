#include "shmSegment.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>

using namespace std;

ofstream myfile ("logfile");

struct msg {
	long mtype;
	int mtext;
};

void log (string out) {
	myfile << "OSS: " << out;
}

int initShmSegment (shm* &sharedMem) {

	key_t key = ftok("JamesSharedMemory", 8675309);
	int shmid = shmget(key, sizeof(sharedMem), 0666|IPC_CREAT);
	sharedMem = (shm*) shmat(shmid,0,0);

	sharedMem->clock[0] = 0;
	sharedMem->clock[1] = 0;

	return shmid;
}

void destroyShmSegment (int shmid, shm* sharedMem) {
	shmdt(sharedMem);
	shmctl(shmid, IPC_RMID, NULL);
} 

int initMsgQueue () {
	key_t key = ftok("JamesMessageQueue", 07221006);
	int msgid = msgget(key, 0666 | IPC_CREAT);
	
	return msgid;
}

void sendMsg (int text, long type, int msgid) {
	msg message;
	message.mtext = text;
	message.mtype = type;
	msgsnd (msgid, &message, sizeof(message), 0);

}

int rcvMsg (int msgid) {
	int inMsg;
	msg message;

	msgrcv(msgid, &message, sizeof(message), 1, 0);

	inMsg = message.mtext;
	return inMsg;
}

void destroyMsgQueue (int msgid) {
	msgctl(msgid, IPC_RMID, NULL);
}

void spawn (int simPid, int shmid, int msgid) {

	pid_t pid;

	pid = fork();
	
	if (pid == -1) {
		cout <<"error"<<endl;
	} else if (pid == 0) {

		stringstream a1, b1, c1;
		a1 << simPid;
		b1 << shmid;
		c1 << msgid;
		string a, b, c;
		a = a1.str();
		b = b1.str();
		c = c1.str();
		char * args[] = {"./child", (char*)a.c_str(), (char*)b.c_str(), (char*)c.c_str()};
		execvp(args[0], args);
		terminate();
	} else {
		//parent
	}
}

int main (int argc, char * argv []) {

	myfile << "Logfile:\n";
		
	shm* segment;
	
	int shmid;
	int msgid;

	shmid = initShmSegment (segment);

	initProcessTable(segment);
	msgid = initMsgQueue();

	int simPids [18][2];
	int timeElapsed;
	int index, min;

	for (int i = 0; i < 50; i) {
		min = -1;
		for (long o = 2; o < 20; o++) {
			if (segment->processTable[o-2][0] == -1) {

				spawn (o, shmid, msgid);
				
				stringstream iHateMakingIntsIntoStrings;
				iHateMakingIntsIntoStrings << o;
				string time = clockToString(segment);
				string out = "Generating process with PID " + iHateMakingIntsIntoStrings.str() + " at time " + time + "\n";
				log(out);i++;

				sendMsg (1, o, msgid);

				timeElapsed = rcvMsg(msgid);

				updateClock(timeElapsed, segment);
				if (segment->processTable[o-2][5] == 1) {
					segment->processTable[o-2][3] = 1;
					out = "Putting process with PID " + iHateMakingIntsIntoStrings.str() + " into queue 1\n";
					log(out);i++;
				} else {
					segment->processTable[o-2][3] = 0;
					out = "Putting process with PID " + iHateMakingIntsIntoStrings.str() + " into queue 0\n";
					log(out);i++;
				}
				if (segment->processTable[o-2][4] == 1) {	
					out = "Putting process with PID " + iHateMakingIntsIntoStrings.str() + " into blocked queue\n";
					log(out);i++;
				}

				stringstream whyDoIDoThis;
				whyDoIDoThis << timeElapsed;
				out = "Recieving that process with PID " + iHateMakingIntsIntoStrings.str() + " ran for " + whyDoIDoThis.str() + " nanoseconds\n";
				log(out);i++;

				
				break;
			}
		}
		min = -1;
		for (int x = 0; x < 18; x++) {
			if (segment->processTable[x][0] != -1 && segment->processTable[x][3] == 0) {
				if (min == -1 || segment->processTable[x][1] < min) {
					index = x;
					min = segment->processTable[x][1];
				} 
			}
		}
		if (min == -1) {
			for (int y = 0; y < 18; y++) {
				if (segment->processTable[y][0] != -1 && segment->processTable[y][3] == 1) {
					if (min == -1 || segment->processTable[y][1] < min) {
						index = y;
						min = segment->processTable[y][1];
					} 
				}
			}
		}
		if (min == -1) {	
			for (int z = 0; z < 18; z++) {
				if (segment->processTable[z][0] != -1 && segment->processTable[z][4] == 0) {
					if (min == -1 || segment->processTable[z][1] < min) {
						index = z;
						min = segment->processTable[z][1];
					} 
				}
			}
		}
		if (min != -1) {
			spawn (index + 2, shmid, msgid);
			
			stringstream iHateMakingIntsIntoStrings;
			iHateMakingIntsIntoStrings << (index + 2);
			string time = clockToString(segment);
			string out = "Dispatching process with PID " + iHateMakingIntsIntoStrings.str() + " at time " + time + "\n";
			log(out);i++;

			sendMsg (1, index + 2, msgid);

			timeElapsed = rcvMsg(msgid);

			updateClock(timeElapsed, segment);
	
			stringstream whyDoIDoThis;
			whyDoIDoThis << timeElapsed;
			out = "Recieving that process with PID " + iHateMakingIntsIntoStrings.str() + " ran for " + whyDoIDoThis.str() + " nanoseconds\n";
			log(out);i++;
		}
	}
	myfile.close();
	
	destroyShmSegment (shmid, segment);
	destroyMsgQueue (msgid);

	return 0;
}
