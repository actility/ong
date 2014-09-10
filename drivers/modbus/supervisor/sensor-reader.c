#include "modbus.h"
#include <pthread.h>

static pthread_t threadReader;
static bool isThreadRunning = false;

void
modbusReaderNotifyExit(void *pdata)
{
  isThreadRunning = false;
}

void *
modbusReaderThread(void *pdata)
{
  struct list_head *indexNetwork, *indexDevice, *indexPoint;
  Network_t *network;
  Sensor_t *device;
  Point_t *pt;
  time_t now = 0;
  t_dia_req req[1];
  
  pthread_cleanup_push(modbusReaderNotifyExit, NULL);
  
  do {
    pthread_testcancel();
    //now = time(NULL);
    rtl_timemono(&now);
  
 
    list_for_each(indexNetwork, &NetworkInternalList) {
      network = list_entry(indexNetwork, Network_t, list);

      if (network->isInit == false) {
        continue;
      }
      
      list_for_each(indexDevice, &(network->device)) {
        device = list_entry(indexDevice, Sensor_t, list);

        if (device->isInit == false) {
          continue;
        }
      
        list_for_each(indexPoint, &(device->cnts)) {
          pt = list_entry(indexPoint, Point_t, list);   

          if (pt->isInit == false) {
            continue;
          }
        
          if(pt->nextRead < now) {
            void *xo = NULL;
            Value_t *val = &(pt->lastVal);
            if (pt->nextMaxInterval < now)
            {
              val = NULL; // need a report even if the value has changed
            }
            bool valueChanged = _modbusAccessRead(device, &(pt->attr), NULL, 
              &xo, val);
            if (valueChanged)
            {              
              if (xo != NULL) {
                RTL_TRDBG(0, "[READER OK] %s_%s %s\n", network->name, device->name, 
                  pt->attr.modbusAccess);
                pt->xo = xo;
                pt->nextMaxInterval = now + pt->maxInterval;
                
                memset(req, 0, sizeof(t_dia_req));
                req->rq_dev = device->cp_num;
                req->rq_serial = device->cp_serial;
                req->rq_app = 0;
                req->rq_cluster = pt->rq_cluster;
                req->rq_attribut = pt->rq_attribut;
                req->rq_member = pt->rq_member;
                req->rq_name = (char *)__func__;
                req->rq_cbserial = ModbusDiaLookup;
                req->rq_plcDup  = 1;
                req->rq_flags = 0;
                DiaRepInstCreate(req, NULL);

              } else {
                RTL_TRDBG(0, "[READER ERROR] %s_%s %s\n", network->name, device->name, 
                  pt->attr.modbusAccess);
              }
            }
            
            
            pt->nextRead = now + pt->minInterval;
          }
        }
      }
    }
    
    usleep(100000);
  } while(true);
  
  pthread_cleanup_pop(1);
  return NULL;
}


int
modbusReaderStart(void)
{
  int ret;
  
  if (isThreadRunning == true) {
    return -1;
  }
  
  ret = pthread_create(&threadReader, NULL, modbusReaderThread, NULL);
  if (ret != 0) {
    RTL_TRDBG(0, "Error on pthread_create\n");
    return -2;
  }
  isThreadRunning = true;

  return 0;
}

int
modbusReaderStop(void)
{
  int ret;
  
  if (isThreadRunning == false) {
    return -1;
  }
  
  ret = pthread_cancel (threadReader);
  if (ret < 0) {
    RTL_TRDBG(0, "Error on pthread_cancel, can't stop reader thread !!!\n");
  } else {
    ret = pthread_join (threadReader, NULL);
    if (ret == 0 || ret == -ESRCH) {
      RTL_TRDBG(0, "Reader thread stopped (ret code = %d)\n", ret);
    } else {
      RTL_TRDBG(0, "Can't join the reader thread ! (ret code = %d)\n", ret);
    }
  }
  
  isThreadRunning = false;
  return 0;
}

void
modbusReaderStatus(void *cl)
{
  if (isThreadRunning == true) {
    AdmPrint(cl, "Reader is running.\n");
  } else {
    AdmPrint(cl, "Reader is NOT running.\n");
  }
}
