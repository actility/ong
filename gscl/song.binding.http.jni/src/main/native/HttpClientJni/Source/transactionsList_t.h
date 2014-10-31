#ifndef _TRANSACTIONS_LIST_T__H_
#define _TRANSACTIONS_LIST_T__H_

typedef struct transactionsList_t
{
  void (*free)(struct transactionsList_t *);
  transaction_t *(*get)(struct transactionsList_t *, char *);
  transaction_t *(*getNth)(struct transactionsList_t *, int);
  void (*add)(struct transactionsList_t *, transaction_t *trans);
  transaction_t *(*remove)(struct transactionsList_t *, char *);
  struct list_head transactions;

} transactionsList_t;

transactionsList_t *new_transactionsList_t();
void transactionsList_t_newFree(transactionsList_t *This);

transaction_t *transactionsList_t_get(transactionsList_t *This, char *transId);
transaction_t *transactionsList_t_getNth(transactionsList_t *This, int index);
void transactionsList_t_add(transactionsList_t *This, transaction_t *trans);
transaction_t *transactionsList_t_remove(transactionsList_t *This, char *transId);

#endif

