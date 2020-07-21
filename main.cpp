#include<opencv2/opencv.hpp>
#include<iostream>
#include<math.h>
#include<list>
#include<string>

using namespace std;
using namespace cv;

#define ROWS 240
#define COLS 320

#define RADIUS_MAX 100
#define RADIUS_MIN 20

#define OL 1
#define HOUGH 1

//int P[ROWS][COLS][RADIUS_MAX - RADIUS_MIN];

struct coord{
	int row, col,r;
};

list <coord> ***P;

bool surrounding(int row, int col, int r);
void Hough(const Mat& img);
void getVotes(const Mat& img, const int row, const int col);
void clearP();
double err(int i, int j, int r);

ostream& operator <<(ostream &os, const coord& a){
	os << '('<<a.row<<", "<<a.col<<')';
	return os;
}




int main(){

	coord a = {1,10};
	a = {2,5};
	cout<<a.col<<endl;

	cout<<"32"<<endl;

//	list <coord> ***P;
	P = new list<coord>**[ROWS];
	for(int i=0; i<ROWS; i++){
		P[i] = new list<coord>*[COLS];
		for(int j=0; j<COLS; j++){
			P[i][j] = new list<coord>[RADIUS_MAX-RADIUS_MIN];
		}
	}


	Mat *clear = new Mat[9];
	Mat *out = new Mat[9];//for making a red circle

	clear[0] = imread("circles/c1.pgm", 0);
	clear[1] = imread("circles/c2.pgm", 0);
	clear[2] = imread("circles/c3.pgm", 0);
	clear[3] = imread("circles/c4.pgm", 0);
	clear[4] = imread("circles/c5.pgm", 0);
	clear[5] = imread("circles/c6.pgm", 0);
	clear[6] = imread("circles/overlap1.pgm", 0);
	clear[7] = imread("circles/overlap2.pgm", 0);
	clear[8] = imread("circles/overlap3.pgm", 0);

	out[0] = imread("circles/c1.pgm", 1);
	out[1] = imread("circles/c2.pgm", 1);
	out[2] = imread("circles/c3.pgm", 1);
	out[3] = imread("circles/c4.pgm", 1);
	out[4] = imread("circles/c5.pgm", 1);
	out[5] = imread("circles/c6.pgm", 1);
	out[6] = imread("circles/overlap1.pgm", 1);
	out[7] = imread("circles/overlap2.pgm", 1);
	out[8] = imread("circles/overlap3.pgm", 1);


//	String loc = "result/outline";
	char pgm[5] = {'0','.','p','g','m'};
	char ppm[5] = {'0','.','p','p','m'};
	

	Scalar red = Scalar(0,0,255);

//	Mat test;
	for(int ind=0; ind<9; ind++){
		char n=(char)ind+'0';
		if(OL==1){
			GaussianBlur(clear[ind], clear[ind], Size(13,13), 0,0);
			Canny(clear[ind], clear[ind], 10, 40);

			pgm[0] = n;
			String loc = "result/outline";
			loc += pgm;
			imwrite(loc, clear[ind]);
		}
		if(HOUGH==1){
			String loc_ = "result/circled";
			ppm[0] = n;
			loc_ += ppm;
			Hough(clear[ind]);
			for(int i=0; i<ROWS; i++){
				for(int j=0; j<COLS; j++){
					for(int r=0; r<RADIUS_MAX-RADIUS_MIN; r++){	
						if(P[i][j][r].size() > 15){
							if(surrounding(i,j,r)){
								Point pt=Point(j,i);
								circle(out[ind], pt, r+20, red, 2);
								cout<<"In image ("<<ind<<"), circle found at ["<<i<<"]["<<j<<"] with radius "<<r+RADIUS_MIN<<endl;
								cout<<"\tError: "<<err(i,j,r)<<endl;
							}
						}
					}
				}
			}
			imwrite(loc_, out[ind]);
		}
		cout<<endl;
	}

	cout<<"Works at least"<<endl;

	waitKey(0);

	delete[] clear;
	delete[] out;

	for(int i=0; i<ROWS; i++){
		for(int j=0; j<COLS; j++){
			delete[] P[i][j];
		}
		delete[] P[i];
	}
	delete[] P;

	return 0;
}

double err(int i, int j, int r){
	double di=0;
	int size = P[i][j][r].size();
	for(int x=0; x<size; x++){
		di += abs(sqrt((P[i][j][r].front().row-i)*(P[i][j][r].front().row-i) + (P[i][j][r].front().col-j)*(P[i][j][r].front().col-j))-(r+20));
		P[i][j][r].pop_front();
	}
	di /= size;
	return di;
}

bool surrounding(int row, int col, int r){
	unsigned int thresh = P[row][col][r].size();
	for(int i=0; i<10; i++){
		for(int j=0; j<10; j++){
			for(int k=0; k<10; k++){
				if(row+i < ROWS && col+j < COLS){
					if(P[row+i][col+j][r+k].size() > thresh) return false;
				}
				if(row-i >= 0 && col-j >= 0){
					if(P[row-i][col-j][r+k].size() > thresh) return false;
				}
			}
		}
	}
	return true;
}

void Hough(const Mat& img){
	clearP();
	for(int i=0; i<img.rows; i++){
		for(int j=0; j<img.cols; j++){
			getVotes(img, i, j);
		}
	}
}

void getVotes(const Mat& img, const int row, const int col){
	for(int r=RADIUS_MIN; r<RADIUS_MAX; r++){
		for(float theta=0.0; theta<2*M_PI; theta+=M_PI/16){
			int x=col+r*cos(theta);
			int y=row+r*sin(theta);

			if(y>=0 && x<COLS && y>=0 && y<ROWS){
				if(img.at<uchar>(y, x) != 0){
					coord temp = {y,x,r};
					P[row][col][r-RADIUS_MIN].push_back(temp);
				}
			}
		}
	}
}

void clearP(){
	for(int i=0; i<ROWS; i++){
		for(int j=0; j<COLS; j++){
			for(int k=0; k<RADIUS_MAX-RADIUS_MIN; k++){
				P[i][j][k].clear()	;
			}
		}
	}
}
