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
};

struct max_joint_steps{
	double step[joints];	
};

struct min_axis_steps{
	double step[axis];	
};

struct vec split(struct vec A, struct vec B){
	struct vec C;

	for(int i = 0; i < axis; i++){
		C.axis_pos[i] = (A.axis_pos[i] + B.axis_pos[i]) / 2;
	}

	for(int i = 0; i < joints; i++){
		C.joint_pos[i] = (A.joint_pos[i] + B.joint_pos[i]) / 2;
	}
	
	return(C);
}

struct vec fkin(struct vec A){ // forward kin axis -> joints
	struct vec a = A;
	
	// wire kin:
	// A     B
	//  \   /
	//   \ /
	//    C
	//
	// A = (0/0)
	// B = (10/0) 
	// AC = joint[0]
	// BC = joint[0]
	
	a.joint_pos[0] = sqrt(pow(A.axis_pos[0], 2) + pow(A.axis_pos[1], 2)); // AC = sqrt(x^2+y^2)
	a.joint_pos[1] = sqrt(pow(10 - A.axis_pos[0], 2) + pow(A.axis_pos[1], 2)); // BC = sqrt((10-x)^2+y^2)
	
	
	// floppy kin
	// rot disk = joint[0]
	// lin head = joint[1] (radius)

	a.joint_pos[0] = atan2(A.axis_pos[1], A.axis_pos[0]);
	a.joint_pos[1] = sqrt(pow(A.axis_pos[0], 2) + pow(A.axis_pos[1], 2));
	
	/*for(int i = 0; i < joints; i++){
		a.joint_pos[i] = A.axis_pos[i];
	}*/
	/*a.joint_pos[0] = A.axis_pos[0];
	a.joint_pos[1] = A.axis_pos[1];
	a.joint_pos[2] = A.axis_pos[2];
	a.joint_pos[3] = A.axis_pos[3];
	a.joint_pos[4] = A.axis_pos[4];
	a.joint_pos[5] = A.axis_pos[5];*/
	
	return(a);	
}

struct vec ikin(struct vec a){ // inverse kin joints -> axis
	struct vec A = a;

	// wire kin:
	// A     B
	//  \   /
	//   \ /
	//    C
	//
	// A = (0/0)
	// B = (10/0) 
	// AC = joint[0]
	// BC = joint[0]

	A.axis_pos[0] = (pow(a.joint_pos[0], 2) - pow(a.joint_pos[1], 2) + pow(10, 2)) / (2 * 10);
	A.axis_pos[1] = sqrt(pow(a.joint_pos[0], 2) - pow(A.axis_pos[0], 2));


	// floppy kin
	// rot disk = joint[0]
	// lin head = joint[1] (radius)
	
	A.axis_pos[0] = sqrt(pow(a.joint_pos[1], 2) / ( 1+ pow(tan(a.joint_pos[0]), 2)));
	A.axis_pos[1] = tan(a.joint_pos[0]) * A.axis_pos[0];

	/*for(int i = 0; i < axis; i++){
		A.axis_pos[i] = a.joint_pos[i];
	}*/

	/*A.axis_pos[0] = a.joint_pos[0];
	A.axis_pos[1] = a.joint_pos[1];
	A.axis_pos[2] = a.joint_pos[2];
	A.axis_pos[3] = a.joint_pos[3];
	A.axis_pos[4] = a.joint_pos[4];
	A.axis_pos[5] = a.joint_pos[5];*/
	
	return(a);	
}

void insert(struct path* A, struct vec B){
	struct path* tmp = A->next;
	A->next = (struct path *)malloc(sizeof(struct path));
	A->next->pos = B;
	A->next->next = tmp;
}

int check_joint_steps(struct vec A, struct vec B, struct max_joint_steps max_j_s){
	for(int i = 0; i < joints; i++){
		if(fabs(A.joint_pos[i] - B.joint_pos[i]) > max_j_s.step[i]){
			return(1);
		}
	}
	return(0);
}

int check_axis_steps(struct vec A, struct vec B, struct min_axis_steps min_a_s){
	for(int i = 0; i < axis; i++){
		if(fabs(A.axis_pos[i] - B.axis_pos[i]) > min_a_s.step[i]){
			return(1);
		}
	}
	return(0);
}

int intp(struct path* A, struct max_joint_steps max_j_s, struct min_axis_steps min_a_s){
	struct path* p = A;
	
	if(!A){
		return(0);
	}

	p->pos = fkin(p->pos);
	while(p->next){
		p->next->pos = fkin(p->next->pos);
		p = p->next;
	}
	
	p = A;
	
	while(p->next){

		if(check_joint_steps(p->pos, p->next->pos, max_j_s)){ // must split
			insert(p, split(p->pos, p->next->pos)); // split
			if(check_axis_steps(p->pos, p->next->pos, min_a_s)){ // take new axis
				printf("split axis\n");
				p->next->pos = fkin(p->next->pos); // axis -> joints
			}
			else{ // take new joints pos
				printf("split joints\n");
				p->next->pos = ikin(p->next->pos); // joints -> axis
			}
		}
		else{
			printf("step\n");
			p = p->next;
		}
	}
	
	return(0);
}

int main(){
	FILE *file;
	file = fopen("intp.txt","w+");
	
	struct max_joint_steps max_j_s; // min axis res
	max_j_s.step[0] = 0.5 / 180 * 3.141526; // 0.1 deg res -> rad
	max_j_s.step[1] = 0.1; // 0.1 mm res
	
	struct min_axis_steps min_a_s; // min joint res
	min_a_s.step[0] = 0.1; // 0.1 mm res
	min_a_s.step[1] = 0.1; // 0.1 mm res
		
	struct vec A;
	A.axis_pos[0] = 1;
	A.axis_pos[1] = 1;
	
	struct vec B;
	B.axis_pos[0] = -1;
	B.axis_pos[1] = -1;

	struct vec C;
	C.axis_pos[0] = 0;
	C.axis_pos[1] = 1;

	struct vec D;
	D.axis_pos[0] = 0;
	D.axis_pos[1] = -1;
				
	struct path* AB = (struct path *)malloc(sizeof(struct path));
	AB->pos = A;
	AB->next = 0;
	insert(AB, D);
	insert(AB, C);
	insert(AB, B);

	
	intp(AB, max_j_s, min_a_s);
	
	while(AB){
		printf("Axis Pos (%f/%f)", AB->pos.axis_pos[0], AB->pos.axis_pos[1]);
		printf(" -> ");
		printf("joint Pos (%f/%f)", AB->pos.joint_pos[0], AB->pos.joint_pos[1]);
		printf("\n");
		
		fprintf(file, "%f %f %f %f\n", AB->pos.axis_pos[0], AB->pos.axis_pos[1], AB->pos.joint_pos[0], AB->pos.joint_pos[1]);
		// plot with: gnuplot -e 'set multiplot layout 6,1; plot "intp.txt" using 0:1 title "axis[0]";plot "intp.txt" using 0:2 title "axis[1]";plot "intp.txt" using 0:3 title "joint[0]";plot "intp.txt" using 0:4 title "joint[1]";plot "intp.txt" using 1:2 title "axis"; plot "intp.txt" using 3:4 title "joints"' -p
		AB = AB->next;
	}
	
	fclose(file);
	return(0);	
}

