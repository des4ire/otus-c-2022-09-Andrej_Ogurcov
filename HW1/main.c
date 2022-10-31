#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#define ZIP_SIG 0x04034b50L //начальная сигнатура ZIP файла
#define ZIPEOCD 0x06054b50 //сигнатура конца CD 
#define CDSTART 0x02014b50 //сигнатура заголовка CD 

#if 0
#define MOD_TIME 12                /* file modification time offset*/
#define MOD_DATE 14              /* file modification time offset */
#define FILENAME_L		  /*File name length (n) offset*/
#define EXTRA_SIZE	 	  /* Extra field length (m) offset*/
#define COMMENT_SIZE 	                 /* File comment length (k) offset*/
struct zipinfo{
	short int modtime[2];
	short int moddate[2];
	char filename[50];
	char filecomment[50];
};
#endif

int main( )
{
    //struct zipinfo zip1;
    setlocale(LC_ALL, "Rus");
    //unsigned short signature=0;
    //unsigned short n;
int signature=0; //переменная для сигнатуры
int start_CD=0;
//int cd_offset = 0; //offset from EOCD to the first record of central signature
short int record_num=0; //количество записей в архиве
short int dir_len=0; //размер центральной директории
short int name_size=0; //размер имени файла центральной директории
short int extra_field=0; //размер дополнительного поля за именем
short int comment_field=0; //размер поля с комментариями
int n;
//int counter; //счетчик цикла 
char fname[25]; //переменная для записи файла

    FILE *filePointer ; // Указатель на файл
    char file_name[100]; //Абсолютный путь к файлу
    int line_count = 0;
    //char data[200]; // Буффер для содержимого файла
    printf("\n\nENTER FULL PATH TO FILE: ");
    scanf("%s",&file_name);
    filePointer = fopen(file_name, "rb"); //Открываем файл по указанному пути режиме "для чтения"
    // Условие проверки существования файла
    if ( filePointer == NULL )
    {
        printf( "\n\n\n%s\"ФАЙЛ НЕ НАЙДЕН!", file_name );
    }
    else
    {

        printf("\n\n\n###############  FILE SUCCESSFULLY OPENED!#############\n\n\n") ;
        fseek(filePointer, 0, SEEK_END); //переходим в конец файла
        fseek(filePointer,-22,SEEK_END); //прыгаем на 22 байта назад, где долхна быть сигнатура EOCD (такой номер не пройдет, если в конце есть поле с комментариями)
        //printf("Position of file pointer is : "); //считываем 4 байта
        //printf("%ld \n", ftell(filePointer)); //положение курсора от начала файла
        int count=fread(&signature, sizeof(int), 1, filePointer); //считываем предпологаемую сигнатуру EOCD
	
        if (signature == ZIPEOCD){       //если прочитанные данные совпадают с началом CD, то поехали 
        	printf("The file is ZIP archive\n");
		fseek(filePointer, 4, SEEK_CUR);
		fread(&record_num, sizeof(short int),1,filePointer); //читаем количество записей в центральной директории архива
		printf("Number of records in archive: %i\n", record_num);
		fseek(filePointer, 2, SEEK_CUR);
		fread(&dir_len, sizeof(short int),1,filePointer); //получаем размер центральной директории
		printf("Size of central directory in  bytes: %i\n", dir_len);
		fseek(filePointer,-(dir_len+22),SEEK_END); //c конца файла прыгаем к началу центральной директории (повезет, если EOCD без комментов)
		for (int counter=0; counter<record_num; counter++){
			//fseek(filePointer,-(dir_len+22),SEEK_END); //c конца файла прыгаем к началу центральной директории (повезет, если EOCD без комментов)
			fread(&start_CD, sizeof(int),1,filePointer); 
                	    if (start_CD==CDSTART){     
		      		//printf("Start of CD header found: %x\n", start_CD);	  		
		      		//printf("Start of central directory: %x\n", start_CD);   
		      		fseek(filePointer, 24, SEEK_CUR);
		      		fread(&name_size, sizeof(short int),1,filePointer); //размер названия файла в байтах
		      		fread(&extra_field, sizeof(short int),1,filePointer); //размер дополнительного поля в байтах
		      		fread(&comment_field, sizeof(short int),1,filePointer);
		      		
		      		
				fseek(filePointer, 12, SEEK_CUR);  //переходим на позицию чтения файла  
		      		fread(&fname, sizeof(char),name_size,filePointer);
		      		printf("\nFilename# %i: %s\n",counter, fname);
				//printf("	 Extra field: %i\n", extra_field);
				printf("	 File comment: %i\n", comment_field);
				fseek(filePointer,(extra_field+comment_field),SEEK_CUR); //переходим к следующей записи
				
		 		}   
	
		
			}	
		
		printf("\n\n############  ZIP FILE SUCCESSFULLY READ! ############\n\n");	
			
	}
        else{
        printf("\n\n############  THE FILE IS NOT A ZIP ARCHIVE  ############\n\n");
	      
	}
        	
	//printf("Position of file pointer is : ");
        //printf("%ld \n", ftell(filePointer));
        //printf("Bytes read: %x\n",signature);
        //printf("Elements read: %d\n", count);
        //printf("Размер EOCD = %i \n", sizeof(ZIPEOCD));
        //Считываем данные из файла функцией fgets()
        //while( fgets ( data, 200, filePointer ) != NULL )
        //{
          //  line_count++;
          //  printf( "\nLine:%d -> %s" , line_count, data ); //ВЫводим полученные данные
       // }

	//printf("Content of structure %s is:", zip1.filename);
        fclose(filePointer); //Закрывем файл
	printf("\n\n ############  THE FILE IS CLOSED  ############\n\n .") ;
    }
    return 0;
}





