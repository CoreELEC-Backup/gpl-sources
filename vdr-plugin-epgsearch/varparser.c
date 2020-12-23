/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#include "varparser.h"
#include <vdr/plugin.h>
#include "log.h"
#include "epgsearchtools.h"
#include <sstream>

bool cVarParser::Parse(const string& input)
{
    return ParseAssign(input);
}

bool cVarParser::ParseAssign(const string& input)
{
    int assignPos = input.find("=");
    if (assignPos >= 0) {
        string var(input.begin(), input.begin() + assignPos);
        if (ParseVar(var)) {
            varName = Strip(var);
            string assign(input.begin() + assignPos + 1, input.end());
            return ParseExp(assign);
        }
    }
    LogFile.eSysLog("error parsing '%s'", input.c_str());
    return false;
}

bool cVarParser::ParseExp(const string& input)
{
    // system call?
    int sysPos = input.find("system");
    if (sysPos == 0)
        return ParseShellCmd(input);
    // connect command?
    int conPos = input.find("connect");
    if (conPos == 0)
        return ParseConnectCmd(input);
    // length command?
    int lenPos = input.find("length");
    if (lenPos == 0)
        return ParseLengthCmd(input);
    // conditional expression?
    int varPos = Strip(input).find("%");
    if (varPos == 0) {
        int queryPos = input.find("?");
        if (queryPos >= 0) {
            int colonPos = input.find(":");
            if (colonPos > queryPos)
                return ParseCondExp(input);
        }
    }
    // composed expression
    compExpr = input;
    return true;
}

bool cVarParser::ParseShellCmd(const string& input)
{
    int cmdPos = input.find("(");
    int cmdArgsBegin = input.find(",");
    int cmdArgsEnd = input.rfind(")");
    if (cmdPos == -1 || cmdArgsEnd == -1) return false;
    string shellcmd(input.begin() + cmdPos + 1, input.begin() + (cmdArgsBegin >= 0 ? cmdArgsBegin : cmdArgsEnd));
    shellcmd = Strip(shellcmd);

    cmdArgs = "";
    if (cmdArgsBegin >= 0)
        cmdArgs = string(input.begin() + cmdArgsBegin + 1, input.begin() + cmdArgsEnd);

    string cmdVDR = "varcmd: " + shellcmd;
    cmd = new cCommand;
    if (!cmd->Parse(cmdVDR.c_str())) {
        LogFile.eSysLog("error parsing command: %s", input.c_str());
        delete cmd;
        cmd = NULL;
        return false;
    }
    type = cVarParser::shellcmd;
    return true;
}

bool cVarParser::ParseConnectCmd(const string& input)
{
    int startCon = input.find("(");
    int endCon = input.find(")");
    if (startCon == -1 || endCon == -1) return false;
    string connect(input.begin() + startCon + 1, input.begin() + endCon);
    std::stringstream ss(connect);
    std::string item;
    if (std::getline(ss, item, ','))
        connectAddr = item;
    if (std::getline(ss, item, ','))
        connectPort = atoi(item.c_str());
    if (std::getline(ss, item))
        cmdArgs = item;

    connectAddr = Strip(connectAddr);
    cmdArgs = Strip(cmdArgs);

    if (connectAddr.size() == 0 || connectPort == -1) {
        LogFile.eSysLog("error parsing command: %s", input.c_str());
        return false;
    }
    type = cVarParser::connectcmd;
    return true;
}

bool cVarParser::ParseLengthCmd(const string& input)
{
    int startLen = input.find("(");
    int endLen = input.find(")");
    if (startLen == -1 || endLen == -1) return false;
    string arg(input.begin() + startLen + 1, input.begin() + endLen);
    compExpr = arg;
    type = cVarParser::lengthcmd;
    return true;
}

bool cVarParser::ParseCondExp(const string& input)
{
    int condEndPos = input.find("?");
    string cond(input.begin(), input.begin() + condEndPos);
    int condNeqPos = cond.find("!=");
    int condEqPos = cond.find("==");

    if (condEqPos == -1 && condNeqPos == -1) {
        cond += "!=";
        condNeqPos = cond.find("!=");
    }

    if (condEqPos >= 0 || condNeqPos >= 0) {
        if (!ParseEquality(cond)) {
            LogFile.eSysLog("error parsing '%s'", input.c_str());
            return false;
        }
        condOp = (condEqPos >= 0) ? condEq : condNeq;
    } else {
        LogFile.eSysLog("error parsing '%s'", input.c_str());
        return false;
    }

    string truefalse(input.begin() + condEndPos + 1, input.end());
    int elsePos = truefalse.find(":");
    if (elsePos >= 0) {
        string truePart(truefalse.begin(), truefalse.begin() + elsePos);
        string falsePart(truefalse.begin() + elsePos + 1, truefalse.end());
        if (ParseVar(truePart) && ParseVar(falsePart)) {
            condvarTrue = Strip(truePart);
            condvarFalse = Strip(falsePart);
            type = cVarParser::condition;
            return true;
        }
    }
    LogFile.eSysLog("error parsing '%s'", input.c_str());
    condEqLeft = condEqRight = "";
    return false;
}

bool cVarParser::ParseEquality(const string& input)
{
    int condEqPos = input.find("==");
    int condNeqPos = input.find("!=");
    int condOpPos = -1;
    if (condEqPos >= 0) condOpPos = condEqPos;
    if (condNeqPos >= 0) condOpPos = condNeqPos;
    if (condOpPos == -1) return false;
    string left(input.begin(), input.begin() + condOpPos);
    string right(input.begin() + condOpPos + 2, input.end());
    if (ParseExp(left) && ParseExp(right)) {
        condEqLeft = Strip(left);
        condEqRight = Strip(right);
        return true;
    }
    return false;
}

bool cVarParser::ParseVar(const string& input)
{
    string str = Strip(input);
    if (str.size() > 2 && str[0] == '%' && str[str.size() - 1] == '%')
        return true;
    return false;
}

bool cVarParser::IsCondExpr()
{
    return type == cVarParser::condition;
}

bool cVarParser::IsShellCmd()
{
    return type == cVarParser::shellcmd;
}

bool cVarParser::IsConnectCmd()
{
    return type == cVarParser::connectcmd;
}

bool cVarParser::IsLengthCmd()
{
    return type == cVarParser::lengthcmd;
}
