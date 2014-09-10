
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "common.h"
#include "song_over_http_api.h"
#include "srvTransaction_t.h"
#include "srvTransactionsList_t.h"

static void srvTransactionsList_t_init(srvTransactionsList_t *This);

/**
 * Initialize the srvTransactionsList_t data structure members.
 * @param This the object-like instance.
 */
static void srvTransactionsList_t_init(srvTransactionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "srvTransactionsList_t::init\n");

  /* Assign the pointers on functions */
  This->get = srvTransactionsList_t_get;
  This->getNth = srvTransactionsList_t_getNth;
  This->add = srvTransactionsList_t_add;
  This->remove = srvTransactionsList_t_remove;

  /* Initialize the members */
  INIT_LIST_HEAD(&(This->transactions));
}

/* dynamic allocation */
/**
 * Constructor-like
 * @return the built transactionsList-t instance.
 */
srvTransactionsList_t *new_srvTransactionsList_t()
{
  srvTransactionsList_t *This = malloc(sizeof(srvTransactionsList_t));
  if (!This)
  {
    return NULL;
  }
  srvTransactionsList_t_init(This);
  This->free = srvTransactionsList_t_newFree;
  RTL_TRDBG(TRACE_DEBUG, "new srvTransactionsList_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * Destructor
 * @param This the object-like instance.
 */
void srvTransactionsList_t_newFree(srvTransactionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "srvTransactionsList_t::newFree (This:0x%.8x)\n", This);
  while ( ! list_empty(&(This->transactions)))
  {
    srvTransaction_t * trans = list_entry(This->transactions.next, srvTransaction_t, chainLink);
    list_del(&trans->chainLink);
    trans->free(trans);
  }
  free(This);
}

/**
 * Get the Nth transaction that matches the provided transaction ID.
 * @param This the object-like instance.
 * @param transId the transaction ID to match.
 * @param the matching srvTransaction_t instance.
 */
srvTransaction_t *srvTransactionsList_t_get(srvTransactionsList_t *This, char *transId)
{
  srvTransaction_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->transactions))
  {
    srvTransaction_t *trans = list_entry(link, srvTransaction_t, chainLink);
    if (!strcmp(transId, trans->transId))
    {
      res = trans;
      break;
    }
  }
  return res;
}

/**
 * Get the Nth transaction in the list.
 * @param This the object-like instance.
 * @param index the position index in the list of the transaction to get.
 * @param the matching srvTransaction_t instance.
 */
srvTransaction_t *srvTransactionsList_t_getNth(srvTransactionsList_t *This, int index)
{
  srvTransaction_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->transactions))
  {
    srvTransaction_t *trans = list_entry(link, srvTransaction_t, chainLink);
    if (0 == index)
    {
      res = trans;
      break;
    }
    index --;
  }
  return res;
}

/**
 * Add new pending transaction.
 * @param This the object-like instance.
 * @param the transaction to add to the pending transactions.
 */
void srvTransactionsList_t_add(srvTransactionsList_t *This, srvTransaction_t *trans)
{
  list_add_tail(&trans->chainLink, &(This->transactions));
}

/**
 * Remove a transaction (given by its transactionId) from the list of the pending
 * transactions.
 * @param This the object-like instance.
 * @return the transaction, not anymore in the list.
 */
srvTransaction_t *srvTransactionsList_t_remove(srvTransactionsList_t *This, char *transId)
{
  srvTransaction_t *toRemove = This->get(This, transId);
  if (toRemove)
  {
    list_del(&(toRemove->chainLink));
  }
  return toRemove;
}



