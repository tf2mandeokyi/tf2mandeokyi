// (c) 2021 m4ndeokyi
// This code is licensed under MIT license (see LICENSE.txt for details)


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


template<typename T> void swap(T* array, int index1, int index2)
{
	T temp = array[index1];
	array[index1] = array[index2];
	array[index2] = temp;
}


template<typename T> void quicksort(T* array, int begin, int end, int (*comp)(T t1, T t2))
{
	if(begin >= end) return;
	if(begin == end - 1)
	{
		if(comp(array[begin], array[end]) > 0) swap(array, begin, end);
		return;
	}
	
	int p1 = begin, p2 = end - 1;
	while(p1 != p2)
	{
		if(comp(array[p2], array[end]) < 0) 
		{
			if(comp(array[p1], array[end]) < 0) 
			{
				++p1; 
				continue;
			}
			else swap(array, p1, p2);
		}
		--p2;
	}
	swap(array, comp(array[p1], array[end]) > 0 ? p1 : p1+1, end);
	
	quicksort(array, begin, p1, comp);
	quicksort(array, p1+1, end, comp);
}


void random_test(int), massive_test(int);


int main()
{
	srand(time(NULL));
	
	int operation, array_size;
	
	printf("퀵정렬 테스트 프로그램입니다. 다음 숫자 중 하나를 입력해주세요.\n\n");
	printf("   0 - 랜덤 배열 정렬 테스트\n");
	printf("     입력받은 배열의 크기로 배열을 생성하고, 해당 배열을 랜덤한 숫자로 채운 뒤 퀵정렬로 정렬합니다.\n\n");
	printf("   1 - 다중 랜덤 배열 정렬 테스트\n");
	printf("     0번을 무한 반복하나, 배열의 요소 대신 소요 시간과 테스트 성공 여부가 출력됩니다.\n\n");
	
	re:
	printf("입력: ");
	scanf("%d", &operation);
	
	switch(operation) {
		case 0:
			printf("배열의 크기를 입력하세요 (권장: 10 ~ 40): ");
			scanf("%d", &array_size);
			random_test(array_size);
			break;
		case 1:
			printf("배열의 크기를 입력하세요 (권장: 1,000,000): ");
			scanf("%d", &array_size);
			massive_test(array_size);
			break;
		default:
			printf("숫자를 잘못 입력하셨습니다. 다시 시도해주세요.\n");
			goto re;
	}
	
	return 0;
}

	
inline int comp(int t1, int t2)
{
	return t1 - t2;
}


void random_test(int array_size)
{
	int* a = new int[array_size];
	printf("정렬 전: \n   ");
	for(int i = 0; i < array_size; ++i)
	{
		a[i] = (rand() * rand()) & 0xffff;
		printf("%d ", a[i]);
	}
	printf("\n");
	
	quicksort(a, 0, array_size-1, comp);
	
	printf("정렬 후: \n   ");
	for(int i = 0; i < array_size; ++i)
	{
		printf("%d ", a[i]);
	}
}


void massive_test(int array_size)
{
	int* a = new int[array_size];
	struct timeval start, end;
	
	for(int i=1;;++i)
	{
		// Pre-Test
		for(int i = 0; i < array_size; ++i)
		{
			a[i] = (rand() * rand()) & 0x3fffffff;
		}
		gettimeofday(&start, NULL);
		printf("%d번째 테스트 소요 시간: ", i);
		
		// Test
		quicksort(a, 0, array_size-1, comp);
		
		// Post-Test
		gettimeofday(&end, NULL);
		float delta_s = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000000.;
		printf("%.3f초; 결과: ", delta_s);
		for(int i = 0; i < array_size-1; ++i) 
		{
			if(a[i] > a[i+1])
			{
				printf("실패\n");
				return;
			}
		}
		printf("성공\n");
	}
}
