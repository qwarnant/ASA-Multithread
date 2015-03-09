#include <stdio.h>
#include <readline/readline.h>
#include <stdlib.h>

#include <math.h>
#include <tools.h>
#include "yield.h"


/* ------------------------------
 command list
 ------------------------------------------------------------*/
struct _cmd {
	char *name;
	void (*fun)(struct _cmd *c);
	char *comment;
};

static void compute(struct _cmd *c);
static void top(struct _cmd *c);
static void new(struct _cmd *c);

static void help(struct _cmd *c);
static void quit(struct _cmd *c);
static void xit(struct _cmd *c);
static void none(struct _cmd *c);

static struct _cmd commands[] =
		{
                { "new", new, "create a new dummy process" },
				{ "compute", compute, "" },
                { "top", top, "display all process" },
                { "quit", quit,
						"save the MBR and quit" }, { "exit", xit,
						"exit (without saving)" }, { "help", help,
						"display this help" }, { 0, none,
						"unknown command, try help" } };

/* ------------------------------
 dialog and execute
 ------------------------------------------------------------*/

static void execute(const char *name) {
	struct _cmd *c = commands;

    printf("%s\n", name);
	while (c->name && strcmp(name, c->name))
		c++;
	(*c->fun)(c);
}

static void loop(void) {
    char *name;
	char temp[64];
    int status;

    //memset(name, '\0', sizeof(name));
    //memset(temp, '\0', sizeof(temp));
    // avant la boucle
	while (1){

	      name = readline ("> ");

	      if (!name)
	        break;

		if (name[0] == '&') {
			strncpy(temp, name+1, strlen(name));
			status = create_ctx(STACK_WIDTH, execute, temp);
			printf("temp %s \n", temp);
            if(status == RETURN_FAILURE) {
                fprintf(stderr, "Failed to create context : %s\n", name);
                return;
            }

			printf("%s %d\n", temp, status);
		} else {
			execute(name);
		}

	}

	printf("finished\n");
}


static void new(struct _cmd *c) {
    create_ctx(STACK_WIDTH, compute, NULL );
}

static void compute(struct _cmd *c) {
	long i;
	long j;
	for (i = 0; i < 10000000; i++) {
		pow(i, j);
		j++;
		j %= 1000;
	}
	printf("job finished\n");
}



static void top(struct _cmd *c) {
    unsigned core;
    char state_name[4];

    for(core = 0; core < CORE_NCORE; core++) {

        if(ctx_ring[core] == NULL) {
        	continue;
        }

        struct ctx_s tmp = *ctx_ring[core];

        printf("PID\tCID\t\tEBP\t\tESP\t\tSTATE\t\tSTART\tUPTIME\n");
        do {
            //get_state_name(tmp.ctx_state, state_name);
            printf("%d\t%d\t%p\t%p\t%s\t\t%d\t%d\n", tmp.ctx_id, tmp.ctx_core_id, tmp.ctx_ebp, tmp.ctx_esp, state_name, 0, 0);

            if(tmp.ctx_next == NULL) {
            	break;
            }

            tmp = *tmp.ctx_next;
        } while (tmp.ctx_id != ctx_ring[core]->ctx_id);
    }
}



static void quit(struct _cmd *c) {
	exit(EXIT_SUCCESS);
}

static void do_xit() {
	exit(EXIT_SUCCESS);
}

static void xit(struct _cmd *dummy) {
	do_xit();
}

static void help(struct _cmd *dummy) {
	struct _cmd *c = commands;

	for (; c->name; c++)
		printf("%s\t-- %s\n", c->name, c->comment);
}

static void none(struct _cmd *c) {
	printf("%s\n", c->comment);
}


int main(int argc, char **argv) {

    init_ctx_tab();
    init_multicore();

	/*
	 * init
	 */
    create_ctx(STACK_WIDTH, loop,NULL );

    create_ctx(STACK_WIDTH, compute,NULL );
    create_ctx(STACK_WIDTH, compute,NULL );
    create_ctx(STACK_WIDTH, compute,NULL );


    //create_ctx(STACK_WIDTH, loop, NULL );
	//yield();
	while(1) {

	}

	printf("No more active ctx\n");

	/* abnormal end of dialog (cause EOF for xample) */
	do_xit();

	/* make gcc -W happy */
	exit(EXIT_SUCCESS);
}
