#ifndef DW_H_
#define DW_H_

/* type definitions */
typedef struct room room;
typedef struct node node;
typedef struct list list;

/* for directions for door placement in a room */
enum direction {UP=0, RIGHT=1, DOWN=2, LEFT=3};

/* data structure to store room information */
struct room {
  char name[21];      /* room name */
  int num_doughnuts;  /* number of doughnuts in the room */
  int num_milkshakes; /* number of milkshakes in the room */
  room* portal[4];    /* portals of the room for each direction
                           + portal[LEFT] -> left portal
                           + portal[RIGHT] -> right portal
                           + portal[UP] -> top portal
                           + portal[DOWN] -> bottom portal */
};

/* doubly linked list node to store each room */
struct node {
  node* next; /* pointer to next room in list */
  node* prev; /* pointer to previous node in list */
  room* room; /* pointer to room data structure */
};

/* doubly linked list to store all rooms */
struct list {
  int length; /* length of the entire list */
  node* head; /* pointer to the head of the list */
  node* tail; /* pointer to the tail of the list */
};

#endif
