#ifndef MSGQ_H
#define MSGQ_H

struct msg {
	long mtype;
	int mtext [6];
};

#endif
