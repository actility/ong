
#ifndef _SRV_TRANSACTIONS_LIST_T__H_
#define _SRV_TRANSACTIONS_LIST_T__H_

typedef struct srvTransactionsList_t
{
  void (* free) (struct srvTransactionsList_t *);
  srvTransaction_t *(* get) (struct srvTransactionsList_t *, char *);
  srvTransaction_t *(* getNth) (struct srvTransactionsList_t *, int);
  void (* add) (struct srvTransactionsList_t *, srvTransaction_t *trans);
  srvTransaction_t *(* remove) (struct srvTransactionsList_t *, char *);
  struct list_head transactions;
  
} srvTransactionsList_t;

srvTransactionsList_t *new_srvTransactionsList_t();
void srvTransactionsList_t_newFree(srvTransactionsList_t *This);

srvTransaction_t *srvTransactionsList_t_get(srvTransactionsList_t *This, char *transId);
srvTransaction_t *srvTransactionsList_t_getNth(srvTransactionsList_t *This, int index);
void srvTransactionsList_t_add(srvTransactionsList_t *This, srvTransaction_t *trans);
srvTransaction_t *srvTransactionsList_t_remove(srvTransactionsList_t *This, char *transId);

#endif

