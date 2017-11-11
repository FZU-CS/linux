#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>

#include "umsg.h"

#define _XOPEN_SOURCE 1

int main(int argc, char *argv[]) {
    struct msgid msg_id1, msg_id2;
    int key1, key2, flag;
    char inpt[5];
    struct letter send, recv;

    printf("LOCAL  KEY:");
    scanf("%d", &key1);
    msg_id1 = umsgget(key1, IPC_CREAT | 0600);
    
    printf("REMOTE KEY:");
    scanf("%d", &key2);
    msg_id2 = umsgget(key2, IPC_CREAT | 0600);
    
    printf("MSGID1A:%d,MSGID2A:%d\n", msg_id1.semid, msg_id2.semid);
    printf("MSGID1B:%d,MSGID2B:%d\n", msg_id1.shmid, msg_id2.shmid);
    printf("OPERATE (H:HELP E:EXIT)\n");
    
    for(int loop = 1; inpt[0] != 'E'; ++loop) {
        printf("INPUT :");
        scanf("%s", inpt);
        
        switch(inpt[0]) {
        case 'S': case 's':
            send.type = loop;
            scanf("%s", send.text);
            flag = umsgsnd(msg_id2, &send);
            if (flag != -1) printf("SEND#%05ld %-32s [  O  K  ]\n", send.type, send.text);
            else printf("SEND#%05ld %-32s [ FAILED ]\n", send.type, send.text);
            break;
        case 'R': case 'r':
            flag = umsgrcv(msg_id1, &recv);
            if(flag != -1)printf("RECV#%05ld %-32s [  O  K  ]\n", recv.type, recv.text);
            else          printf("RECV#ERROR %-32s [ FAILED ]\n", " ");
            break;
        case 'E': case 'e':
            inpt[0] = 'E';
            break;
        default:
            printf("S:Send R:Receive E:Exit\n");
        }
    }
    umsgctl(msg_id1, IPC_RMID);
    umsgctl(msg_id2, IPC_RMID);
    return EXIT_SUCCESS;
}
