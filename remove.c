int remove(void* start, void* end, int elem_size, void* value){
	int cnt = 0;
	while (start != end){
		if (memcmp(start,value,elem_size)){
			memcpy(start + cnt++ * elem_size, start, elem_size);
		}
		start += elem_size;
	}
	return cnt;
}
