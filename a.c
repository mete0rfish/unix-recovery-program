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
int *cnt_addr;

int mode = 0;
int buf_fd, cnt_fd;

void r_init() {
    int i;

    // data1 파일 매핑 (입력 값)
    buf_fd = open("data1", O_RDWR | O_CREAT, 0600);
    ftruncate(buf_fd, sizeof(DataUnion) * BUFFER_SIZE);
    buf_addr = mmap(NULL, sizeof(DataUnion) * BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0);
    close(buf_fd);

    // data2 파일 매핑 (시작 지점 저장)
    cnt_fd = open("data2", O_RDWR | O_CREAT, 0600);
    ftruncate(cnt_fd, sizeof(int));
    cnt_addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, cnt_fd, 0);
    close(cnt_fd);

    // cnt 초기화 또는 복구
    if (mode == 1) {
        printf("Current saved data:\n");

        int start = (*cnt_addr - BUFFER_SIZE + 1)%BUFFER_SIZE;
        int end = *cnt_addr % BUFFER_SIZE;
        if (start < 0) start = 0;

        printf("\nstart: %d, end: %d\n\n", start, end);

        for (i = start; i != end; i=(i+1)%BUFFER_SIZE) {
            printf("[%d]: %s\n", i, buf_addr[i].str);
        }

        int input;
        printf("Enter recovery point (%d~%d) or -1 to continue: ", start, *cnt_addr);
        scanf("%d", &input);    input--;
        if (input < start) {
            *cnt_addr = start;
        }
        else if(input >= start && input <= *cnt_addr){
            *cnt_addr = input;
        }
    } else {
        *cnt_addr = 0; // cnt 초기화
    }
}

void r_scanf(char *arg) {
    int index = *cnt_addr % BUFFER_SIZE;

    scanf("%s", arg);
    strcpy(buf_addr[index].str, arg);
    
    printf("Saved at: %d\n", index);
    (*cnt_addr)++;
}

void r_printf(char *value) {
    printf("%s", value);
}

void r_cleanup() {
    munmap(buf_addr, sizeof(DataUnion) * BUFFER_SIZE);
    munmap(cnt_addr, sizeof(int));
    unlink("data1");
    unlink("data2");
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
            struct sigaction sa_child = {0};
            sa_child.sa_handler = SIG_DFL;
            sigaction(SIGINT, &sa_child, NULL);

            for (i = *cnt_addr; i < 20; i++) {
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
