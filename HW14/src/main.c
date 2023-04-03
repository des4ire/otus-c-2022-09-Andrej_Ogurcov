#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

void check_input(char *str)
{
    size_t curr = 0, cnt = 0;
    char ch = str[0];
                         //проверить содержание строки
    while (ch != '\0') {
        ch = str[curr];
        if (isalnum(ch))
            cnt++;
        curr++;
    }
    if (cnt!=strlen(str))
     {
      printf("\nINPUT ERROR: Argument contains ivalid characters\n");
      exit(0);
      }
    else if (strlen(str)>10)
     {
        printf("\nINPUT ERROR: Argument is too long\n"); //проверить длину 
  	exit(0);
      }
}

int callback(void *, int, char **, char **);

int main(int argc , char *argv[]) {
   
     if (argc !=4) {
        printf("Usage: %s database tabe column\n", argv[0]);
        exit(EXIT_FAILURE);
  	}
     
    for (int i=2; i<4; i++)
       {
        check_input(argv[i]);  
         }
    
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(argv[1], &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
        sqlite3_errmsg(db));
	
        sqlite3_close(db);
        
        return 1;
         }
     
  char param[4][5]={"MAX","MIN","AVG","SUM"};
  char attrib[4][10]={"MAXIMUM", "MINIMUM", "AVERAGE","SUM"}; 
     
    for (uint8_t i=0; i<4; i++){ //встроенные статистические функции можно выполнить в цикле, код короче получается
     char* sql = sqlite3_mprintf("SELECT \"%q\" (\"%q\") AS \"%q\" FROM \"%q\"  where typeof(\"%q\") = 'real' or typeof(\"%q\")='integer'", param[i], argv[3], attrib[i], argv[2],argv[3],argv[3]); 
      rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
  
       if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
         } 
    
    sqlite3_free(sql);
    }
                                 //В SQLITE нет втроенной дисперсии, поэтому пришлось делать эквивалент в отдельном запросе 
     char *sql2 =sqlite3_mprintf("SELECT SUM((\"%q\"-(SELECT AVG(\"%q\") FROM \"%q\"))*(\"%q\"-(SELECT AVG(\"%q\") FROM \"%q\")) ) / (COUNT(\"%q\")-1) AS VARIANCE FROM \"%q\"  where \"%q\" glob '*[0-9]'", argv[3],argv[3],argv[2],argv[3],argv[3],argv[2],argv[3],argv[2],argv[3]); //variance equivalent
     rc = sqlite3_exec(db, sql2, callback, 0, &err_msg);                                      
     
      if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 1;
    } 
    sqlite3_close(db);
    sqlite3_free(sql2);
    return 0;
}

int callback(void *NotUsed  __attribute__((unused)), int argc, char **argv, char **azColName) {
    
    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        
   }
    printf("\n");
    return 0;
}