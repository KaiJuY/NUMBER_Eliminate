#ifndef __game_H
#define __game_H
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

struct Game_Filed
{
	char topstream[16];
	char botstream[16];
	char top_temp;
	char bot_temp;
	int top_move_count;
	int top_new_element_count;
	int bot_move_count;
	int bot_new_element_count;
	int upgrade;
	int level;
	int goal;
	int lock;
};

void init_game(struct Game_Filed *obj);
int is_game_over(struct Game_Filed *obj);
void check_event_happend(struct Game_Filed *obj, int count);
void trigger_move(struct Game_Filed *obj, int t);
void move_char(char* str);
int trigger_new_element(struct Game_Filed *obj, int t);
//int create_new_element(char* str);
void compare_input_element(struct Game_Filed *obj, int keyin);
void check_level_speed(struct Game_Filed *obj);
void Inc_Goal(struct Game_Filed *obj);

#endif
