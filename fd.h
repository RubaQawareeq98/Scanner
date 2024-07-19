#include <cstdio>
#define SET 1
#define UNSET 0
#define BUFFER_SIZE 256
class FileDescriptor
{
/* You must fill in these fields */
private:
FILE *fp;
int line_number; /* line number in the file */
int char_number; /* character number in the line */
int flag; /* to prevents two ungets in a row */
int buf_size; /* stores the buffer size */
char *buffer; /* buffer to store a line */
char *file; /* file name, allocate memory for this */
int flag2;
// add other fields or functions if you want

public:
/* Externally-visible functions: */
char *GetFileName();
bool IsOpen() ; // returns true if a file is open without errors,

//otherwise returns false

char *GetCurrLine(); // returns a pointer to buffer, null if EOF
int GetLineNum();
int GetCharNum();
void SetLineNumber( int n);
FileDescriptor(char *FileName); // opens the stdin FileName is Null
~FileDescriptor();
FileDescriptor(); //opens the stdin
void Close ( ); // closes the file descriptor
char GetChar (); // Gets the current character in the file
//bool open_file_descriptor (char *filename);
void ReportError (char *msg); // reports the error specifying the current line and
void ReportError (char *msg, int line_number, int char_number); // reports the error specifying the current line and

//character

void UngetChar (char c); // puts back the current character, modifies char number
};