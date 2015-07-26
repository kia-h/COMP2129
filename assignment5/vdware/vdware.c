#include "vdware.h"

// standard includes
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <time.h>
#include <math.h>

// system includes
#define _OPEN_SYS
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

// some file localities
#define SCRIPT "./usb/%s"
#define USB_PIPE "./dev/USB%d"
#define NEW_DEVICE_PIPE "./new_devices"

#define PROCESS "\033[0m[\033[1;32mVIRTUAL DEVICE\033[0m]\033[0m:"
#define SUBPROCESS (_my_device->name)

// device stuff
Device *_my_device;
char ERR_FLAG = 0;
pid_t devm_pid;
int new_dev_fd;
int shmid;
int *shm;

// shared memory stuff
key_t key = 1234;

const char CHAR_SET[] =
    "0123456789"
    "-=[]\\;',./"
    "abcdefghijklmnopqrstuvwxyz" // lowercase set
    ")!@#$%^&*("
    "_+{}|:\"<>?"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // uppercase set

/*
 Main program
*/
int
main(void)
{
    pid_t my_children[MAX_DEVICES];
    DIR *script_dir;
    struct dirent *script;
    FILE *known_dev_stream, * devm_stream, * new_dev_file;

    shmid = shmget( key, sizeof(int), 0666 | IPC_CREAT);
    shm = shmat(shmid, 0, 0);
    *shm = 0;

    if ( mkfifo( NEW_DEVICE_PIPE , S_IRUSR | S_IWUSR ) != 0 )
    {
        perror("mkfifo() error!\n");
        return EXIT_FAILURE;
    }

    printf("Waiting for someone to open %s...\n", NEW_DEVICE_PIPE);
    new_dev_fd = open( NEW_DEVICE_PIPE, O_WRONLY );
    new_dev_file = fdopen(new_dev_fd, "w");

    // Read devm's PID
    while ( ( devm_stream = fopen( "devm.pid", "r" ) ) == NULL )
    {
        sleep(1);
    }

    if ( fscanf( devm_stream, "%d\n", &devm_pid ) < 1 )
    {
        perror("No PID written to devm.pid, exiting.\n");
        return EXIT_FAILURE;
    }
    fclose( devm_stream );

    known_dev_stream = fopen( "known_devices", "r" );
    script_dir = opendir("./usb");

    printf("%s Setting up processes, do not press `ctrl + c' in the next few nanoseconds...\n", PROCESS);
    int children = 0;
    if ( script_dir )
    {
        // find all device input files (in ./usb) and fork a process for each file...
        while (( script = readdir( script_dir )) != NULL)
        {
            // ...whilst ignoring leading '.' files
            if ( *script->d_name != '.' )
            {
                // fork our process to start the virtual device/driver
                my_children[children] = fork();
                if ( my_children[children] == 0 )
                {
                    initiate_driver( script->d_name, new_dev_file );
                    driver();
                }
                ++children;
            }
        }
        closedir( script_dir );
    }

    // setup parent signal handler in local scope, don't need act after use
    {
        struct sigaction act;

        memset(&act, 0, sizeof(act));
        act.sa_handler = parent_interrupt;

        sigaction(SIGINT,  &act, 0);
        sigaction(SIGTERM, &act, 0);
    }
    fclose( known_dev_stream );

    printf("%s Set up processes, press `ctrl + c' (2) or send SIGTERM (15) to exit safely\n", PROCESS);

    for (int i = 0; i < children; ++i)
    {
        int status;
        // check we haven't received an interrupt and the children haven't failed
        while ( ERR_FLAG == 0 &&
                waitpid( my_children[i], &status, 0) == -1 )
        {
            sleep(1);
        }

        if ( ERR_FLAG )
        {
            kill( my_children[i], SIGINT );
        }
        else if ( ! WIFEXITED(status) || WEXITSTATUS(status) != 0 )
        {
            printf("%s Process %d (pid %d) failed\n", PROCESS, i, my_children[i]);
        }
    }

    printf("%s Reached end of control, unlinking %s FIFO\n", PROCESS, NEW_DEVICE_PIPE);
    close( new_dev_fd );
    unlink( NEW_DEVICE_PIPE );
    kill(devm_pid, SIGTERM);

    return 0;
}

/*

*/
void
initiate_driver(char *file, FILE *new_dev_file )
{
    char str[BUFFERSIZE], type[BUFFERSIZE], script_file[BUFFERSIZE];
    struct timespec delay;

    _my_device = (Device *) malloc(sizeof(Device));
    if ( _my_device == NULL )
    {
        perror("Que Paso? Malloc failed. Deallocate heap or increase heap size.");
        exit(EXIT_FAILURE);
    }

    // setup child signal handler in local scope (don't need act after use).
    {
        struct sigaction act;

        memset(&act, 0, sizeof(act));
        act.sa_handler = child_interrupt;

        sigaction(SIGINT,  &act, 0);
        sigaction(SIGTERM, &act, 0);
    }

    // initialize pipe name for read/write operations
    sprintf( script_file, SCRIPT, file );

    // open script and...
    _my_device->script_stream = fopen( script_file, "r" );
    if ( file == NULL )
    {
        printf("[\033[1;31m%s:\033[1;33m%d\033[0m] File %s open failure.\n", SUBPROCESS, __LINE__, script_file);
        fflush(stdout);
        cleanup_and_die(EXIT_FAILURE);
    }

    // ... read in the first time delay, vendor and device ID as well as type (mouse/keyboard)
    fgets(str, BUFFERSIZE, _my_device->script_stream);
    sscanf(str, "%ld %ld %x:%x %s", &delay.tv_sec,
           &delay.tv_nsec,
           &_my_device->header.id.vendor,
           &_my_device->header.id.device,
           type );


    nanosleep( &delay, NULL );

    printf("Connecting %04x:%04x\n", _my_device->header.id.vendor, _my_device->header.id.device );
    fprintf(new_dev_file, "%04x:%04x\n", _my_device->header.id.vendor, _my_device->header.id.device );
    fflush(stdout);
    fflush(new_dev_file);

    sprintf( _my_device->name, USB_PIPE, (*shm)++ );

    // make our named pipe with read/write permission
    if ( mkfifo( _my_device->name , S_IRUSR | S_IWUSR ) != 0 )
    {
        perror("mkfifo() error!\n");
        cleanup_and_die(EXIT_FAILURE);
    }

    // Wake up devm!
    kill( devm_pid, SIGUSR1 );

    // make a read only named pipe './dev/USB<device#>'
    _my_device->pipe_fd = open( _my_device->name, O_WRONLY );
    _my_device->pipe_stream = fdopen( _my_device->pipe_fd , "w");

    // alas, there is no 'eval' function in C, so we'll get a bit `if'fy here
    // and return some function pointers to the correct driver.
    if (strcmp( type, "mouse" ) == 0)
    {
        _my_device->codec = mouse_codec;
        _my_device->n_elements = 3;
    }
    else if (strcmp( type, "keyboard" ) == 0)
    {
        _my_device->codec = keyboard_codec;
        _my_device->n_elements = 1;
    }
    else
    {
        printf("[\033[1;31m%s:\033[1;33m%d\033[0m] What is a '%s'? Something went wrong with input scripts.\n", SUBPROCESS, __LINE__, type);
        cleanup_and_die(EXIT_FAILURE);
    }

    printf( "[%s] streaming data from %s\n", SUBPROCESS, file );
}

void
driver(void)
{
    char input[BUFFERSIZE], msg[BUFFERSIZE];
    struct timespec sleep_time;

    // get lines of input from activity script until EOF...
    while ( fgets(input, BUFFERSIZE - 1, _my_device->script_stream ) != NULL )
    {
        // ... and send these into our codec manage
        if ( _my_device->codec( msg, input, &sleep_time ) != 0 )
        {
            perror("Codec returned error, exiting...\n");
            cleanup_and_die(EXIT_FAILURE);
        }
        nanosleep( &sleep_time, NULL );

        // write n_elements to standard out
        if ( fwrite( msg, CHAR_BIT / 8, _my_device->n_elements, _my_device->pipe_stream ) != _my_device->n_elements )
        {
            printf("[\033[1;31m%s:\033[1;33m%d\033[0m] Something went wrong.\n", SUBPROCESS, __LINE__);
            cleanup_and_die(EXIT_FAILURE);
        }
        fflush( _my_device->pipe_stream );
        fflush(stdout);
    }

    // garbage collection
    cleanup_and_die(EXIT_SUCCESS);
}


/*

*/
int
mouse_codec( char *to, const char *from, struct timespec *delay )
{
    unsigned char clicks, scroll;
    signed char u, v;
    char garbage[BUFFERSIZE];

    // move our union to the output message string
    //msg.str = to;

    // scan our script file for activity (see below)
    if ( sscanf(from, "%ld %ld %hhu %hhu %hhi %hhi %s\n", &delay->tv_sec,
                &delay->tv_nsec,
                &clicks,
                &scroll,
                &u, &v, garbage) > 6 )
    {
        printf("[\033[1;31m%s:\033[1;33m%d\033[0m] Unexpected input %s.\n", SUBPROCESS, __LINE__, garbage);
        return -1;
    }
    else if ( abs(v) >= pow(2, 7) || abs(u) >= pow(2, 7) || scroll > 4 )
    {
        printf("[\033[1;31m%s:\033[1;33m%d\033[0m] Something went wrong with input files.\n", SUBPROCESS, __LINE__ );
        return -1;
    }

    /* nasty encoding technique:
        message: <7b y vel><7b x vel><2b scroll><2b clicks>
            -y vel [0-127]
            -x vel [0-127]
            -scroll {down, stationary, up, redundant}
            -clicks {left, middle, right, none}
    */

    unsigned char flags = clicks | ( ( u < 0 ) * X_SIGN ) | ( ( v < 0 ) * Y_SIGN );
    short int movement = ( abs(v) << 9 ) | ( abs(u) << 2 ) | scroll;

    /*
        Note I'm not using a union/struct combo. This type of encoding can
        be risky when considering implementation based byte alignment.
    */

    to[0] = flags;
    to[1] = (unsigned char) (movement >> CHAR_BIT); // first byte of word
    to[2] = (unsigned char) movement; // second byte of word

    return 0;
}

/*

*/
int
keyboard_codec(char *to, const char *from, struct timespec *delay )
{
    unsigned char shift, caps, key;
    char garbage[BUFFERSIZE];

    // scan our script file for activity (key presses)...
    if ( sscanf(from, "%ld %ld %hhu %hhu %hhu %s\n", &delay->tv_sec,
                &delay->tv_nsec,
                &key,
                &shift, &caps, garbage) > 6 )
    {
        printf("[\033[1;31m%s:\033[1;33m%d\033[0m] Unexpected input %s.\n", SUBPROCESS, __LINE__, garbage);
        return -1;
    }
    else if ( shift > 1 || caps > 1 || key > strlen(CHAR_SET) )
    {
        printf("[\033[1;31m%s:\033[1;33m%d\033[0m] Something went wrong with input files.\n", SUBPROCESS, __LINE__ );
        return -1;
    }

    // ...and send the key as one char
    *to = ( key | ( shift * SHIFT ) | ( caps * CAPS ) );
    return 0;
}

/*

*/
void
parent_interrupt(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
    {
        printf("\nReceived signal %d, performing shutdown duties...\n", signo);
        close( new_dev_fd );
        unlink( NEW_DEVICE_PIPE );
        kill(devm_pid, SIGTERM);
        ERR_FLAG = 1;
    }
}

/*

*/
void
child_interrupt(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
    {
        // pass signal to cleanup fcn
        cleanup_and_die(signo);
    }
}

/*

*/
void
cleanup_and_die(int status)
{
    fclose( _my_device->pipe_stream );
    close( _my_device->pipe_fd );
    unlink( _my_device->name );
    free( _my_device );
    shmdt( shm );
    exit( status );
}
