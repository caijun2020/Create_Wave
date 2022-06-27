/************************************************************************
Project 		:	create_wave
Version 		:	1.0
Update			:	2009-11-10
Modified by		:	Caijun < jun.cai@ge.com >
Company			:	GE Healthcare
	

Describe		:	将数据文件创建生成WAVE文件
						

************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* waveFilename = "test.wav";

typedef struct _RIFF_BLOCK
{
	unsigned int chunkID;			// Contains the letters "RIFF" in ASCII form
	unsigned int chunkSize;			// Size of the rest of the chunk following this number
	unsigned int format;			// Contains the letters "WAVE"
}RIFF_BLOCK;

typedef struct _FMT_BLOCK
{
	unsigned int subchunk1ID;		// Contains the letters "fmt "
	unsigned int subchunk1Size;		// 16 for PCM.  This is the size of the rest of the Subchunk which follows this number
	unsigned short audioFormat;		// PCM = 1 (i.e. Linear quantization). Values other than 1 indicate some form of compression
	unsigned short numChannels;		// Mono = 1, Stereo = 2, etc
	unsigned int   sampleRate;		// 8000, 44100, etc
	unsigned int   byteRate;		// SampleRate * NumChannels * BitsPerSample/8
	unsigned short blockAlign;		// NumChannels * BitsPerSample/8
	unsigned short bitsPerSample;	// 8 bits = 8, 16 bits = 16, etc
	
	unsigned short reserved;			
}FMT_BLOCK;

typedef struct _FACT_BLOCK
{
	unsigned int subchunk2ID;		// Contains the letters "fact"
	unsigned int subchunk2Size;		// Number of bytes in the fact
	unsigned int subchunk2Data; 	// Data of  bytes in the fact = 4 bytes data
}FACT_BLOCK;


//Standard WAV file header information
typedef struct _WavHeader
{
	unsigned int chunkID;			// Contains the letters "RIFF" in ASCII form
	unsigned int chunkSize;			// Size of the rest of the chunk following this number
	unsigned int format;			// Contains the letters "WAVE"
	
	unsigned int subchunk1ID;		// Contains the letters "fmt "
	unsigned int subchunk1Size;		// 16 for PCM.  This is the size of the rest of the Subchunk which follows this number
	unsigned short audioFormat;		// PCM = 1 (i.e. Linear quantization). Values other than 1 indicate some form of compression
	unsigned short numChannels;		// Mono = 1, Stereo = 2, etc
	unsigned int   sampleRate;		// 8000, 44100, etc
	unsigned int   byteRate;		// SampleRate * NumChannels * BitsPerSample/8
	unsigned short blockAlign;		// NumChannels * BitsPerSample/8
	unsigned short bitsPerSample;	// 8 bits = 8, 16 bits = 16, etc
	
	unsigned short reserved;		// 2byte reserve, but in the arm architecture, the reserved bytes should be 4
	
	unsigned int subchunk2ID;		// Contains the letters "fact"
	unsigned int subchunk2Size;		// Number of bytes in the fact
	unsigned int subchunk2Data; 	// Data of  bytes in the fact = 4 bytes data

	unsigned int subchunk3ID;		// Contains the letters "data"
	unsigned int subchunk3Size;		// Number of bytes in the data

} WavHeader;

WavHeader testWave;
static WavHeader *userWav = &testWave;

#define SIN_TABLE_LEN  ( 8000*10 )
unsigned int sine_wave_tal_stero[ SIN_TABLE_LEN ];

const int sine_val_tal_10[10] = { 0, 19259, 31163, 31163, 19259, 0, -19259, -31163, -31163, -19259 };
const int sine_val_tal_20[20] = { 0, 10125, 19259, 26509, 31163, 32766, 31163, 26509, 19259, 10125, 0, -10125, -19259, -26509, -31163, -32766, -31163, -26509, -19259, -10125 };

void initSineTable( void )
{
	unsigned int n;
	
	for( n=0; n < ( SIN_TABLE_LEN ) / 10; n++ )
	{
		sine_wave_tal_stero[ n ] = ( unsigned int )sine_val_tal_10[ n % 10 ];
		sine_wave_tal_stero[ (SIN_TABLE_LEN / 10) + n ] = ( ( ( unsigned int )sine_val_tal_20[ n % 20 ] ) << 16 ) & 0xffff0000;
	}

	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*2,  sine_wave_tal_stero ,sizeof( unsigned int)*(SIN_TABLE_LEN / 10) );
	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*3,  sine_wave_tal_stero+(SIN_TABLE_LEN / 10) ,sizeof( unsigned int )*(SIN_TABLE_LEN / 10) );

	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*4,  sine_wave_tal_stero ,sizeof( unsigned int)*(SIN_TABLE_LEN / 10) );
	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*5,  sine_wave_tal_stero+(SIN_TABLE_LEN / 10) ,sizeof( unsigned int )*(SIN_TABLE_LEN / 10) );

	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*6,  sine_wave_tal_stero ,sizeof( unsigned int)*(SIN_TABLE_LEN / 10) );
	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*7,  sine_wave_tal_stero+(SIN_TABLE_LEN / 10) ,sizeof( unsigned int )*(SIN_TABLE_LEN / 10) );

	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*8,  sine_wave_tal_stero ,sizeof( unsigned int)*(SIN_TABLE_LEN / 10) );
	memcpy( sine_wave_tal_stero + (SIN_TABLE_LEN / 10)*9,  sine_wave_tal_stero+(SIN_TABLE_LEN / 10) ,sizeof( unsigned int )*(SIN_TABLE_LEN / 10) );
}

void initWaveHeader( void )
{
	userWav->chunkID = 0x46464952;
	userWav->chunkSize = SIN_TABLE_LEN * sizeof( int ) + sizeof( WavHeader ) - 8;	// Size of the rest of the chunk following this number
	userWav->format = 0x45564157;					// Contains the letters "WAVE"
	
	userWav->subchunk1ID = 0x20746d66;				// Contains the letters "fmt "
	userWav->subchunk1Size = sizeof( FMT_BLOCK ) - 8;	// 16 for PCM.  This is the size of the rest of the Subchunk which follows this number
	userWav->audioFormat = 1;		// PCM = 1 (i.e. Linear quantization). Values other than 1 indicate some form of compression
	userWav->numChannels = 2;		// Mono = 1, Stereo = 2, etc
	userWav->sampleRate = 8000;		// 8000, 44100, etc
	userWav->byteRate = 32000;		// SampleRate * NumChannels * BitsPerSample/8
	userWav->blockAlign = 4;		// NumChannels * BitsPerSample/8
	userWav->bitsPerSample = 16;	// 8 bits = 8, 16 bits = 16, etc
	
	userWav->reserved = 0;			// 2byte reserve, but in the arm architecture, the reserved bytes should be 4
	
	userWav->subchunk2ID = 0x74636166;	// Contains the letters "fact"
	userWav->subchunk2Size = sizeof( FACT_BLOCK ) - 8;		// Number of bytes in the fact
	userWav->subchunk2Data = 0; 		// Data of  bytes in the fact = 4 bytes data

	userWav->subchunk3ID = 0x61746164;			// Contains the letters "data"
	userWav->subchunk3Size = SIN_TABLE_LEN * sizeof( int );	// Number of bytes in the data
}

static void DisplayWavInfo(void)
{
    printf( "\nWave file header information, header size = 0x%x\n\r", sizeof(WavHeader) );
	printf( "\nsizeof( unsigned short ) = 0x%x\n\r", sizeof(unsigned short) );
    printf( "--------------------------------\n\r");
    printf( "  - Chunk ID        = %c%c%c%c\n\r", userWav->chunkID, userWav->chunkID>>8, userWav->chunkID>>16, userWav->chunkID>>24 );
    printf( "  - Chunk Size      = 0x%08X\n\r", userWav->chunkSize);
    printf( "  - Format          = %c%c%c%c\n\r", userWav->format, userWav->format>>8, userWav->format>>16,userWav->format>>24);
    printf( "  - SubChunk ID     = %c%c%c%c\n\r", userWav->subchunk1ID, userWav->subchunk1ID>>8, userWav->subchunk1ID>>16, userWav->subchunk1ID>>24);
    printf( "  - Subchunk1 Size  = %d\n\r", userWav->subchunk1Size);
    printf( "  - Audio Format    = 0x%04X\n\r", userWav->audioFormat);
    printf( "  - Num. Channels   = %d\n\r", userWav->numChannels);
    printf( "  - Sample Rate     = %u\n\r", userWav->sampleRate);
    printf( "  - Byte Rate       = %u\n\r", userWav->byteRate);
    printf( "  - Block Align     = %d\n\r", userWav->blockAlign);
    printf( "  - Bits Per Sample = %d\n\r", userWav->bitsPerSample);		//bits.
	printf( "  - Subchunk2 ID    = %c%c%c%c\n\r",  userWav->subchunk2ID, userWav->subchunk2ID>>8, userWav->subchunk2ID>>16, userWav->subchunk2ID>>24 );
    printf( "  - Subchunk2 Size  = 0x%x\n\r", userWav->subchunk2Size);
    printf( "  - Subchunk3 ID    = %c%c%c%c\n\r", userWav->subchunk3ID, userWav->subchunk3ID>>8, userWav->subchunk3ID>>16, userWav->subchunk3ID>>24 );
    printf( "  - Subchunk3 Size  = 0x%x\n\r", userWav->subchunk3Size );
}

int main ( void )
{
	FILE	*fp_wave;
	unsigned int i;
	unsigned long crc = 0;

	if( ( fp_wave = fopen( waveFilename,"w+") ) == NULL )
	{
  		printf("Create %s file error!\n", waveFilename );
		exit (EXIT_FAILURE);
	}
	else
	{
  		printf("Create %s file successful!\n", waveFilename );
	}
  
	initWaveHeader();
	initSineTable();
  
	DisplayWavInfo();
  
	fwrite( userWav, sizeof( WavHeader ), 1, fp_wave );
	fwrite( sine_wave_tal_stero, sizeof( int )* SIN_TABLE_LEN, 1, fp_wave );
  
	fclose( fp_wave );
	return 1;
}
