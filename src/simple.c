#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple.h"


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

void parseiro(char BUFFER[], data_g *get_atual) {

  char *JSON_STRING ;
  char *pto ;
  int start = 0 ;
  int end = 0;

  for (int da =0;da<strlen(BUFFER);da++){
    if(BUFFER[da]=='{'){
      //printf("\nDATA START at %d\n", da);
      start = da;
    }
    if(BUFFER[da]=='}'){
      //printf("\nDATA FINISHED at %d\n", da);
      end = da;
    }
  }
  
  
  pto= strndup( BUFFER+start, end-start+1);
  JSON_STRING=pto;

 int i;
 int r;
 jsmn_parser p;
 jsmntok_t t[30]; /* We expect no more than 30 tokens */

 jsmn_init(&p);
 char buff[80];

 r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                sizeof(t) / sizeof(t[0]));
 if (r < 0) {
   printf("Failed to parse JSON: %d\n", r);
   // AQUI status 
   return 1;
 }

 /* Assume the top-level element is an object */
 if (r < 1 || t[0].type != JSMN_OBJECT) {
   printf("Object expected\n");
   return 1;
 }

 /* Loop over all keys of the root object */
 for (i = 1; i < r; i++) {
   if (jsoneq(JSON_STRING, &t[i], "_id") == 0) {
     /* We may use strndup() to fetch string value */
     //printf("- ID: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     sprintf(get_atual->id,"%.*s", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     //strcpy(get_atual.id,buff);
     //printf(get_atual.id);
     i++;
   } else if (jsoneq(JSON_STRING, &t[i], "device") == 0) {
     /* We may additionally check if the value is either "true" or "false" */
     //printf("- Dev: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   } else if (jsoneq(JSON_STRING, &t[i], "date") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- DATE: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   } else if (jsoneq(JSON_STRING, &t[i], "dateString") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- DATEStr: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "sgv") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- SGV: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     sprintf(get_atual->glicose,"%.*s", t[i + 1].end - t[i + 1].start,
     JSON_STRING + t[i + 1].start);
     
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "delta") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- DELTA: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "direction") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- DIR: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
      sprintf(get_atual->direction,"%.*s", t[i + 1].end - t[i + 1].start,
      JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "type") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- TYPE: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "filtered") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- FILT  : %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "unfiltered") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- UNFILT: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "rssi") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- RSSI: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "noise") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- NOISE: %.*s\n", t[i + 1].end - t[i + 1].start,
     //       JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "sysTime") == 0) {
     /* We may want to do strtol() here to get numeric value */
     //printf("- TIME: %.*s\n", t[i + 1].end - t[i + 1].start,
            //JSON_STRING + t[i + 1].start);
     i++;
   }else {
     //printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
     //       JSON_STRING + t[i].start);
   }
 }
 
}
