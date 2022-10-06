#include <stdio.h>
#include <time.h>
#define MAX_SIZE 100

void printArray(int*, int); // 배열 출력 함수
void randArray(int*, int); // 배열에 랜덤값 저장 함수
void countingSort(int*, int*, int); // Counting Sort 함수

int main() {
	int a[MAX_SIZE];
	int sorted[MAX_SIZE];
	srand((unsigned)time(NULL)); // Initialize random number generator

	randArray(a, MAX_SIZE); // 배열 a에 랜덤값 저장

	printf("배열 a : \n"); // 배열 a 출력
	printArray(a, MAX_SIZE);
	
	countingSort(a, sorted, MAX_SIZE); // 배열 a 정렬

	printf("\n배열 sorted : \n"); // 배열 sorted 출력
	printArray(sorted, MAX_SIZE);

	return 0;
}

void printArray(int* arr, int size) { // 배열 출력 함수
	for (int i = 0; i < size; i++) {
		if (i % 10 == 0 && i != 0) printf("\n");
		printf("[%d] : %d\t", i, arr[i]);
	}
	printf("\n");
}

void randArray(int* arr, int size) { // 배열에 랜덤값 저장 함수
	for (int i = 0; i < size; i++) 
		arr[i] = rand() % (size + 1); // 0 ~ MAX_SIZE 범위의 정수 랜덤값을 생성하여 저장
}

void countingSort(int* arr, int* sorted, int size) { // Counting Sort 함수
	int row_terms[MAX_SIZE + 1] = { 0 }; // a의 원소의 개수를 저장하는 배열
	int starting_pos[MAX_SIZE + 1] = { 0 }; // row_terms을 누적합한 배열

	for (int i = 0; i < size; i++) // row_terms 연산
		row_terms[arr[i]]++;

	printf("\n배열 row_terms : \n");
	printArray(row_terms, size + 1);

	for (int i = 1; i <= size; i++) { // starting_pos 연산
		starting_pos[i] = row_terms[i - 1] + starting_pos[i - 1];
	}

	printf("\n배열 starting_pos : \n");
	printArray(starting_pos, size + 1);

	for (int i = 0; i < size; i++) {
		sorted[starting_pos[arr[i]]++] = arr[i];
	}
}