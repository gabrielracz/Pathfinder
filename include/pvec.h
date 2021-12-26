#define DEFAULT_VEC_SIZE 25
#define DEFAULT_VEC_GROWTH 2

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
