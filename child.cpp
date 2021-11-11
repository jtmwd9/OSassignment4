#include "shmSegment.h"
#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

using namespace std;

struct msg {
	long mtype;
	int mtext;
};

void initShmSegment (shm* &sharedMem) {
	key_t key = ftok("JamesSharedMemory", 8675309);
	int shmid = shmget(key, sizeof(sharedMem), 0666);
	sharedMem = (shm*) shmat(shmid,0,0);
} 

void sendMsg (int text, long type, int msgid) {

	msg message;
	message.mtext = text;
	message.mtype = type;
	msgsnd (msgid, &message, sizeof(message), 0);

}

int rcvMsg (int msgid, long pid) {
	int inMsg;
	msg message;
	msgrcv(msgid, &message, sizeof(message), pid, 0);
	inMsg = message.mtext;
	return inMsg;
}

int isBlocked () {
	int b = rand() % 100 + 1;

	if (b >= 10) {
		return 1;
	} else {
		return 0;
	}
}

int timeSpent () {

	int t = rand() % 3000 + 2000;

	return t;
}

int isCpuOrIo () {

	srand (time(NULL));
	int c = rand() % 100 + 1;

	if (c <= 35){
		c = 1;
	} else {
		c = 0;
	}
	return c;
}

int main (int argc, char* argv []) {

	srand (time(NULL));rand();rand();rand();rand();rand();
	shm* segment;
	long pid;
	int shmid, msgid;
	pid = atol(argv[1]);
	shmid = atoi(argv[2]);
	msgid = atoi(argv[3]);
	initShmSegment (segment);

	int mess = rcvMsg(msgid, pid);

	segment->processTable[pid-2][0] = pid;

	segment->processTable[pid-2][5] = isCpuOrIo();


	int b = isBlocked();

	segment->processTable[pid][4] = b;

	int timer = timeSpent();

	segment->processTable[pid-2][1] = timer;
	segment->processTable[pid-2][2] += timer;

	sendMsg (timer, 1, msgid);

	return (0);
}
