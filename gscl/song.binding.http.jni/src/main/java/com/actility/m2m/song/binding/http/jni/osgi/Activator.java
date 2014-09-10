package com.actility.m2m.song.binding.http.jni.osgi;

import java.util.Dictionary;
import java.util.Enumeration;
import java.util.Hashtable;

import org.apache.felix.shell.Command;
import org.apache.log4j.Level;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;

import com.actility.m2m.http.client.ni.HttpClientNiService;
import com.actility.m2m.song.binding.http.jni.command.HttpClientDumpTransactions;
import com.actility.m2m.song.binding.http.jni.command.HttpClientStatsCommand;
import com.actility.m2m.song.binding.http.jni.impl.HttpClientNiServiceImpl;
import com.actility.m2m.song.binding.http.jni.impl.HttpClientStatsImpl;
import com.actility.m2m.song.binding.http.jni.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public class Activator implements BundleActivator, ManagedService {
    private static final org.apache.log4j.Logger LOG = OSGiLogger.getLogger(Activator.class, BundleLogger.LOG);

    private static final Boolean DEFAULT_CURL_DEBUG_MODE = new Boolean(false);
    private static final Integer DEFAULT_TRACE_LEVEL = new Integer(0);

    private BundleContext context;
    private Dictionary config;
    private ServiceRegistration managedServiceRegistration;

    private HttpClientNiServiceImpl jniSongBindingHttp;
    private HttpClientStatsImpl statModule;

    private ServiceRegistration niHttpClientServiceRegistration;
    private ServiceRegistration httpStatsRegistration;
    private ServiceRegistration httpDumpTransRegistration;

    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.LOG.init(context);

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
        }

        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }

        config = null;
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
        }

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();
        managedServiceRegistration = null;

        stopJniSongBindingHttp();
        this.context = null;
        config = null;
        BundleLogger.LOG.init(null);
    }

    private Object checkWithDefault(Dictionary config, String name, Class type, Object defaultValue) {
        Object result = defaultValue;
        Object value = config.get(name);
        if (value == null) {
            if (LOG.isEnabledFor(Level.WARN)) {
                LOG.warn("Configuration property " + name + " is null -> use default value instead " + defaultValue);
            }
        } else if (!type.equals(value.getClass())) {
            LOG.error("Configuration property " + name + " must be of type " + type.toString() + " while it is of type "
                    + value.getClass() + " -> use default value instead " + defaultValue);
        } else if (type != String.class || ((String) value).length() != 0) {
            result = value;
        }
        return result;
    }

    private boolean getCurlDebugMode(Dictionary config) {
        return ((Boolean) checkWithDefault(config, "curlDebugMode", Boolean.class, DEFAULT_CURL_DEBUG_MODE)).booleanValue();
    }

    private int getTraceLevel(Dictionary config) {
        return ((Integer) checkWithDefault(config, "traceLevel", Integer.class, DEFAULT_TRACE_LEVEL)).intValue();
    }

    synchronized private void startJniSongBindingHttp() {
        if (config != null) {
            boolean error = false;
            try {
                boolean curlDebugMode = getCurlDebugMode(config);
                int traceLevel = getTraceLevel(config);

                statModule = new HttpClientStatsImpl();

                jniSongBindingHttp = new HttpClientNiServiceImpl(curlDebugMode, statModule);
                jniSongBindingHttp.setTraceLevel(traceLevel);
                jniSongBindingHttp.start();

                if (LOG.isInfoEnabled()) {
                    LOG.info("Registering service " + HttpClientNiService.class.getName() + "...");
                }
                Dictionary properties = new Hashtable();
                properties.put("curlDebugMode", new Boolean(curlDebugMode));
                niHttpClientServiceRegistration = context.registerService(HttpClientNiService.class.getName(),
                        jniSongBindingHttp, properties);
                httpStatsRegistration = context.registerService(Command.class.getName(),
                        new HttpClientStatsCommand(statModule), null);
                httpDumpTransRegistration = context.registerService(Command.class.getName(), new HttpClientDumpTransactions(
                        jniSongBindingHttp.getHttpClientNi()), null);
            } catch (Throwable t) {
                LOG.error("Unexpected exception", t);
                error = true;
            } finally {
                if (error) {
                    stopJniSongBindingHttp();
                }
            }
        }
    }

    synchronized private void stopJniSongBindingHttp() {
        if (jniSongBindingHttp != null) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Unregistering service " + HttpClientNiService.class.getName() + "...");
            }
            niHttpClientServiceRegistration.unregister();
            niHttpClientServiceRegistration = null;
            httpStatsRegistration.unregister();
            httpStatsRegistration = null;
            httpDumpTransRegistration.unregister();
            httpDumpTransRegistration = null;
            jniSongBindingHttp.stop();
            jniSongBindingHttp = null;
        }
    }

    synchronized public void updated(Dictionary properties) throws ConfigurationException {
        if (LOG.isInfoEnabled()) {
            if (properties != null) {
                LOG.info("Received a configuration for pid " + context.getBundle().getSymbolicName() + ".config");
                Enumeration e = properties.keys();
                while (e.hasMoreElements()) {
                    String key = (String) e.nextElement();
                    Object value = properties.get(key);
                    LOG.info("  " + key + " -> " + value + " (" + value.getClass() + ")");
                }
            } else {
                LOG.info("Received a null configuration for pid " + context.getBundle().getSymbolicName() + ".config");
            }
        }
        config = properties;
        if (config == null) {
            config = new Hashtable();
        }
        stopJniSongBindingHttp();
        startJniSongBindingHttp();
    }

}
