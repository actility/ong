
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "song_over_http_api.h"
#include "http_subs_api.h"
#include "common.h"
#include "subscription_t.h"
#include "subscriptionsList_t.h"


static void subscriptionsList_t_init(subscriptionsList_t *This);

/**
 * Initialize the subscriptionsList_t data structure members.
 * @param This the object-like instance.
 */
static void subscriptionsList_t_init(subscriptionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "subscriptionsList_t::init\n");

  /* Assign the pointers on functions */
  This->get = subscriptionsList_t_get;
  This->getNth = subscriptionsList_t_getNth;
  This->add = subscriptionsList_t_add;
  This->remove = subscriptionsList_t_remove;

  /* Initialize the members */
  INIT_LIST_HEAD(&(This->subscriptions));
}

/* dynamic allocation */
/**
 * Constructor-like
 * @return the built subscriptionsList-t instance.
 */
subscriptionsList_t *new_subscriptionsList_t()
{
  subscriptionsList_t *This = malloc(sizeof(subscriptionsList_t));
  if (!This)
  {
    return NULL;
  }
  subscriptionsList_t_init(This);
  This->free = subscriptionsList_t_newFree;
  RTL_TRDBG(TRACE_DEBUG, "new subscriptionsList_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * Destructor
 * @param This the object-like instance.
 */
void subscriptionsList_t_newFree(subscriptionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "subscriptionsList_t::newFree (This:0x%.8x)\n", This);
  while ( ! list_empty(&(This->subscriptions)))
  {
    subscription_t * subs = list_entry(This->subscriptions.next, subscription_t, chainLink);
    list_del(&subs->chainLink);
    subs->free(subs);
  }
  free(This);
}

/**
 * Get the Nth subscription that matches the provided subscription ID.
 * @param This the object-like instance.
 * @param id the subscription ID to match.
 * @param the matching subscription_t instance.
 */
subscription_t *subscriptionsList_t_get(subscriptionsList_t *This, unsigned long id)
{
  subscription_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->subscriptions))
  {
    subscription_t *subs = list_entry(link, subscription_t, chainLink);
    if (id == subs->id)
    {
      res = subs;
      break;
    }
  }
  return res;
}

/**
 * Get the Nth subscription in the list.
 * @param This the object-like instance.
 * @param index the position index in the list of the subscription to get.
 * @param the matching subscription_t instance.
 */
subscription_t *subscriptionsList_t_getNth(subscriptionsList_t *This, int index)
{
  subscription_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->subscriptions))
  {
    subscription_t *subs = list_entry(link, subscription_t, chainLink);
    if (0 == index)
    {
      res = subs;
      break;
    }
    index --;
  }
  return res;
}

/**
 * Add new pending subscription.
 * @param This the object-like instance.
 * @param the subscription to add to the pending subscriptions.
 */
void subscriptionsList_t_add(subscriptionsList_t *This, subscription_t *subs)
{
  list_add_tail(&subs->chainLink, &(This->subscriptions));
}

/**
 * Remove a subscription (given by its subscriptionId) from the list of the pending
 * subscriptions.
 * @param This the object-like instance.
 * @return the subscription, not anymore in the list.
 */
subscription_t *subscriptionsList_t_remove(subscriptionsList_t *This, unsigned long id)
{
  subscription_t *toRemove = This->get(This, id);
  if (toRemove)
  {
    list_del(&(toRemove->chainLink));
  }
  return toRemove;
}



