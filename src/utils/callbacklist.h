#ifndef _UTILS_CALLBACKLIST_H_
#define _UTILS_CALLBACKLIST_H_

struct CallbackNode
{
    void *funcPtr;
    struct CallbackNode *next;
};

struct CallbackList
{
    struct CallbackNode* head;
    struct CallbackNode* tail;
};

void initCallbackList(struct CallbackList* callbackList);

void insertCallback(struct CallbackList* callbackList, void* funcPtr);

#endif