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

static void freeTransactionsList_t_init(freeTransactionsList_t *This);

/**
 * Initialize the data structure.
 * @param This the object-like instance.
 */
static void freeTransactionsList_t_init(freeTransactionsList_t *This)
{
  LOG(TRACE_DEBUG, "freeTransactionsList_t::init");

  /* Assign the pointers on functions */
  This->add = freeTransactionsList_t_add;
  This->recycle = freeTransactionsList_t_recycle;

  /* Initialize the members */
  This->maxSize = 0;
  This->size = 0;
  This->client = NULL;
  INIT_LIST_HEAD(&(This->transactions));
}

/* dynamic allocation */
/**
 * Constructor.
 * @param client the HTTP client the list belongs to.
 * @param maxSize the httpClient_t instance the created transaction_t is attached to. 
 * @return the built instance of transaction_t.
 */
freeTransactionsList_t *new_freeTransactionsList_t(httpClient_t *client, int maxSize)
{
  freeTransactionsList_t *This = malloc(sizeof(freeTransactionsList_t));
  if (!This)
  {
    return NULL;
  }
  freeTransactionsList_t_init(This);
  This->free = freeTransactionsList_t_newFree;
  This->client = client;
  This->maxSize = maxSize;
  LOG(TRACE_DEBUG, "new freeTransactionsList_t (This:0x%.8x)", This);
  return This;
}

/**
 * Destructor.
 * @param This the object-like instance.
 */
void freeTransactionsList_t_newFree(freeTransactionsList_t *This)
{
  LOG(TRACE_DEBUG, "freeTransactionsList_t::newFree (This:0x%.8x)", This);
  while (!list_empty(&(This->transactions)))
  {
    transaction_t * t = list_entry(This->transactions.next, transaction_t, chainLink);
    list_del(&t->chainLink);
    t->free(t);
  }
  free(This);
}

/**
 * Add a new free transaction to the list. The transaction is then ready to be re-used.
 * @param This the object-like instance.
 * @param transaction the transaction_t instance to recycle.
 */
void freeTransactionsList_t_add(freeTransactionsList_t *This, transaction_t *transaction)
{
  if (This->size < This->maxSize)
  {
    transaction->reset(transaction);
    list_add_tail(&transaction->chainLink, &(This->transactions));
    This->size++;
  }
  else
  {
    transaction->free(transaction);
  }
}

/**
 * Get a free transaction (and remove it from the list of free transactions).
 * It creates a new transaction if none are available, and resets it if it was 
 * recycled from the list.
 */
transaction_t *freeTransactionsList_t_recycle(freeTransactionsList_t *This)
{
  struct list_head *link = (&(This->transactions))->next;
  transaction_t *toRemove = NULL;
//  if (link != (&(This->transactions))) // list is not empty
  if (This->size > (This->maxSize / 2))
  {
    toRemove = list_entry(link, transaction_t, chainLink);
    list_del(&(toRemove->chainLink));
    //toRemove->reset(toRemove);
    This->size--;
  }
  else
  {
    // allocate a new transaction
    toRemove = new_transaction_t(This->client);
  }
  return toRemove;
}

