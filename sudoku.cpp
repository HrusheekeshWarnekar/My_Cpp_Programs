#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#define print(x) std::cout << x << std::endl;
#define LOG 0

int isSolved(int** matrix){
	for(int i=0; i<9; i++){
		for(int j=0; j<9; j++){
			if(matrix[i][j] == 0) return 0;
		}
	}
	return 1;
}

void log(int** matrix) {
	for(int t=0; t<25; t++){
		std::cout << "-";
	}

	std::cout << "\n";

	for(int j = 8; j >= 0; j--){
		for(int i=0; i<9; i++){
			if(i%3 == 0) std::cout << "| ";
			if(matrix[i][j] == 0) std::cout << ". ";
			else std::cout << matrix[i][j] << " ";
		}

		std::cout << "|\n";

		if(j%3 == 0){
			for(int t=0; t<25; t++){
				std::cout << "-";
			}

			std::cout << "\n";
		}
	}
}

enum class Group {horizontal, vertical, square};

void pencil(int* list, int ** matrix, int x, int y){
	for (int i=0; i<9; i++) {
		list[i] = 1;
	}

	for(int i=0; i<9; i++){
		int x1 = x - (x%3);
		int y1 = y - (y%3);
		
		int x2 = i/3;
		int y2 = i%3;

		int a = matrix[x][i];
		int b = matrix[i][y];
		int c = matrix[x1+x2][y1+y2];

		if(a) { list[a-1] = 0; }
		if(b) { list[b-1] = 0; }
		if(c) { list[c-1] = 0; }
	}
}

int isSolvable(int** matrix){
	int pList[9], s=0;
	for(int i=0; i<9; i++){
		for(int j=0; j<9; j++){
			if(!matrix[i][j]) {
				pencil(pList, matrix, i, j);
				for(int k=0; k<9; k++){
					if(pList[k]) s=1;
				}
				if(!s) {
					#if LOG
						std::cout << "Unsolvable at (" << i+1 << ", " << j+1 << ")\n";
					#endif

					return 0;
				}	
			}
		}
	}

	return 1;
}

void coords(Group g, int* x, int* y, int i, int t){
	if(g == Group::horizontal){
		(*x) = i;
		(*y) = t;
	} else if(g == Group::vertical){
		(*x) = t;
		(*y) = i;
	} else {
		int x1 = t/3;
		int y1 = t%3;
		int x2 = i/3;
		int y2 = i%3;

		(*x) = 3*x1 + x2;
		(*y) = 3*y1 + y2;
	}
}

void eliminate(int* list, int** matrix, Group g, int t, int n){
	int x, y;

	for (int i=0; i<9; i++)	{
		list[i] = 1;
	}

	for(int i=0; i<9; i++) {
		coords(g, &x, &y, i, t);

		if(matrix[x][y] == n){
			for(int i=0; i<9; i++){
				list[i] = 0;
			}
			break;
		}

		if(matrix[x][y]) { list[i] = 0; }
		else {
			int list2[9];

			pencil(list2, matrix, x, y);
			if(!list2[n-1]) list[i] = 0;
		}
	}

	for(int i=0; i<9; i++){
		list[i] = list[i];
	}
}

int solve_pencil(int** matrix){
	int solved=0, list[9];
	for(int i=0; i<9; i++){
		for(int j=0; j<9; j++){
			if(!matrix[i][j]){
				int a=0, num=0;

				pencil(list, matrix, i, j);

				for (int k=0; k<9; ++k) {
					if(list[k]) num=k+1;
					a += list[k];
				}

				if(1==a){
					matrix[i][j]=num;
					solved=1;

					#if LOG
						std::cout << num << " at (" << i+1 << ", " << j+1 << "). using pencil.\n";
					#endif
				}
			}
		}
	}

	return solved;
}

int solve_elim(int** matrix, Group g){
	int solved=0, list[9], x, y;
	for(int i=0; i<9; i++){
		for(int n=1; n <=9; n++){
			int a=0, pos=0;

			eliminate(list, matrix, g, i, n);

			for(int j=0; j<9; j++){
				if(list[j]) { pos=j; }
				a += list[j];
			}

			if(1==a){
				coords(g, &x, &y, pos, i);
				matrix[x][y] = n;
				solved=1;

				#if LOG
					std::cout << n << " at (" << x+1 << ", " << y+1 << "). ";
					if(g == Group::horizontal) std::cout << "using _horizontal.\n";
					else if(g == Group::vertical) std::cout << "using _vertical.\n";
					if(g == Group::square) std::cout << "using _square.\n";
				#endif
			}
		}
	}

	return solved;
}

void copy(int** matrix, int** newMatrix) {
	for(int i=0; i<9; i++) {
		for(int j=0; j<9; ++j) {
			(newMatrix[i])[j] = (matrix[i])[j];
		}
	}
}

void getFirstEmptyCoords(int** matrix, int* x, int* y){
	for(int i=0; i<9; ++i) {
		for(int j=0; j<9; ++j) {
			if(!matrix[i][j]) {
				(*x) = i;
				(*y) = j;
				return;
			}
		}
	}
}

void solve(int** matrix) {
	int i=0, fails=0, s=1;
	while(fails != 4) {
		if(0==i) {
			s = solve_pencil(matrix);
		} else if(1==i) {
			s = solve_elim(matrix, Group::square);
		} else if(2==i) {
			s = solve_elim(matrix, Group::horizontal);
		} else if(3==i) {
			s = solve_elim(matrix, Group::horizontal);
		}
		if(s){
			fails = 0;
		} else {
			i += 1;
			fails += 1;
			i %= 4;
		}
	}

	if(!isSolved(matrix) && isSolvable(matrix)) {
		int newSudoku[9][9], pList[9], x, y;

		int* columns[9];
		for(int i=0; i<9; ++i){
			columns[i] = newSudoku[i];
		}
		int** newMatrix = columns;

		copy(matrix, newMatrix);

		getFirstEmptyCoords(matrix, &x, &y);

		pencil(pList, matrix, x, y);

		for (int i=0; i<9; ++i) {
			if(pList[i]){
				newMatrix[x][y] = i+1;
				#if LOG
					std::cout << "Trying " << i+1 << " at (" << x+1 << ", " << y+1 << ").\n";
				#endif

				solve(newMatrix);

				if(isSolved(newMatrix)) {
					copy(newMatrix, matrix);
					return;
				} else {
					copy(matrix, newMatrix);
				}
			}
		}
	}
}

int main(int argc, char* argv[]){
	int mySudoku[9][9];
	int* columns[9];
	for(int i=0; i<9; ++i){
		columns[i] = mySudoku[i];
	}
	int** matrix = columns;
	memset(mySudoku, 0, sizeof(int)*81);

	// std::string filename = "sudoku.txt";
	std::ifstream myfile(argv[1]);

	std::string line;
	for(int y=8; getline(myfile, line); y--){
		for(int x=0; x<9; x++){
			int n = line[x];
			n -= '0';
			mySudoku[x][y] = n;
		}
	}

	myfile.close();
	log(matrix);

	print("");

	solve(matrix);
	log(matrix);

	std::cin.get();
	return 0;
}

