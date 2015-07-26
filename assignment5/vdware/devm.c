/*
 * author: Internet's own boy
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include <sys/stat.h>

#define BUFSIZE 64
#define MAX 1024

/* function protoypes */

/* SIGTERM signal handler.
 * recives signal, log it in devm.log, and terminate the prgram, on error opening file or reading from file returns
 */
void t_handler(int signo);

/* SIGUSR1 signal handler.
 * recives signal and handles it, on error opening file or reading from returns
 */
void u_handler(int signo);

/* find the index of passed device id in the already read device list and returs the index
 * @input: passed[][11] is the already read device list with id and their type(mouse/keyboard)
 *                    passsedid is the device id passed to be searched                  
 *                    length is the actual lenght of the passed array
 * @output: on successfull returns the index of the found device id, otherwise returns -1 
 */
int find(char passed[][11],char* passedid);

/*Global variables*/

char list_array[MAX][11];
int length;
char* curtime; 

//for dev.log file 
FILE* dlog;

/**
 * Main function
 */
int main(int argc, char** argv)
{  
  char buff[BUFSIZ]; 
  int i=0;   
  
  /*get the current time*/
  curtime = (char*)(time (NULL)); 

  /* open dev.log and devm.pid file for writing the relevant data */
  dlog = fopen("./devm.log", "a+");
  FILE *dpid = fopen("./devm.pid" ,"w+");
  
  if (dlog == NULL) 
  {
    fprintf(stderr, "error opening <devm.log> file\n");
    exit(1);
  }       

  if (dpid == NULL) 
  {
    fprintf(stderr, "error opening <devm.pid> file\n");
    exit(1);
  }   

  //write the process PID to devm.pid file and close it
  fprintf(dpid, "%d\n",getpid()); 
  fclose(dpid);

  //write the data to devm.log file
  fprintf(dlog,"[%ld] ",(long int)curtime);
  fprintf(dlog,"devm started\n");      

  if ( argc != 2 ) /* argc should be 2 for correct execution */
    {        
      fprintf(stderr, "error in number of files\n");
       exit(1);
    }
    else /* read the known_devices file and save the device data in list_array variable */
    {       
      FILE *in = fopen( argv[1], "r" );

      if (in == NULL) 
      {
        fprintf(stderr, "error in opening the file\n");
        exit(1);
      }
      int index;           

      /* loop through the file, read the lines till EOF, 
       * searchs for 'mouse' or 'keyboard' and save the found device in the list_array variable 
       */
      while( fgets( buff, sizeof buff, in ) != NULL ) 
      {              
        if ((strstr(buff,"mouse"))!=NULL || (strstr(buff,"Mouse"))!=NULL)
        {                        
          index = strcspn(buff,":");             

          strncpy(list_array[i],&buff[index-4],9);  
          list_array[i][10]='m';             
          i++;            
        }
        else if ((strstr(buff,"keyboard"))!=NULL || (strstr(buff,"Keyboard"))!=NULL)
        {           
          index = strcspn(buff,":");
          strncpy(list_array[i],&buff[index-4],9); 
          list_array[i][10]='k';           
          i++;                        
        }           
      } //end of while       
      fclose(in);
    } //end of else
    
    //holds the number of non-empty cell in the list_array
    length=i;

    /*star of the catching signal, opening FIFO file */   
    
    int fdnew_dev;     

    if ((fdnew_dev = open("./new_devices", O_RDONLY)) <0)  
    {
      fprintf(stderr, "error in opening fifo\n");
      exit(1);
    }
    fdopen(fdnew_dev, "r");  

    signal(SIGTERM, t_handler); //handler for SIGTERM signal
    signal(SIGUSR1, u_handler); //handler for SIGUSR1 signal    

    while (1) 
    {      
      sleep(1);
    }

  fclose(dlog); 

  //remove the devm.pid file
  if(remove("./devm.pid")!=0)
  {
      fprintf(stderr, "error\n");
      exit(1);
  }   

  return 0;
}/*end of main*/

/*==================================================Helper Fuction==================================================*/

/*SIGUSR1 Handler */
void u_handler(int signo)
{

  char data[11];
  int num, fd;
  char result[9];    
  
  //open FIFO, and log file for reading and logging
  fd = open("./new_devices", O_RDONLY);
  dlog = fopen("./devm.log", "a+");
  if (dlog == NULL) 
  {
    fprintf(stderr, "error opening log\n");
    exit(1);
  }         
  
  int x=0;

  do 
  {
    if ((num = read(fd, data, 10)) == -1)
      perror("read");
    else 
    {
      data[num] = '\0';
      int index;

      //found the device the list record the data in the devm.log file
      index = find(list_array,data);
      if  (index!=-1)
      { 
        fprintf(dlog,"[%ld] ",(long int)curtime);
        fprintf(dlog,"Device connected :");
        list_array[index][10]=='m' ? fprintf(dlog," mouse ") : fprintf(dlog," keyboard ");
        strncpy(result,data,9);
        fprintf(dlog,"%s at dev/USB%d\n",result,x++);
      }            
    }
  } while (num > 0);
  fclose(dlog);
  exit(signo);
}

/*SIGTERM Handler */
void t_handler(int signo)
{
  pid_t my_pid;

  dlog = fopen("./devm.log", "a+");
  if (dlog == NULL) 
  {
      fprintf(stderr, "error opening log file in SIGTERM handler\n");
      exit(1);
  }       

  fprintf(dlog,"[%ld] ",(long int)curtime);
  fprintf(dlog,"SIGTERM caught, devm closing\n"); 
  fclose(dlog); 
  
  fflush(stdout);
  my_pid = getpid();
  kill(my_pid, SIGTERM); 
  exit(signo); 
}

/*Searchs the list_array for passed id. 
 * on success returns the found id index otherwise retuns -1 
 */
int find(char passed[][11],char* passedid)
{
  for (int k=0;k<length;k++)
  {
    if ((strstr(passedid,passed[k])) !=0)
      return k;
  }
  return -1;
}