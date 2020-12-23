#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <vdr/tools.h>
#include <vdr/thread.h>
#include "extpipe.h"



cExtPipe::cExtPipe(void)
{
    pid = -1;
    f_stderr = -1;
    f_stdout=  -1;
}

cExtPipe::~cExtPipe()
{
    int status;
    Close(status);
}

bool cExtPipe::Open(const char *Command)
{
    int fd_stdout[2];
    int fd_stderr[2];

    if (pipe(fd_stdout) < 0)
    {
        LOG_ERROR;
        return false;
    }
    if (pipe(fd_stderr) < 0)
    {
        close(fd_stdout[0]);
        close(fd_stdout[1]);
        LOG_ERROR;
        return false;
    }

    if ((pid = fork()) < 0)   // fork failed
    {
        LOG_ERROR;
        close(fd_stdout[0]);
        close(fd_stdout[1]);
        close(fd_stderr[0]);
        close(fd_stderr[1]);
        return false;
    }

    if (pid > 0)   // parent process
    {
        close(fd_stdout[1]); // close write fd, we need only read fd
        close(fd_stderr[1]); // close write fd, we need only read fd
        f_stdout = fd_stdout[0];
        f_stderr = fd_stderr[0];
        return true;
    }
    else   // child process
    {
        close(fd_stdout[0]); // close read fd, we need only write fd
        close(fd_stderr[0]); // close read fd, we need only write fd

        if (dup2(fd_stdout[1], STDOUT_FILENO) == -1)   // now redirect
        {
            LOG_ERROR;
            close(fd_stderr[1]);
            close(fd_stdout[1]);
            _exit(-1);
        }

        if (dup2(fd_stderr[1], STDERR_FILENO) == -1)   // now redirect
        {
            LOG_ERROR;
            close(fd_stderr[1]);
            close(fd_stdout[1]);
            _exit(-1);
        }

        int MaxPossibleFileDescriptors = getdtablesize();
        for (int i = STDERR_FILENO + 1; i < MaxPossibleFileDescriptors; i++)
            close(i); //close all dup'ed filedescriptors
        if (execl("/bin/sh", "sh", "-c", Command, NULL) == -1)
        {
            LOG_ERROR_STR(Command);
            close(fd_stderr[1]);
            close(fd_stdout[1]);
            _exit(-1);
        }
        _exit(0);
    }
}

int cExtPipe::Close(int &status)
{
    int ret = -1;

    if (f_stderr!=-1)
    {
        close(f_stderr);
        f_stderr = -1;
    }

    if (f_stdout!=-1)
    {
        close(f_stdout);
        f_stdout=-1;
    }

    if (pid > 0)
    {
        int i = 5;
        while (i > 0)
        {
            ret = waitpid(pid, &status, WNOHANG);
            if (ret < 0)
            {
                if (errno != EINTR && errno != ECHILD)
                {
                    LOG_ERROR;
                    break;
                }
                else if (errno == ECHILD)
                {
                    ret = pid;
                    break;
                }
            }
            else if (ret == pid)
                break;
            i--;
            cCondWait::SleepMs(100);
        }
        if (!i)
        {
            kill(pid, SIGKILL);
            ret = -1;
        }
        else if (ret == -1 || !WIFEXITED(status))
            ret = -1;
        pid = -1;
    }

    return ret;
}
