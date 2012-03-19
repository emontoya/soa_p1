#include "mheap.h"

#include <stdio.h>

static void increase_key(struct mthread_list *list, struct mthread *thread){
        int index = list->thread_indexs[thread->id];
        int parent, tindex, lc, rc;
        struct mthread *tthread;

        while(index > 0){
                // Get the parent index (div by 2)
                parent = index % 2 == 0 ? index / 2 - 1 : (index + 1) / 2 - 1;
                
                /*
                 * Swap the node and its parent if they are not in the proper
                 * order
                 */
                if (list->threads[index]->ticketc > list->threads[parent]->ticketc){
                        // Swap the indexes
                        tindex = list->thread_indexs[thread->id];
                        list->thread_indexs[thread->id] = list->thread_indexs[list->threads[parent]->id];
                        list->thread_indexs[list->threads[parent]->id] = tindex; 

                        // Swap the node and it's parent 
                        tthread = list->threads[index];
                        list->threads[index] = list->threads[parent];
                        list->threads[parent]= tthread;

                        // Set the new sum to the former parent
                        list->threads[index]->fticket = list->threads[parent]->fticket;
                }
                
                rc = 2 * (parent + 1);
                lc = rc - 1;

                //count Set the value of the left child
                list->threads[parent]->fticket = list->threads[lc]->fticket + list->threads[lc]->ticketc;

                // Add the value of the right child if exist
                if (rc < list->count){
                        list->threads[parent]->fticket += list->threads[rc]->fticket + list->threads[rc]->ticketc;
                }

                index = parent;
        }
}

static void update_branch_by(struct mthread_list *list, int index, long value){
        int parent;

        while(index > 0){
                parent = index % 2 == 0 ? index / 2 - 1 : (index + 1) / 2 - 1;
                
                list->threads[parent]->fticket += value;
                index = parent;
        }
}

static void push_down(struct mthread_list *list, int index){
        // TODO: Optimize this function
        int rc;
        int lc;
        int max = 1, ti;
        long long t;
        struct mthread *tthread;

        while(max != 0){
                rc = 2 * (index + 1);
                lc = rc - 1;
                max = 0;

                if (lc < list->count){
                        max = lc;
                        if (rc < list->count && 
                                        list->threads[rc]->ticketc > list->threads[lc]->ticketc){
                                max = rc;
                        }
                }

                if (max != 0 && list->threads[max]->ticketc > list->threads[index]->ticketc){
                        // Update tickets
                        t = list->threads[index]->fticket;
                        list->threads[index]->fticket = list->threads[max]->fticket; 
                        list->threads[max]->fticket = t - (list->threads[max]->ticketc -
                                        list->threads[index]->ticketc);

                        // Update indexs
                        ti = list->thread_indexs[list->threads[index]->id];
                        list->thread_indexs[list->threads[index]->id] = 
                                list->thread_indexs[list->threads[max]->id];
                        list->thread_indexs[list->threads[max]->id] = ti;

                        // Swap the positions
                        tthread = list->threads[index];
                        list->threads[index] = list->threads[max];
                        list->threads[max] = tthread; 
                }

                index = max;
        }
}

struct mthread * mheap_add(struct mthread_list * list, int tc, int wc){
        // TODO: Add validations
        // Adding the node at the bottom of the tree
        struct mthread *nthread = mthread_new(list->count, tc, wc);
        list->thread_indexs[list->count] = list->count;
        list->threads[list->count] = nthread;
        list->count++;

        // Initialize the partial sum to the tickets own by this threads
        nthread->fticket = 0;

        // Set the correct position for the thread
        increase_key(list, nthread);

        // Return the new thread
        return nthread;
}

struct mthread * mheap_remove(struct mthread_list * list, int thread_id){
        // TODO: Optimize this function
        int pos = list->thread_indexs[thread_id];
        struct mthread *thread = list->threads[pos];

        printf("Removing thread %d; pos = %d\n", thread_id, pos);

        // Get the last element
        int last  = list->count - 1;

        // Decrease the parents' partial sum 
        update_branch_by(list, last, -list->threads[last]->ticketc);

        // Substitute the element to delete with  last one
        if (last != pos){ // If we are not removing the last element 
                printf("Promoting thread %d\n", list->threads[last]->id);
                // Update the partial sum for the thread to be promoted
                list->threads[last]->fticket = list->threads[pos]->fticket;
                // Update the parents for the promoted thread
                update_branch_by(list, pos, 
                        -(list->threads[pos]->ticketc -
                        list->threads[last]->ticketc));
                
                list->thread_indexs[list->threads[last]->id] = pos;
                list->threads[pos] = list->threads[last];
        }

        list->count--;

        push_down(list, pos);

        return thread;
}
