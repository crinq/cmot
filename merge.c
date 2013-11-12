#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define axis 2
#define joints 2

struct vec{
	double axis_pos[axis];
	double joint_pos[joints];
};

struct path{
	struct vec pos;
	struct path* next;
	struct path* prev;
};