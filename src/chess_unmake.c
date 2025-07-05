#include "chess_unmake.h"

UnmakeInfo* push(UnmakeInfo* nextunmake, UnmakeInfo* unmakeStack){
    nextunmake->next = unmakeStack;
    return nextunmake;
}

UnmakeInfo* pop(UnmakeInfo* unmakeStack){
    if(unmakeStack->next == NULL) return NULL;
    UnmakeInfo* discard = unmakeStack;
    unmakeStack = unmakeStack->next;
    free(discard);
    return unmakeStack; 
}