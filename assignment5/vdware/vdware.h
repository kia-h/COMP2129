#ifndef VDWARE_H
#define VDWARE_H

#include <stdio.h>

#define ABS(X) ( ( X ) < 0 ? ( -X ) : ( X ) )

#define BUFFERSIZE 255
#define MAX_DEVICES 10
#define MODULO 16

#define ONE_FLAG 0x08
#define Y_SIGN 0x20
#define X_SIGN 0x10
#define M_BUTTON 0x03
#define R_BUTTON 0x02
#define L_BUTTON 0x01

#define SHIFT 0x40
#define CAPS 0x80

typedef struct Device Device;
typedef struct timespec timespec;

struct Device
{
    // files
    char name[BUFFERSIZE];
    size_t n_elements;
    int pipe_fd;
    FILE *script_stream;
    FILE *pipe_stream;

    // codec (encode/decode)

    int (*codec)(char *, const char *, timespec *);

    // header
    union
    {
        struct
        {
            int vendor;
            int device;
        } id;
        char str[5];
    } header;
};

/* function protoypes */

/*!
 \brief initialize the driver with a respective DID and VID, pipe name, script file descriptor and output (pipe) fd
 \param file name of the script file to read from (e.g. ./usb/input0)
 \param devn which device this is (for ./dev/USB#), does not necessarily correspond to above filename
 \param new_dev_file to print the device id into
 \return codec for encoding the in the correct packet
*/
void
initiate_driver(char *file, FILE *new_dev_file );

/*!
 \brief main driving capabilities of the process: encode data from script and send messages to pipe
*/
void
driver(void);

/*!
 \brief terminate the process to safely handle SIGINT (most likely student signal to require handling)
 \param signal to handle
*/
void
parent_interrupt(int sig);

/*!
 \brief terminate the process to safely handle SIGINT (most likely student signal to require handling)
 \param signal to handle
*/
void
child_interrupt(int sig);

/*!
 \brief parse human readable mouse activities from one line of a script file and encode this to a machine readable message body.
 \param to string to be encoded
 \param from the line of input previously scanned from the script file
 \param delay pointer to the time wait wait until sending this new message
 \return the time delay until next pipe in seconds and nanoseconds
*/
int
mouse_codec( char *to, const char *from, struct timespec *delay );

/*!
 \brief parse human readable keyboard activities from one line of a script file and encode this to a machine readable message body.
 \param to string to be encoded
 \param from the line of input previously scanned from the script file
 \param delay pointer to the time wait wait until sending this new message
 \return the time delay until next pipe in seconds and nanoseconds
*/
int
keyboard_codec( char *to, const char *from , struct timespec *delay);

/*!
 \brief garbage collection function to close pipes (file descriptors), free memory, unlink pipes, and anything else then die
 \param status to exit with
*/
void
cleanup_and_die(int status);

#endif // VDWARE_H
