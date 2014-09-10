
#ifndef _CLT_TRANSACTIONS_LIST_T__H_
#define _CLT_TRANSACTIONS_LIST_T__H_

typedef struct cltTransactionsList_t
{
  void (* free) (struct cltTransactionsList_t *);
  cltTransaction_t *(* get) (struct cltTransactionsList_t *, char *);
  cltTransaction_t *(* getNth) (struct cltTransactionsList_t *, int);
  void (* add) (struct cltTransactionsList_t *, cltTransaction_t *trans);
  cltTransaction_t *(* remove) (struct cltTransactionsList_t *, char *);
  struct list_head transactions;
  
} cltTransactionsList_t;

cltTransactionsList_t *new_cltTransactionsList_t();
void cltTransactionsList_t_newFree(cltTransactionsList_t *This);

cltTransaction_t *cltTransactionsList_t_get(cltTransactionsList_t *This, char *transId);
cltTransaction_t *cltTransactionsList_t_getNth(cltTransactionsList_t *This, int index);
void cltTransactionsList_t_add(cltTransactionsList_t *This, cltTransaction_t *trans);
cltTransaction_t *cltTransactionsList_t_remove(cltTransactionsList_t *This, char *transId);

#endif

