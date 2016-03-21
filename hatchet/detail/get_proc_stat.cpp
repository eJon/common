#include "get_proc_stat.h"
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int get_proc_stat (proc_stat_t* p_stat, pid_t pid)
{
    char stat_filename [_POSIX_PATH_MAX];
    char content [2048];
    char *s, *t;
    FILE *fp;
    struct stat st;

    if (NULL == p_stat) {
        errno = EINVAL;
        return -1;
    }

    sprintf (stat_filename, "/proc/%u/stat", (unsigned) pid);

    if (-1 == access (stat_filename, R_OK)) {
        return (p_stat->pid = -1);
    } /** if **/

    if (-1 != stat (stat_filename, &st)) {
        p_stat->euid = st.st_uid;
        p_stat->egid = st.st_gid;
    } else {
        p_stat->euid = p_stat->egid = (unsigned int)-1;
    }


    if ((fp = fopen (stat_filename, "r")) == NULL) {
        return (p_stat->pid = -1);
    } /** IF_NULL **/

    if ((s = fgets (content, 2048, fp)) == NULL) {
        fclose (fp);
        return (p_stat->pid = -1);
    }


    /** pid **/
    sscanf (content, "%u", &(p_stat->pid));
    s = strchr (content, '(') + 1;
    t = strchr (content, ')');
    strncpy (p_stat->exName, s, t - s);
    p_stat->exName [t - s] = '\0';

    sscanf (t + 2, "%c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
            /*       1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33*/
            &(p_stat->state),
            &(p_stat->ppid),
            &(p_stat->pgrp),
            &(p_stat->session),
            &(p_stat->tty),
            &(p_stat->tpgid),
            &(p_stat->flags),
            &(p_stat->minflt),
            &(p_stat->cminflt),
            &(p_stat->majflt),
            &(p_stat->cmajflt),
            &(p_stat->utime),
            &(p_stat->stime),
            &(p_stat->cutime),
            &(p_stat->cstime),
            &(p_stat->counter),
            &(p_stat->priority),
            &(p_stat->timeout),
            &(p_stat->itrealvalue),
            &(p_stat->starttime),
            &(p_stat->vsize),
            &(p_stat->rss),
            &(p_stat->rlim),
            &(p_stat->startcode),
            &(p_stat->endcode),
            &(p_stat->startstack),
            &(p_stat->kstkesp),
            &(p_stat->kstkeip),
            &(p_stat->signal),
            &(p_stat->blocked),
            &(p_stat->sigignore),
            &(p_stat->sigcatch),
            &(p_stat->wchan));
    fclose (fp);
    return 0;
}

int get_current_proc_stat (proc_stat_t * p_stat)
{ return get_proc_stat (p_stat, getpid()); }
    
