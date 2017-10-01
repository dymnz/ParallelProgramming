#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define printDegree
//#define printConnect
//#define printFile
//#define printDist
//#define testGetRoot

typedef struct s_node {
	int index, x, y;
	struct s_node *next;
	int start;
} node;

typedef struct s_dist {
	int index_left, index_right;
	double dist;
} dist;

int cnm(int n, int m) {return m == 0 ? 1 : cnm(n - 1, m - 1) * n / m;}
double distance(node a, node b) {return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));}
node *getRoot(node *nPtr, dist *distSet, node *nodeSet);
voindex connectNode(node *a, node *b);
voindex inverseAdd(node *a);
voindex bubbleSort_dist(dist *d, int n);
node *inverse(node *start);


int main(int argc, char *argv[]) {
	/*define variables*/
//	argv[1]="test0.i";
//	argv[2]="test0.o";

	int root_1, root_2, root;
	int i, j;
	int num_city, city_i = 0, num_dist, num_edge = 0;
	int *degree;
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
	while (fscanf(fptr, "%d %d %d", &(nodeSet[city_i].index), &(nodeSet[city_i].x), &(nodeSet[city_i].y)) != EOF) {
		degree[city_i] = 0;
		nodeSet[city_i].index--;//序號與input差1;
		nodeSet[city_i].start = nodeSet[city_i].index;
		nodeSet[city_i++].next = NULL;

	}
	fclose(fptr);

#ifdef printFile
	printf("Content of file\n-\n");
	for (i = 0; i < num_city; i++) {
		printf("%c %d %d %d\n", nodeSet[i].index + 65, nodeSet[i].x, nodeSet[i].y, nodeSet[i].next);
	}
	printf("\n");
#endif


	/*establish the dist*/
	int temp_index = 0;
	num_dist = cnm(num_city, 2);
	distSet = (dist *) malloc(sizeof(dist) * num_dist);
	for (i = 0; i < num_city; i++) {
		for (j = 0; j < i; j++) {
			distSet[temp_index].index_left = j;
			distSet[temp_index].index_right = i;
			distSet[temp_index].dist = distance(nodeSet[i], nodeSet[j]);
			++temp_index;
		}
	}
	bubbleSort_dist(distSet, num_dist);

#ifdef printDist
	printf("Distance Map:\n-\n");
	for (i = 0; i < num_dist; i++) {
		printf("%c %c %lf\n", distSet[i].index_left + 65, distSet[i].index_right + 65, distSet[i].dist);
	}
	printf("\n");
#endif

#ifdef testGetRoot
	nodeSet[distSet[0].index_left].next = &(nodeSet[distSet[0].index_right]);
	printf("the root of %c : %c\n\n", distSet[0].index_left + 65, getRoot(&(nodeSet[distSet[0].index_left]), distSet, nodeSet).index + 65);
#endif

#ifdef printConnect
	printf("Connect Process:\n-\n");
#endif
	for (i = 0; (num_edge < num_city) && (i < num_dist); i++) {
		root_1 = getRoot(&(nodeSet[distSet[i].index_left]), distSet, nodeSet)->index;
		root_2 = getRoot(&(nodeSet[distSet[i].index_right]), distSet, nodeSet)->index;
		//printf("root_1: %c root_2: %c\n", root_1+65, root_2+65);
#ifdef printDegree
		for (j = 0; j < num_city; j++) printf("%d ", degree[j]); printf("\n");
#endif

		if (root_1 == root_2 || degree[distSet[i].index_left] >= 2 || degree[distSet[i].index_right] >= 2)
			continue;

#ifdef printConnect
		printf("\nroot %c: %c, root %c: %c\n", distSet[i].index_left + 65, root_1 + 65, distSet[i].index_right + 65, root_2 + 65);
#endif
		//distSet[i].index_left!=root_1 <- distSet[i].index_right=root_2
		if (distSet[i].index_left == root_1 ^ distSet[i].index_right == root_2) { //直接銜接且更新start值
			if (distSet[i].index_left == root_1) {
#ifdef printConnect
				printf("直接銜接 左到右\n");
#endif
				connectNode(&nodeSet[distSet[i].index_left], &nodeSet[distSet[i].index_right]);
				nodeSet[distSet[i].index_right].start = distSet[i].index_left;
				getRoot(&nodeSet[distSet[i].index_right], distSet, nodeSet)->start = nodeSet[distSet[i].index_left].start;
			} else {
#ifdef printConnect
				printf("直接銜接 右到左\n");
#endif
				connectNode(&nodeSet[distSet[i].index_right], &nodeSet[distSet[i].index_left]);
				nodeSet[distSet[i].index_left].start = distSet[i].index_right;
				getRoot(&nodeSet[distSet[i].index_left], distSet, nodeSet)->start = nodeSet[distSet[i].index_right].start;
			}
		} else if (distSet[i].index_left == root_1) {
#ifdef printConnect
			printf("旋轉右邊，接銜接 左到右\n");
#endif
			inverse(&nodeSet[nodeSet[distSet[i].index_right].start]);
			connectNode(&nodeSet[distSet[i].index_left], &nodeSet[distSet[i].index_right]);
			getRoot(&nodeSet[distSet[i].index_right], distSet, nodeSet)->start = nodeSet[distSet[i].index_left].start;
		} else {
#ifdef printConnect
			printf("旋轉右邊，接銜接 右到左\n");
#endif
			inverse(&nodeSet[distSet[i].index_right]);
			getRoot(&nodeSet[distSet[i].index_left], distSet, nodeSet)->start = getRoot(&nodeSet[distSet[i].index_right], distSet, nodeSet)->index;
			connectNode(&nodeSet[distSet[i].index_right], &nodeSet[distSet[i].index_left]);
		}

		num_edge++;
		degree[distSet[i].index_left]++;
		degree[distSet[i].index_right]++;
		total_dist += distSet[i].dist;
	}
	//system("pause");
	/*Summary: 找尋與尋訪每個Node*/



	/*頭接尾*/
#ifdef printConnect
	printf("End: ");
#endif;
	root_1 = getRoot(&(nodeSet['A' - 65]), distSet, nodeSet)->index;
	connectNode(&nodeSet[root_1], &nodeSet[nodeSet[root_1].start]);

	total_dist += distance(nodeSet[root_1], nodeSet[nodeSet[root_1].start]);
	printf("total distance: %lf\n", total_dist);


#ifdef printFile
	printf("Content of file\n-\n");
	for (i = 0; i < num_city; i++) {
		printf("%c %d %d %d %d\n", nodeSet[i].index + 65, nodeSet[i].x, nodeSet[i].y, &nodeSet[i], nodeSet[i].next);
	}
	printf("\n");
#endif


	/*生成 output file*/
	fptr = fopen(argv[2], "w");
	if (!fptr) {
		printf("ERROR\n");
		exit(1);
	}

	fprintf(fptr, "%lf\n", total_dist);
	nPtr = &nodeSet[0];
	for (i = 0; i < num_city; i++) {
		fprintf(fptr, "%d\n", nPtr->index + 1);
		printf("%c->", nPtr->index + 65);
		nPtr = nPtr->next;
	}
	printf("NULL\n");

	nPtr = &nodeSet[0];
	for (i = 0; i < num_city; i++) {
		printf("%d->", nPtr->index + 1);
		nPtr = nPtr->next;
	}
	printf("NULL\n");


	/*回收資源垃圾*/
	free(nodeSet);
	free(distSet);
	free(degree);
	fclose(fptr);

	return 0;
}



voindex connectNode(node *a, node *b) {
#ifdef printConnect
	printf("%c->%c\n", a->index + 65, b->index + 65);
#endif
	a->next = b;
}

voindex bubbleSort_dist(dist *d, int n) {
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




