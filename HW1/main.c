#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdint.h>
#define ZIP_SIG 0x04034b50L //начальная сигнатура ZIP файла
#define ZIPEOCD 0x06054b50 //сигнатура конца CD
#define CDSTART 0x02014b50 //сигнатура заголовка CD
#define JPEGSTART 0xE0FFD8FF //сигнатура заголовка JPEG
#define JPEGEND 0xD9FF     //сигнатура конца JPEG


int main ( int argc, char *argv[] )
{

setlocale(LC_ALL, "Rus");

int signature=0; //переменная для сигнатуры
int start_CD=0;
//int cd_offset = 0; //offset from EOCD to the first record of central signature
short int record_num=0; //количество записей в архиве
short int dir_len=0; //размер центральной директории
short int name_size=0; //размер имени файла центральной директории
short int extra_field=0; //размер дополнительного поля за именем
short int comment_field=0; //размер поля с комментариями
uint16_t jpeg_end=0; //переменная для сигнатуры конца jpeg файла
int start_jpeg=0; //переменная для сигнатуры начала jpeg файла
char fname[25]; //буффер
int n;


    if ( argc != 2 ) //
    {
        printf( "\nusage: %s filename\n", argv[0] ); /* Выводим значение argv[0] */
    }
    else
    {
        FILE *file = fopen( argv[1], "rb" );  //argv[1]  - предпологаемое название файла
    if ( file == 0 ) // fopen возвращает 0, если не поучилось открыть файл
        {
            printf( "\n\n\n\t############# COULD NOT OPEN FILE! #############\n\n\n" );
        }
        else
        {
           printf("\n\n\n\t############### FILE SUCCESSFULLY OPENED! #############\n\n\n") ;

    	fseek(file,0, SEEK_SET); //установка курсора на начало файла, проверяем сигнатуру
        fread(&start_jpeg, sizeof(int),1,file);
    	fseek(file,-2, SEEK_END); //читаем последние 2 байта файла
    	fread(&jpeg_end, sizeof(uint16_t),1,file);
        fseek(file,-22,SEEK_END); //прыгаем на 22 байта назад, где должна быть сигнатура EOCD (такой номер не пройдет, если в конце есть поле с комментариями)
        fread(&signature, sizeof(int), 1, file); //считываем предпологаемую сигнатуру EOCD
    
    if((start_jpeg == JPEGSTART)&&(jpeg_end == JPEGEND)&&(signature != ZIPEOCD)){ //условие №1
            printf("\n ############# THE FILE IS A SIMPLE JPEG WITH NO ZIP ARCHIVE #############\n\n");
            

            }
    else if ((start_jpeg != JPEGSTART)&&(jpeg_end != JPEGEND)&&(signature != ZIPEOCD)){ //условие №2
        printf("\n\t\t############ WRONG FORMAT!!! ############\n\n\n");
        }
	
    else if ((start_jpeg != JPEGSTART)&&(jpeg_end != JPEGEND)&&(signature == ZIPEOCD)){ //условие №3
        printf("\n\t\t############ WRONG FORMAT!!! ############\n\n\n");
        }


    else if ((start_jpeg == JPEGSTART)&&(jpeg_end != JPEGEND)&&(signature == ZIPEOCD)){ //условие №4
        printf("\n\n\n############ THE FILE IS JPEG CONTAINING ZIP ARCHIVE ############\n\n\n");
        
         
        fseek(file, 4, SEEK_CUR);
        fread(&record_num, sizeof(short int),1,file); //читаем количество записей в центральной директории архива
        printf("Number of records in archive: %i\n", record_num);
        fseek(file, 2, SEEK_CUR);
        fread(&dir_len, sizeof(short int),1,file); //получаем размер центральной директории
        printf("Size of central directory in  bytes: %i\n", dir_len);
        fseek(file,-(dir_len+22),SEEK_END); //c конца файла прыгаем к началу центральной директории (повезет, если EOCD без комментов)
        for (int counter=0; counter<record_num; counter++){
            fread(&start_CD, sizeof(int),1,file);
                        if (start_CD==CDSTART){
                    		fseek(file, 24, SEEK_CUR);
                    		fread(&name_size, sizeof(short int),1,file); //размер названия файла в байтах
                    		fread(&extra_field, sizeof(short int),1,file); //размер дополнительного поля в байтах
                    		fread(&comment_field, sizeof(short int),1,file);
                    		fseek(file, 12, SEEK_CUR);  //переходим на позицию чтения файла
                    		fread(&fname, sizeof(char),name_size,file);
                    		printf("\nFilename# %i: %s\n",counter, fname);
                    		printf("\tFile comment: %i\n", comment_field);
                    		fseek(file,(extra_field+comment_field),SEEK_CUR); //переходим к следующей записи
                }
            }

      
        }
          fclose( file );
        }
    }
}
