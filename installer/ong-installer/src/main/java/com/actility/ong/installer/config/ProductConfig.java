package com.actility.ong.installer.config;

import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

public final class ProductConfig {
    private final String name;
    private final int index;
    private final String label;
    private final Map<String, String> genericToInternalModule;

    public ProductConfig(String name, Properties properties) {
        this.name = name;
        this.index = Integer.parseInt((String) properties.get("index"));
        this.label = (String) properties.get("label");
        genericToInternalModule = new HashMap<String, String>();
        genericToInternalModule.put(Modules.CORE, (String) properties.get("coreModule"));
        genericToInternalModule.put(Modules.TTY_MAPPER, (String) properties.get("ttyMapperModule"));
        genericToInternalModule.put(Modules.NTPCLIENT, (String) properties.get("ntpclientModule"));
        genericToInternalModule.put(Modules.GSCL, (String) properties.get("gsclModule"));
        genericToInternalModule.put(Modules.ZIGBEE, (String) properties.get("zigbeeModule"));
        genericToInternalModule.put(Modules.WATTECO, (String) properties.get("wattecoModule"));
        genericToInternalModule.put(Modules.WMBUS, (String) properties.get("wmbusModule"));
        genericToInternalModule.put(Modules.KNX, (String) properties.get("knxModule"));
        genericToInternalModule.put(Modules.MODBUS, (String) properties.get("modbusModule"));
        genericToInternalModule.put(Modules.IEC61131, (String) properties.get("iec61131Module"));
        genericToInternalModule.put(Modules.TCPDUMP, (String) properties.get("tcpdumpModule"));
    }

    public String getName() {
        return name;
    }

    public int getIndex() {
        return index;
    }

    public String getLabel() {
        return label;
    }

    public String getInternalModule(String genericModule) {
        return genericToInternalModule.get(genericModule);
    }

    @Override
    public String toString() {
        return name;
    }
}
