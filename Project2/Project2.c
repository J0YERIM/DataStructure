#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COL  50         /*  maximum number of column + 1 */
#define MAX_TERMS 101
#define COMPARE(x, y) (((x)<(y)) ? -1 : ((x)==(y)) ? 0 : 1)

typedef struct { // 행렬
	int	row;
	int	col;
	int	value;
} term;

/*
SPARSE: 희소 행렬
MATRIX: 일반 행렬
*/
typedef enum shape { SPARSE, MATRIX } printShape;

void printMatrix(term t[], printShape shape, char* title); // 행렬 출력 함수
void fast_transpose(term a[], term b[]); // 전치 함수
void storesum(term d[], int* totald, int row, int column, int* sum);
void mMult(term a[], term b[], term d[]); // 희소 행렬 곱 함수
void mAdd(term a[], term b[], term d[]); // 희소 행렬 합 함수

void main()
{
	term d[MAX_TERMS];
	term a[MAX_TERMS] = { {6,6,8},
				{0,0,15},
				{0,3,22},
				{0,5,-15},
				{1,1,11},
				{1,2,3},
				{2,3,-6},
				{4,0,91},
				{5,2,28} };
	term b[MAX_TERMS] = { {6,2,5},
				{0,0,1},
				{1,1,1},
				{2,0,3},
				{3,1,1},
				{4,0,2} };

	printf("\n***** Sparse Matrix Multiplication *****\n");
	/* Debug */printMatrix(a, SPARSE, "A");		 // SPARSE: 희소 행렬 형태로 출력
	/* Debug */printMatrix(b, SPARSE, "B");
	/* Debug */printMatrix(a, MATRIX, "A");
	/* Debug */printMatrix(b, MATRIX, "B");
	mMult(a, b, d);
	/* Debug */printMatrix(d, SPARSE, "D=AxB");
	/* Debug */printMatrix(d, MATRIX, "D=AxB");	 // MATRIX: 일반 행렬 형태로 출력

	/*printf("\n***** Sparse Matrix Addition *****\n");*/
	fast_transpose(a, b);	// b행렬을 a를 전치하여 생성
	/* Debug */printMatrix(a, SPARSE, "A");
	/* Debug */printMatrix(b, SPARSE, "B=A.T");
	/* Debug */printMatrix(a, MATRIX, "A");
	/* Debug */printMatrix(b, MATRIX, "B=A.T");
	mAdd(a, b, d);
	/* Debug */printMatrix(d, SPARSE, "D=A+B");
	/* Debug */printMatrix(d, MATRIX, "D=A+B");
}

void printMatrix(term t[], printShape shape, char* title) // 행렬 출력 함수
{
	int n = t[0].value; // 행렬의 0이 아닌 총 원소 수
	int i, len;
	char* line = "---------------", tLine[32];

	len = strlen(line);
	sprintf(tLine, "%s: %s", title, line); // D=A+B: --------------- 형태로 저장

	if (shape == SPARSE)	// 희소행렬 형태로 출력
	{
		printf("%.*s\n", len, tLine);
		for (i = 0; i <= n; i++)
			printf("%2d | %d, %d, %d\n", i, t[i].row, t[i].col, t[i].value);
		printf("%s\n", line);
	}
	else if (shape == MATRIX)  // 일반 행렬 형태로 출력
	{
		int j, p = 1;
		printf("%s:\n", title);
		for (i = 0; i < t[0].row; i++)
		{
			for (j = 0; j < t[0].col; j++)
				if (t[p].row == i && t[p].col == j)
					printf("%4d", t[p++].value);
				else
					printf("%4d", 0);
			printf("\n");
		}
		printf("\n");
	}
}

void fast_transpose(term a[], term b[]) // 전치 함수
{
	/*  the transpose of a is placed in b */
	int  row_terms[MAX_COL], starting_pos[MAX_COL];
	int  i, j, num_cols = a[0].col, num_terms = a[0].value;
	b[0].row = num_cols; b[0].col = a[0].row;
	b[0].value = num_terms;
	if (num_terms > 0)     /*  nonzero matrix  */
	{
		for (i = 0; i < num_cols; i++) // row_terms 배열 0으로 초기화
			row_terms[i] = 0;
		for (i = 1; i <= num_terms; i++) // col 번호를 count하여 row_terms의 해당 번호에 저장
			row_terms[a[i].col]++;
		starting_pos[0] = 1;
		for (i = 1; i < num_cols; i++) // 0행을 1로 고정 후, 다음 행 위치는 이전 행 위치 + 개수
			starting_pos[i] = starting_pos[i - 1] + row_terms[i - 1];
		for (i = 1; i <= num_terms; i++) // col을 차례대로 읽으면서 전치
		{
			j = starting_pos[a[i].col]++;
			b[j].row = a[i].col;
			b[j].col = a[i].row;
			b[j].value = a[i].value;
		}
	}
}

void mMult(term a[], term b[], term d[]) // a, b: 입력 , d: 출력, d = a * b
{	// multiply two sparse matrics
	// a, b -> [0] : 행렬 정보 저장 [1] ~ [원소의 개수] : 원소 저장 [원소의 개수 + 1] : 여유분
	int i, j, column, totald = 0; // totald : d의 원소 개수
	int rows_a = a[0].row, cols_a = a[0].col, totala = a[0].value; // a[0] 데이터를 변수에 저장
	int cols_b = b[0].col, totalb = b[0].value; // b[0] 데이터를 변수에 저장
	int row_begin = 1, row = a[1].row, sum = 0; // row: a의 현재 행, sum: A의 특정 행과 B의 특정 열의 행렬곱 결과
	term newB[MAX_TERMS];
	
	if (cols_a != b[0].row) { // a의 열의 수와 b의 행의 수 동일 여부 확인
		fprintf(stderr, "Incompatible matricses for MMult\n");
		exit(0);
	}
	
	fast_transpose(b, newB); // b를 전치한 값을 newB에 저장

	/* set boundary condition */
	a[totala + 1].row = rows_a;
	newB[totalb + 1].row = cols_b;
	newB[totalb + 1].col = b[0].row;

	for (i = 1; i <= totala; ) // 1부터 a 원소의 개수까지 반복
	{
		// a의 행과 b의 열 곱셈
		column = newB[1].row; // b의 현재 열 : a의 row가 곱하고 있는 b의 열
		for (j = 1; j <= totalb + 1; ) // 1부터 b 원소의 개수까지 반복
		{
			// multiply a's row by b's column
			if (a[i].row != row) // a의 현재 행을 벗어남
			{
				storesum(d, &totald, row, column, &sum); // d[row][colum]에 합 저장
				i = row_begin; // a는 원 위치로, b는 다음 열로
				for (; newB[j].row == column; j++) ;
				column = newB[j].row;
			}
			else if (newB[j].row != column) // b의 현재 열을 벗어남
			{
				storesum(d, &totald, row, column, &sum); // d[row][colum]에 합 저장
				i = row_begin; // a는 원 위치
				column = newB[j].row; // b는 다음 열로
			}
			else switch (COMPARE(a[i].col, newB[j].col))
			{
			case -1: /* go to next term in a */
				// a[i].col < newB[j].col a 증가
				i++;
				break;
			case 0: // 계산 후, a와 b를 모두 진행
				/* add terms, go to next term in a and b */
				sum += (a[i++].value * newB[j++].value);
				break;
			case 1: /* advance to next term in b */
				// a[i].col > newB[j].col b 증가
				j++;
			}
		} // end of J <= totalb+1
		for (; a[i].row == row; i++) ; // b의 모든 원소를 처리한 후,
		row_begin = i; // a의 현재 행을 다음 행으로
		row = a[i].row;
	} /* end of for I <= totala */
	d[0].row = rows_a;
	d[0].col = cols_b;
	d[0].value = totald;
}

void storesum(term d[], int* totald, int row, int column, int* sum)
{
	/* if *sum != 0, then it along with its row and column
	position is stored as the *totald+1 entry in d */
	if (*sum) // 썸이 0이 아니라면
		if (*totald < MAX_TERMS)
		{
			// totald 값 1 증가(d의 원소 1 증가) 이후 row, column, sum 저장 후 0으로 초기화
			d[++ *totald].row = row; 
			d[*totald].col = column;
			d[*totald].value = *sum;
			*sum = 0;
		}
		else
		{
			fprintf(stderr, "Numbers of terms in product exceeds %d\n", MAX_TERMS);
			return;
		}
}

void mAdd(term a[], term b[], term d[]) { // 희소 행렬 합 함수
	int rows_a = a[0].row, cols_a = a[0].col, totala = a[0].value;
	int rows_b = b[0].row, cols_b = b[0].col, totalb = b[0].value;
	int aIndex = 1, bIndex = 1, dIndex = 1;

	if (rows_a != rows_b || cols_a != cols_b) { // a, b의 크기가 같은지 확인
		fprintf(stderr, "Incompatible matricses for MAdd\n");
		exit(0);
	}

	// 0 인덱스에 d 행렬 정보 넣기
	d[0].row = rows_a; 
	d[0].col = cols_a;

	while (aIndex <= totala && bIndex <= totalb) { // 둘 중에 하나라도 마지막 index일 때까지 반복
		// a와 b 크기 연산
		int aNum = a[aIndex].row * a[0].col + a[aIndex].col;
		int bNum = b[bIndex].row * b[0].col + b[bIndex].col;

		switch (COMPARE(aNum, bNum))
		{
		case -1: // a보다 b가 작을 경우
			d[dIndex].col = a[aIndex].col;
			d[dIndex].row = a[aIndex].row;
			d[dIndex++].value = a[aIndex++].value;
			break;
		case 0: // a와 b가 같을 경우
			d[dIndex].col = a[aIndex].col;
			d[dIndex].row = a[aIndex].row;
			d[dIndex++].value = a[aIndex++].value + b[bIndex++].value;
			break;
		case 1: // a보다 b가 큰 경우
			d[dIndex].col = b[bIndex].col;
			d[dIndex].row = b[bIndex].row;
			d[dIndex++].value = b[bIndex++].value;
		}
	}

	for (; aIndex <= totala;) { // a 원소를 다 읽을 때까지 행렬 값 넣기
		d[dIndex].col = a[aIndex].col;
		d[dIndex].row = a[aIndex].row;
		d[dIndex++].value = a[aIndex++].value;
	}
	for (; bIndex <= totalb;) { // b 원소를 다 읽을 때까지 행렬 값 넣기
		d[dIndex].col = b[bIndex].col;
		d[dIndex].row = b[bIndex].row;
		d[dIndex++].value = b[bIndex++].value;
	}
	d[0].value = (dIndex - 2); // d 원소의 개수 : dIndex 마지막 ++dIndex 한 값과 0인덱스 값
}

