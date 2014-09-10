
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <curl/curl.h>

#include "rtlbase.h"
#include "rtllist.h"

#include "common.h"
#include "song_over_http_api.h"
#include "header_t.h"
#include "response_t.h"
#include "cltTransaction_t.h"
#include "cltTransactionsList_t.h"
#include "freeCltTransactionsList_t.h"
#include "httpClient_t.h"

static void cltTransactionsList_t_init(cltTransactionsList_t *This);

/**
 * Initialize the cltTransactionsList_t data structure members.
 * @param This the object-like instance.
 */
static void cltTransactionsList_t_init(cltTransactionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransactionsList_t::init\n");

  /* Assign the pointers on functions */
  This->get = cltTransactionsList_t_get;
  This->getNth = cltTransactionsList_t_getNth;
  This->add = cltTransactionsList_t_add;
  This->remove = cltTransactionsList_t_remove;

  /* Initialize the members */
  INIT_LIST_HEAD(&(This->transactions));
}

/* dynamic allocation */
/**
 * Constructor-like
 * @return the built transactionsList-t instance.
 */
cltTransactionsList_t *new_cltTransactionsList_t()
{
  cltTransactionsList_t *This = malloc(sizeof(cltTransactionsList_t));
  if (!This)
  {
    return NULL;
  }
  cltTransactionsList_t_init(This);
  This->free = cltTransactionsList_t_newFree;
  RTL_TRDBG(TRACE_DEBUG, "new cltTransactionsList_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * Destructor
 * @param This the object-like instance.
 */
void cltTransactionsList_t_newFree(cltTransactionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "cltTransactionsList_t::newFree (This:0x%.8x)\n", This);
  while ( ! list_empty(&(This->transactions)))
  {
    cltTransaction_t * trans = list_entry(This->transactions.next, cltTransaction_t, chainLink);
    list_del(&trans->chainLink);
    trans->free(trans);
  }
  free(This);
}

/**
 * Get the Nth transaction that matches the provided transaction ID.
 * @param This the object-like instance.
 * @param transId the transaction ID to match.
 * @param the matching cltTransaction_t instance.
 */
cltTransaction_t *cltTransactionsList_t_get(cltTransactionsList_t *This, char *transId)
{
  cltTransaction_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->transactions))
  {
    cltTransaction_t *trans = list_entry(link, cltTransaction_t, chainLink);
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
 * @param the matching cltTransaction_t instance.
 */
cltTransaction_t *cltTransactionsList_t_getNth(cltTransactionsList_t *This, int index)
{
  cltTransaction_t *res = NULL;
  struct list_head *link;
  list_for_each (link, &(This->transactions))
  {
    cltTransaction_t *trans = list_entry(link, cltTransaction_t, chainLink);
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
void cltTransactionsList_t_add(cltTransactionsList_t *This, cltTransaction_t *trans)
{
  list_add_tail(&trans->chainLink, &(This->transactions));
}

/**
 * Remove a transaction (given by its transactionId) from the list of the pending
 * transactions.
 * @param This the object-like instance.
 * @return the transaction, not anymore in the list.
 */
cltTransaction_t *cltTransactionsList_t_remove(cltTransactionsList_t *This, char *transId)
{
  cltTransaction_t *toRemove = This->get(This, transId);
  if (toRemove)
  {
    list_del(&(toRemove->chainLink));
  }
  return toRemove;
}



