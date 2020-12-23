#ifndef _EXTPIPE_H
#define _EXTPIPE_H

#include <sys/types.h>
#include <stdio.h>

class cExtPipe
{
private:
    pid_t pid;
    int f_stdout;
    int f_stderr;
public:
    cExtPipe(void);
    ~cExtPipe();
    int Out() const
    {
        return f_stdout;
    }
    int Err() const
    {
      return f_stderr;
    }
    bool Open(const char *Command);
    int Close(int &status);
};

#endif

