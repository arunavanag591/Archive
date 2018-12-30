//written by Arunava Nag
//date_created:6th Feb; 2016.
//ifslib was used for 
//Functions used are= ifspin, ifscreate, fifs2farr, flDoG, ifsfgp, ifsfpp, ifspot. 

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include "ifs.h"
#include "flip.h"

using namespace std;
int main(){
	//Initialization of input images
	IFSIMG ipImg1, ipImg2; //itImg1, itImg2;
	cout<<"Input has been taken in for processing"<<endl;		
	int *len;
	int  maxCol, maxRow, rowVal, colVal;
	int valNew=0;	
	//int dimenVal=0;	
	ipImg1=ifspin((char *) "site8.c.ifs"); //reading in the image
	len = ifssiz(ipImg1);
	//dimenVal = len[0];//not required
	maxCol = len[1];
	maxRow =len[2];
	//creating ipImg2 with the same values as that of input ipImg1
	ipImg2=ifscreate((char *) "float", len, IFS_CR_ALL, 0);		 
	
	//perform Edge Detection using sobel operators
	//float h_x[3][3]= {{-1,0,1},{-2,0,2},{-1,0,1}};
	//float h_y[3][3]= {{-1,-2,-1},{0,0,0},{1,2,1}};
	for (int r=1; r<maxRow-1;r++){
		for(int c=1;c<maxCol-1;c++){
			rowVal=(0*ifsfgp(ipImg1,r,c))+ (-1*ifsfgp(ipImg1,r-1,c-1))+ (-2*ifsfgp(ipImg1,r,c-1))+(-1*ifsfgp(ipImg1,r+1,c-1))+(0*ifsfgp(ipImg1,r-1,c))+ (0*ifsfgp(ipImg1,r+1,c))+(1*ifsfgp(ipImg1,r-1,c+1))+(2*ifsfgp(ipImg1,r,c+1))+(1*ifsfgp(ipImg1,r+1,c+1));

			//ifsfpp(itImg1, r-1, c-1, rowVal);//for Checking purposes
			
			colVal=(0*ifsfgp(ipImg1,r,c))+ (-1*ifsfgp(ipImg1,r-1,c-1))+ (0*ifsfgp(ipImg1,r,c-1))+(1*ifsfgp(ipImg1,r+1,c-1))+(-2*ifsfgp(ipImg1,r-1,c))+ (2*ifsfgp(ipImg1,r+1,c))+(-1*ifsfgp(ipImg1,r-1,c+1))+(0*ifsfgp(ipImg1,r,c+1))+(1*ifsfgp(ipImg1,r+1,c+1));

			//ifsfpp(itImg2, r-1, c-1, colVal);//for Checking purposes

			valNew=sqrt((rowVal*rowVal)+(colVal*colVal));
			//applying the threshold for edgeDetection
			if (valNew>255)
			ifsfpp(ipImg2,r-1,c-1,255);
			else
			ifsfpp(ipImg2,r-1,c-1,0);
		}
	}

	//Creating the 3D Accumulator

	float acc[128][128][100];
	float radiusCircle=0.0;
	for (int i=0;i<maxRow;i++){
		for(int j=0;j<maxCol;j++){
			for(int k=0; k<maxCol;k++){			
				acc[i][j][k]=0;
			}	
		}
	}
	//Storing in 3D accumulator with respect to radius
	int value=0;
	int newCount=0;
	for (int x=0;x<maxRow;x++){
		for(int y=0;y<maxCol;y++){
			value=ifsfgp(ipImg2,x,y);
			if(value==255){
				for (int i=0;i<maxRow;i++){
					for(int j=0;j<maxCol;j++){	
						radiusCircle=sqrt(((x-i)*(x-i))+((y-j)*(y-j)));
						acc[i][j][newCount]=radiusCircle;
					}
				}
				newCount++;
			}
		}
	}

	//	cout<<"Total number of edge points="<<newCount<<endl; 

	int flag1,flag2,cPoint1,cPoint2;
	//initializing to zero
	flag1=flag2=cPoint1=cPoint2=0;
	float newRadius=0.0;
	
	for (int i=0;i<maxRow;i++){
		for(int j=0;j<maxCol;j++){
			for(int x=0;x<newCount;x++){
				for(int y=0;y<newCount;y++){
					if(((acc[i][j][x]-acc[i][j][y])*(acc[i][j][x]-acc[i][j][y]))<=0){
						flag1=flag1+1;
						if(flag1>=flag2){
							flag2=flag1;
							cPoint1=i;
							cPoint2=j;
							newRadius=acc[i][j][x];
						}
					}
				}
					flag1=0;			
			}	
		}
	}
	
	cout<<"Center of the circle is"<<cPoint1<<","<<cPoint2<<endl;
	ifsfpp(ipImg1,cPoint1,cPoint2,255);

	float displayRadius=0.0;
	for(int i=0; i<maxRow;i++){
		for(int j=0;j<maxCol;j++){
			displayRadius=sqrt(((i-cPoint1)*(i-cPoint1))+((j-cPoint2)*(j-cPoint2)));
			if(((displayRadius-newRadius)*(displayRadius-newRadius))<=0.25)
					ifsfpp(ipImg1,i,j,255);
		}	
	}
	
	ifspot(ipImg1,(char *) "result.ifs");

	ifspot(ipImg2,(char *) "edgeDet.ifs");

	ifsfree (ipImg1, IFS_FR_ALL ) ;
	ifsfree (ipImg2, IFS_FR_ALL ) ;
	cout<<"All Done. Check the Result"<<endl;

	//second method using perpendicular bisectors
	/*float val=0.0;
	float a[3],b[3],x[3],y[3];
	a[3]=b[3]=x[3]=y[3]={0};
	int flag=0;
	for (int i=0;i<maxRow;i++){
		for(int j=0; j<maxCol;j++){
			val=ifsfgp(ipImg2,i,j);
			if(val==255 && count<3){
				a[flag]=i;
				b[flag]=j;
				flag++;
			}
		}
	}
	x[0]=((b[1]-b[0])/(a[1]-a[0]));
	y[0]=tan(atan(x[0])+90);
	x[1]=((b[2]-b[1])/(a[2]-a[1]));
	y[1]=tan(atan(x[1])+90);

	double mCal1=(a[0]+a[1])/2;
	double nCal1=(a[1]+a[2])/2;
	double mCal2=(b[0]+b[1])/2;
	double nCal2=(b[1]+b[2])/2;
	
	double var1=0,var2=0;
	float detect=y[0]+y[1];
	if(detect!=0){
		var1=(((y[0]*mCal1)-mCal2)+((y[1]*nCal1)-nCal2));
		var2=(((y[0]*nCal1)-nCal2)*y[0])-(y[1]*((y[1]*mCal1)-mCal2));
	}
	else
	{
		cout<<"failure"<<endl;
	}
	cout<<endl<<"The Center is "<<var1<<","<<var2<<endl;

	float perRadius=0.0;
	perRadius=sqrt(((var1-a[0])*(var1-a[0]))-((var2-b[0])*(var2-b[0])));
	cout<<endl<<"The radius is"<<perRadius<<endl;*/
}

