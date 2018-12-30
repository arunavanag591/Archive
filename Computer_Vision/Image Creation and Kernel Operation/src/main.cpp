#include <iostream>
#include <stdio.h>
#include "math.h"
#include "ifs.h"
#include "flip.h"

using namespace std;
void weightMultiply (float matrix[3][3], float weight)
{
    int row, column;
    for ( row = 0; row < 3; ++row )
        for ( column = 0; column < 3; ++column )
            matrix[row][column] *= weight;
}
void copyMatrix(float matrix1[3][3],float matrix2[3][3])
{
	int row, column;
	for ( row = 0; row < 3; ++row )
    	for ( column = 0; column < 3; ++column )
			matrix2[row][column]=matrix1[row][column];
}

void displayMatrix (float matrix[3][3])
{
    int row, column;
    for ( row = 0; row < 3; ++row) {
        for ( column = 0; column < 3; ++column )
            cout<<matrix[row][column]<<"\n";
        cout<<"\n";
    }
}

template <typename T, size_t N> inline
size_t sizeOfMatrix( const T(&)[ N ] )
{
  return N;
}

int main(){	
//Part 1(a)
	//initialize image variable
	cout << "Initializing" << endl;
	IFSIMG image1;
	int len[3];
	//image creation using ifscreate
	cout<<"image creation of 64*64 size"<<endl;
	len[0]=2;
	len[1]=64;
	len[2]=64;
	image1=ifscreate((char*) "double",len, IFS_CR_ALL,0);
	if (image1==NULL){
		cout<<"creation failure"<<endl;
	}
	//customization as specified
	cout<<"customization as required"<<endl;	
	double value0 = 10;
	double value1 = 100;
	double value2 = 150;
	double value3 =  50;
	double value4 =  20;
	
    for (int row = 0; row < 64; row++) {
		for (int col = 0; col < 64; col++) {
                     ifsfpp(image1, row, col, value0);
        }
    }
	
   	for (int row1 = 16; row1 < 32; row1++) {
        for (int col1 = 16; col1 < 32; col1++) {
                     ifsfpp(image1, row1, col1, value1);
        }
    }
   	for (int row2 = 16; row2 < 32; row2++) {
        for (int col2 = 32; col2 < 48; col2++) {
                     ifsfpp(image1, row2, col2, value2);
        }
    }
   	for (int row3 = 32; row3 < 48; row3++) {
        for (int col3 = 16; col3 < 32; col3++) {
            	     ifsfpp(image1, row3, col3, value3);
        }
    }
   	for (int row4 = 32; row4 < 48; row4++) {
        for (int col4 = 32; col4 < 48; col4++) {
           	     ifsfpp(image1, row4, col4, value4);
        }
    }

    	// Write contents of image1 to disk
    cout << "Saving the result as SYNTH1.ifs" << endl;
    ifspot(image1,(char *) "SYNTH1.ifs");

//Part 1(b)
	IFSIMG image2,image3,imagesh;
	cout<<"reading the created image"<<endl;
	//image2=ifspin((char *) "SYNTH1.ifs");
	float ker_mat[3][3]= {1,1,1,1,2,1,1,1,1};
	weightMultiply (ker_mat,0.1);
	float w_mat[3][3];	
	copyMatrix(ker_mat,w_mat);
		
	int *length;
	length=ifssiz(image1);	
	//int dim=length[0];
	int col_im1 = length[1];
    int row_im1 = length[2];
	int im2_ln[3];
	im2_ln[0]=2;
	im2_ln[1]=66;
	im2_ln[2]=66;
	
	cout << "Creating image2 with the same size as image1" << endl;
    image2 = ifscreate((char *) "double", im2_ln, IFS_CR_ALL, 0);
	image3 = ifscreate((char *) "double", length, IFS_CR_ALL, 0);
	//share variable	
	imagesh=ifscreate((char *) "double", im2_ln, IFS_CR_ALL, 0);
	cout << "Copying the contents of image1 to image2" << endl;
    double value= 0;
	double new_val=0;
    for (int rowx = 0; rowx < row_im1; rowx++) {
        for (int colx = 0; colx < col_im1; colx++) {
            // Read the value in (row, col) from image1 and save it in image2
            value = ifsfgp(image1, rowx, colx);
            
			for (int i=1;i<65;i++)
				for(int j=1;j<65;j++)
			ifsfpp(image2, rowx, colx, value);
        }
    }	
	ifspot(image2,(char *) "SYNTH_Pad.ifs");
	int *length1;
	length1=ifssiz(image2);	
	int col_im2 = length1[1];
    int row_im2 = length1[2];	
	float sum=0.0;	
	for(int i=1; i<row_im2-1;i++){
		for (int j=1;j<col_im2-1;j++){
				sum=0.0;
				
			for (int a=-1;a<=1;a++){
				for(int b=-1;b<=1;b++){
					
					sum = sum + w_mat[a+1][b+1]*ifsfgp(image2,i-b, j-a);					
				}		
			}
			ifsfpp(imagesh,i,j,sum);			
		}
	}
	
	//intermediate storage
	ifspot(imagesh,(char *) "INTER.ifs");
	//copying imagesh to image3 to original resolution
	for (int i=0; i<64;i++){
		for(int j=0;j<64;j++){
			new_val = ifsfgp(imagesh,i,j);
			ifsfpp(image3, i, j, new_val);
		}
	
	}
	cout << "Writing the contents of image to the disk as BLUR1.ifs" << endl;
    ifspot(image3,(char *) "BLUR1.ifs");

//Part1(c)
	//gauss -i BLUR1.ifs -o BLUR1.V1.ifs -m 0 -s 3

//Part2
	float h_x[3][3]= {{-1,0,1},{-2,0,2},{-1,0,1}};
	//creating image variables
	IFSIMG image4, imagesh2,imagesh3;
	image4=ifspin((char *) "SYNTH_Pad.ifs");	
	int *newlen;	
	newlen = ifssiz(image4);
    int col_im4 = newlen[1];
    int row_im4 = newlen[2];
	//copying the 66*66 image in new image vars-> image 4,image 5
	
	imagesh2=ifscreate((char *) "double", newlen, IFS_CR_ALL, 0);
	imagesh3=ifscreate((char *) "double", newlen, IFS_CR_ALL, 0);
	double value_a;
	 for (int rowx = 0; rowx < row_im4; rowx++) {
        for (int colx = 0; colx < col_im4; colx++) {
    //putting the contents of image 4 in shared image
            value_a = ifsfgp(image4, rowx, colx);
			ifsfpp(imagesh2, rowx, colx, value_a);			
        }
    }
	double v = 0;
    for (int row = 0; row < row_im4; row++) {
        for (int col = 0; col < col_im4; col++) {
            // Read the value in (row, col) from image1 and save it in image2
            v = ifsfgp(imagesh2, row, col);
            ifsfpp(imagesh3, row, col, v);
        }
    }
	
	
	
	IFSIMG image5;
	int im5_ln[3];
	im5_ln[0]=2;
	im5_ln[1]=66;
	im5_ln[2]=66;
	image5=ifscreate((char *) "double", im5_ln, IFS_CR_ALL, 0);
	//running the kernel h_x in on image share 2
	float sum1=0.0;
	for(int i=1; i<row_im4-1;i++){
		for (int j=1;j<col_im4-1;j++){
				sum1=0.0;
				//sum2=0.0;	
			for (int a=-1;a<=1;a++){
				for(int b=-1;b<=1;b++){					
					sum1 = sum1 + h_x[a+1][b+1]*ifsfgp(imagesh2,i-b, j-a);					
				}		
			}
			//cout<<"the sum1 value:"<<sum1<<endl;
			ifsfpp(image5,i,j,sum1);			
		}
	}
	IFSIMG image6;
	image6=ifscreate((char *) "double", length, IFS_CR_ALL, 0);
	for (int i=0; i<64;i++){
		for(int j=0;j<64;j++){
			new_val = ifsfgp(image5,i,j);
			ifsfpp(image6, i, j, new_val);
		}	
	}
	ifspot(image6,(char *) "after_hx.ifs");		//**************need to take out
	IFSIMG image7,image8;
	image7=ifscreate((char *) "double", im5_ln, IFS_CR_ALL, 0);
	float h_y[3][3]= {-1,-2,-1,0,0,0,1,2,1};
	float sum2=0.0;
	for(int i=1; i<row_im4-1;i++){
		for (int j=1;j<col_im4-1;j++){
				sum2=0.0;	
			for (int a=-1;a<=1;a++){
				for(int b=-1;b<=1;b++){					
					sum2 = sum2 + h_y[a+1][b+1]*ifsfgp(imagesh3,i-b, j-a);					
				}		
			}
			//cout<<"the sum1 value:"<<sum1<<endl;
			ifsfpp(image7,i,j,sum2);
		}
	}
	image8=ifscreate((char *) "double", length, IFS_CR_ALL, 0);
	for (int i=0; i<64;i++){
		for(int j=0;j<64;j++){
			new_val = ifsfgp(image7,i,j);
			ifsfpp(image8, i, j, new_val);
		}	
	}
	ifspot(image8,(char *) "after_hy.ifs");		//**************need to take out



	//doing the square summation and finding the square root of the summation	
	//operation storing array created	
	IFSIMG image9;
	image9=ifscreate((char *) "double", length, IFS_CR_ALL, 0);
	double value_row=0;
	double value_col=0;
	double new_valc=0;
	
	for (int i=0;i<64;i++){
		for (int j=0; j<64; j++){
			value_row= ifsfgp(image6,i,j);
			value_col=ifsfgp(image8,i,j);
			new_valc= sqrt(pow(value_row,2)+pow(value_col,2));
			ifsfpp(image9,i,j,new_valc);
		}
	}
	ifspot(image9,(char *) "final_cal.ifs");		//**************need to take out
	IFSIMG image10;
	double thresVal=0;
	double setVal=45;	
	image10=ifscreate((char *) "double", length, IFS_CR_ALL, 0);
	for (int i=0;i<64;i++){
		for (int j=0; j<64; j++){
			thresVal= ifsfgp(image9,i,j);
				
				if (thresVal>setVal)
					ifsfpp(image10,i,j,1);
				else
					ifsfpp(image10,i,j,0);				
				
		}
	}
	cout<<"After finding the square root of the sum, outputing the final image"<<endl;
	ifspot(image10,(char *) "thresholded_image.ifs");
}