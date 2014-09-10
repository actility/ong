#include "modbus.h"

/*
 *  Generate the Xo object to add into the list 'interface' of M2M APP_* DESCRIPTOR
 */
#define ATTR_NOT_USED     0
#define ATTR_DESCRIPTOR   1
#define ATTR_LOGGING    2
#define ATTR_RETARGETING  3

void
ApplicationITF(void *appInterfaces, void *xoProduct, void *xoModeling, Sensor_t *device)
{
  int iItf, nbItf;
  int iAttr, nbAttr;
  int iModel, nbModel;
  void *itfModel, *itfModelDescriptor, *itfModelCov;
  int iDescriptor, nbDescriptor;
  int attrType = ATTR_NOT_USED;
  
  char itfFilter[1024];
  char href[2048];
  
  if (appInterfaces == NULL || xoProduct == NULL || xoModeling == NULL || device == NULL) {
    return;
  }
  
  nbItf = XoNmNbInAttr(xoProduct, "modbus:interfaces", 0);
  nbModel = XoNmNbInAttr(xoModeling, "ong:interfaces", 0);
  
  // Iterate over each interface
  RTL_TRDBG(1, "nbItf = %d\n", nbItf);
  for (iItf=0; iItf<nbItf; iItf++) {
    //printf("===============\n");
    itfModel = NULL;
    void *interface = XoNmGetAttr(xoProduct, "modbus:interfaces[%d]", 0, iItf);
    CONTINUE_IF_NULL(interface);
      
    char *interfaceName = (char *) XoNmGetAttr(interface, "modbus:id.value$", 0);
    //RTL_TRDBG(1, "interfaceName = %s\n", interfaceName);
    
    snprintf(itfFilter, sizeof(itfFilter), "modbus:/int/%s", interfaceName);
    
    // Test if the interfaceID is in modeling, if not ignore this interface
    for (iModel=0; iModel<nbModel; iModel++) {
      void *o = XoNmGetAttr(xoModeling, "ong:interfaces[%d]", NULL, iModel);
      CONTINUE_IF_NULL(o);

      void *filter = XoNmGetAttr(o, "ong:filter", NULL, 0);
      CONTINUE_IF_NULL(filter);
      
      if (strcmp(filter, itfFilter) == 0) {
        itfModel = o;
        break;
      }
    }
    
    // Interface not in the modeling, ignore it
    CONTINUE_IF_NULL(itfModel);
    
    // Ensure the modeling have a descriptor for this filter
    itfModelDescriptor = XoNmGetAttr(itfModel, "ong:descriptor", 0);
    CONTINUE_IF_NULL(itfModelDescriptor);
    
    // Cov may be missing
    itfModelCov = XoNmGetAttr(itfModel, "ong:covConfigurations", 0);
    
    // Create the object to append in the interface list
    void *itf = XoNmNew("o:obj");
    CONTINUE_IF_NULL(itf);
    
    void *interfaceID = XoNmNew("o:str");
    if (interfaceID == NULL) {
      XoFree(itf, 1);
      continue;
    }
    XoNmSetAttr(interfaceID, "name", "interfaceID", 0);
    XoNmSetAttr(interfaceID, "val", interfaceName, 0);
    XoNmAddInAttr(itf, "[]", interfaceID, 0, 0);
    
    // Iterate over each attributs
    nbAttr = XoNmNbInAttr(interface, "modbus:attributs", 0);
    //RTL_TRDBG(1, "nbAttr = %d\n", nbAttr);
    for (iAttr=0; iAttr<nbAttr; iAttr++) {
      //RTL_TRDBG(1, "--------------------\n");
      void *name;
      void *accessRight;
      void *attr = XoNmGetAttr(interface, "modbus:attributs[%d]", 0, iAttr);

      char *attrName = (char *) XoNmGetAttr(attr, "modbus:name", 0);
      //RTL_TRDBG(1, "attrName = %s\n", attrName);

      // Test if the attribut is in modeling, if not ignore this attribut
      // Then find the type of attribut : descriptor / logging / retargeting
      attrType = ATTR_NOT_USED;
      nbDescriptor = XoNmNbInAttr(itfModel, "ong:descriptor", 0);
      //RTL_TRDBG(1, "nbDescriptor = %d\n", nbDescriptor);
      for (iDescriptor=0; iDescriptor<nbDescriptor; iDescriptor++) {
        void *o = XoNmGetAttr(itfModel, "ong:descriptor[%d]", NULL, iDescriptor);
        CONTINUE_IF_NULL(o);

        name = XoNmGetAttr(o, "ong:name", NULL, 0);
        CONTINUE_IF_NULL(name);
        
        accessRight = XoNmGetAttr(o, "ong:accessRight", NULL, 0);
        // May be NULL
        
        //RTL_TRDBG(1, "ong:name = %s\n", (char *)name);
        //RTL_TRDBG(1, "node = %s\n", XoNmType(o));
        
        if (strcmp(name, attrName) == 0) {

          void *config = XoNmGetAttr(o, "ong:config", NULL, 0);
          CONTINUE_IF_NULL(config);     // Ensure for attr/point mixing in modeling
    
          RTL_TRDBG(1, "config = %p (%s)\n", config, config);
          if (config == NULL) {
            attrType = ATTR_NOT_USED;
          } else {
            if (strcmp(config, "descriptor") == 0) {
              attrType = ATTR_DESCRIPTOR;
            } else
            if (strcmp(config, "logging") == 0) {
              attrType = ATTR_LOGGING;
            } else
            if (strcmp(config, "retargeting") == 0) {
              attrType = ATTR_RETARGETING;
            } else {
              attrType = ATTR_NOT_USED;
            }
          }
        
          break;
        }
      }
      
      if (attrType == ATTR_NOT_USED) {
        continue;
      }
    
      char *attrModbusType = (char *) XoNmGetAttr(attr, "modbus:type", 0);
      CONTINUE_IF_NULL(attrModbusType);
      //RTL_TRDBG(1, "attrType = %s\n", attrModbusType);
      
      char *attrDisplay = (char *) XoNmGetAttr(attr, "modbus:display", 0);
      CONTINUE_IF_NULL(attrDisplay);
      //RTL_TRDBG(1, "attrDisplay = %s\n", attrDisplay);
      
      char *attrReg = (char *) XoNmGetAttr(attr, "modbus:reg", 0);
      CONTINUE_IF_NULL(attrReg);
      //RTL_TRDBG(1, "attrReg = %s\n", attrReg);

      char *containerID = (char *) XoNmGetAttr(attr, "modbus:cnt", 0);
      CONTINUE_IF_NULL(containerID);

//      uint16_t rq_cluster, rq_attribut, rq_member;
//      int ret = sscanf(containerID, "0x%hX.0x%hX.%u.m2m", 
//        &(rq_cluster), &(rq_attribut), &(rq_member));
      unsigned int rq_cluster, rq_attribut, rq_member;
      int ret = sscanf(containerID, "0x%x.0x%x.%u.m2m", 
        &(rq_cluster), &(rq_attribut), &(rq_member));
      if (ret != 3) {
        continue;
      }
      
      char *attrReadable = (char *) XoNmGetAttr(attr, "modbus:readable", 0);
      if (attrReadable == NULL) {
        attrReadable = "false";
      }
      //RTL_TRDBG(1, "attrReadable = %s\n", attrReadable);

      char *attrWritable = (char *) XoNmGetAttr(attr, "modbus:writable", 0);
      if (attrWritable == NULL) {
        attrWritable = "false";
      }
      //RTL_TRDBG(1, "attrWritable = %s\n", attrWritable);         

      char *attrUnit = (char *) XoNmGetAttr(attr, "modbus:unit", 0);
      //RTL_TRDBG(1, "attrUnit = %s\n", attrUnit);

      bool isReadable = *attrReadable == 't' || *attrReadable == 'T';
      bool isWritable = *attrWritable == 't' || *attrWritable == 'T';
      char *obixType;
      void *m2mPoint, *m2mDesc, *m2mOp;
      int ModbusTypeID;
      
      switch(attrType) {
      
        // Create a constant & forget
        case ATTR_DESCRIPTOR:
          if (isReadable) {
            obixType = obixTypeFromModbus(attrModbusType, NULL);
            if (obixType == NULL) break;
            
            m2mDesc = XoNmNew(obixType);
            if (m2mDesc == NULL) break;

            XoNmSetAttr(m2mDesc, "name", name, 0);

            if (attrUnit != NULL) {
              XoNmSetAttr(m2mPoint, "unit", attrUnit, 0);
            }
            
            char *val = NULL;
            Attr_t attr;
            decodeModbusAccess(&attr, attrReg);
            obixType = obixTypeFromModbus(attrModbusType, &ModbusTypeID);
            if (obixType == NULL) break;
            attr.modbusTypeID = ModbusTypeID;
            
            _modbusAccessRead(device, &attr, &val, NULL);
            XoNmSetAttr(m2mDesc, "val", val, 0);
            free(val);
            
            XoNmAddInAttr(itf, "[]", m2mDesc, 0, 0);
          }
          break;
          
        case ATTR_RETARGETING:
          if (isReadable || isWritable) {
            Attr_t *attr = (Attr_t *) malloc(sizeof(Attr_t));
            if (attr == NULL) {
              break;
            }
            
            attr->modbusType = attrModbusType;
            attr->modbusAccess = attrReg;
            attr->modbusTypeID = ACCESS_VOID;
            attr->isReadable = isReadable;
            attr->isWritable = isWritable;
            decodeModbusAccess(attr, attrReg);
            
            list_add(&(attr->list), &(device->ops));
            
            obixType = obixTypeFromModbus(attrModbusType, &ModbusTypeID);
            if (obixType == NULL) break;
            attr->modbusTypeID = ModbusTypeID;
            
            m2mPoint = XoNmNew(obixType);
            if (m2mPoint == NULL) break;

            XoNmSetAttr(m2mPoint, "name", name, 0);

            if (attrUnit != NULL) {
              XoNmSetAttr(m2mPoint, "unit", attrUnit, 0);
            }
            
            unsigned int accessRightLevel = 1;
            if (accessRight != NULL) {
              sscanf(accessRight, "level%u", &accessRightLevel);
            }
            
            // retargeting2 must be variable from modeling
            snprintf(href, sizeof(href), "%s/APP_%s.%u/retargeting%u/%s",
              GetVar("s_rootapp"), device->cp_ieee, 0, accessRightLevel, attr->modbusAccess);
            XoNmSetAttr(m2mPoint, "href", href, 0);
            XoNmAddInAttr(itf, "[]", m2mPoint, 0, 0);
            
            m2mPoint = XoNmNew("o:bool");
            if (m2mPoint != NULL) {
              XoNmSetAttr(m2mPoint, "name", "modbusReadable", 0);
              XoNmSetAttr(m2mPoint, "val", isReadable ? "true" : "false", 0);
              XoNmAddInAttr(itf, "[]", m2mPoint, 0, 0);
            }
          
            m2mPoint = XoNmNew("o:bool");
            if (m2mPoint != NULL) {
              XoNmSetAttr(m2mPoint, "name", "modbusWritable", 0);
              XoNmSetAttr(m2mPoint, "val", isWritable ? "true" : "false", 0);
              XoNmAddInAttr(itf, "[]", m2mPoint, 0, 0);
            }
            
            m2mPoint = XoNmNew("o:str");
            if (m2mPoint != NULL) {
              XoNmSetAttr(m2mPoint, "name", "modbusDescription", 0);
              XoNmSetAttr(m2mPoint, "val", attrDisplay, 0);
              XoNmAddInAttr(itf, "[]", m2mPoint, 0, 0);
            }
          }
          break;
        
        // M2M point - Read Only
        case ATTR_LOGGING:
          if (isReadable) {
            bool isNew = true;
            Point_t *pt = DeviceGetPointOrCreate(device, containerID, &isNew);
            if (pt == NULL) {
              break;
            }
            pt->toDelete = false;
            pt->containerID = containerID;
            pt->rq_cluster = rq_cluster;
            pt->rq_attribut = rq_attribut;
            pt->rq_member = rq_member;
            pt->minInterval = 5;
            pt->maxInterval = 30;
            pt->attr.modbusType = ACCESS_VOID;
            pt->attr.modbusAccess = attrReg;
            pt->xo = NULL;
            
            decodeModbusAccess(&(pt->attr), attrReg);
            if(isNew) { 
              list_add(&(pt->list), &(device->cnts));
            }
            
            obixType = obixTypeFromModbus(attrModbusType, &ModbusTypeID);
            if (obixType == NULL) break;
            pt->attr.modbusTypeID = ModbusTypeID;
            
            m2mPoint = XoNmNew(obixType);
            if (m2mPoint == NULL) break;

            XoNmSetAttr(m2mPoint, "name", name, 0);
            
            if (attrUnit != NULL) {
              XoNmSetAttr(m2mPoint, "unit", attrUnit, 0);
            }
            
            snprintf(href, sizeof(href), "%s/APP_%s.%u/containers/0x%04hX.0x%04hX.%u.m2m"
              "/contentInstances/latest/content", GetVar("s_rootapp"), device->cp_ieee, 0, 
              pt->rq_cluster, pt->rq_attribut, pt->rq_member);
            XoNmSetAttr(m2mPoint, "href", href, 0);
            XoNmAddInAttr(itf, "[]", m2mPoint, 0, 0);
            
            m2mPoint = XoNmNew("o:str");
            if (m2mPoint != NULL) {
              XoNmSetAttr(m2mPoint, "name", "modbusDescription", 0);
              XoNmSetAttr(m2mPoint, "val", attrDisplay, 0);
              XoNmAddInAttr(itf, "[]", m2mPoint, 0, 0);
            }
            
            // Set default timing
            pt->minInterval = parseISO8601Duration(GetVar("loggingDefaultCovMinInterval"));
            pt->maxInterval = parseISO8601Duration(GetVar("loggingDefaultCovMaxInterval"));
            
            // Read timing
            char *minTiming = NULL;
            char *maxTiming = NULL;
            int nbCov = XoNmNbInAttr(itfModel, "ong:covConfigurations");
            int iCov;
            char filterStr[1024];
            snprintf(filterStr, sizeof(filterStr), "modbus:/attribute/%s", name);
            for(iCov=0; iCov<nbCov; iCov++) {
              void *cov = XoNmGetAttr(itfModel, "ong:covConfigurations[%d]", NULL, iCov);
              CONTINUE_IF_NULL(cov);
              
              char *filter = (char *) XoNmGetAttr(cov, "ong:filter", 0);
              CONTINUE_IF_NULL(filter);

              if (strcmp(filter, filterStr) == 0) {
                minTiming = XoNmGetAttr(cov, "ong:minInterval", 0);
                maxTiming = XoNmGetAttr(cov, "ong:maxInterval", 0);
                break;
              }      
            }
            pt->nextRead = 0;
            pt->nextMaxInterval = 0;
            if (minTiming != NULL) {
              pt->minInterval = parseISO8601Duration(minTiming);
            }
            if (maxTiming != NULL) {
              pt->maxInterval = parseISO8601Duration(maxTiming);
            }
          }
          break;
                          
        case ATTR_NOT_USED:
        default:
          // DEAD CODE HERE
          break;
      }
    }
    
    XoNmAddInAttr(appInterfaces, "[name=interfaces].[]", itf, 0, 0);
  }
}

