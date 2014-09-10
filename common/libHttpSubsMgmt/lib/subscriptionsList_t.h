
#ifndef __SUBSCRIPTIONS_LIST_T__H_
#define __SUBSCRIPTIONS_LIST_T__H_

typedef struct subscriptionsList_t
{
  void (* free) (struct subscriptionsList_t *);
  subscription_t *(* get) (struct subscriptionsList_t *, unsigned long);
  subscription_t *(* getNth) (struct subscriptionsList_t *, int);
  void (* add) (struct subscriptionsList_t *, subscription_t *subs);
  subscription_t *(* remove) (struct subscriptionsList_t *, unsigned long);
  struct list_head subscriptions;
  
} subscriptionsList_t;

subscriptionsList_t *new_subscriptionsList_t();
void subscriptionsList_t_newFree(subscriptionsList_t *This);

subscription_t *subscriptionsList_t_get(subscriptionsList_t *This, unsigned long id);
subscription_t *subscriptionsList_t_getNth(subscriptionsList_t *This, int index);
void subscriptionsList_t_add(subscriptionsList_t *This, subscription_t *subs);
subscription_t *subscriptionsList_t_remove(subscriptionsList_t *This, unsigned long id);

#endif

