#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <stdbool.h>
#include <ctype.h>


struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t

//////////////////////////// allocate memory for server response /////////////////////////////////////////////////////

WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

//////////////////////////////////////// concatenate URL with the city name /////////////////////////////////////

char * concatenate(const char *a, const char *b, const char *d)  
{
    /* calculate the length of the new string */
    size_t len = strlen(a) + strlen(b) + strlen(d);
    /* allocate memory for the new string */
    char* str = malloc(len + 1);

    /* concatenate */
    strcpy(str, a);
    strcat(str, b);
    strcat(str, d);

    
    return str;
}
////////////////////////json parser/////////////////////////////////////////////////////////////////

 void print_json_value(json_object *jobj){
  enum json_type type;
	
  type = json_object_get_type(jobj); /*Getting the type of the json object*/
  switch (type) {
    case json_type_boolean: 
			
                         printf("%s\n", json_object_get_boolean(jobj)? "true": "false");
                         break;
    case json_type_double: 
			
                        printf("%lf\n", json_object_get_double(jobj));
                        break;
    case json_type_int: 
			
                        printf("%d\n", json_object_get_int(jobj));
                        break;
    case json_type_string: 
			 
                         printf("%s\n", json_object_get_string(jobj));
                         break;
	default: 
			break;
  }

}
///////////////////////////////////// parse array //////////////////////////////////////
void json_parse_array( json_object *jobj, char *key) {
  void json_parse(json_object * jobj); /*Forward Declaration*/
  enum json_type type;
  
  json_object *jarray = jobj; /*Simply get the array*/
  if(key) {
    jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
	
	
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
 
  int i;
  json_object * jvalue;
  
  for (i=0; i< arraylen; i++){
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
     
	
    
   if (type == json_type_array) {
      json_parse_array(jvalue, NULL);
    }
    else if (type != json_type_object) {
      printf("value[%d]: ",i);
     
    }
    else {
      json_parse(jvalue);
    }
  }
}
///////////////////////////////////////////////// parse object /////////////////////////////////
/*Parsing the json object*/
void json_parse(json_object * jobj) {

  enum json_type type;
  json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
    
      //printf("%s%s ", key, val);
    type = json_object_get_type(val);
    switch (type) {
      case json_type_boolean: 
      case json_type_double: 
      case json_type_int: 
      case json_type_string: 
			    
			    if ( strcmp( key, "temp_C" ) == 0 ) {
                    	    printf( "Temperature, C = ");
                    	    print_json_value(val);
                	    } 
			    else if ( strcmp( key, "cloudcover" ) == 0 ) {
                    	    printf( "Cloudcover = " );
                    	    print_json_value(val);
                            }
			    else if ( strcmp( key, "humidity" ) == 0 ) {
                    	    printf( "Humidity = " );
                    	    print_json_value(val); 
		  	    }
			    else if ( strcmp( key, "winddir16Point" ) == 0 ) {
                    	    printf( "Wind direction = " );
                    	    print_json_value(val); 
		 	    }
			    else if ( strcmp( key, "windspeedKmph" ) == 0 ) {
                    	    printf( "Wind speed, Km/h = " );
                    	    print_json_value(val); 
		  	    }
			    else if ( strcmp( key, "localObsDateTime" ) == 0 ) {
                    	    printf( "Local DateTime = ");
                    	    print_json_value(val); 
		  	    }
			    else if ( strcmp( key, "pressure" ) == 0 ) {
                    	    printf( "Pressure = " );
                    	    print_json_value(val); 
		  	    }
			    else if ( strcmp( key, "value" ) == 0 ) {
                    	    printf( "Current location = " );
			    print_json_value(val);
			    }
			    else {
                    	      continue;
                	   } 
				break;
      
     case json_type_object: 
			   jobj = json_object_object_get(jobj, key);
                           json_parse(jobj);
			   break;
     case json_type_array: 
			   if ( strcmp( key, "current_condition" ) != 0  && strcmp( key, "nearest_area" ) !=0 && strcmp( key, "areaName" ) !=0 ) {
                           continue;
                            }
		           json_parse_array(jobj, key);
                           break;
     default:
        /*Do nothing*/
		break;
    }
  }
} 

////////////////////////////////////check if the city name is alphabet ////////////////////////////////////////////////////

int checkString(char *str)
{

    while (*str) {

        if (isalpha(*str)) {
            
		return 0;
        }
	else {
		printf("Not a valid city name\n");
		exit(1);	
		}
        str++;
     
 }
    return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
 

int main(int argc, char *argv[])
{

if ( argc != 2 ) 
    {
	printf("Usage: %s <city name>\n", argv[0]); 
    }
    else
    {
	char URL_start[] = "https://wttr.in/";
    	char *city = argv[1];
    	char format[] = "?format=j1";
	checkString(city);
	
    	char* str = concatenate(URL_start, city, format);


  	CURL *curl_handle;
  	CURLcode res;

  	struct MemoryStruct chunk;

  	chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  	chunk.size = 0;    /* no data at this point */

  	curl_global_init(CURL_GLOBAL_ALL);

  	/* init the curl session */
  	curl_handle = curl_easy_init();

  	/* specify URL to get */
  	curl_easy_setopt(curl_handle, CURLOPT_URL, str);

  	/* send all data to this function  */
  	curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  	/* we pass our 'chunk' struct to the callback function */
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  	/* some servers do not like requests that are made without a user-agent
     	field, so we provide one */
  	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  	/* get it! */
  	res = curl_easy_perform(curl_handle);

  	/* check for errors */
  	if(res != CURLE_OK) {
    	fprintf(stderr, "curl_easy_perform() failed: %s\n",
            	curl_easy_strerror(res));
			exit(1);
  	}
  	else {
  

    	printf("%lu bytes retrieved from server\n", (unsigned long)chunk.size);
  	}
  
	struct json_object *jobj = json_tokener_parse(chunk.memory);



   	//json_parse_array(med_obj, "current_condition"); /*parse specific json record*/

   	json_parse(jobj);
    	//parse_array
	//json_object_object_foreach(med_obj,key, val) { /*Passing through every array element*/
        //printf("%s%s ", key, val);
	//parse_array(med_obj, weather, "current_condition"); 
		
	curl_easy_cleanup(curl_handle);

  	free(chunk.memory);

  	/* we are done with libcurl, so clean it up */
  	curl_global_cleanup();
	}
  return 0;
}