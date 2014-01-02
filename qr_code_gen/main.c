#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "qr_table.h"


char* convert_to_bin(unsigned int num ) //convert int to binary
{
	int i;
	char *str_bin;
	if (num < 128)
	{
		i=7;
		str_bin = ( char* )calloc( 8, sizeof( char )); //bin str
		memset( str_bin, (char)((int)'0'), 8 ); //initializing array
	}
	else
	{
		i=15;
		str_bin = ( char* )calloc( 16, sizeof( char )); //bin str
		memset( str_bin, (char)((int)'0'), 16 ); //initializing array
	}
	while ( num > 0 )
	{
		str_bin[i] += num%2;
		i--;
		num/=2;
	}
	return str_bin;
}

char* convert_to_utf8( char *str ) //convert int to utf8 binary
{
	int i;
	char *str_bin;
	str_bin = ( char* )calloc( strlen( str ) * 8 + 1, sizeof( char )); //for bin str
	for ( i = 0; i < strlen( str ); i++ )
		memcpy( str_bin + i*8, convert_to_bin( str[i] ), 8 * sizeof( char )); //copy binary code
	str_bin[ strlen( str ) * 8 + 1 ] = '\0'; //add end of str
	return str_bin;
}

int optimal_version ( int size ) //chosing optimal version
{
	int i;
	int version;
	int buffer = 0;

	buffer = fabs( ( float )( size - size_of_information[1] ) );
	version = 1;
	for ( i = 2; i <= sizeof( size_of_information ); i++ )
		if (fabs( ( float )( size - size_of_information[i] ) ) < buffer )
		{
			buffer = fabs( ( float )( size - size_of_information[i] ) );
			version = i;
		}
		else
		{
			break;
		}
	return version;
}

char*  add_service_inf(char *str, int* ver) //service information and addition string to full size
{
	int data = 4; //service data (bit) = coding method(4 bit) + size of data(8 or 16 bit) 
	int zero;
	int extra; //extra byte
	int buffer;
	int i;
	char *str_full;
	char *zero_str;

	if (*ver <= 9) //length of data field
		data+=8;
	else 
		data+=16;

	if ( ( strlen( str ) + data ) > size_of_information[*ver] ) //if big size
	{
		(*ver)++;
		data = 4;
		if (*ver <= 9) //length of data field again, because version changed
			data+=8;
		else 
			data+=16;
	}


	zero = (strlen( str ) + data )%8; //number of zero until str%8 != 0
	zero_str = (char*)calloc( zero, sizeof(char));
	memset( zero_str, (char)((int)'0'), zero ); //initializing array

	buffer = (strlen( str ) + data + zero - size_of_information[*ver]);
	extra = ceil(fabs( buffer/8. )); //number of extra byte

	str_full = ( char* )calloc( strlen( str ) + data + zero + extra*8, sizeof(char) ); //new str full
	strcpy(str_full, "0100"); //add coding method
	strcpy(&str_full[4], convert_to_bin(strlen( str )/8)); //add size of data
	strcpy(&str_full[data], str); //add data(bit)
	strcpy(&str_full[data + strlen(str)], zero_str); //add zero

	for (i = 0; i < extra; i++) //add extra byte
	{
		if (i%2 == 0)
			strcpy(&str_full[data + strlen(str) + zero + i*8], "11101100");
		else
			strcpy(&str_full[data + strlen(str) + zero + i*8], "00010001");
		//printf("bin%d: \n%s\n\n", i+5, str_full);
	}

	free(zero_str);
	return str_full;
}

char** create_blocks( char *str, int version )
{
	char **mas;
	int i;
	int j;
	int size; //size of block
	int extra_size; //extra bit
	int number = number_of_blocks[version]; //number of blocks
	int element = 0; //pointer to starting copy of string
	size = (size_of_information[version] / 8) / number; //number byte in block
	extra_size = (size_of_information[version] / 8) % number; //extra byte
	mas = ( char** )calloc( number, sizeof(char*) );
	for ( i = 0; i < number; i++ )
	{
		if ( (i + extra_size)!=number )
		{
		mas[i] = ( char* )calloc( size * 8 , sizeof(char) ); //without extra size
		memcpy( mas[i], &str[element], size * 8); 
		//for (j = 0; j < (size * 8); j++)
		//	mas[i][j] = str[j + element];
		mas[i][size * 8] = '\0';
		element+=( size * 8 );
		}
		else
		{
		mas[i] = ( char* )calloc( (size + 1) * 8 , sizeof(char) ); //with extra size
		memcpy( mas[i], &str[element], (size + 1) * 8); 
		//for (j = 0; j < ( size + 1 ) * 8 ; j++)
		//	mas[i][j] = str[j + element];
		mas[i][(size + 1) * 8] = '\0';
		element+=(size + 1) * 8;
		extra_size--;
		}
	}
	
	return mas;
}


int main()
{
	char c;
	int i;
	int j;
	int version;
	char *str_source = "hello";
	//char *str_source = "hljhjkhkkjhkhkellohljhjkhkhljhjkhkjhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohjhkhkehljhjkhkjhkhkellohhljhjkhkjhkhkellohllohhljhjkhkjhkhkellohhellohgfhgfkjgjkghjghjghjgjgjkgkjhkhkellohljhjkhkhljhjkhkjhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohjhkhkehljhjkhkjhkhkellohhljhjkhkjhkhkellohllohhljhjkhkjhkhkellohhellohgfhgfkjgjkghjghjghjgjgjkgkjhkhkellohljhjkhkhljhjkhkjhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohjhkhkehljhjkhkjhkhkellohhljhjkhkjhkhkellohllohhljhjkhkjhkhkellohhellohgfhgfkjgjkghjghjghjgjgjkgjhkhkellohljhjkhkhljhjkhkjhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohhkhkellohhljhjkhkjhkhkellohhljhjkhkjhkhkellohjhkhkehljhjkhkjhkhkellohhljhjkhkjhkhkellohllohhljhjkhkjhkhkellohhellohgfhgfkjgjkghjghjghjgjgjkgjgjgjhkgjhkfghfhgfhgfhjfghfghfhgfhgfghfhgjfhjfghkfkjgklkhfgdfhjk";
	//char *str_source = "asdsadfasdfsadfasdgdfagasfasdfgsdagasgasdfsagergfredsmfhberklbgfjlgbfkaljsbfklsdsadfasdfsadfasdgdfagasfasdfgsdagasgasdfsagergfredsmfhberklbgfjlgbfkaljsbfklasdbfsdsadfasdfsadfasdgdfagasfasdfgsdagasgasdfsagergfredsmfhberklbgfjlgbfkaljsbfklasdbfasdbfklbasdklfbkasldbfklasbsdsadfasdfsadfasdgdfagasfasdfgsdagasgasdfsagergfredsmfhberklbgfjlgbfkaljsbfklsdsadfasdfsadfasdgdfagasfasdfgsdagasgasdfsagergfredsmfhberklbgfjlgbfkaljsbfklasdbfsdsadfasdfsadfasdgdfagasfasdfgsdagasgasdfsagergfredsmfhberklbgfjlgbfkaljsbfklasdbfasdbfklbasdklfbk";
	char *str_source_bin;
	char *buffer;
	char **blocks;
	str_source_bin = convert_to_utf8( str_source );
	version = optimal_version( strlen( str_source_bin ) );
	str_source_bin = add_service_inf(str_source_bin, &version);
	blocks = create_blocks( str_source_bin, version );
	printf("version: %d", version);
	printf("\nsize: %d", strlen(str_source_bin));
	printf("\nnumber of blocks: %d", number_of_blocks[version]);
	printf("\n\nbin full: \n%s\n", str_source_bin);
	printf("\nblocks:");
	for ( i = 0; i < number_of_blocks[version]; i++)
			printf ("\nn:%d size:%d\n%s", i, strlen(blocks[i]), blocks[i]);



	getchar();

}