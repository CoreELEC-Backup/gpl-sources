/*
 * lftp - file transfer program
 *
 * Copyright (c) 1996-2012 by Alexander V. Lukyanov (lav@yars.free.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CATJOB_H
#define CATJOB_H

#include "CopyJob.h"
#include "StatusLine.h"
#include "OutputJob.h"

class ArgV;

class CatJob : public CopyJobEnv
{
protected:
   JobRef<OutputJob> output;
   bool ascii;
   bool auto_ascii;

   void	NextFile();

public:
   int Do();
   int Done();
   int ExitCode();

   CatJob(FileAccess *s,OutputJob *output,ArgV *args);

   void Ascii() { ascii=true; }
   void Binary() { ascii=auto_ascii=false; }
   void ShowRunStatus(const SMTaskRef<StatusLine>&);
};

#endif /* CATJOB_H */
