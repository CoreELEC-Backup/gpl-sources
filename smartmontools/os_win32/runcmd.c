/*
 * Run console command and wait for user input
 *
 * Home page of code is: http://www.smartmontools.org
 *
 * Copyright (C) 2011 Christian Franke
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

char svnid[] = "$Id: runcmd.c 4760 2018-08-19 18:45:53Z chrfranke $";

#include <stdio.h>
#include <windows.h>

int main(int argc, char **argv)
{
  char * cmd = GetCommandLineA();
  DWORD exitcode;
  STARTUPINFOA si = { sizeof(si), };
  PROCESS_INFORMATION pi;
  int key;

  if (*cmd == '"') {
    cmd++;
    while (*cmd && !(*cmd == '"' && cmd[-1] != '\\'))
      cmd++;
    if (*cmd)
      cmd++;
  }
  else {
    while (*cmd && !(*cmd == ' ' || *cmd == '\t'))
      cmd++;
  }

  while (*cmd == ' ' || *cmd == '\t')
    cmd++;

  if (*cmd) {
    printf("%s\n\n", cmd); fflush(stdout);
  }

  if (!*cmd) {
    printf("Usage: %s COMMAND [ARG ...]\n", argv[0]);
    exitcode = 1;
  }
  else if (!CreateProcessA((char *)0, cmd,
      (SECURITY_ATTRIBUTES *)0, (SECURITY_ATTRIBUTES *)0,
      TRUE/*inherit*/, 0/*no flags*/, (void *)0, (char *)0, &si, &pi)
  ) {
    DWORD err = GetLastError();
    if (err == ERROR_FILE_NOT_FOUND)
      printf("Command not found\n");
    else
      printf("CreateProcess() failed with error=%u\n", err);
    exitcode = 1;
  }
  else {
    CloseHandle(pi.hThread);

    exitcode = 42;
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitcode);
    CloseHandle(pi.hProcess);

    if (exitcode)
      printf("\nExitcode: %u (0x%02x)", exitcode, exitcode);
  }

  printf("\nType <return> to exit: "); fflush(stdout);
  while (!((key = getc(stdin)) == EOF || key == '\n' || key == '\r'))
    ;
  printf("\n");

  return exitcode;
}
