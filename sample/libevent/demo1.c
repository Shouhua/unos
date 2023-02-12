#include <stdio.h>
#include <unistd.h>

#include <event2/event.h>
#include <event2/util.h>

int main(int argc, char *argv[])
{
    printf("Libevent version is %s\n", event_get_version());

    printf("Libevent I/O multiplexing are: \n") ;
    const char **method = event_get_supported_methods();
    int i;
    for(i = 0; method[i] != NULL; i++)
    {
        printf("\t%s\t", method[i]);
    }
    printf("\n");

    struct event_base *base = NULL;
    struct event_config *cfg = NULL;

    cfg = event_config_new();
    // event_config_avoid_method(cfg, "epoll");
    base = event_base_new_with_config(cfg);
    if(!base)
    {
        event_config_free(cfg);
        return -1;
    }

    printf("default is %s\n", event_base_get_method(base));

    event_base_free(base);
    event_config_free(cfg);

    return 0;
}