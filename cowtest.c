#include "types.h"
#include "user.h"

/* ATTENTION: to ensure correct compilation of the base code, 
   stub functions for the system call user space wrapper functions are provided. 
   REMEMBER to disable the stub functions (by commenting the following macro) to 
   allow your implementation to work properly. */
/*#define STUB_FUNCS
#ifdef STUB_FUNCS
int enable_cow(int enable) {return enable;}
#endif*/

int global_array[2048] = {0};
int global_var = 0;

void testcase3(){
    int pid;
    int v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0, v6 = 0, v7 = 0;

    printf(1,"\n----- Test case 3 -----\n");
    v1 = get_free_frame_cnt();
    printf(1,"[prnt] v1 = %d\n", v1);

    if((pid = fork()) == 0)
    {
        //child
        sleep(100);
        v4 = get_free_frame_cnt();
        printf(1,"[chld] v4 = %d\n", v4);
        
        global_array[0] = 222;
        printf(1,"[chld] modified one element in the 1st page, global_array[0]=%d\n", global_array[0]);

        v5 = get_free_frame_cnt();
        printf(1,"[chld] v5 = %d\n", v5);

        global_array[2047] = 333;           
        printf(1,"[chld] modified two elements in the 2nd page, global_array[2047]=%d\n", global_array[2047]);

        v6 = get_free_frame_cnt();
        printf(1,"[chld] v6 = %d\n", v6);

        printf(1, "=====> v5 - v6 = %d\n", v5 - v6);    
            
        exit();
    }
    else 
    {
        //parent
        v2 = get_free_frame_cnt();
        printf(1,"[prnt] v2 = %d\n", v2);

        global_array[0] = 111;     
        printf(1,"[prnt] modified one element in the 1st page, global_array[0]=%d\n", global_array[0]);

        v3 = get_free_frame_cnt();
        printf(1,"[prnt] v3 = %d\n", v3);

    }

    if (wait() != pid)
    {
        printf(1,"wait() error!");
        exit();
    }    

    v7 = get_free_frame_cnt();
    printf(1, "[prnt] v7 = %d\n", v7);
    printf(1, "=====> v1 = v7 ? %s\n", v1 == v7 ? "YES" : "NO");
    printf(1, "=====> v1 - v2 = %d\n", v1 - v2);    
    printf(1, "=====> v2 - v3 = %d\n", v2 - v3);    
}

void testcase2(){
    int pid;
    int v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0, v6 = 0;

    printf(1,"\n----- Test case 2 -----\n");
    v1 = get_free_frame_cnt();
    printf(1,"[prnt] v1 = %d\n", v1);

    if((pid = fork()) == 0)
    {
        //child
        sleep(100);
        v4 = get_free_frame_cnt();
        printf(1,"[chld] v4 = %d\n", v4);

        
        global_var = 100;
        printf(1,"[chld] modified global_var, global_var=%d\n", global_var);
        
        v5 = get_free_frame_cnt();
        printf(1,"[chld] v5 = %d\n", v5);        

        printf(1,"=====> v4 - v5 = %d\n", v4 - v5);
             
        exit();
    }
    else 
    {
        //parent
        v2 = get_free_frame_cnt();
        printf(1,"[prnt] v2 = %d\n", v2);

        
        printf(1,"[prnt] read global_var, global_var=%d\n", global_var);

        v3 = get_free_frame_cnt();
        printf(1,"[prnt] v3 = %d\n", v3);
    }
    
    if (wait() != pid)
    {
        printf(1,"wait() error!");
        exit();
    }    

    v6 = get_free_frame_cnt();
    printf(1, "[prnt] v6 = %d\n", v6);
    printf(1, "=====> v1 = v6 ? %s\n", v1 == v6 ? "YES" : "NO");
    printf(1, "=====> v1 - v2 = %d\n", v1 - v2);    
}


void testcase1(){
    int pid;
    int v1 = 0, v2 = 0, v3 = 0, v4 = 0;        

    printf(1,"\n----- Test case 1 -----\n");
    v1 = get_free_frame_cnt();
    printf(1,"[prnt] v1 = %d\n", v1);
    
    if((pid = fork()) == 0)
    {
        //child
        sleep(100);        
        v3 = get_free_frame_cnt();
        printf(1, "[chld] v3 = %d\n", v3);
        exit();
    }
    else 
    {
        //parent
        v2 = get_free_frame_cnt();
        printf(1, "[prnt] v2 = %d\n", v2);
    }
    
    if (wait() != pid)
    {
        printf(1, "wait() error!");
        exit();
    }

    v4 = get_free_frame_cnt();    
    printf(1, "[prnt] v4 = %d\n", v4);
    printf(1, "=====> v1 = v4 ? %s\n", v1 == v4 ? "YES" : "NO");
    printf(1, "=====> v1 - v2 = %d\n", v1 - v2);
}

int main(){
    printf(1, ">>>>>>>>>>>>>>>>> WITHOUT COW <<<<<<<<<<<<<<<<<<<<<<\n");
    enable_cow(0);
    testcase1();
    testcase2();
    testcase3();

    printf(1, "\n\n>>>>>>>>>>>>>>>>> WITH COW <<<<<<<<<<<<<<<<<<<<<<\n");    enable_cow(1);
    testcase1();
    testcase2();
    testcase3();
    exit();
}
