#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

char *BUFFER = "1 200 OK\n"
"Server: Cowboy\n"
"Connection: keep-alive\n"
"X-Dns-Prefetch-Control: off\n"
"X-Frame-Options: SAMEORIGIN\n"
"Strict-Transport-Security: max-age=31536000\n"
"X-Download-Options: noopen\n"
"X-Content-Type-Options: nosniff\n"
"X-Xss-Protection: 1; mode=block\n"
"Access-Control-Allow-Origin: *\n"
"Access-Control-Allow-Methods: GET,PUT,POST,DELETE,OPTIONS\n"
"Access-Control-Allow-Headers: Content-Type, Authorization, Content-Length, X-Requested-With\n"
"X-Powered-By: Express\n"
"Last-Modified: Sun, 26 May 2019 15:50:06 GMT\n"
"Vary: Accept, Accept-Encoding\n"
"Content-Type: application/json; charset=utf-8\n"
"ent-Length: 308\n"
"Etag: W/\"134-lDqnmVRplhLONuljFWXyRmwTEdc\"\n"
"Date: Sun, 26 May 2019 15:55:02 GMT\n"
"Via: 1.1 vegur\n"
"\n"
"[{\"_id\":\"5ceab5b2fd74a2bdcb26a27c\",\"device\":\"xDrip-LimiTTer\",\"date\":1558885806822,\"dateString\":\"2019-05-26T12:50:06.822-0300\",\"sgv\":116,\"delta\":-1.734,\"direction\":\"Flat\",\"type\":\"sgv\",\"filtered\":151882.34154999998,\"unfiltered\":151882.34154999998,\"rssi\":100,\"noise\":1,\"sysTime\":\"2019-05-26T12:50:06.822-0300\"}]";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

int parseiro() {

  char *JSON_STRING ;
  char *pto ;
  int start = 0 ;
  int end = 0;

  for (int da =0;da<strlen(BUFFER);da++){
    if(BUFFER[da]=='{'){
      printf("\nDATA START at %d\n", da);
      start = da;
    }
    if(BUFFER[da]=='}'){
      printf("\nDATA FINISHED at %d\n", da);
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

 r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                sizeof(t) / sizeof(t[0]));
 if (r < 0) {
   printf("Failed to parse JSON: %d\n", r);
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
     printf("- ID: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   } else if (jsoneq(JSON_STRING, &t[i], "device") == 0) {
     /* We may additionally check if the value is either "true" or "false" */
     printf("- Dev: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   } else if (jsoneq(JSON_STRING, &t[i], "date") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- DATE: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   } else if (jsoneq(JSON_STRING, &t[i], "dateString") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- DATEStr: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "sgv") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- SGV: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "delta") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- DELTA: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "direction") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- DIR: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "type") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- TYPE: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "filtered") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- FILT  : %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "unfiltered") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- UNFILT: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "rssi") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- RSSI: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "noise") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- NOISE: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else if (jsoneq(JSON_STRING, &t[i], "sysTime") == 0) {
     /* We may want to do strtol() here to get numeric value */
     printf("- TIME: %.*s\n", t[i + 1].end - t[i + 1].start,
            JSON_STRING + t[i + 1].start);
     i++;
   }else {
     printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
            JSON_STRING + t[i].start);
   }
 }
 return EXIT_SUCCESS;
}

int main(){
   parseiro();
}
