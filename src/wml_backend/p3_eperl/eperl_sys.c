/*
**  ePerl -- Embedded Perl 5 Language
**
**  ePerl interprets an ASCII file bristled with Perl 5 program statements
**  by evaluating the Perl 5 code while passing through the plain ASCII
**  data. It can operate both as a standard Unix filter for general file
**  generation tasks and as a powerful Webserver scripting language for
**  dynamic HTML page programming.
**
**  ======================================================================
**
**  Copyright (c) 1996,1997,1998,1999 Ralf S. Engelschall <rse@engelschall.com>
**
**  This program is free software; it may be redistributed and/or modified
**  only under the terms of either the Artistic License or the GNU General
**  Public License, which may be found in the ePerl source distribution.
**  Look at the files ARTISTIC and COPYING or run ``eperl -l'' to receive
**  a built-in copy of both license files.
**
**  This program is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See either the
**  Artistic License or the GNU General Public License for more details.
**
**  ======================================================================
**
**  eperl_sys.c -- ePerl system functions
*/

#include "eperl_config.h"
#include "eperl_global.h"
#include "eperl_proto.h"

/*
**  own setenv() function which works with Perl
*/

extern char **environ;

char **mysetenv(char **env, char *var, char *str, ...)
{
    va_list ap;
    char ca[1024];
    char ca2[1500];
    char *cp;
    int i;
    char **envN;
    static bool stillcalled = false;
    bool replaced = false;

    /*  create the key=val string  */
    va_start(ap, str);
    vsnprintf(ca, sizeof(ca), str, ap);
    ca[sizeof(ca)-1] = NUL;
    snprintf(ca2, sizeof(ca2), "%s=%s", var, ca);
    ca2[sizeof(ca2)-1] = NUL;
    cp = strdup(ca2);

    /*  now duplicate the old structure  */
    for (i = 0; env[i] != NULL; i++)
        ;
    envN = (char **)malloc(sizeof(char *) * (i+2));
    for (i = 0; env[i] != NULL; i++) {
        if (strncmp(env[i], var, strlen(var)) == 0) {
            envN[i] = cp;
            replaced = true;
        }
        else
            envN[i] = env[i];
    }

    /*  add the new entry if not replaced */
    if (!replaced) {
        envN[i++] = cp;
    }
    envN[i] = NULL;

    /*  set the libc/exec variable which Perl uses */
    if (stillcalled)
        free(environ);
    stillcalled = true;
    environ = envN;

    va_end(ap);
    return envN;
}


/*
**
**  I/O handle redirection
**
*/

#define HANDLE_STDIN  0
#define HANDLE_STDOUT 1
#define HANDLE_STDERR 2
#define HANDLE_STORE_STDIN  10
#define HANDLE_STORE_STDOUT 11
#define HANDLE_STORE_STDERR 12

static bool IO_redirected_stdin  = false;
static bool IO_redirected_stdout = false;
static bool IO_redirected_stderr = false;

void IO_redirect_stdin(FILE *fp)
{
    if (IO_redirected_stdin)
        IO_restore_stdin();

    /* first copy the current stdin to the store handle */
    (void)dup2(HANDLE_STDIN, HANDLE_STORE_STDIN);

    /* then copy the new handle to stdin */
    (void)dup2(fileno(fp), HANDLE_STDIN);

    /* and remember the fact */
    IO_redirected_stdin = true;
}

void IO_redirect_stdout(FILE *fp)
{
    if (IO_redirected_stdout)
        IO_restore_stdout();

    /* first copy the current stdout to the store handle */
    (void)dup2(HANDLE_STDOUT, HANDLE_STORE_STDOUT);

    /* then copy the new handle to stdout */
    (void)dup2(fileno(fp), HANDLE_STDOUT);

    /* and remember the fact */
    IO_redirected_stdout = true;
}

void IO_redirect_stderr(FILE *fp)
{
    if (IO_redirected_stderr)
        IO_restore_stderr();

    /* first copy the current stderr to the store handle */
    (void)dup2(HANDLE_STDERR, HANDLE_STORE_STDERR);

    /* then copy the new handle to stderr */
    (void)dup2(fileno(fp), HANDLE_STDERR);

    /* and remember the fact */
    IO_redirected_stderr = true;
}

bool IO_is_stdin_redirected(void)
{
    return IO_redirected_stdin;
}

bool IO_is_stdout_redirected(void)
{
    return IO_redirected_stdout;
}

bool IO_is_stderr_redirected(void)
{
    return IO_redirected_stderr;
}

void IO_restore_stdin(void)
{
    if (IO_redirected_stdin) {
        dup2(HANDLE_STORE_STDIN, HANDLE_STDIN);
        IO_redirected_stdin = false;
    }
}

void IO_restore_stdout(void)
{
    if (IO_redirected_stdout) {
        dup2(HANDLE_STORE_STDOUT, HANDLE_STDOUT);
        IO_redirected_stdout = false;
    }
}

void IO_restore_stderr(void)
{
    if (IO_redirected_stderr) {
        dup2(HANDLE_STORE_STDERR, HANDLE_STDERR);
        IO_redirected_stderr = false;
    }
}


/*
**
**  Temporary filename support
**
*/

static char *mytmpfiles[100] = { NULL };

char *mytmpfile(const char *id)
{
    char ca[1024];
    char *cp;
    char tmpfile[]="eperl_sourceXXXXXX";
    int i;
    int fd=-1;

    const char *tmpdir = getenv ("TMPDIR");
    if (! tmpdir)
        tmpdir="/tmp";

    snprintf(ca, sizeof(ca), "%s/%s", tmpdir, tmpfile);
    if((fd = mkstemp(ca)) == -1){
        perror("can not create tmpfile");
        return NULL;
    }
    close(fd);
    ca[sizeof(ca)-1] = NUL;
    cp = strdup(ca);
    for (i = 0; mytmpfiles[i] != NULL; i++)
        ;
    mytmpfiles[i++] = cp;
    mytmpfiles[i] = NULL;
    return cp;
}

void remove_mytmpfiles(void)
{
    int i;

    for (i = 0; mytmpfiles[i] != NULL; i++) {
        unlink(mytmpfiles[i]);
    }
}


/*
**
**  ISO time
**
*/

char *isotime(time_t *t)
{
    struct tm *tm;
    char timestr[128];

    tm = localtime(t);
    sprintf(timestr, "%02d-%02d-%04d %02d:%02d",
                      tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900,
                      tm->tm_hour, tm->tm_min);
    return strdup(timestr);
}


/*
**
**  read source file into internal buffer
**
*/
char *ePerl_ReadSourceFile(char *filename, char **cpBufC, int *nBufC)
{
    char *rc;
    FILE *fp = NULL;
    char *cpBuf = NULL;
    int nBuf;
    char tmpfile[256], *ptr_tmpfile;
    int usetmp = 0;
    int c;

    if (stringEQ(filename, "-")) {
        /* file is given on stdin */
        ptr_tmpfile = mytmpfile("ePerl.source");
        sprintf(tmpfile, "%s", ptr_tmpfile);
        if ((fp = fopen(tmpfile, "w")) == NULL) {
            ePerl_SetError("Cannot open temporary source file %s for writing", tmpfile);
            CU(NULL);
        }
        nBuf = 0;
        while ((c = fgetc(stdin)) != EOF) {
            fprintf(fp, "%c", c);
        }
        fclose(fp);
        fp = NULL;
        filename = tmpfile;
        usetmp = 1;
    }

    if ((fp = fopen(filename, "r")) == NULL) {
        ePerl_SetError("Cannot open source file %s for reading", filename);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_END);
    nBuf = ftell(fp);
    if (nBuf == 0) {
        cpBuf = (char *)malloc(sizeof(char) * 1);
        *cpBuf = NUL;
    }
    else {
        if ((cpBuf = (char *)malloc(sizeof(char) * nBuf+1)) == NULL) {
            ePerl_SetError("Cannot allocate %d bytes of memory", nBuf);
            CU(NULL);
        }
        fseek(fp, 0, SEEK_SET);
        if (fread(cpBuf, nBuf, 1, fp) == 0) {
            ePerl_SetError("Cannot read from file %s", filename);
            CU(NULL);
        }
        cpBuf[nBuf] = '\0';
    }
    *cpBufC = cpBuf;
    *nBufC  = nBuf;
    RETURN_WVAL(cpBuf);

    CUS:
    if (cpBuf)
        free(cpBuf);
    if (fp)
        fclose(fp);
    if (usetmp)
        unlink(tmpfile);
    RETURN_EXRC;
}

/*
**
**  read an error file to internal buffer and substitute the filename
**
*/
char *ePerl_ReadErrorFile(char *filename, char *scriptfile, char *scripturl)
{
    char *rc;
    FILE *fp = NULL;
    char *cpBuf = NULL;
    int nBuf;
    char *cp;

    if ((fp = fopen(filename, "r")) == NULL) {
        ePerl_SetError("Cannot open error file %s for reading", filename);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_END);
    nBuf = ftell(fp);
    if ((cpBuf = (char *)malloc(sizeof(char) * nBuf * 2)) == NULL) {
        ePerl_SetError("Cannot allocate %d bytes of memory", nBuf * 2);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_SET);
    if (fread(cpBuf, nBuf, 1, fp) == 0) {
        ePerl_SetError("Cannot read from file %s", filename);
        CU(NULL);
    }
    cpBuf[nBuf] = '\0';
    for (cp = cpBuf; cp < cpBuf+nBuf; ) {
        if ((cp = strstr(cp, scriptfile)) != NULL) {
            (void)memmove(cp+strlen(scripturl), cp+strlen(scriptfile), strlen(cp+strlen(scriptfile))+1);
            (void)memcpy(cp, scripturl, strlen(scripturl));
            cp += strlen(scripturl);
            continue;
        }
        break;
    }
    RETURN_WVAL(cpBuf);

    CUS:
    if (cpBuf)
        free(cpBuf);
    if (fp)
        fclose(fp);
    RETURN_EXRC;
}

/*
**
**  path support
**
*/

char *filename(char *path)
{
    static char file[MAXPATHLEN];
    char *cp;

    if (path[strlen(path)-1] == '/')
        return "";
    for (cp = path+strlen(path); cp > path && *(cp-1) != '/'; cp--)
        ;
    if (cp == path+1)
        cp--;
    strncpy(file, cp, sizeof(file));
    file[sizeof(file)-1] = '\0';
    return file;
}

char *dirname(char *path)
{
    static char dir[MAXPATHLEN];
    char *cp;

    if (path[strlen(path)-1] == '/')
        return path;
    strncpy(dir, path, sizeof(dir));
    dir[sizeof(dir)-1] = '\0';
    for (cp = dir+strlen(dir); cp > dir && *(cp-1) != '/'; cp--)
        ;
    *cp = NUL;
    return dir;
}

char *abspath(char *path)
{
    static char apath[MAXPATHLEN];
    static char cwd[MAXPATHLEN];
    char *cp;

    if (path[0] == '/')
        return path;
    /* remember current working dir */
    if (! getcwd(cwd, MAXPATHLEN)) {
        return NULL;
    }
    /* determine dir of path */
    cp = dirname(path);
    if (chdir(cp) != 0) {
        return NULL;
    }
    if (! getcwd(apath, MAXPATHLEN)) {
        return NULL;
    }
    /* restore cwd */
    if (chdir(cwd) != 0) {
        return NULL;
    }
    /* add file part again */
    if (apath[strlen(apath)-1] != '/') {
        strncpy(apath+strlen(apath), "/", sizeof(apath)-strlen(apath));
        apath[sizeof(apath)-1] = '\0';
    }
    strncpy(apath+strlen(apath), path, sizeof(apath)-strlen(apath));
    apath[sizeof(apath)-1] = '\0';
    return apath;
}
