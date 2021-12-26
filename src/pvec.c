#include<stdio.h>
#include<stdlib.h>
#include"../include/pvec.h"


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

