/*
	 ____     __  __ 
   /  _   |  |  |    |
  |  | |  |  |     __/       
  |  -_|  |  |   -
   \___   |  |  |
    __ |  |  |  |
   |  ||  |  |  |
   \_____ /  |__|

*/   


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#define clear() printf("\033[H\033[J")

#define DEFAULT_VEC_SIZE 25
#define DEFAULT_VEC_GROWTH 2


enum COLORS {
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	PURPLE,
	CYAN,
	WHITE
};

/*================================= PtrVector Declerations =================================*/
typedef struct PtrVectorType{
	unsigned int 	size;
	unsigned int 	used;
	char**			data;
	unsigned char	growth;
}PtrVector;

PtrVector* pvecConstruct(void);
int pvecGrow(PtrVector*);
int pvecShrink(PtrVector*);

unsigned int pvecSize(PtrVector*);
unsigned int pvecLength(PtrVector*);

int pvecAppend(PtrVector*, void*);
int pvecInsert(PtrVector*, void*, unsigned int);
int pvecSearch(PtrVector*, void*);
void* pvecGet(PtrVector*, unsigned int);
void* pvecPop(PtrVector*, unsigned int);
int pvecRemove(PtrVector*, void*);
int pvecFree(PtrVector*);



/*================================= Pathfinder Delerations =================================*/

typedef struct NodeType{
    struct NodeType* parent;
	unsigned int h;                      //distance from the goal
    unsigned int g;                      //distance from the start
    unsigned int f;                      //g(n) + h(n)
	int xpos;
    int ypos;
	unsigned char visited;
}Node;

unsigned int NODES;
unsigned int delay;

void recursiveBacktrack(Node* n, Node*** grid, int c);
Node* aStar(Node*** grid, PtrVector* open, PtrVector* closed, PtrVector* walls, Node* goal, Node* start);
void calcCost(Node* n, Node* start, Node* goal);
int nodeSearch(Node* target, PtrVector* open, PtrVector* closed, PtrVector* walls);

void printGrid(Node*** grid, PtrVector* open, PtrVector* closed, PtrVector* walls, PtrVector* path, Node* goal, Node* start);
void fillGrid();
void disp(int x, int y, char c, int color);

int max(int* arr, int arr_length);
int min(int* arr, int arr_length);
void clearKeyboardBuffer(void);


/*================================= Pathfinder Functions =================================*/
int main(int argc, char* argv[]){
	int choice;
	int speed;
	int temp_size;

	printf(" A* Pathfinder\n");
	printf("[1] Maze\n");
	printf("[2] Obstacles\n$");
	scanf("%d", &choice);
	printf("Enter grid size:\n$");
	scanf("%d", &temp_size);

	if(temp_size % 4 != 1){
		temp_size += temp_size%4 + 1;
		printf("Rounded to %d \n", temp_size);
	}
	NODES = temp_size;

	printf("Enter speed [1-4]:\n$");
	scanf("%d", &speed);
	switch(speed){
		case 1:
			delay = 30000;
			break;
		case 2:
			delay = 10000;
			break;
		case 3:
			delay = 5000;
			break;
		case 4:
			delay = 0;
			break;
	}


	//-----------------------------------Init vars
    PtrVector* open = pvecConstruct();
    PtrVector* closed = pvecConstruct();
    PtrVector* path = pvecConstruct();
	PtrVector* walls = pvecConstruct();

    Node*** grid = malloc(sizeof(Node**) * NODES);
    for(int i = 0; i < NODES; i++){
        grid[i] = malloc(sizeof(Node**) * NODES);
    }

    //Init grid
    for(int y = 0; y < NODES; y++){
        for(int x = 0; x < NODES; x++){
            Node* n = malloc(sizeof(Node));
            n->ypos = y;
            n->xpos = x;
            grid[x][y] = n;
        }
    }
	

	time_t t;
	srand((unsigned) time(&t));
    //--------------------------------------Main loop
	Node* start = grid[1][1];
    Node* goal = grid[NODES-2][NODES-2];
    int count = 0;
    char stepper;
	
	if(choice == 1){		//Generate maze walls
		int rx = rand() % (NODES - 1);
		if(rx % 2 == 0)
			rx += 1;
		int ry = rand() % (NODES - 1);
		if(ry % 2 == 0)
			ry += 1;
		clear();
		fillGrid();
		printf("\033[%d;%dH%s\n", NODES+1, 2, "Generating Maze...");
		recursiveBacktrack(grid[rx][ry], grid, 0);
		clear();
		for(int y = 0; y < NODES; y++){
			for(int x = 0; x < NODES; x++){
				if(grid[x][y]->visited == 0)
					pvecAppend(walls, grid[x][y]);
			}
		}
	}else{
		return 0;
	}
    
	printGrid(grid, open, closed, walls, path, goal, start);
	clearKeyboardBuffer();
	printf("\033[%d;%dH%s\n", NODES+1, 2, "Press Enter");
    scanf("%c", &stepper);

	Node* tail = aStar(grid, open, closed, walls, goal, start);

	//Reconstruct path from tail to head
    Node* node = tail;
    while(node != NULL){
        pvecAppend(path, node);
        node = node->parent;
    }
	
	//Print the solution
	PtrVector* blank = pvecConstruct();
	clear();
    printGrid(grid, blank, blank, walls, path, goal, start);


	//------------------------FREE-------------------------------
	for(int y = 0; y < NODES; y++){
		for(int x = 0; x < NODES; x++){
			free(grid[x][y]);
		}
	}

	for(int i = 0; i < NODES; i++){
		free(grid[i]);
	}
	free(grid);

	pvecFree(open);
	free(open);
	pvecFree(closed);
	free(closed);
	pvecFree(walls);
	free(walls);
	pvecFree(path);
	free(path);
	pvecFree(blank);
	free(blank);

    return 0;
}

void recursiveBacktrack(Node* n, Node*** grid, int c){
	PtrVector* neighbours = pvecConstruct();
	
	//Find all valid neihgbours
	if(n->ypos > 2 && grid[n->xpos][n->ypos - 2]->visited == 0){
		pvecAppend(neighbours, grid[n->xpos][n->ypos - 2]);
    }
	if(n->xpos < NODES - 2 && grid[n->xpos + 2][n->ypos]->visited == 0){
		pvecAppend(neighbours, grid[n->xpos + 2][n->ypos]);
    }
    if(n->ypos < NODES - 2 && grid[n->xpos][n->ypos + 2]->visited == 0){
		pvecAppend(neighbours, grid[n->xpos][n->ypos + 2]);
    }
	if(n->xpos > 2 && grid[n->xpos - 2][n->ypos]->visited == 0){
		pvecAppend(neighbours, grid[n->xpos - 2][n->ypos]);
    }
	
	while(pvecLength(neighbours) > 0){
		int move = rand() % pvecLength(neighbours);
		Node* neighbour = pvecGet(neighbours, move);
		if(neighbour->visited == 1){
			pvecPop(neighbours, move);
			continue;
		}
		Node* next = pvecGet(neighbours, move);
		int index = pvecSearch(neighbours, next);
		pvecPop(neighbours, index);
		next->visited = 1;

		int deltax = n->xpos - next->xpos;
		int deltay = n->ypos - next->ypos;
		
		if(c == 0){
			c = 1;
		}else{
			grid[next->xpos + deltax/2][next->ypos + deltay/2]->visited = 1;
			disp(next->xpos + deltax/2, next->ypos + deltay/2, ' ', WHITE);
		}

		disp(next->xpos, next->ypos, ' ', WHITE);
		usleep(delay + 100);

		recursiveBacktrack(next, grid, c);

	}
	
	pvecFree(neighbours);
	free(neighbours);

	return;
}

Node* aStar(Node*** grid, PtrVector* open, PtrVector* closed, PtrVector* walls, Node* goal, Node* start){
    Node* current = start;

    while(current != goal){
        PtrVector* neighbours = pvecConstruct();
        pvecAppend(closed, current);
        disp(current->xpos, current->ypos, 'x', RED);

        //Neighbours are stored in clockwise order starting from 12 oclock
        if(current->ypos > 1){
            pvecAppend(neighbours, grid[current->xpos][current->ypos - 1]);
        }
		if(current->xpos < NODES - 1){
            pvecAppend(neighbours, grid[current->xpos + 1][current->ypos]);
        }
		if(current->ypos < NODES - 1){
            pvecAppend(neighbours, grid[current->xpos][current->ypos + 1]);
        }
		if(current->xpos > 1){
            pvecAppend(neighbours, grid[current->xpos - 1][current->ypos]);
		}

        for(int i = 0; i < pvecLength(neighbours); i++){
            int cost = current->g + 10;
            Node* neighbour = pvecGet(neighbours, i);
            int rc = 0;     									
            rc = nodeSearch(neighbour, open, closed, walls);		//1: open, 2: closed, 3: walls, 0: none
            if(rc == 2 || rc == 3){
                continue;
            }

            if(rc == 1 && cost < neighbour->g){
				pvecRemove(open, neighbour);
                rc = 0;
            }


            if(rc == 0){
                neighbour->g = cost;
				calcCost(neighbour, start, goal);
                pvecAppend(open, neighbour);
                neighbour->parent = current;
                
				printf("\033[0;32m");
				disp(neighbour->xpos, neighbour->ypos, 'o', GREEN);
            }
        }
        
		//SEARCH FRO LOWEST F IN OPEN
        int min = 4000000;
        Node* next;
        for(int i = 0; i < pvecLength(open); i++){
            Node* n = pvecGet(open, i);
            if(n->f < min){
                min = n->f;
                next = n;
            }else if(n->f == min){
                if(n->g < next->g){
                    min = n->f;
                    next = n;
                }
            }
        }
        Node* copy = current;
        current = next;
		pvecRemove(open, next);
		pvecFree(neighbours);
		free(neighbours);
        
		usleep(delay);


    }
	return current;

}


void calcCost(Node* n, Node* start, Node* goal){
    int deltax;
    int deltay;

    deltax = abs(n->xpos - goal->xpos);
    deltay = abs(n->ypos - goal->ypos);
    n->h = (deltax + deltay) * 10;

    n->f = n->g + n->h;                                     	//total cost
    return;
}

void printGrid(Node*** grid, PtrVector* open, PtrVector* closed, PtrVector* walls, PtrVector* path, Node* goal, Node* start){
    int l[3] = {pvecLength(open), pvecLength(closed), pvecLength(walls)};
	int length = max(l, 3);
	
    for(int y = 0; y < NODES; y++){
        for(int x = 0; x < NODES; x++){
			for(int i = 0; i < length; i++){
				if(grid[x][y] == pvecGet(walls, i)){
					disp(x,y,'#', WHITE);
                }else if(grid[x][y] == pvecGet(path, i)){
					disp(x,y,'x', CYAN);
                }else if(grid[x][y] == pvecGet(closed, i)){
                    disp(x,y,'x', RED);
                }else if(grid[x][y] == pvecGet(open, i)){
					disp(x,y,'o', GREEN);
                }else if(grid[x][y] == goal){
                    disp(x,y,'$', YELLOW);
                }else if(grid[x][y] == start){
					disp(x,y,'X', GREEN);
                }
			}
        }
    }
}

void fillGrid(){
	for(int y = 0; y < NODES; y++){
		for(int x = 0; x < NODES; x++){
			disp(x, y, '#', WHITE);
		}
	}

}

int nodeSearch(Node* target, PtrVector* open, PtrVector* closed, PtrVector* walls){
    int l[3] = {pvecLength(open), pvecLength(closed), pvecLength(walls)};
	int length = max(l, 3);
   

    for(int i = 0; i < length + 1; i++){
        if(pvecGet(closed, i) == target){
            return 1;
        }else if(pvecGet(open, i) == target){
            return 2;
        }else if(pvecGet(walls, i) == target){
			return 3;
		}
    }
    return 0;

}

int max(int* arr, int arr_length){
	int rslt = 0;
	for(int i = 0; i < arr_length; i++){
		if(arr[i] > rslt)
			rslt = arr[i];
	}
	return rslt;
}

int min(int* arr, int arr_length){
	int rslt = 4300000;
	for(int i = 0; i < arr_length; i++){
		if(arr[i] < rslt)
			rslt = arr[i];
	}
	return rslt;
}

void disp(int x, int y, char c, int color){
	switch(color){
		case BLACK:
			printf("\033[0;30m");
			break;
		case RED:
			printf("\033[0;31m");
			break;
		case GREEN:
			printf("\033[0;32m");
			break;
		case YELLOW:
			printf("\033[0;33m");
			break;
		case BLUE:
			printf("\033[0;34m");
			break;
		case PURPLE:
			printf("\033[0;35m");
			break;
		case CYAN:
			printf("\033[0;36m");
			break;
		case WHITE:
			printf("\033[0;37m");
			break;
	}

	printf("\033[%d;%dH%c\n", y+1, (x+1)*2, c);
	printf("\033[0m");
}

void clearKeyboardBuffer(void){
	char ch;
	while((ch = getchar()) != '\n'&& ch != EOF);
}


/*================================= PtrVector Functions =================================*/

PtrVector* pvecConstruct(void){
	PtrVector* vec;
	vec = malloc(sizeof(PtrVector));
	vec->size = DEFAULT_VEC_SIZE;
	vec->data = malloc(sizeof(void*) * vec->size);
	vec->used = 0;
	vec->growth = DEFAULT_VEC_GROWTH;

	return vec;
}

int pvecGrow(PtrVector* vec){
	vec->size *= vec->growth;
	char** temp = realloc(vec->data, sizeof(void*) * vec->size);
	if(temp == NULL){
		return 1;		//Memory error
	}else{
		vec->data = temp;
		return 0;
	}
}

int pvecShrink(PtrVector* vec){
	vec->size /= vec->growth;
	char** temp = realloc(vec->data, sizeof(void*) * vec->size);
	if(temp == NULL){
		return 1;		//Memory error
	}else{
		vec->data = temp;
		return 0;
	}
}

unsigned int pvecLength(PtrVector* vec){
	return vec->used;
}

unsigned int pvecSize(PtrVector* vec){
	return vec->size;
}

int pvecAppend(PtrVector* vec, void* data_ptr){
	int result;
	(vec->data)[vec->used] = (char*) data_ptr;
	vec->used++;
	
	if(vec->used > vec->size - 1){
		result = pvecGrow(vec);
	}

	return result;
}

void* pvecGet(PtrVector* vec, unsigned int index){
	if(index <= vec->used && index >= 0)
		return (void*)(vec->data)[index];
	else
		return NULL;		//Access illegal data
}

int pvecInsert(PtrVector* vec, void* data_ptr, unsigned int index){
	if(index == vec->used){
		return pvecAppend(vec, data_ptr);	
	}else if(index < vec->used && index >= 0){
		vec->data[index] = (char*) data_ptr;
		return 0;
	}else{
		return 1;		//Write to illegal index
	}
}

void* pvecPop(PtrVector* vec, unsigned int index){
	void* result = NULL;
	if(index <= vec->used && index >= 0){
		result = (void*)vec->data[index];

		for(int i = index; i < vec->used; i++){
			vec->data[i] = vec->data[i+1];
		}
		vec->used--;
	}

	if(vec->used < vec->size/vec->growth && vec->size > DEFAULT_VEC_SIZE)
		pvecShrink(vec);

	return result;
}

int pvecSearch(PtrVector* vec, void* data_ptr){
	for(int i = 0; i < vec->used+1;i++){
		if((void*)vec->data[i] == data_ptr)
			return i;
	}
	return -1;
}

int pvecRemove(PtrVector* vec, void* data_ptr){
	int index;
	index = pvecSearch(vec, data_ptr);
	if(index == -1)
		return -1;

	pvecPop(vec, index);
	return index;
}

int pvecFree(PtrVector* vec){
	free(vec->data);
	return 0;
}

