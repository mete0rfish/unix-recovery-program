#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 10

typedef union {
    char str[10];
} DataUnion;

DataUnion *buf_addr;
int *start_addr;
int *end_addr;

int mode = 0;
int buf_fd, end_fd, start_fd;

void r_init() {
    int i;

    // data1 파일 매핑 (입력 값)
    buf_fd = open("data1", O_RDWR | O_CREAT, 0600);
    ftruncate(buf_fd, sizeof(DataUnion) * BUFFER_SIZE);
    buf_addr = mmap(NULL, sizeof(DataUnion) * BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0);
    close(buf_fd);

    // data2 파일 매핑 (시작 지점 저장)
    end_fd = open("data2", O_RDWR | O_CREAT, 0600);
    ftruncate(end_fd, sizeof(int));
    end_addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, end_fd, 0);
    close(end_fd);

    // data2 파일 매핑 (시작 지점 저장)
    start_fd = open("data3", O_RDWR | O_CREAT, 0600);
    ftruncate(start_fd, sizeof(int));
    start_addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, start_fd, 0);
    close(start_fd);

    // cnt 초기화 또는 복구
    if (mode == 1) {
        printf("Current saved data:\n");

        printf("\nstart: %d, end: %d\n", *start_addr, *end_addr);

        int iter = *start_addr;
        for(i=0;i<10;i++){ 
            if(iter == *end_addr)   break;
            printf("[%d]: %s\n", iter, buf_addr[iter].str);
            iter = (iter+1)%BUFFER_SIZE;
        }
        

        int input;
        printf("Enter recovery point (%d~%d) or -1 to continue: ", *start_addr, *end_addr);
        scanf("%d", &input);    input--;
        if (input < *start_addr) {
            input = *start_addr;
        }
        else if(input > *end_addr){
            input = *end_addr;
        }
        
        *end_addr = input;
    }
}

void r_scanf(char *arg) {
    int index = *end_addr % BUFFER_SIZE;

    scanf("%s", arg);
    strcpy(buf_addr[index].str, arg);
    
    printf("Saved at: %d\n", index);
    if((*end_addr + 1) % BUFFER_SIZE == *start_addr) {
        *start_addr = (*start_addr + 1) % BUFFER_SIZE;
    }
    *end_addr = (*end_addr + 1) % BUFFER_SIZE;
    
}

void r_printf(char *value) {
    printf("%s", value);
}

void r_cleanup() {
    munmap(buf_addr, sizeof(DataUnion) * BUFFER_SIZE);
    munmap(end_addr, sizeof(int));
    munmap(start_addr, sizeof(int));
    unlink("data1");
    unlink("data2");
    unlink("data3");
}

int main() {
    int i;
    char input[10];
    struct sigaction sa = {0};
    sa.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        r_init();

        pid_t pid = fork();
        if (pid == 0) {
            printf("\ncnt: %d\n\n", *end_addr);
            struct sigaction sa_child = {0};
            sa_child.sa_handler = SIG_DFL;
            sigaction(SIGINT, &sa_child, NULL);

            for (i = *end_addr; i < 20; i++) {
                printf("Enter Data: ");
                r_scanf(input);
                r_printf(input);
            }
            exit(0);
        } else {
            int status;
            wait(&status);

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                printf("Child process exited successfully.\n");
                r_cleanup();
                break;
            } else {
                printf("Child process interrupted. Restarting...\n");
                mode = 1;
            }
        }
    }

    return 0;
}
