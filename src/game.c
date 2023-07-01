#include "game.h"
char new_char[2];
void init_game(struct Game_Filed *obj)
{
	for(int i = 0; i < 16; ++i)
	{
		obj->topstream[i] = '\x20';
		obj->botstream[i] = '\x20';
	}
	obj->top_temp = ' ';
	obj->bot_temp = ' ';
	obj->top_move_count = 1;
	obj->top_new_element_count = 4;
	obj->bot_move_count = 0;
	obj->bot_new_element_count = 0;
	obj->upgrade = 0;
	obj->level = 1;
	obj->goal = 0;
	obj->lock = 0;
}
int is_game_over(struct Game_Filed *obj)
{
	if(obj->top_temp != ' ' || obj->bot_temp != ' ')
		return 1;
	else
		return 0;
}
void check_event_happend(struct Game_Filed *obj, int count)
{
	while(obj->lock);
	obj->lock = 1;

	if(count & obj->top_move_count)
		trigger_move(obj, 1);
	if(count & obj->bot_move_count)
		trigger_move(obj, 2);
	if(count & obj->top_new_element_count)
		trigger_new_element(obj, 1);
	if(count & obj->bot_new_element_count)
		trigger_new_element(obj, 2);
	if(obj->upgrade)
		check_level_speed(obj);
	obj->lock = 0;

}

void trigger_move(struct Game_Filed *obj, int t)
{
	int index = 0;
	switch(t)
	{
	case 1 ://top
		obj->top_temp = obj->topstream[0];
		while(index < 16)
		{
			if(index == 15)
			{
				obj->topstream[index] = ' ';
				break;
			}
			if(obj->topstream[index + 1] != ' ')
			{
				obj->topstream[index] = obj->topstream[index + 1];
				obj->topstream[index + 1] = ' ';
			}
			index++;
		}
		//move_char(obj->topstream);
		break;
	case 2 ://bot
		obj->bot_temp = obj->botstream[0];
		while(index < 16)
		{
			if(index == 15)
			{
				obj->botstream[index] = ' ';
				break;
			}
			if(obj->botstream[index + 1] != ' ')
			{
				obj->botstream[index] = obj->botstream[index + 1];
				obj->botstream[index + 1] = ' ';
			}
			index++;
		}
		//move_char(obj->botstream);
		break;
	}

}

void move_char(char* str)
{
	int index = 0;
	while(index < 16)
	{
		if(index == 15)
		{
			str[index] = ' ';
			break;
		}
		str[index] = str[index + 1];
		str[index + 1] = ' ';
		index++;
	}
}
/*
int create_new_element(char* str)
{
	int ne = rand()%10;
	char new_char[2];
	sprintf(new_char, "%d", ne);
	str[15] = new_char[0];
	return 0;
}
*/

int trigger_new_element(struct Game_Filed *obj, int t)
{
    int ne = 0;
    switch(t)
    {
        case 1:
            if (obj->topstream[15] != ' ')
                return 1;
            ne = 1 + rand() % 5;
            sprintf(new_char, "%d", ne);
            obj->topstream[15] = new_char[0];
            break;
        case 2:
            if (obj->botstream[15] != ' ')
                return 1;
            ne = 5 + rand() % 5;
            ne = ne % 10;
            sprintf(new_char, "%d", ne);
            obj->botstream[15] = new_char[0];
            break;
    }
    return 0;
}

void compare_input_element(struct Game_Filed *obj, int keyin)
{
	while(obj->lock);
	obj->lock = 1;
	char s[2];
	sprintf(s, "%d", keyin);
	char first_t_c[2];
	char first_b_c[2];
	int top_index = 0;
	int bot_index = 0;
	first_t_c[0] = ' ', first_b_c[0] = ' ';
	for(int i = 0; i < 16; i++)
	{
		if(first_t_c[0] == ' ' && obj->topstream[i] != ' ')
		{
			first_t_c[0] = obj->topstream[i];
			top_index = i;

		}
		if(first_b_c[0] == ' ' && obj->botstream[i] != ' ')
		{
			first_b_c[0] = obj->botstream[i];
			bot_index = i;
		}
	}
	//after got first then compare input
	if(first_t_c[0] == s[0] || first_b_c[0] == s[0])
	{
		if(top_index <= bot_index)
		{
			if(first_t_c[0] == s[0])
			{
				obj->topstream[top_index] = ' ';
			}
			if(first_b_c[0] == s[0])
			{
				obj->botstream[bot_index] = ' ';
			}
		}
		else
		{
			if(first_b_c[0] == s[0])
			{
				obj->botstream[bot_index] = ' ';
			}
			if(first_t_c[0] == s[0])
			{
				obj->topstream[top_index] = ' ';
			}
		}
		Inc_Goal(obj);
	}
	obj->lock = 0;
}

void check_level_speed(struct Game_Filed *obj)
{
	switch(obj->level)
	{
	case 1:
		break;
	case 2:
		obj->top_move_count = obj->top_move_count | 2;
		obj->top_new_element_count = obj->top_new_element_count | 8;
		break;
	case 3:
		obj->bot_move_count = 1;
		obj->bot_new_element_count = 4;
		break;
	case 4:
		obj->bot_move_count = obj->bot_move_count | 2;
		obj->bot_new_element_count = obj->bot_new_element_count | 8;
		break;
	case 5:
		obj->top_move_count = obj->top_move_count | 511;
		obj->top_new_element_count = obj->top_new_element_count | 511;
		obj->bot_move_count = obj->bot_move_count | 511;
		obj->bot_new_element_count = obj->bot_new_element_count | 511;
		break;
	}
	obj->upgrade = 0;
}
void Inc_Goal(struct Game_Filed *obj)
{
	obj->goal += 20;
	if(obj->goal >= 2000)
	{
		obj->level = 5;
		obj->upgrade = 1;
	}
	else if(obj->goal >= 1000)
	{
		obj->level = 4;
		obj->upgrade = 1;
	}
	else if(obj->goal >= 500)
	{
		obj->level = 3;
		obj->upgrade = 1;
	}
	else if(obj->goal >= 250)
	{
		obj->level = 2;
		obj->upgrade = 1;
	}
}
