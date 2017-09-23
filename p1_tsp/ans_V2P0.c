#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define printDegree
//#define printConnect
//#define printFile
//#define printDist
//#define testGetRoot

typedef struct s_node {
	int id, x, y;
	struct s_node *next;
	int start;
} node;

typedef struct s_dist {
	int idl, idr;
	double dist;
} dist;

int cnm(int n, int m) {return m == 0 ? 1 : cnm(n - 1, m - 1) * n / m;}
double distance(node a, node b) {return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));}
node *getRoot(node *nPtr, dist *distSet, node *nodeSet);
void connectNode(node *a, node *b);
void inverseAdd(node *a);
void bubbleSort_dist(dist *d, int n);
node *inverse(node *start);


int main(int argc, char *argv[]) {
	/*define variables*/
	// argv[1]="test4.i";
	// argv[2]="test4.o";

	int r1, r2, root;
	int i, j;
	int num_city, city_i = 0, num_dist, dist_i = 0, num_edge = 0;
	int *degree, *ans;
	double total_dist = 0;
	node *nodeSet, *nPtr;
	dist *distSet;
	FILE *fptr;
	fptr = fopen(argv[1], "r");
	if (!fptr) {
		printf("ERROR!!\n");
		exit(1);
	}

	/*read file*/
	fscanf(fptr, "%d", &num_city);
	nodeSet = (node *)malloc(sizeof(node) * num_city);
	degree = (int *)malloc(sizeof(int) * num_city);
	ans = (int *)malloc(sizeof(int) * num_city);
	while (fscanf(fptr, "%d %d %d", &(nodeSet[city_i].id), &(nodeSet[city_i].x), &(nodeSet[city_i].y)) != EOF) {
		degree[city_i] = 0;
		nodeSet[city_i].id--;//序號與input差1;
		nodeSet[city_i].start = nodeSet[city_i].id;
		nodeSet[city_i++].next = NULL;

	}
	fclose(fptr);

#ifdef printFile
	printf("Content of file\n-\n");
	printf("Number of City : %d\n", num_city);
	for (i = 0; i < num_city; i++) {
		printf("%c %d %d %d\n", nodeSet[i].id + 65, nodeSet[i].x, nodeSet[i].y, nodeSet[i].next);
	}
	printf("\n");
#endif


	/*establish the dist*/
	dist_i = 0;
	num_dist = cnm(num_city, 2);
	distSet = (dist *)malloc(sizeof(dist) * num_dist);
	for (i = 0; i < num_city; i++) {
		for (j = 0; j < i; j++) {
			distSet[dist_i].idl = j;
			distSet[dist_i].idr = i;
			distSet[dist_i++].dist = distance(nodeSet[i], nodeSet[j]);
		}
	}
	bubbleSort_dist(distSet, num_dist);

#ifdef printDist
	printf("Distance Map:\n-\n");
	for (i = 0; i < num_dist; i++) {
		printf("%c %c %lf\n", distSet[i].idl + 65, distSet[i].idr + 65, distSet[i].dist);
	}
	printf("\n");
#endif

#ifdef testGetRoot
	nodeSet[distSet[0].idl].next = &(nodeSet[distSet[0].idr]);
	printf("the root of %c : %c\n\n", distSet[0].idl + 65, getRoot(&(nodeSet[distSet[0].idl]), distSet, nodeSet).id + 65);
#endif

#ifdef printConnect
	printf("Connect Process:\n-\n");
#endif
	for (i = 0; (num_edge < num_city) && (i < num_dist); i++) {
		r1 = getRoot(&(nodeSet[distSet[i].idl]), distSet, nodeSet)->id;
		r2 = getRoot(&(nodeSet[distSet[i].idr]), distSet, nodeSet)->id;
		//printf("r1: %c r2: %c\n", r1+65, r2+65);
#ifdef printDegree
		for (j = 0; j < num_city; j++) printf("%d ", degree[j]); printf("\n");
#endif

		if (r1 != r2 && degree[distSet[i].idl] < 2 && degree[distSet[i].idr] < 2) { //進入前必須要先小於2 才能滿足小於等於2

#ifdef printConnect
			printf("\nroot %c: %c, root %c: %c ---", distSet[i].idl + 65, r1 + 65, distSet[i].idr + 65, r2 + 65);
#endif
			//distSet[i].idl!=r1 <- distSet[i].idr=r2
			if (distSet[i].idl == r1 ^ distSet[i].idr == r2) { //直接銜接且更新start值
				if (distSet[i].idl == r1) {
#ifdef printConnect
					printf("直接銜接 左到右\n");
#endif
					connectNode(&nodeSet[distSet[i].idl], &nodeSet[distSet[i].idr]);
					nodeSet[distSet[i].idr].start = distSet[i].idl;
					getRoot(&nodeSet[distSet[i].idr], distSet, nodeSet)->start = nodeSet[distSet[i].idl].start;
				} else {
#ifdef printConnect
					printf("直接銜接 右到左\n");
#endif
					connectNode(&nodeSet[distSet[i].idr], &nodeSet[distSet[i].idl]);
					nodeSet[distSet[i].idl].start = distSet[i].idr;
					getRoot(&nodeSet[distSet[i].idl], distSet, nodeSet)->start = nodeSet[distSet[i].idr].start;
				}
			} else if (distSet[i].idl == r1) {
#ifdef printConnect
				printf("旋轉右邊，銜接 左到右\n");
#endif
				inverse(&nodeSet[nodeSet[distSet[i].idr].start]);
				connectNode(&nodeSet[distSet[i].idl], &nodeSet[distSet[i].idr]);
				getRoot(&nodeSet[distSet[i].idr], distSet, nodeSet)->start = nodeSet[distSet[i].idl].start;
			} else {
#ifdef printConnect
				printf("旋轉右邊，銜接 右到左\n");
#endif
				getRoot(&nodeSet[distSet[i].idl], distSet, nodeSet)->start = getRoot(&nodeSet[distSet[i].idr], distSet, nodeSet)->id; //NOTE 要先找start的位置
				inverse(&nodeSet[distSet[i].idr]);
				connectNode(&nodeSet[distSet[i].idr], &nodeSet[distSet[i].idl]);
			}

			num_edge++;
			degree[distSet[i].idl]++;
			degree[distSet[i].idr]++;
			total_dist += distSet[i].dist;
		}
		//system("pause");
	}

	/*Summary: 找尋與尋訪每個Node*/



	/*頭接尾與計算總距離*/
#ifdef printConnect
	printf("End: ");
#endif

	r1 = getRoot(&(nodeSet['A' - 65]), distSet, nodeSet)->id;
	connectNode(&nodeSet[r1], &nodeSet[nodeSet[r1].start]);
	total_dist += distance(nodeSet[r1], nodeSet[nodeSet[r1].start]);
	printf("total distance: %lf\n", total_dist);


#ifdef printFile
	printf("Content of file\n-\n");
	for (i = 0; i < num_city; i++) {
		printf("%c %d %d %d %d\n", nodeSet[i].id + 65, nodeSet[i].x, nodeSet[i].y, &nodeSet[i], nodeSet[i].next);
	}
	printf("\n");
#endif



	/*生成 output file*/
	fptr = fopen(argv[2], "w");
	if (!fptr) {
		printf("ERROR\n");
		exit(1);
	}



	/*確認小的index優先*/
	nPtr = &nodeSet[0];
	for (i = 0; i < num_city; i++) {
		ans[i] = nPtr->id;
		nPtr = nPtr->next;
	}

	fprintf(fptr, "%lf\n", total_dist);
	if (ans[1] < ans[num_city - 1]) {
		for (i = 0; i < num_city; i++) {
			printf("%d->", ans[i] + 1);
			fprintf(fptr, "%d\n", ans[i] + 1);
		}
	} else {
		printf("%d->", ans[0] + 1);
		fprintf(fptr, "%d\n", ans[0] + 1);
		for (i = num_city - 1; i > 0; i--) {
			printf("%d->", ans[i] + 1);
			fprintf(fptr, "%d\n", ans[i] + 1);
		}
	}

	/*回收資源垃圾*/
	fclose(fptr);

	free(nodeSet);
	free(distSet);
	free(degree);

	return 0;
}



void connectNode(node *a, node *b) {
#ifdef printConnect
	printf("%c->%c\n", a->id + 65, b->id + 65);
#endif
	a->next = b;
}

void bubbleSort_dist(dist *d, int n) {
	int i, j;
	dist temp;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n - 1; j++) {
			if (d[i].dist < d[j].dist) {
				temp = d[i];
				d[i] = d[j];
				d[j] = temp;
			}
		}
	}
}

node *getRoot(node *nPtr, dist *distSet, node *nodeSet) {
	while (nPtr->next != NULL) {
		nPtr = nPtr->next;
	}
	return nPtr;
}

node *inverse(node *start) { //指標方向相反
	node *cpre = NULL, *cur = start, *cnext = cur->next;
	while (cnext != NULL) {
		cpre = cur;
		cur = cnext;
		cnext = cnext->next;
		cur->next = cpre;
	}
	start->next = NULL;
	return cur;
}




