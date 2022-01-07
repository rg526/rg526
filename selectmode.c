#include "esUtil.h"
#include "state.h"
#include "playmode.h"
#include "homemode.h"

typedef struct{
    Device* dev;
    GLuint button;
	GLuint active_button;
	GLuint background;
	size_t count;
	size_t current;
    int expand;
	GLuint leftarrow;
	GLuint rightarrow;
	GLuint leftarrow_end;
	GLuint rightarrow_end;
	char* music;
	char* speed;
} SelectmodeData;


int selectmode_init(ESContext* esContext, State* state, Device* dev)
{
    state->data = malloc(sizeof(SelectmodeData));
    if (state->data == NULL){
        fprintf(stderr, "state data alloc failed\n");
        return -1;
    }
    SelectmodeData* data = state->data;
    data->dev = dev; 

	int button_width, button_height, active_button_width, active_button_height;
	char* active_button_buf = esLoadTGA(NULL, "image/active-button.tga", &active_button_width, &active_button_height);
    char* button_buf = esLoadTGA(NULL, "image/button.tga", &button_width, &button_height);
	data->button = image_load(&dev->image, button_width, button_height, button_buf, 4);
    data->active_button = image_load(&dev->image, active_button_width, active_button_height, active_button_buf, 4);
	free(button_buf);
    free(active_button_buf);

	int left_width, left_height, right_width, right_height;
    char* left_buf = esLoadTGA(NULL, "image/left-arrow.tga", &left_width, &left_height);
	char* right_buf = esLoadTGA(NULL, "image/right-arrow.tga", &right_width, &right_height);
	char* left_end_buf = esLoadTGA(NULL, "image/left-arrow-disabled.tga", &left_width, &left_height);
	char* right_end_buf = esLoadTGA(NULL, "image/right-arrow-disabled.tga", &right_width, &right_height);
	data->leftarrow = image_load(&dev->image, left_width, left_height, left_buf, 4);
	data->rightarrow = image_load(&dev->image, right_width, right_height, right_buf, 4);
	data->leftarrow_end = image_load(&dev->image, left_width, left_height, left_end_buf, 4);
	data->rightarrow_end = image_load(&dev->image, right_width, right_height, right_end_buf, 4);
	free(left_buf);
	free(right_buf);
	free(left_end_buf);
	free(right_end_buf);


	int bg_width, bg_height;
	char* bg_buf = esLoadTGA(NULL, "bg.tga", &bg_width, &bg_height);
	data->background = image_load(&dev->image, bg_width, bg_height, bg_buf, 3);
	free(bg_buf);

	data->count = 4;
	data->current = 0;
    data->expand = 0;
	data->music = "music";
	data->speed = "speed";
    return 0;
}

void selectmode_destroy(ESContext* esContext, State* state){
	SelectmodeData* data = state->data;
	image_unload(&data->dev->image, data->button);
	image_unload(&data->dev->image, data->active_button);
	image_unload(&data->dev->image, data->background);
	image_unload(&data->dev->image, data->leftarrow);
	image_unload(&data->dev->image, data->rightarrow);
	image_unload(&data->dev->image, data->leftarrow_end);
	image_unload(&data->dev->image, data->rightarrow_end);
    free(state->data);
}

StateChg selectmode_update(ESContext* esContext, State* state)
{
    SelectmodeData* data = state->data;
    //Test enter button
    InputLine enter_line = input_query_clear(&data->dev->input, 0);
    
	if (enter_line.active) {
		input_clearall(&data->dev->input);

		StateChg change;
		if (data->current <= 1){
			if(data->expand && data->current == 0){
				data->music = theme_options[data->dev->theme_opt].name;
			}
			else if(data->expand && data->current == 1){
				data->speed = difficulty_options[data->dev->difficulty_opt].name;
			}
			data->expand = !data->expand;
		}
        else if (data->current == 2){
            change.ret = STATE_SWITCH_NOSAVE;
			change.next = &playmode_state;
			return change;
        }
        else {
            change.ret = STATE_SWITCH_NOSAVE;
			change.next = &homemode_state;
			return change;
        }          
	}
    //Test up button
	InputLine up_line = input_query_clear(&data->dev->input, 2);
	if (up_line.active) {
		input_clearall(&data->dev->input);
        if (data->expand){
           if(data->current == 0){
               if(data->dev->theme_opt > 0){
                   data->dev->theme_opt --;
               }
           }else if(data->current == 1){
               if(data->dev->difficulty_opt > 0){
                   data->dev->difficulty_opt --;
               }
           }
        }else if(data->current > 0){
		    data->current --;  
        }
	}

	InputLine down_line = input_query_clear(&data->dev->input, 3);
	if (down_line.active) {
		input_clearall(&data->dev->input);
        if (data->expand){
           if(data->current == 0){
               if(data->dev->theme_opt < theme_count-1){
                   data->dev->theme_opt ++;
               }
           } else if(data->current == 1){
                if(data->dev->difficulty_opt < difficulty_count-1){
                   data->dev->difficulty_opt ++;
               }
           }
        }else if(data->current < data->count-1){
		    data->current ++;  
        }

	}


    //continue pause mode
    StateChg change;
	change.ret = STATE_CONT;
	return change;
}
void selectmode_draw(ESContext* esContext, State* state)
{   
    SelectmodeData* data = state->data;
    glClear(GL_COLOR_BUFFER_BIT);

	image_render(&data->dev->image, 0.0, 0.0, 1.0, 1.0, data->background, NULL);
    
    image_render(&data->dev->image, 0.35, 0.65, 0.3, 0.1, data->current == 0 ? data->active_button : data->button, NULL);
	image_render(&data->dev->image, 0.35, 0.5, 0.3, 0.1, data->current == 1 ? data->active_button : data->button, NULL);
    image_render(&data->dev->image, 0.35, 0.35, 0.3, 0.1, data->current == 2 ? data->active_button : data->button, NULL);
	image_render(&data->dev->image, 0.35, 0.2, 0.3, 0.1, data->current == 3 ? data->active_button : data->button, NULL);

	Vec color;
	color.v[0] = 0.0;
	color.v[1] = 0.0;
	color.v[2] = 0.0;
	if(data->current == 0){
		if(!data->expand){
			text_draw(&data->dev->textbold, data->music , 0.435, 0.6675, 0.8, &color); 
		}else{
			if(data->dev->theme_opt == theme_count-1){
				image_render(&data->dev->image, 0.25, 0.65, 0.05, 0.1, data->leftarrow, NULL);
				image_render(&data->dev->image, 0.7, 0.65, 0.05, 0.1, data->rightarrow_end, NULL);
				text_draw(&data->dev->textbold, theme_options[data->dev->theme_opt].name , 0.435, 0.6675, 0.8, &color); 
			}
			else if(data->dev->theme_opt == 0){
				image_render(&data->dev->image, 0.25, 0.65, 0.05, 0.1, data->leftarrow_end, NULL);
				image_render(&data->dev->image, 0.7, 0.65, 0.05, 0.1, data->rightarrow, NULL);
				text_draw(&data->dev->textbold, theme_options[data->dev->theme_opt].name , 0.435, 0.6675, 0.8, &color); 
			}
			else{
			    image_render(&data->dev->image, 0.25, 0.65, 0.05, 0.1, data->leftarrow, NULL);
				image_render(&data->dev->image, 0.7, 0.65, 0.05, 0.1, data->rightarrow, NULL);
				text_draw(&data->dev->textbold, theme_options[data->dev->theme_opt].name , 0.435, 0.6675, 0.8, &color); 	
			}			
		}
	}else{
		text_draw(&data->dev->textregular, data->music , 0.45, 0.68, 0.6, &color);
	}
	
	if(data->current == 1){
		if(!data->expand){
			text_draw(&data->dev->textbold, data->speed , 0.4255, 0.526, 0.8, &color); 
		}else{
			if(data->dev->difficulty_opt == difficulty_count-1){
				image_render(&data->dev->image, 0.25, 0.5, 0.05, 0.1, data->leftarrow, NULL);
		    	image_render(&data->dev->image, 0.7, 0.5, 0.05, 0.1, data->rightarrow_end, NULL);
				text_draw(&data->dev->textbold, difficulty_options[data->dev->difficulty_opt].name , 0.4255, 0.522, 0.8, &color);
			}
			else if(data->dev->difficulty_opt == 0){
				image_render(&data->dev->image, 0.25, 0.5, 0.05, 0.1, data->leftarrow_end, NULL);
		    	image_render(&data->dev->image, 0.7, 0.5, 0.05, 0.1, data->rightarrow, NULL);
				text_draw(&data->dev->textbold, difficulty_options[data->dev->difficulty_opt].name , 0.4255, 0.522, 0.8, &color);
			}
			else{
				image_render(&data->dev->image, 0.25, 0.5, 0.05, 0.1, data->leftarrow, NULL);
		    	image_render(&data->dev->image, 0.7, 0.5, 0.05, 0.1, data->rightarrow, NULL);
				text_draw(&data->dev->textbold, difficulty_options[data->dev->difficulty_opt].name , 0.4255, 0.522, 0.8, &color);
			}
		}
	}else{
		text_draw(&data->dev->textregular, data->speed , 0.45, 0.53, 0.6, &color);
	}

    data->current == 2 ? text_draw(&data->dev->textbold, "Start" , 0.435, 0.3711, 0.8, &color) : text_draw(&data->dev->textregular, "Start" , 0.46, 0.376, 0.6, &color);
	data->current == 3 ? text_draw(&data->dev->textbold, "Exit" , 0.458, 0.226, 0.8, &color) : text_draw(&data->dev->textregular, "Exit" , 0.472, 0.226, 0.6, &color);;
}

State selectmode_state = {
	.init = selectmode_init,
	.destroy = selectmode_destroy,
	.resume = NULL,
	.update = selectmode_update,
	.draw = selectmode_draw,
	.data = NULL
};
