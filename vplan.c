#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define axis 2
#define joints 2

struct vec{
	double axis_pos[axis];
	double joint_pos[joints];
	double max_turn_speed;
	double axis_speed;
	double joint_speed;
	double axis_max_speed;
	double joint_max_speed;
};

struct path{
	struct vec pos;
	struct path* next;
	struct path* prev;
};

struct max_joint_acc{
	double acc[joints];
};

void printvec(struct vec A){
	printf("(");
	for(int i = 0; i < joints; i++){
		printf("%f/", A.joint_pos[i]);
	}
	printf("\b)");
}

double v_max(struct vec A, struct vec B, struct vec C, struct max_joint_acc mja){
	struct vec AB;
	struct vec BC;
	double sum_AB = 0;
	double sum_BC = 0;
	double vmax = -1;

	for(int i = 0; i < joints; i++){
		AB.joint_pos[i] = B.joint_pos[i] - A.joint_pos[i];
		BC.joint_pos[i] = C.joint_pos[i] - B.joint_pos[i];
		sum_AB += pow(AB.joint_pos[i], 2);
		sum_BC += pow(BC.joint_pos[i], 2);
	}

	printvec(AB);
	printf(" -> ");
	printvec(BC);
	printf("\n");


	if(sum_AB == 0 || sum_BC == 0){
		return(0);
	}

	for(int i = 0; i < joints; i++){
		AB.joint_pos[i] /= sqrt(sum_AB);
		BC.joint_pos[i] /= sqrt(sum_BC);
	}

	for(int i = 0; i < joints; i++){
		if(vmax == -1  || fabs(BC.joint_pos[i] - AB.joint_pos[i]) != 0 && vmax > (mja.acc[i] / fabs(BC.joint_pos[i] - AB.joint_pos[i]))){
			vmax = mja.acc[i] / fabs(BC.joint_pos[i] - AB.joint_pos[i]);
		}
	}

	return(vmax);
}

int init(struct path *AB, struct max_joint_acc mja){
	struct path *ab = AB;
	while(ab){
		if(ab->next && ab->next->next){
			ab->pos.max_turn_speed = v_max(ab->pos, ab->next->pos, ab->next->next->pos, mja);
		}
		else{
			ab->pos.max_turn_speed = 0;
		}
		ab = ab->next;
	}

	return(0);
}

int vplan(struct path *AB){

	return(0);
}

int main(){
	struct max_joint_acc mja;
	mja.acc[0] = 0.1;
	mja.acc[1] = 0.1;

	struct vec A;
	A.joint_pos[0] = 1;
	A.joint_pos[1] = 1;

	struct vec B;
	B.joint_pos[0] = 2;
	B.joint_pos[1] = 2;

	struct vec C;
	C.joint_pos[0] = 3;
	C.joint_pos[1] = 2;

	printf("(%f/%f) -> (%f/%f) -> (%f/%f) vmax <= %f", A.joint_pos[0], A.joint_pos[1], B.joint_pos[0], B.joint_pos[1], C.joint_pos[0], C.joint_pos[1], v_max(A, B, C, mja));

	return(0);
}



//todo kinematik + tool + vise offset
