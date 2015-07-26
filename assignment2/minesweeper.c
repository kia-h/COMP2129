// author: Internet's own boy

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define BUFSIZE 6

int WIDTH,HEIGHT ; // width and heoght of the board
const int NUMBOMBS = 10; // number of bombs on the board

 
void drawBorder(char [][HEIGHT]);
int numberbombs(int,int,int[][2]);
int domove(char[][HEIGHT], char[][HEIGHT], int[][2], int, int, char, int*,int*);
void getbombs(int[][2]);
void getmove(char*, int*, int*, int*, int*, int*);
int valid(int,int);

int main() 
{
    
    //open the file and read data
    FILE *in = fopen("tests/sample.in", "r");

    if (in == NULL) 
    {
      fprintf(stderr, "error\n");
      exit(1);
    }
    
    char buff[BUFSIZ];
    int xx,yy,args;
    char temp;
    fgets( buff, sizeof buff, in );
    args = sscanf( buff, "%c %d %d", &temp, &xx, &yy );
    
    if (args!=3 || temp!='g') 
    { 
        fprintf(stderr,"error\n");
        exit(1);
    }
    fclose(in);    
    
    WIDTH=xx;
    HEIGHT=yy;  
    if (WIDTH*HEIGHT<10)
    {
        fprintf(stderr,"error\n");
        exit(1);
    }
    printf ("%c %d %d\n",temp,xx,yy); 
    int gameover = 0; // used to check if the game is over or not
  
    char uncovered_board[HEIGHT][WIDTH]; // board that stores the location of all the bombs and numbers of adjacent bombs

    char current_board[HEIGHT][WIDTH]; // stores the current board as it should be printed out

    int totalmoves; // stores the total moves left for the user before they win

    int listbombs[NUMBOMBS][2]; // stores (x,y) coordinates of all bombs

    int i,j;
    int x,y;
    char cell;
    int numbombs = NUMBOMBS;  
    int numberf = NUMBOMBS;  
    int numberu = WIDTH*HEIGHT - 2;
  
    // Initialize the current board.
    for (i=0;i<HEIGHT;i++) {
        for (j=0;j<WIDTH;j++) {
            current_board[i][j] = '*';
            uncovered_board[i][j] = '*';             
        }
    }
    
    // get bombs location from given input file and print it out and place it on the board
    getbombs(listbombs);
    for (i=0;i<NUMBOMBS;i++)
    {
        printf("b %d %d\n",listbombs[i][0],listbombs[i][1]);
        uncovered_board[listbombs[i][1]][listbombs[i][0]]='#';        
    }
     
    totalmoves = WIDTH*HEIGHT;
    int toRead = NUMBOMBS+1;
    drawBorder(current_board);    
    
    // Continue playing until the game ends.
    while (!gameover) {
        
        // Get a move from the file
        getmove(&cell,&x,&y,&toRead,&numberf,&numberu);
                  
        // Execute that move.
        gameover = domove(current_board,uncovered_board,listbombs,x,y,cell,&totalmoves,&numbombs);
    
        // Check if they have won.
        if ((!gameover) && (totalmoves==0) && (numbombs==0)) 
        {
            printf("won\n");
            gameover=1;
        }        
    }    
    return 0;  
}

//get the movement coordinate and type..f/u...from the file
void getmove(char *cell,int *x,int *y, int* read,int* numberf, int* numberu) {

    // read move data from file
    FILE *in = fopen("tests/sample.in", "r");

    if (in == NULL) 
    {
      fprintf(stderr, "error\n");
      exit(1);
    }

    char buff[BUFSIZ];
    int xx,yy,args;
    char temp;

    for (int i=0;i<(*read);i++) 
        fgets( buff, sizeof buff, in );

    if (fgets( buff, sizeof buff, in ) != NULL);
        args = sscanf( buff, "%c %d %d", &temp, &xx, &yy );
    
    if (args!=3) 
    { 
        fprintf(stderr,"error\n");
        exit(1);
    }
    if ((*numberf)==0)
    { 
        fprintf(stderr,"error\n");
        exit(1);
    }   
    if ((*numberu)==0)
    { 
        fprintf(stderr,"error\n");
        exit(1);
    }   
    if (temp=='f')
    {
        *cell='f';
        *x=xx;
        *y=yy;
        (*numberf)--;

    }
    else if (temp=='u')
    {
        *cell='u';
        *x=xx;
        *y=yy;
        (*numberu)--;
    }
    fclose(in);

    printf("%c %d %d\n",*cell,*x,*y);
    (*read)++;
         
    // check the validation of the input coordinate
    if (!valid(*x,*y)) {
        fprintf(stderr, "error\n");
        exit(1);    
    }
}

//do the move based on type of cell,u/f, and its coordinates
//# used to show the bombs, * for uncoverd 
int domove(char board[][WIDTH], char realboard[][WIDTH],
            int listbombs[][2], int row, int column, char cellType, 
            int *totalmoves,int *numbombs) 
{

    int num;  
        
    if (cellType=='u')
    {
        if (realboard[column][row]=='#') 
        {
           printf("lost!\n");
           return 1;
        }
        else 
        {
            num = numberbombs(row, column, listbombs);
            board[column][row]=(char)(num+'0');
            (*totalmoves)--;            
            drawBorder(board);
            return 0;
        }
    }    
    if (cellType=='f') 
    {
        if (realboard[column][row]=='#')
            (*numbombs)--;
        board[column][row]='f';
        (*totalmoves)--;        
        drawBorder(board);
        if (numbombs!=0)
            return 0;
        else 
            return 1;       
    }
    return 0;    
}

// check to see if given coordinate is in the valid range
int valid(int row, int column) 
{

    // Check four possibilities for an invalid coordinate.
    if (row < 0) return 0;
    
    else if (row >= WIDTH) return 0;
  
    else if (column < 0) return 0;
  
    else if (column >= HEIGHT) return 0;
  
    // all non valid passed so we have valid 
    else return 1;
}

// finds the number of adjacent bombs to the given coordinate of the cell to be uncovered
int numberbombs(int row ,int column ,int listbombs[][2]) 
{

    int i;
    int count = 0;
    for (i=0;i<NUMBOMBS;i++) 
    {

        if ((abs(row-listbombs[i][0])==1) || (abs(row-listbombs[i][0])==0)) 
        {
            if ((abs(column-listbombs[i][1])==1) || (abs(column-listbombs[i][1])==0))
                count++; 
        }  
    }
    
    return count;
}


//get the bombs location from the input file and put in 2 dimensional array
void getbombs(int bomblist[][2]) 
{
  
    char buff[BUFSIZ];
    int x,y,args;
    char temp;
    
    FILE *in = fopen("tests/sample.in", "r");

    if (in == NULL) 
    {
      fprintf(stderr, "error\n");
      exit(1);
    }    
    
    fgets( buff, sizeof buff, in );
    for (int i=0;i<NUMBOMBS;i++)  
    {
        fgets( buff, sizeof buff, in );        
        args = sscanf( buff, "%c %d %d", &temp, &x, &y );
        if (args!=3) 
        { 
            fprintf(stderr,"error\n");
            exit(1);
        }
        if (temp!='b') 
        {
            fprintf(stderr,"error\n");
            exit(1);
        }
        bomblist[i][0]=x;
        bomblist[i][1]=y;        
    }
    fclose(in);    
}

//draw the board
void drawBorder(char board[][WIDTH])
{
       
    int width=WIDTH,height=HEIGHT;
    if (width*height <10)
    { 
        fprintf(stderr,"error\n");
        exit(1);
    }
    
    width+=2;
    height+=2;
    int k;
    for(int i=0;i<height;i++)
    {
        if(i==0 || (i==height-1))
        {
            printf("+");
            for (int k=0;k<WIDTH;k++)
                printf("-");
            printf("+\n");            
        }          
        else 
        {
            printf("|");
            for (k=0;k<width-2;k++)
                printf("%c",board[i-1][k]);
            printf("|\n");                   
        }
    }
}
