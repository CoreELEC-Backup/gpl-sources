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

#include <algorithm>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#include "uservars.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"
#include "log.h"
#include <sys/socket.h>
#include <netdb.h>

cUserVars UserVars;

string cPlugconfdirVar::dir = "";
string cExtEPGVar::nameSpace = "epg";
string cTimerVar::nameSpace = "timer";
string cSearchVar::nameSpace = "search";

// cUserVar

cUserVar::cUserVar()
{
    oldEvent = NULL;
    oldescapeStrings = false;
}

string cUserVar::Evaluate(const cEvent* e, bool escapeStrings)
{
    if (oldEvent && oldEvent == e && oldescapeStrings == escapeStrings)
        return oldResult;
    usedVars.clear();
    string result;
    if (IsShellCmd())
        result = EvaluateShellCmd(e);
    else if (IsConnectCmd())
        result = EvaluateConnectCmd(e);
    else if (IsLengthCmd())
        result = EvaluateLengthCmd(e);
    else if (IsCondExpr())
        result = EvaluateCondExpr(e);
    else
        result = EvaluateCompExpr(e);
    oldResult = result;
    oldEvent = e;
    oldescapeStrings = escapeStrings;

    // avoid double dir separators
    int pos = 0;
    while ((pos = result.find("~~")) >= 0)
        result.replace(pos, 2, "~");

    return result;
}

string cUserVar::EvaluateShellCmd(const cEvent* e)
{
    if (!varparser.cmd) return "";
    string cmdArgs;
    if (varparser.cmdArgs != "") {
        string args = varparser.cmdArgs;
        varparser.compExpr = args; //handle the args as composed expr
        cmdArgs = EvaluateCompExpr(e, true);
    }
    const char* res = varparser.cmd->Execute(cmdArgs.c_str());
    string result = res ? res : "";
    int crPos = 0; // remove any CR
    while ((crPos = result.find("\n")) >= 0)
        result.replace(crPos, 1, "");

    return result;
}

#define MAX_LINE 1000
string cUserVar::EvaluateConnectCmd(const cEvent* e)
{
    if (varparser.connectAddr == "") return "";

    int       conn_s;                /*  connection socket         */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */

    if ((conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LogFile.eSysLog("Error creating listening socket");
        return "";
    }

    memset(&servaddr, 0, sizeof(varparser.connectAddr.c_str()));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(varparser.connectPort);

    if (getAddrFromString(varparser.connectAddr.c_str(), &servaddr) != 0) {
        LogFile.eSysLog("Invalid remote address");
        return "";
    }

    if (connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        LogFile.eSysLog("Error calling connect()");
        return "";
    }

    varparser.compExpr = varparser.cmdArgs;
    string resexp = EvaluateCompExpr(e, true);
    sprintf(buffer, "%s\n", resexp.c_str());
    Writeline(conn_s, buffer, strlen(buffer));
    Readline(conn_s, buffer, MAX_LINE - 1);

    close(conn_s);
    return buffer;
}

string cUserVar::EvaluateLengthCmd(const cEvent* e)
{
    return NumToString(EvaluateCompExpr(e, false).size());
}

string cUserVar::EvaluateCondExpr(const cEvent* e, bool escapeStrings)
{
    string condresult = "";
    cVarExpr leftExp(varparser.condEqLeft);
    string resLeft = leftExp.Evaluate(e);
    cVarExpr rightExp(varparser.condEqRight);
    string resRight = rightExp.Evaluate(e);
    if (varparser.condOp == condEq && resLeft == resRight)  condresult = "1"; // assign any value
    if (varparser.condOp == condNeq && resLeft != resRight) condresult = "1"; // assign any value

    cUserVar* condVarTrue = UserVars.GetFromName(varparser.condvarTrue);
    cUserVar* condVarFalse = UserVars.GetFromName(varparser.condvarFalse);
    if (!condVarTrue || !condVarFalse) return "";

    if (!AddDepVar(condVarTrue)) return "";
    if (!AddDepVar(condVarFalse)) return "";

    if (condresult != "") {
        LogFile.Log(3, "using case 'true'");
        return condVarTrue->Evaluate(e, escapeStrings);
    } else {
        LogFile.Log(3, "using case 'false'");
        return condVarFalse->Evaluate(e, escapeStrings);
    }
}

string cUserVar::EvaluateCompExpr(const cEvent* e, bool escapeStrings)
{
    string expr = varparser.compExpr;
    if (expr.find('%') == string::npos) return expr;

    // handle internal vars like title and subtitle
    expr = EvaluateInternalVars(expr, e, escapeStrings);

    // replace ext. EPG variables
    expr = EvaluateExtEPGVars(expr, e, escapeStrings);

    // replace other user vars
    expr = EvaluateUserVars(expr, e, escapeStrings);

    return expr;
}

string cUserVar::EvaluateInternalVars(const string& Expr, const cEvent* e, bool escapeStrings)
{
    string expr = Expr;
    if (expr.find('%') == string::npos) return expr;

    std::map<string, cInternalVar*>::iterator it;
    for (it = UserVars.internalVars.begin(); it != UserVars.internalVars.end(); ++it) {
        string varName = (it->second)->Name();
        int varPos = 0;
        while ((varPos = FindIgnoreCase(expr, varName)) >= 0) {
            usedVars.insert(it->second);
            expr.replace(varPos, varName.size(), (it->second)->Evaluate(e, escapeStrings));
        }
    }
    return expr;
}

string cUserVar::EvaluateExtEPGVars(const string& Expr, const cEvent* e, bool escapeStrings)
{
    string expr = Expr;
    if (expr.find('%') == string::npos) return expr;

    std::map<string, cExtEPGVar*>::iterator evar;
    for (evar = UserVars.extEPGVars.begin(); evar != UserVars.extEPGVars.end(); ++evar) {
        // replace ext. EPG variables with leading namespace
        string varName = evar->second->Name(true);
        int varPos = 0;
        while ((varPos = FindIgnoreCase(expr, varName)) >= 0) {
            expr.replace(varPos, varName.size(), evar->second->Evaluate(e, escapeStrings));
            usedVars.insert(evar->second);
        }
        // replace ext. EPG variables without leading namespace
        varName = evar->second->Name();
        varPos = 0;
        while ((varPos = FindIgnoreCase(expr, varName)) >= 0) {
            expr.replace(varPos, varName.size(), evar->second->Evaluate(e, escapeStrings));
            usedVars.insert(evar->second);
        }
    }
    return expr;
}

string cUserVar::EvaluateUserVars(const string& Expr, const cEvent* e, bool escapeStrings)
{
    string expr = Expr;
    if (expr.find('%') == string::npos) return expr;

    std::set<cUserVar*>::iterator it;
    for (it = UserVars.userVars.begin(); it != UserVars.userVars.end(); ++it) {
        string varName = (*it)->Name();
        int varPos = 0;
        while ((varPos = FindIgnoreCase(expr, varName)) >= 0) {
            if (!AddDepVar(*it)) return "";
            expr.replace(varPos, varName.size(), (*it)->Evaluate(e, escapeStrings));
        }
    }
    return expr;
}

string cUserVar::EvaluateInternalTimerVars(const string& Expr, const cTimer* t)
{
    string expr = Expr;
    if (expr.find('%') == string::npos) return expr;

    std::map<string, cTimerVar*>::iterator tvar;
    for (tvar = UserVars.internalTimerVars.begin(); tvar != UserVars.internalTimerVars.end(); ++tvar) {
        string varName = tvar->second->Name();
        int varPos = 0;

        while ((varPos = FindIgnoreCase(expr, varName)) >= 0) {
            expr.replace(varPos, varName.size(), tvar->second->Evaluate(t));
        }
    }
    return expr;
}

string cUserVar::EvaluateInternalSearchVars(const string& Expr, const cSearchExt* s)
{
    string expr = Expr;
    if (expr.find('%') == string::npos) return expr;

    std::map<string, cSearchVar*>::iterator svar;
    for (svar = UserVars.internalSearchVars.begin(); svar != UserVars.internalSearchVars.end(); ++svar) {
        string varName = svar->second->Name();
        int varPos = 0;
        while ((varPos = FindIgnoreCase(expr, varName)) >= 0) {
            expr.replace(varPos, varName.size(), svar->second->Evaluate(s));
        }
    }
    return expr;
}

bool cUserVar::DependsOnVar(const string& varName)
{
    string VarName = Strip(varName);
    cUserVar* var = UserVars.GetFromName(VarName);
    if (!var) return false;
    return DependsOnVar(var);
}

bool cUserVar::DependsOnVar(cUserVar* var)
{
    if (!var) return false;
    if (usedVars.find(var) != usedVars.end()) return true;
    std::set<cUserVar*>::iterator it;
    for (it = usedVars.begin(); it != usedVars.end(); ++it)
        if ((*it)->DependsOnVar(var))
            return true;
    return false;
}

bool cUserVar::AddDepVar(cUserVar* var)
{
    if (var == this || var->DependsOnVar(this)) {
        LogFile.eSysLog("ERROR - found cylic reference to var '%s' in var '%s'", var->Name().c_str(), Name().c_str());
        return false;
    }
    usedVars.insert(var);
    return true;
}

void cUserVar::ResetCache()
{
    oldEvent = NULL;
    oldResult = "";
}

// cUserVarLine
bool cUserVarLine::Parse(char *s)
{
    if (!s) return false;
    if (s[0] == '#')
        return true;
    char *p = strchr(s, '=');
    if (p) {
        cUserVar* userVar = new cUserVar;
        if (userVar->varparser.Parse(s)) {
            cUserVar* oldVar = UserVars.GetFromName(userVar->Name(), false);
            if (oldVar) { // allow redefintion of existing vars
                LogFile.Log(1, "variable '%s' gets overwritten", oldVar->Name().c_str());
                UserVars.userVars.erase(oldVar);
                delete oldVar;
            }
            UserVars.userVars.insert(userVar);
            return true;
        }
    }
    return false;
}

// cUserVars
cUserVar* cUserVars::GetFromName(const string& varName, bool log)
{
    string VarName = Strip(varName);
    std::transform(VarName.begin(), VarName.end(), VarName.begin(), tolower);

    std::map<string, cInternalVar*>::iterator ivar = internalVars.find(VarName);
    if (ivar != internalVars.end())
        return ivar->second;

    std::set<cUserVar*>::iterator uvar;
    for (uvar = userVars.begin(); uvar != userVars.end(); ++uvar)
        if (EqualsNoCase((*uvar)->Name(), VarName))
            return (*uvar);

    std::map<string, cExtEPGVar*>::iterator evar = extEPGVars.find(VarName);
    if (evar != extEPGVars.end())
        return evar->second;

    if (log)
        LogFile.eSysLog("var '%s' not defined!", VarName.c_str());
    return NULL;
}

// cVarExpr
string cVarExpr::Evaluate(const cEvent* e)
{
    // create a dummy user var
    cUserVar var;
    if (!var.varparser.ParseExp(expr))
        return "";

    LogFile.Log(3, "start evaluating expression '%s'", expr.c_str());
    string result = var.Evaluate(e);
    LogFile.Log(3, "stop evaluating expression '%s' - result: '%s'", expr.c_str(), result.c_str());
    usedVars = var.usedVars;
    return result;
}

string cVarExpr::Evaluate(const cTimer* t)
{
    // create a dummy user var
    cUserVar var;
    if (!var.varparser.ParseExp(expr))
        return "";

    usedVars = var.usedVars;
    return var.EvaluateInternalTimerVars(expr, t);
}

string cVarExpr::Evaluate(const cSearchExt* s)
{
    // create a dummy user var
    cUserVar var;
    if (!var.varparser.ParseExp(expr))
        return "";

    usedVars = var.usedVars;
    return var.EvaluateInternalSearchVars(expr, s);
}

bool cVarExpr::DependsOnVar(const string& varName, const cEvent* e)
{
    string VarName = Strip(varName);
    if (FindIgnoreCase(expr, VarName) >= 0)
        return true;
    // create a dummy user var
    cUserVar var;
    var.varparser.varName = expr;
    if (!var.varparser.ParseExp(expr))
        return false;
    var.Evaluate(e);
    return var.DependsOnVar(VarName);
}
