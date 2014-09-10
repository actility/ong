
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

static void freeCltTransactionsList_t_init(freeCltTransactionsList_t *This);

/**
 * Initialize the data structure.
 * @param This the object-like instance.
 */
static void freeCltTransactionsList_t_init(freeCltTransactionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "freeCltTransactionsList_t::init\n");

  /* Assign the pointers on functions */
  This->add = freeCltTransactionsList_t_add;
  This->recycle = freeCltTransactionsList_t_recycle;

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
 * @param maxSize the httpClient_t instance the created cltTransaction_t is attached to. 
 * @return the built instance of cltTransaction_t.
 */
freeCltTransactionsList_t *new_freeCltTransactionsList_t(httpClient_t *client, int maxSize)
{
  freeCltTransactionsList_t *This = malloc(sizeof(freeCltTransactionsList_t));
  if (!This)
  {
    return NULL;
  }
  freeCltTransactionsList_t_init(This);
  This->free = freeCltTransactionsList_t_newFree;
  This->client = client;
  This->maxSize = maxSize;
  RTL_TRDBG(TRACE_DEBUG, "new freeCltTransactionsList_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * Destructor.
 * @param This the object-like instance.
 */
void freeCltTransactionsList_t_newFree(freeCltTransactionsList_t *This)
{
  RTL_TRDBG(TRACE_DEBUG, "freeCltTransactionsList_t::newFree (This:0x%.8x)\n", This);
  while ( ! list_empty(&(This->transactions)))
  {
    cltTransaction_t * t = list_entry(This->transactions.next, cltTransaction_t, chainLink);
    list_del(&t->chainLink);
    t->free(t);
  }
  free(This);
}

/**
 * Add a new free transaction to the list. The transaction is then ready to be re-used.
 * @param This the object-like instance.
 * @param transaction the cltTransaction_t instance to recycle.
 */
void freeCltTransactionsList_t_add(freeCltTransactionsList_t *This, cltTransaction_t *transaction)
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
cltTransaction_t *freeCltTransactionsList_t_recycle(freeCltTransactionsList_t *This)
{
  struct list_head *link = (&(This->transactions))->next;
  cltTransaction_t *toRemove = NULL;
//  if (link != (&(This->transactions))) // list is not empty
  if (This->size > (This->maxSize/2))
  {
    toRemove = list_entry(link, cltTransaction_t, chainLink);
    list_del(&(toRemove->chainLink));
    //toRemove->reset(toRemove);
    This->size--;
  }
  else
  {
    // allocate a new transaction
    toRemove = new_cltTransaction_t(This->client);
  }
  return toRemove;
}


