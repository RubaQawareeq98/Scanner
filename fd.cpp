#include "fd.h"
#include <cstdio>
#include <cstring>

FileDescriptor::FileDescriptor() {
    fp = stdin;
    line_number = 1; // Start from the first line
    char_number = 0;
    flag = UNSET;
    buf_size = BUFFER_SIZE;
    buffer = new char[buf_size];
    file = nullptr;
    flag2 = UNSET;
}

FileDescriptor::FileDescriptor(char *FileName) {
    if (FileName == nullptr) {
        fp = stdin;
        file = nullptr;
    } else {
        fp = fopen(FileName, "r");
        if (fp == nullptr) {
            ReportError("Failed to open file");
            file = nullptr;
        } else {
            file = new char[strlen(FileName) + 1];
            strcpy(file, FileName);
        }
    }
    line_number = 1; // Start from the first line
    char_number = 0;
    flag = UNSET;
    buf_size = BUFFER_SIZE;
    buffer = new char[buf_size];
    flag2 = UNSET;
}

FileDescriptor::~FileDescriptor() {
    if (fp != nullptr && fp != stdin) {
        fclose(fp);
    }
    delete[] buffer;
    delete[] file;
}

char *FileDescriptor::GetFileName() {
    return file;
}

bool FileDescriptor::IsOpen() {
    return (fp != nullptr && fp != stdin);
}

char *FileDescriptor::GetCurrLine() {
    if (feof(fp))
        return nullptr; // End of file reached, return nullptr

    // Read a line from the file into the buffer
    if (fgets(buffer, buf_size, fp) != nullptr) {
        // Increment line number
        line_number++;
        // Reset character number
        char_number = 0;
        return buffer; // Return pointer to the buffer
    } else {
        // Error reading line
        return nullptr;
    }
}

int FileDescriptor::GetLineNum() {
    return line_number;
}

int FileDescriptor::GetCharNum() {
    return char_number;
}

void FileDescriptor::SetLineNumber(int n)
{
    line_number = n;
}

void FileDescriptor::Close() {
    if (fp != nullptr && fp != stdin) {
        fclose(fp);
        fp = nullptr;
    }
}

char FileDescriptor::GetChar() {
    char c = fgetc(fp);
    if (c == EOF) {
        return EOF;
    } else {
        if (flag == SET) {
            flag = UNSET;
        } else {
            char_number++;
            if (c == '\n') {
                line_number++;
                char_number = 0; // Reset character number at new line
            }
        }
        return c;
    }
}


void FileDescriptor::ReportError(char *msg) {
    fprintf(stderr, "Error in file %s at line %d, character %d: %s\n", file ? file : "stdin", line_number, char_number, msg);
}

void FileDescriptor::ReportError(char *msg, int line_number, int char_number)
{
        fprintf(stderr, "Error in file %s at line %d, character %d: %s\n", file ? file : "stdin", line_number, char_number, msg);

}

void FileDescriptor::UngetChar(char c) {
    if (c == EOF)
        return;
    if (flag == SET) {
        ReportError("Cannot unget two characters in a row");
        return;
    }
    if (c == '\n') {
        line_number--;
    } else {
        char_number--;
    }
    ungetc(c, fp);
    flag = SET;
}
