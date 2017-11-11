#ifndef __UMSG_H__
#define __UMSG_H__

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>

#define LETTERNUM 8
#define LETTERSIZE 300  
#define INT_MAX 2147483647 

struct mailbox {
    long type[LETTERNUM];
    char text[LETTERNUM][LETTERSIZE];
}; 

struct letter {
    long type;
    char text[LETTERSIZE];
}; 

struct msgid {
    int semid;
    int shmid;
    struct mailbox* mcont;
}; 

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
}; 

extern struct msgid umsgget(key_t ukey, int umsgflg) {
    struct msgid ret = { -1, -1, NULL};
    union semun rec;
    ret.semid = semget(ukey, 3, umsgflg); 
    
    if (ret.semid == -1) return ret;
    ret.shmid = shmget(ukey, sizeof(struct mailbox), umsgflg); 
    
    if (ret.shmid == -1) { 
        semctl(ret.semid, 0, IPC_RMID, NULL);
        semctl(ret.semid, 1, IPC_RMID, NULL);
        semctl(ret.semid, 2, IPC_RMID, NULL);
        ret.semid = -1;
        return ret;
    }
    
    void *gshm = shmat(ret.shmid, NULL, 0);
    
    if (gshm == (void*) - 1) { 
        semctl(ret.semid, 0, IPC_RMID, NULL);
        semctl(ret.semid, 1, IPC_RMID, NULL);
        semctl(ret.semid, 2, IPC_RMID, NULL);
        shmctl(ret.shmid, IPC_RMID, NULL);
        ret.semid = -1;
        ret.shmid = -1;
        return ret;
    }
    
    rec.val = LETTERNUM;
    semctl(ret.semid, 0, SETVAL, rec); 
    rec.val = 0;
    semctl(ret.semid, 1, SETVAL, rec); 
    rec.val = 1;
    semctl(ret.semid, 2, SETVAL, rec); 
    ret.mcont = (struct mailbox*) gshm;
    
    for (int i = 0; i < LETTERNUM; ++i) 
        ret.mcont->type[i] = -1; 
    
    return ret; 
}

extern int umsgctl(struct msgid mid, int cmd) {
    if (cmd != IPC_RMID) return -1; 

    int flag;
    flag = semctl(mid.semid, 0, cmd, NULL); 
    
    if (flag == -1) return -1;
    flag = shmctl(mid.shmid, cmd, NULL); 
    
    return flag;
}

extern int umsgsnd(struct msgid mid, const struct letter * scont) {
    int semstat, i;
    struct sembuf sp = {0, -1, IPC_NOWAIT}; 
    struct sembuf rv = {1, +1, IPC_NOWAIT}; 
    struct sembuf mp = {2, -1, IPC_NOWAIT}; 
    struct sembuf mv = {2, +1, IPC_NOWAIT}; 

    semstat = semop(mid.semid, &sp, 1); 
    if (semstat == -1)return -1;
    semstat = semop(mid.semid, &mp, 1); 
    if (semstat == -1)return -1;
    
    for (i = 0; i < LETTERNUM; ++i) { 
        if(mid.mcont->type[i] == -1)break;
    }
    
    if (i == LETTERNUM)return -1;
    
    mid.mcont->type[i] = scont->type; 
    strcpy(mid.mcont->text[i], scont->text); 
    semstat = semop(mid.semid, &mv, 1); 
    
    if (semstat == -1)return -1;
    semstat = semop(mid.semid, &rv, 1); 

    return semstat;
}

extern int umsgrcv(struct msgid mid, struct letter * scont) {
    int semstat, i, min = INT_MAX, mini;
    struct sembuf rp = {1, -1, IPC_NOWAIT}; 
    struct sembuf sv = {0, +1, IPC_NOWAIT}; 
    struct sembuf mp = {2, -1, IPC_NOWAIT}; 
    struct sembuf mv = {2, +1, IPC_NOWAIT};

    semstat = semop(mid.semid, &rp, 1); 
    if(semstat == -1) return -1;
    semstat = semop(mid.semid, &mp, 1); 
    if(semstat == -1) return -1;

    for (i = 0, mini = -1; i < LETTERNUM; ++i) {
        if ((mid.mcont->type[i] > 0) && (min > mid.mcont->type[i])) {
            mini = i;
            min = mid.mcont->type[i];
        }
    }
    if (mini == -1) return -1;
    
    scont->type = mid.mcont->type[mini]; 
    strcpy(scont->text, mid.mcont->text[mini]); 
    mid.mcont->type[mini] = -1; 
    semstat = semop(mid.semid, &mv, 1); 

    if (semstat == -1)return -1;
    semstat = semop(mid.semid, &sv, 1); 

    return semstat;
}

#endif