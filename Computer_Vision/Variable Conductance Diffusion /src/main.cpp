//written by Arunava Nag
//date_created:6th Feb; 2016.
//ifslib was used for noise removal and edge preserving purpose.
//Functions used are= ifspin, ifscreate, fifs2farr, flDoG, ifsfgp, ifsfpp, ifspot. 

#include <iostream>
#include <stdio.h>
#include <math.h>
#include "ifs.h"
#include "flip.h"

using namespace std;
int main(){
	//intializing the IMGHDR* 
	IFSIMG image1,addimg,temp1,temp2,temp3,temp4,xout,yout; 
	cout<<"Input has been taken in for processing"<<endl;	
	image1=ifspin((char *) "angio.ifs"); //reading in the image
	
	int *len;
	float tau=2.0; 		//value of tau
	float alpha=1.0;	//value of aplha
	int iteration=300;	//set the number of iteration here	
	//cout<<"The value of tau set is = "<<tau<<"\n"<<"The value of alpha set is = "<<alpha<<"\n"<<"The number of iteration set is = "<<iteration<<endl;
	float cscal=0;	
	len=ifssiz(image1);
	addimg=ifscreate((char *) "float", len, IFS_CR_ALL, 0);		 	
		int col = len[1];
    	int row = len[2];
	//getting the image pointer for addimg, for faster execution		
	float value1 = 0;
    float **addimgP = fifs2farr(addimg);

    for (int i = 0; i < row ; i++){ 
        for (int j = 0; j < col; j++){ 
		    value1 = ifsfgp(image1, i, j);
            addimgP[i][j] = value1;
        }	
	}
	
	//creating intermediate image variables of similar configuration
	temp1=ifscreate((char *) "float", len, IFS_CR_ALL, 0);
	temp2=ifscreate((char *) "float", len, IFS_CR_ALL, 0);
	temp3=ifscreate((char *) "float", len, IFS_CR_ALL, 0);	
	temp4=ifscreate((char *) "float", len, IFS_CR_ALL, 0);	
	xout=ifscreate((char *) "float", len, IFS_CR_ALL, 0);		
	yout=ifscreate((char *) "float", len, IFS_CR_ALL, 0);	
	cout<<"Initialization Done"<<endl;
	cout<<"Processing..."<<endl;
	
	for(int i=0;i<iteration;i++){
		//tau=tau+0.006;
		//cout<<"tau="<<tau<<endl;
		//alpha=alpha+0.003;
		//cout<<"alpha="<<alpha<<endl;		
		//first part of the operation to find the C(x,y)									
		flDoG(addimg,temp1,2.0,1,0);//derivative towards x		
		flDoG(addimg,temp2,2.0,1,1);//derivative towards y
		for(int i=0;i<col;i++){
			for(int j=0;j<row;j++){
				float A=ifsfgp(temp1,j,i);
				float B=ifsfgp(temp2,j,i);
				cscal=exp(-((A*A)+(B*B))/tau);//calculating c 

				float Amult=cscal* A;
				float Bmult=cscal* B;
				ifsfpp(xout, j, i, Amult);//assigning the calculated values
				ifsfpp(yout, j, i, Bmult);//assigning the calculated values
			}
		}	
		//executing the rest of the equation of f (x, y) ← f (x, y)+0.01∇ T (c(x, y)∇f (x, y))	
		
		flDoG(xout,temp3,2.0,1,0);//derivative towards x		
		flDoG(yout,temp4,2.0,1,1);//derivative towards y
		
		for(int i=0;i<col;i++){
			for(int j=0;j<row;j++){
				float X = ifsfgp(temp3,j,i);
				float Y = ifsfgp(temp4,j,i);
				//FINAL OPERATION
				float newValue= ifsfgp(addimg,j,i)+(alpha*(X+Y));	//finding the new value to add to the original image
				ifsfpp(addimg, j, i, newValue);//adding to the final timage.
			}
		}
	}
	cout<<"Output created"<<endl;
	ifspot(addimg,(char *) "vcd.ifs");
	cout<<"Freeing Image Memory for better system performance"<<endl;	
	//freeing memory
	free(len);
	ifsfree(image1, IFS_FR_ALL);
	ifsfree(temp1, IFS_FR_ALL);
	ifsfree(xout, IFS_FR_ALL);
	ifsfree(temp2, IFS_FR_ALL);
	ifsfree(yout, IFS_FR_ALL);
	ifsfree(temp3, IFS_FR_ALL);
	ifsfree(temp4, IFS_FR_ALL);
	ifsfree(addimg, IFS_FR_ALL);
	cout<<"Check output file named vcd.ifs"<<endl;
	
}

