#ifndef _FREE_TRANSACTIONS_LIST_T__H_
#define _FREE_TRANSACTIONS_LIST_T__H_

typedef struct freeTransactionsList_t
{
  void (*free)(struct freeTransactionsList_t *);
  void (*add)(struct freeTransactionsList_t *, transaction_t *transaction);
  transaction_t *(*recycle)(struct freeTransactionsList_t *);
  int maxSize;
  int size;
  struct httpClient_t *client;
  struct list_head transactions;

} freeTransactionsList_t;

freeTransactionsList_t *new_freeTransactionsList_t(struct httpClient_t *client, int maxSize);
void freeTransactionsList_t_newFree(freeTransactionsList_t *This);

void freeTransactionsList_t_add(freeTransactionsList_t *This, transaction_t *transaction);
transaction_t *freeTransactionsList_t_recycle(freeTransactionsList_t *This);

#endif

