#include <brutal/log.h>
#include <syscalls/syscalls.h>

int main()
{
    while (true)
    {
        BrIpcArgs args = {};

        args.flags = BR_IPC_RECV | BR_IPC_BLOCK;
        args.timeout = BR_TIMEOUT_INFINITY;
        br_ipc(&args);

        args.flags = BR_IPC_SEND;
        swap(args.msg.from, args.msg.to);

        br_ipc(&args);
    }
}