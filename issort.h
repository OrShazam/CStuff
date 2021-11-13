typedef struct _list {
	struct _list *next,
	struct _list *prev,
	void *value
} list, *plist;

static int issort(void* data, int size,int esize, int (*compare)(const void *key1,const void *key2)){
	void* key;
	int i,j;
	if ((key = malloc(esize)) == NULL)
		return -1;
	for (int j = 1; j < size; j++){
		memcpy(key, &data[j * esize], esize);
		i = j-1;
		while ( i>= 0 && compare(&data[i * esize],key) > 0){
			memcpy(&data[(i+1) * esize], &data[i * esize], esize);
			i--;
		}
		memcpy(&data[(i+1) * esize], key, esize);
	}
	free(key);
	return 0;
}

static int issort(plist data, int (*compare)(const void *key1, const void *key2)){
	void *key;
	plist root = data,temp;
	while (data != NULL){
		key = data->value;
		temp = data->prev;
		while (temp != root && compare(temp->value, key) > 0) {
			temp->next->value = temp->value;		
			temp = temp->prev;
		}
		temp->value = key;
		data = data->next;
	}
	return 0;
}