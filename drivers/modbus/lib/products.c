#include "modbus-acy.h"

LIST_HEAD(ProductList);
unsigned int productID = 0;

void **
ProductsGetDescription(char *ref)
{
  struct list_head *index;
  Product_t *product;
  
  list_for_each(index, &ProductList) {
    product = list_entry(index, Product_t, list);

    if (!strcmp(ref, product->ref)) {
      return &(product->xo);
    }
  }
  
  // Not found
  product = (Product_t *) malloc(sizeof(Product_t));
  if (product == NULL) {
    RTL_TRDBG(TRACE_ERROR, "malloc failure\n");
    return NULL;
  }
  
  productID++;
  product->id = productID;
  product->ref = strdup(ref);
  product->xo = NULL;
  list_add(&(product->list), &ProductList);
  RTL_TRDBG(TRACE_ERROR, "Product description unknown for '%s'\n", product->ref);

  // Fetch
  product->fetching = true;
  DiaProductsFetch(product->id);
  
  return NULL;
}

void
ProductsSyncAll(void)
{
  struct list_head *index;
  Product_t *product;
  
  list_for_each(index, &ProductList) {
    product = list_entry(index, Product_t, list);

    if (product->fetching != true) {
      product->fetching = true;
      DiaProductsFetch(product->id);
    }
  }
}


void
ProductsSyncNull(void)
{
  struct list_head *index;
  Product_t *product;
  
  list_for_each(index, &ProductList) {
    product = list_entry(index, Product_t, list);

    if (product->fetching != true && product->xo == NULL) {
      product->fetching = true;
      DiaProductsFetch(product->id);
    }
  }
}

void
ProductsSync(char *ref)
{
  struct list_head *index;
  Product_t *product;
  
  list_for_each(index, &ProductList) {
    product = list_entry(index, Product_t, list);

    if (strcmp(product->ref, ref) == 0) {
      product->fetching = true;
      DiaProductsFetch(product->id);
    }
  }
}

void
ProductsList(void *cl)
{
  struct list_head *index;
  Product_t *product;
  
  list_for_each(index, &ProductList) {
    product = list_entry(index, Product_t, list);
    AdmPrint(cl, "\tref=%s xo=%p fetch=%s\n", product->ref, product->xo, 
      product->fetching ? "yes" : "no");
  }
}

char *
ProductsGetRefFromId(int id)
{
  struct list_head *index;
  Product_t *product;
  
  list_for_each(index, &ProductList) {
    product = list_entry(index, Product_t, list);

    if (id == product->id) {
      return product->ref;
    }
  }
  
  return "";
}

void
ProductsSetDescription(int id, void *xo)
{
  struct list_head *index;
  Product_t *product;
       
  list_for_each(index, &ProductList) {
    product = list_entry(index, Product_t, list);

    if (id == product->id) {
      RTL_TRDBG(TRACE_API, "Product description received for '%s'\n", product->ref);
      
      product->fetching = false;

      if (xo == NULL) {
        return;
      }
  
      // Swap descriptor and Free 
      void *xoxo = product->xo;
      product->xo = xo;
      if (xoxo) {
      XoFree(xoxo, 1);
      }    
      break;
    }
  }
  
  // Create or update cache
  void *dir;
  char *name;
  //char *point;
  char fullPath[NAME_MAX];
  char path[NAME_MAX];
  int reflen = strlen(product->ref);

    snprintf(path, NAME_MAX, "%s/usr/data/%s/products", rootactPath, GetAdaptorName());
  
  dir = rtl_openDir(path);
  if (!dir) {
    return;
  }
  
  while ((name = rtl_readDir(dir)) && *name) {
    if (*name == '.' && *(name + 1) == '\0') {
      continue;
    }
    if (*name == '.' && *(name + 1) == '.') {
      continue;
    }
    if (strncmp(name, product->ref, reflen) == 0) {
      sprintf(fullPath, "%s/%s.xml", path, name);
      
      RTL_TRDBG(TRACE_API, "Drop cache for product description: '%s'\n", product->ref);
      remove(fullPath);
      break;
    }
  }
  
  sprintf(fullPath, "%s/%s.xml", path, product->ref);
  RTL_TRDBG(TRACE_API, "Create cache for product description: '%s'\n", product->ref);
  XoWritXmlEx(fullPath, xo, 0, 0);
  
  // Update all device based on this product !
  DeviceNotifyProductUpdate(product->ref);
}

void
ProductsClearCache(void)
{
  void *dir;
  char *name;
  char *point;
  char fullPath[NAME_MAX];
  char path[NAME_MAX];


    snprintf(path, NAME_MAX, "%s/usr/data/%s/products", rootactPath, GetAdaptorName());
  
  dir = rtl_openDir(path);
  if (!dir) {
    return;
  }
  
  while ((name = rtl_readDir(dir)) && *name) {
    if (*name == '.' && *(name + 1) == '\0') {
      continue;
    }
    if (*name == '.' && *(name + 1) == '.') {
      continue;
    }
    
    point = rtl_suffixname(name);
    if (point && *point && strcmp(point + 1, "xml") == 0) {
      sprintf(fullPath, "%s/%s", path, name);
      remove(fullPath);
    }
  }
}

void
ProductsLoadCache(void)
{
  void *dir;
  char *name;
  char *point;
  char fullPath[NAME_MAX];
  char path[NAME_MAX];
  Product_t *product;

  
    snprintf(path, NAME_MAX, "%s/usr/data/%s/products", rootactPath, GetAdaptorName());
  
  RTL_TRDBG(TRACE_API, "Loading product description form cache : '%s'\n", path);
  dir = rtl_openDir(path);
  if (!dir) {
    return;
  }
  
  while ((name = rtl_readDir(dir)) && *name) {
    if (*name == '.' && *(name + 1) == '\0') {
      continue;
    }
    if (*name == '.' && *(name + 1) == '.') {
      continue;
    }
    
    point = rtl_suffixname(name);
    if (point && *point && strcmp(point + 1, "xml") == 0) {
      sprintf(fullPath, "%s/%s", path, name);
      
      int  parse;
      void *xo = XoReadXmlEx(fullPath, NULL, 0, &parse);
      if (xo == NULL) {
        continue;
      }

      product = (Product_t *) malloc(sizeof(Product_t));
      if (product == NULL) {
        RTL_TRDBG(TRACE_ERROR, "malloc failure\n");
        XoFree(xo, 1);
        continue;
      }
  
      productID++;
      product->id = productID;
      *point = '\0';
      product->ref = strdup(name);
      product->xo = xo;
      product->fetching = false;
      list_add(&(product->list), &ProductList);
      RTL_TRDBG(TRACE_INFO, "\t-> ref='%s', xo=%p, itfCount=%d\n", product->ref, 
        xo, XoNmNbInAttr(xo, "modbus:interfaces", 0));
    }
  }
}
