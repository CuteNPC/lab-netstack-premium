#include "utils/callbacklist.h"
#include <stdlib.h>

void initCallbackList(struct CallbackList *callbackList)
{
    callbackList->head = (struct CallbackNode *)malloc(sizeof(struct CallbackNode));
    callbackList->head->funcPtr = NULL;
    callbackList->head->next = NULL;
    callbackList->tail = callbackList->head;
    return;
}

void insertCallback(struct CallbackList *callbackList, void *funcPtr)
{
    struct CallbackNode *node = (struct CallbackNode *)malloc(sizeof(struct CallbackNode));
    node->funcPtr = funcPtr;
    node->next = NULL;
    callbackList->tail->next = node;
    callbackList->tail = node;
    return;
}