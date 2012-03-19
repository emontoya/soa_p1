#include "scheduler.h"
#include "mheap.h"

#include <stdlib.h>
#include <stdio.h>


static struct mthread *find_ticket_owner(long ticket, struct mthread_list *list){
        // TODO: Add validations
        // Start in the first node
        int index, rc, lc;
        long long lsum, csum;
        
        index = 0; 
        lc = 0;
        csum = 0;

        while(lc < list->count){
                rc = 2 * (index + 1);
                lc = rc - 1;

                lsum = (lc < list->count ? 
                        list->threads[lc]->ticketc + list->threads[lc]->fticket : 0) + csum; 

                // Check for the current node (most probable)
                if (ticket > lsum && ticket <= lsum + list->threads[index]->ticketc){
                        return list->threads[index];
                } else if (ticket <= lsum){// Check for containned in left tree 
                        index = lc; 
                } else { // Containned in right tree
                        csum = lsum + list->threads[index]->ticketc;
                        index = rc; 
                }
        }

        // We should never get here: //TODO: throw "exception"
        return NULL;
}

struct mthread * scheduler_next(struct config_info *config){
        struct mthread_list * list = config->thread_list;
        long ticket;

        if (list->count == 0){
                return NULL;
        }

        // Generate the valid random ticket number
        ticket = (random() % (list->threads[0]->ticketc + list->threads[0]->fticket)) + 1;
        //ticket = 10;
        printf("Ticked %ld generated from 1 - (%ld + %lld = %lld)\n", ticket,
                        list->threads[0]->ticketc,
                        list->threads[0]->fticket,
                        list->threads[0]->ticketc + list->threads[0]->fticket);

        // Return the ticket owner
        return find_ticket_owner(ticket, list);
}
