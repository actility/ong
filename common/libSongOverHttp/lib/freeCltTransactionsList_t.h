
#ifndef _FREE_CLT_TRANSACTIONS_LIST_T__H_
#define _FREE_CLT_TRANSACTIONS_LIST_T__H_

typedef struct freeCltTransactionsList_t
{
  void (* free) (struct freeCltTransactionsList_t *);
  void (* add) (struct freeCltTransactionsList_t *, cltTransaction_t *transaction);
  cltTransaction_t *(* recycle) (struct freeCltTransactionsList_t *);
  int maxSize;
  int size;
  struct httpClient_t *client;
  struct list_head transactions;
  
} freeCltTransactionsList_t;

freeCltTransactionsList_t *new_freeCltTransactionsList_t(struct httpClient_t *client, int maxSize);
void freeCltTransactionsList_t_newFree(freeCltTransactionsList_t *This);

void freeCltTransactionsList_t_add(freeCltTransactionsList_t *This, cltTransaction_t *transaction);
cltTransaction_t *freeCltTransactionsList_t_recycle(freeCltTransactionsList_t *This);

#endif

