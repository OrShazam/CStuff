int remove(void* start, void* end, int elem_size, void* value){
	int cnt = 0;
	void* ptr = start;
	while (ptr != end){
		if (memcmp(start,value,elem_size)){
			memcpy(start + cnt, ptr, elem_size);
			cnt += elem_size;
		}
		ptr += elem_size;
	}
	return cnt;
}

int remove_v2(void* start, void* end, int elem_size, void* value){
	// 'lazy' approach
	int cnt = 0, sum = 0;
	void* cpy = start, ptr = start;
	while (ptr != end){
		if (!memcmp(ptr,value,elem_size)){
			memcpy(start + sum, cpy, cnt);
			sum += cnt;
			cpy = ptr + elem_size; cnt = 0;
		}
		else {
			cnt += elem_size;
		}
		ptr += elem_size;
	}
	return sum;
}
