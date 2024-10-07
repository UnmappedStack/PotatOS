#include <stdio.h>
#include <event.h>

void wait_for_event(uint64_t event_type) {
    while (true) {
        static Event this_event;
        poll(&this_event);
        if (this_event.do_handle && this_event.event_id == event_type) return;
    }
}

const char *argvals[] = {"R:/exec/shell", "Arg test 1!", "Second :D"};

int main(int argc, char **argv) {
    fputs("This message was printed from a userspace application :)\n", stdout);
    for (;;);
    fputs("PotatOS PowerlessShell, compiled on " __DATE__ "\n"
          "This is free & open source software under the Mozilla Public License 2.0. See LICENSE in the GitHub repository for more information.\n\n", stdout);
    static char cwd_buffer[100];
    static char input_buffer[200];
    while (true) {
        get_cwd(cwd_buffer, 99);
        printf("SH %s> ", cwd_buffer);
        fgets(input_buffer, 200, stdin);
        printf("\n");
        int status = spawn(input_buffer, argvals, 3);
        if (status != 0) {
            printf("Couldn't execute program \"%s\". The program may not exist (status code: %i)\n", input_buffer, status);
            continue;
        }
        wait_for_event(EVENT_TASK_EXITED);
    }
    return 0;
}