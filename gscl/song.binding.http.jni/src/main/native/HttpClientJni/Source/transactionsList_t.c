#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <curl/curl.h>
#include <jni.h>

#ifndef typeof
#define typeof(T) __typeof__(T)
#endif

#include "rtlbase.h"
#include "rtllist.h"

#include "common.h"
#include "header_t.h"
#include "response_t.h"
#include "transaction_t.h"
#include "transactionsList_t.h"
#include "freeTransactionsList_t.h"
#include "httpClient_t.h"

static void transactionsList_t_init(transactionsList_t *This);

/**
 * Initialize the transactionsList_t data structure members.
 * @param This the object-like instance.
 */
static void transactionsList_t_init(transactionsList_t *This)
{
  LOG(TRACE_DEBUG, "transactionsList_t::init");

  /* Assign the pointers on functions */
  This->get = transactionsList_t_get;
  This->getNth = transactionsList_t_getNth;
  This->add = transactionsList_t_add;
  This->remove = transactionsList_t_remove;

  /* Initialize the members */
  INIT_LIST_HEAD(&(This->transactions));
}

/* dynamic allocation */
/**
 * Constructor-like
 * @return the built transactionsList-t instance.
 */
transactionsList_t *new_transactionsList_t()
{
  transactionsList_t *This = malloc(sizeof(transactionsList_t));
  if (!This)
  {
    return NULL;
  }
  transactionsList_t_init(This);
  This->free = transactionsList_t_newFree;
  LOG(TRACE_DEBUG, "new transactionsList_t (This:0x%.8x)", This);
  return This;
}

/**
 * Destructor
 * @param This the object-like instance.
 */
void transactionsList_t_newFree(transactionsList_t *This)
{
  LOG(TRACE_DEBUG, "transactionsList_t::newFree (This:0x%.8x)", This);
  while (!list_empty(&(This->transactions)))
  {
    transaction_t * trans = list_entry(This->transactions.next, transaction_t, chainLink);
    list_del(&trans->chainLink);
    trans->free(trans);
  }
  free(This);
}

/**
 * Get the Nth transaction that matches the provided transaction ID.
 * @param This the object-like instance.
 * @param transId the transaction ID to match.
 * @param the matching transaction_t instance.
 */
transaction_t *transactionsList_t_get(transactionsList_t *This, char *transId)
{
  transaction_t *res = NULL;
  struct list_head *link;
  list_for_each(link, &(This->transactions))
  {
    transaction_t *trans = list_entry(link, transaction_t, chainLink);
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
 * @param the matching transaction_t instance.
 */
transaction_t *transactionsList_t_getNth(transactionsList_t *This, int index)
{
  transaction_t *res = NULL;
  struct list_head *link;
  list_for_each(link, &(This->transactions))
  {
    transaction_t *trans = list_entry(link, transaction_t, chainLink);
    if (0 == index)
    {
      res = trans;
      break;
    }
    index--;
  }
  return res;
}

/**
 * Add new pending transaction.
 * @param This the object-like instance.
 * @param the transaction to add to the pending transactions.
 */
void transactionsList_t_add(transactionsList_t *This, transaction_t *trans)
{
  list_add_tail(&trans->chainLink, &(This->transactions));
}

/**
 * Remove a transaction (given by its transactionId) from the list of the pending
 * transactions.
 * @param This the object-like instance.
 * @return the transaction, not anymore in the list.
 */
transaction_t *transactionsList_t_remove(transactionsList_t *This, char *transId)
{
  transaction_t *toRemove = This->get(This, transId);
  if (toRemove)
  {
    list_del(&(toRemove->chainLink));
  }
  return toRemove;
}

