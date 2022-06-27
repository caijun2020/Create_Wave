/************************************************************************
Project 		:	create_dds_sine
Version 		: 1.0
Create			:	2011-03-18
Complete		：2011-03-18
Update			：
Modified by	：Caijun < jun.cai@ge.com >
Company			:	GE Healthcare
	

Describe	: 将Sine函数的抽样数据生成hex文件,用于FPGA内部的ROM做初始化
						

************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

const char* sineWaveFilename = "sine.txt";
#define SAMPLE_SIZE  1024
short sineWave[ SAMPLE_SIZE ];

#define PI 3.1415926

int main ( void )
{
	FILE	*fp_sine;
	unsigned int i;
	unsigned long crc = 0;

  if( ( fp_sine = fopen( sineWaveFilename,"w+") ) == NULL )
  {
  	printf("Create %s file error!\n", sineWaveFilename );
		exit (EXIT_FAILURE);
  }
  else
  {
  	printf("Create %s file successful!\n", sineWaveFilename );
  }
  
  for( i = 0; i < SAMPLE_SIZE; i++ )
  {
  	sineWave[ i ] = sin( ( 2 * PI / SAMPLE_SIZE ) * i ) * 2047 + 2048;
  	//sineWave[ i ] = 4095;
  	if( i % 10 == 0 )
  	printf("%s() sineWave[%d] = %ld\n", __FUNCTION__, i, sineWave[ i ] );
  	fprintf( fp_sine, "	%d	:	%d;\n", i, sineWave[i] );
  }
  
  //fwrite( sineWave, sizeof(short), SAMPLE_SIZE, fp_sine );
  
  fclose(fp_sine);
}
