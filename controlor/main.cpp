#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <math.h>
#include <ecrt.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <sched.h> /* sched_setscheduler() */

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <vector>

#include "ThreadTest.hpp"
#include "Controller.hpp"
#include "base/Time.hpp"
#include "commu/ReceiveData.hpp"
#include "commu/TargetData.hpp"
#include "commu/PositionQueue.hpp"

using namespace std;

static int latency_target_fd;
int32_t latency_target_value = 0;
int run = 1;

void signal_stop(int sig)
{
        run = 0;
}

static void set_latency_target(void)
{
        struct stat s;
        int err;

        err = stat("/dev/cpu_dma_latency", &s);
        if (err == -1) {
                perror("WARN: stat /dev/cpu_dma_latency failed: ");
                return;
        }


        latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
        if (latency_target_fd == -1) {
                perror("WARN: open /dev/cpu_dma_latency: ");
                return;
        }
        err = write(latency_target_fd, &latency_target_value, 4);
        if (err < 1) {
                printf("# error setting cpu_dma_latency to %d!", latency_target_value);
                close(latency_target_fd);
                return;
        }
        printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
}


int main()
{
    signal(SIGINT, signal_stop);
    signal(SIGTERM, signal_stop);

    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1){
        printf("mlockall failed: %m\n");
        exit(-2);
    }

    set_latency_target();

    ReceiveData recvdata;
    TargetData tardata;
    PositionQueue posqueue;
    recvdata.init();
    tardata.init();
    posqueue.init();

    Controller control(500);
    control.start();
    incPos_t pos = {{0,0,0,0,0,0}};
    
    while (run)
    {
//        receiveData_t rdata = recvdata.getData();
        pos.targetPosition[0]++;
//        std::cout << "the main thread, actual position is: " << rdata.actualPosition[0] << endl;
        std::cout << "the main thread, send position is: " << pos.targetPosition[0] << endl;
//        targetData_t tdata = tardata.getData();
//        tdata.targetPosition[0]++;
//        tardata.writeData((tdata));
        while(run){
            if(-1 != posqueue.sendPosition(pos, 1000))
            {
                break;
            }
            usleep(500);
        }
        sleep(1);
    }
    control.quit();
    control.wait();
    
    return 0;



    struct sched_param param;
    pthread_attr_t attr;
    pthread_t thread;
    int ret;
 /*
    ret = pthread_attr_init(&attr);
    if(ret)
    {
        perror("init pthread attributes failed\n");
        exit(EXIT_FAILURE);
    }

    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        printf("pthread setstacksize failed\n");
        goto out;
    }
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
            printf("pthread setschedpolicy failed\n");
            goto out;
    }

    param.sched_priority = 98;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
            printf("pthread setschedparam failed\n");
            goto out;
    }
    
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
                printf("pthread setinheritsched failed\n");
                goto out;
        }
    
        ret = pthread_create(&thread, &attr, ecat_task, NULL);
        if (ret) {
                printf("create pthread failed\n");
                goto out;
        }
*/
        while(run)
        {
                usleep(100000);
        }

out:
        close(latency_target_fd);
        munlockall();

    ret = pthread_join(thread, NULL);
        if (ret)
                printf("join pthread failed: %m\n");
 
    cout << "Hello World!" << endl;
    return 0;
}

