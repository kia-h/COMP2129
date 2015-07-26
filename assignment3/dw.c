#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dw.h"

#define BUFSIZE 64
#define MAX_ARGS 3
#define MAX_NAME_LENGHT 21

void newList(list*);
int listAdd(list* , room* );
int listLength(list* );
room* list_get(list* , char* );
int update_list(room* ,room* ,char );

typedef struct status status;
struct status {
	int p_num_doughnuts;  /* number of doughnuts in the player */
  	int p_num_milkshakes; /* number of milkshakes in the player */
};

/**
 * Main function
 */
int main(int argc, char *argv[])
{
	char buff[BUFSIZ];
	int args;
	int num_room, num_d, num_m;
	char room_name[MAX_NAME_LENGHT];		    
		
	char room1[MAX_NAME_LENGHT],room2[MAX_NAME_LENGHT];
	char start_room[MAX_NAME_LENGHT],end_room[MAX_NAME_LENGHT];
	char door;

	int num_portal;

	num_portal=0;
	num_room=0;

	status* curr_stats = (status*)malloc(sizeof(status*));
	curr_stats->p_num_doughnuts=0;
	curr_stats->p_num_milkshakes=0;


	room* temp = (room*)calloc(1,sizeof(room*)) ;
	list* my_list = (list*)calloc(1,sizeof(list*));	
	newList(my_list);

	int tempLength= 0;

	if ( argc != 2 ) /* argc should be 2 for correct execution */
    {
        /* We print argv[0] assuming it is the program name */
        fprintf(stderr, "error\n");
	    exit(1);
    }
    else 
    {
        // We assume argv[1] is a filename to open
        FILE *in = fopen( argv[1], "r" );

        /* fopen returns 0, the NULL pointer, on failure */
	    if (in == NULL) 
	    {
	      fprintf(stderr, "error\n");
	      exit(1);
	    }
	    else
	    {
  		    int line_num = 0;
  		    int temp_num_room = 0;
  		    int temp_num_portal = 0;
  		    
  		    
  		    while ( fgets( buff, sizeof buff, in ) != NULL ) 
  		    {		    	
			    if (line_num==0)
			    {
				    if ((sscanf( buff, "%d", &num_room))!=1) 
		    		{ 	
				        fprintf(stderr,"error\n");
				        exit(1);
		    		}
		    		line_num++;		    	
			    }
			    else if (temp_num_room<num_room) 
			    {		    		
		    		args = sscanf( buff, "%s %d %d", room_name, &num_d, &num_m );
			    		    		
				    if (args!=MAX_ARGS) 
					{ 
						fprintf(stderr,"error\n");
					    exit(1);
					}

					strcpy(temp->name,room_name);
					temp->num_doughnuts = num_d;
					temp->num_milkshakes = num_m;
					temp->portal[0] = NULL;
					temp->portal[1] = NULL;
					temp->portal[2] = NULL;
					temp->portal[3] = NULL;
					
					listAdd(my_list, temp);
					tempLength++;
					my_list->length = tempLength;
										
					temp_num_room++;
					line_num++;	
				}
				else if (line_num==num_room+1)
				{
					if ((sscanf( buff, "%d", &num_portal))!=1) 
		    		{ 	
				        fprintf(stderr,"error\n");
				        exit(1);
		    		}		    		
			    	line_num++;		    	
				}
				else if (temp_num_portal<num_portal)
				{
					args = sscanf( buff, "%s %c %s", room1, &door, room2 );			    	
			    	if (args!=MAX_ARGS) 
					{ 
						fprintf(stderr,"error\n");
					    exit(1);
					}
					temp_num_portal++;
					line_num++;						
					
					room* r1 =(room*)malloc(sizeof(room*));
					room* r2 =(room*)malloc(sizeof(room*));
					if (r1!=NULL || r2!=NULL)
					{ 
						fprintf(stderr,"error\n");
					    exit(1);
					}
					
					r1 = list_get(my_list,room1);					
					r2 = list_get(my_list,room2);
					
					if (update_list(r1,r2,door)==0)
					{
						fprintf(stderr,"error\n");
					    exit(1);
					}								
					if(door=='U')
						door='D';
					else if (door='R')
						door='L';
					if (update_list(r2,r1,door)==0)
					{
						fprintf(stderr,"error\n");
					    exit(1);
					}	
					free(r1);
					free(r2);				
				}
				else if (line_num==num_room+num_portal+2)
				{
					args = sscanf( buff, "%s %s", start_room, end_room );			    	
			    	room* temp2 = (room*)malloc(sizeof(room*));
			    	temp2 = list_get(my_list,end_room);
			    	printf("%s %d %d %d %d\n",temp2->name,
			    							  temp2->num_doughnuts,
			    							  temp2->num_milkshakes,
			    							  curr_stats->p_num_doughnuts,
			    							  curr_stats->p_num_milkshakes);
		    		
				    if (args!=MAX_ARGS-1) 
					{ 
						fprintf(stderr,"error\n");
					    exit(1);
					}
					temp_num_portal++;
					line_num++;	
					free(temp2);
				}				
			}//end while
	    }//end else	    
	}
	free(temp);
	free(my_list);
	
	free(curr_stats);

	return 0;
}//end main

/**
 * Creates a pointer to a new empty list and returns it.
 */
void newList(list* my_list)
{
	node* head = (node*)calloc(1,sizeof(node*));	
	node* tail = (node*)calloc(1,sizeof(node*));	
	my_list->head = my_list->tail=0;
	my_list->length = 0;	
	return;
}

/**
 * Adds an element to the list given.
 */
int listAdd(list* my_list, room* data)
{
	
	node* new_node;
	new_node = (node*)calloc(1,sizeof(node));
	if(new_node == NULL)
	{
		return 0;
	}
	new_node->room = data;
	
	if(my_list->head == NULL)
	{
		printf("adding to head\n");
		my_list->head = new_node;
		my_list->tail = new_node;
		new_node->prev = NULL;
		new_node->next = NULL;
	}
	else
	{
		printf("adding after head\n");
		my_list->tail->next=new_node;
		new_node->prev = my_list->tail;
		new_node->next = NULL;
		my_list->tail=new_node;
	}
	my_list->length++;	
	return 1;//success
}

/**
 * Returns the length of the list.
 */
int listLength(list* my_list)
{
	return my_list->length;
}

//find and return a node pointer to the found room, otherwise return NULL
room* list_get(list* my_list, char* room_name)
{
	if(room_name==NULL || my_list==NULL)
	{
		printf("error in list get\n");
		fprintf(stderr,"error");
		return NULL;
	}
	
	node* temp;
	for(temp=my_list->head; temp; temp=temp->next)
	{
		if(strcmp(room_name,temp->room->name)==0)
		{
			return temp->room;			
		}		
	}	
	return NULL;
}

int update_list(room* src_room,room* dest_room,char dir)
{
	if (src_room==NULL || dir=='\0')
	{
		printf("error in update\n");
		fprintf(stderr,"error");
		return 0;
	}
	if(dir=='U')
		src_room->portal[0]=dest_room;
	else if (dir=='R')
		src_room->portal[1] = dest_room;
	else if (dir=='D')
		src_room->portal[2] = dest_room;
	else if (dir=='L')
		src_room->portal[3] = dest_room;
	else 
	{
		fprintf(stderr,"error");
		return 0;
	}	
	return 1;
}

int doMove(char move,room* curr_room,status* player_status)
{
	if(move=='\0' || curr_room==NULL || player_status==NULL)
	{
		fprintf(stderr,"error");
		return 0;
	}
	switch (move)
	{
		case 'G':
			break;
		case 'M':
			break;
		case 'U':
			break;
		case 'R':
			break;
		case 'D':
			break;
		case 'L':
			break;
		default:
			break;
	}
}