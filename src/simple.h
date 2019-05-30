/*
 * simple.h
 *
 * Created: 27/05/2019 17:55:27
 *  Author: gmonteiro42
 */ 


#ifndef SIMPLE_H_
#define SIMPLE_H_


typedef struct {
  char id[80];
  char glicose[3];
  char direction[80];
} data_g;

void parseiro(char BUFFER[],data_g *get_atual);


#endif /* SIMPLE_H_ */