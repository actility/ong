/*******************************************************************************
 * Copyright   Actility, SA. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 *******************************************************************************/

package com.actility.m2m.cm;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.Filter;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;
import org.osgi.service.cm.Configuration;
import org.osgi.service.cm.ConfigurationAdmin;
import org.osgi.service.cm.ConfigurationEvent;
import org.osgi.service.cm.ConfigurationListener;
import org.osgi.service.cm.ConfigurationPlugin;
import org.osgi.service.cm.ManagedService;
import org.osgi.service.cm.ManagedServiceFactory;

import com.actility.m2m.cm.log.BundleLogger;
import com.actility.m2m.util.ArrayUtils;
import com.actility.m2m.util.EqualsUtils;
import com.actility.m2m.util.UUID;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Manages reading and writing of configurations on the file system.
 */
public final class ConfigurationAdminManager {
    private static final Logger LOG = OSGiLogger.getLogger(ConfigurationAdminManager.class, BundleLogger.getStaticLogger());

    private final BundleContext context;
    private final Set/* <String> */bundleLocations;
    private final Map/* <String, List<ServiceReference>> */pidToServiceReferences;
    private final ConfigurationDictionaryManager configurationDictionaryManager;
    private final AsyncDispatcher asyncDispatcher;
    private ServiceReference cmRef;

    /**
     * * Sorted list of ServiceReferences to ConfigurationPlugins.
     */
    private final List preModificationPlugins;

    /**
     * * Sorted list of ServiceReferences to ConfigurationPlugins.
     */
    private final List modifyingPlugins;

    /**
     * * Sorted list of ServiceReferences to ConfigurationPlugins.
     */
    private final List postModificationPlugins;

    /**
     * * Mapping of a ServiceReference to its ranking (Integer).
     */
    private final Map/* <Long, Integer> */pluginRankings;

    public ConfigurationAdminManager(BundleContext context, File etcConfigDir, File usrConfigDir) throws IOException {
        this.context = context;
        this.bundleLocations = new HashSet/* <String> */();
        this.pidToServiceReferences = new HashMap/* <String, List<ServiceReference>> */();
        this.configurationDictionaryManager = new ConfigurationDictionaryManager(etcConfigDir, usrConfigDir);
        this.preModificationPlugins = new ArrayList();
        this.modifyingPlugins = new ArrayList();
        this.postModificationPlugins = new ArrayList();
        this.pluginRankings = new HashMap/* <Long, Integer> */();
        this.asyncDispatcher = new AsyncDispatcher();
    }

    public synchronized void setCmRef(ServiceReference cmRef) {
        this.cmRef = cmRef;
    }

    /**
     * Adds a bundle location to the list of known locations.
     *
     * @param location The bundle location to add
     */
    public synchronized void addedBundle(String location) {
        bundleLocations.add(location);
    }

    /**
     * Removes and unbinds a bundle location from the list of known locations.
     *
     * @param location The bundle location to remove
     */
    public synchronized void removedBundle(String location) {
        bundleLocations.remove(location);
        String filter = "&((" + ConfigurationAdmin.SERVICE_BUNDLELOCATION + "=" + location + ")("
                + CMConstants.SERVICE_DYNAMICLOCATION + "=" + Boolean.TRUE + "))";
        try {
            List /* <Configuration> */configurations = getConfigurations(filter, null, false);
            Iterator it = configurations.iterator();
            while (it.hasNext()) {
                Configuration configuration = (Configuration) it.next();
                ConfigurationDictionary dictionary = (ConfigurationDictionary) configuration.getProperties();
                setBundleLocation(configuration.getPid(), configuration.getFactoryPid(), null, dictionary);
            }
        } catch (IOException e) {
            LOG.error("Error while unbinding configurations bound to " + location, e);
        } catch (InvalidSyntaxException e) {
            LOG.error("Error while unbinding configurations bound to " + location, e);
        }
    }

    /**
     * Registers a new incoming ManagedServiceFactory
     *
     * @param serviceReference The reference to the managed service factory
     */
    public synchronized void registeredManagedServiceFactory(ServiceReference serviceReference) {
        String[] factoryPids = getServicePids(serviceReference);
        if (factoryPids == null) {
            String bundleLocation = serviceReference.getBundle().getLocation();
            LOG.error("ManagedServiceFactory is registering without a " + ConfigurationAdmin.SERVICE_FACTORYPID + ": "
                    + bundleLocation);
        } else {
            registerPidsAndCheck(serviceReference, factoryPids);
            if (LOG.isDebugEnabled()) {
                LOG.debug("ManagedServiceFactory registered: " + ArrayUtils.toString(factoryPids, "[", "]", ", "));
            }
            try {
                updateManagedServiceFactory(serviceReference, factoryPids);
            } catch (IOException e) {
                LOG.error("Error while notifying services.", e);
            }
        }
    }

    /**
     * Unregisters a ManagedServiceFactory
     *
     * @param serviceReference The reference to the managed service factory
     */
    public synchronized void unregisteredManagedServiceFactory(ServiceReference serviceReference) {
        unregisterPids(serviceReference);
    }

    /**
     * Registers a new incoming ManagedService
     *
     * @param serviceReference The reference to the managed service
     */
    public synchronized void registeredManagedService(ServiceReference serviceReference) {
        String[] servicePids = getServicePids(serviceReference);
        if (servicePids == null) {
            String bundleLocation = serviceReference.getBundle().getLocation();
            LOG.error("ManagedService is registering without a " + Constants.SERVICE_PID + ": " + bundleLocation);
        } else {
            registerPidsAndCheck(serviceReference, servicePids);
            if (LOG.isDebugEnabled()) {
                LOG.debug("ManagedService registered: " + ArrayUtils.toString(servicePids, "[", "]", ", "));
            }
            try {
                updateManagedService(serviceReference, servicePids);
            } catch (IOException e) {
                LOG.error("Error while notifying services.", e);
            }
        }
    }

    /**
     * Unregisters a ManagedService
     *
     * @param serviceReference The reference to the managed service
     */
    public synchronized void unregisteredManagedService(ServiceReference serviceReference) {
        unregisterPids(serviceReference);
    }

    /**
     * Registers a new incoming ConfigurationPlugin
     *
     * @param serviceReference The reference to the configuration plugin
     */
    public synchronized void registeredConfigurationPlugin(ServiceReference serviceReference) {
        Object rankingProperty = serviceReference.getProperty(ConfigurationPlugin.CM_RANKING);
        if (rankingProperty == null) {
            rankingProperty = new Integer(0);
        } else if (rankingProperty.getClass() != Integer.class) {
            rankingProperty = new Integer(0);
        }

        Long serviceId = (Long) serviceReference.getProperty(Constants.SERVICE_ID);
        if (serviceId == null) {
            LOG.error("Missing service id for a ConfigurationPlugin");
        } else {
            int ranking = ((Integer) rankingProperty).intValue();
            pluginRankings.put(serviceId, rankingProperty);
            insertPluginReference(serviceReference, ranking);
        }
    }

    /**
     * Modifies an already registered ConfigurationPlugin
     *
     * @param serviceReference The reference to the configuration plugin
     */
    public synchronized void modifiedConfigurationPlugin(ServiceReference serviceReference) {
        Object rankingProperty = serviceReference.getProperty(ConfigurationPlugin.CM_RANKING);
        if (rankingProperty == null) {
            rankingProperty = new Integer(0);
        } else if (rankingProperty.getClass() != Integer.class) {
            rankingProperty = new Integer(0);
        }

        Long serviceId = (Long) serviceReference.getProperty(Constants.SERVICE_ID);
        if (serviceId == null) {
            LOG.error("Missing service id for a ConfigurationPlugin");
            return;
        }

        int ranking = ((Integer) rankingProperty).intValue();
        int oldRanking = ((Integer) pluginRankings.get(serviceId)).intValue();
        if (ranking != oldRanking) {
            removePluginReference(serviceId, oldRanking);
            pluginRankings.put(serviceId, rankingProperty);
            insertPluginReference(serviceReference, ranking);
        }
    }

    /**
     * Unregisters a ConfigurationPlugin
     *
     * @param serviceReference The reference to the managed service
     */
    public synchronized void unregisteredConfigurationPlugin(ServiceReference serviceReference) {
        Object rankingProperty = serviceReference.getProperty(ConfigurationPlugin.CM_RANKING);
        if (rankingProperty == null) {
            rankingProperty = new Integer(0);
        } else if (rankingProperty.getClass() != Integer.class) {
            rankingProperty = new Integer(0);
        }

        Long serviceId = (Long) serviceReference.getProperty(Constants.SERVICE_ID);
        if (serviceId == null) {
            LOG.error("Missing service id for a ConfigurationPlugin");
            return;
        }

        int ranking = ((Integer) rankingProperty).intValue();
        pluginRankings.remove(serviceId);
        removePluginReference(serviceId, ranking);
    }

    public synchronized Configuration createFactoryConfiguration(String factoryPid, String location, boolean checkLocation)
            throws IOException {
        if (checkLocation) {
            ConfigurationDictionary dictionary = configurationDictionaryManager.loadOne(factoryPid);

            String locationFactoryPidIsBoundTo = null;
            if (dictionary != null && dictionary.size() > 0) {
                locationFactoryPidIsBoundTo = dictionary.getLocation();
            }
            if (locationFactoryPidIsBoundTo != null && !location.equals(locationFactoryPidIsBoundTo)) {
                throw new SecurityException("Not owner of the factoryPid");
            }
        }
        // TODO should check and ensure availability
        String pid = factoryPid + "-" + UUID.randomUUID(8);
        return new ConfigurationImpl(this, location, factoryPid, pid);
    }

    public synchronized Configuration getConfiguration(String servicePid, String location, boolean checkLocation)
            throws IOException {
        ConfigurationDictionary dictionary = configurationDictionaryManager.load(servicePid);
        if (checkLocation) {
            String locationPidIsBoundTo = null;
            if (dictionary != null && dictionary.size() > 0) {
                locationPidIsBoundTo = dictionary.getLocation();
            }
            if (locationPidIsBoundTo != null && !location.equals(locationPidIsBoundTo)) {
                throw new SecurityException("Not owner of the servicePid");
            }
        }
        ConfigurationImpl configuration = null;
        if (dictionary == null) {
            configuration = new ConfigurationImpl(this, location, null, servicePid);
        } else {
            configuration = new ConfigurationImpl(this, dictionary);
        }
        return configuration;
    }

    /**
     * Gets all configurations that matches the given filter
     *
     * @param filterString The filter to match
     * @param callingBundleLocation The location from which the list was done (could be null)
     * @param checkLocation Whether to check the location
     * @return An array of matching configurations
     */
    public synchronized Configuration[] listConfigurations(String filterString, String callingBundleLocation,
            boolean checkLocation) throws IOException, InvalidSyntaxException {
        List matchingConfigurations = getConfigurations(filterString, callingBundleLocation, checkLocation);
        Configuration[] configurations = null;
        if (matchingConfigurations.size() > 0) {
            configurations = new Configuration[matchingConfigurations.size()];
            configurations = (Configuration[]) matchingConfigurations.toArray(configurations);
        }
        return configurations;
    }

    public synchronized void update(String factoryPid, String servicePid, ConfigurationDictionary dictionary)
            throws IOException {
        configurationDictionaryManager.save(servicePid, dictionary);
        ConfigurationDictionary newDictionary = configurationDictionaryManager.load(servicePid);
        updateTargetServicesMatching(servicePid, factoryPid, newDictionary.getLocation(), newDictionary);

        if (cmRef == null) {
            LOG.error("update: Could not get service reference for event delivery");
        } else {
            sendConfigurationEvent(new ConfigurationEvent(cmRef, ConfigurationEvent.CM_UPDATED, factoryPid, servicePid));
        }
    }

    /**
     * Deletes the configuration from configuration admin
     *
     * @param configuration Configuration to delete
     * @throws IOException If any problem occurs while deleting the configuration
     */
    public synchronized void delete(ConfigurationImpl configuration, String servicePid) throws IOException {
        ConfigurationDictionary dictionary = configurationDictionaryManager.load(servicePid);
        String factoryPid = (String) dictionary.get(ConfigurationAdmin.SERVICE_FACTORYPID);
        String location = dictionary.getLocation();

        configurationDictionaryManager.delete(servicePid);

        dictionary = configurationDictionaryManager.load(servicePid);
        updateTargetServicesMatching(servicePid, factoryPid, location, dictionary);

        if (cmRef == null) {
            LOG.error("delete: Could not get service reference");
        } else {
            sendConfigurationEvent(new ConfigurationEvent(cmRef, (dictionary != null) ? ConfigurationEvent.CM_UPDATED
                    : ConfigurationEvent.CM_DELETED, factoryPid, servicePid));
        }
    }

    /**
     * Call all applicable ConfigurationPlugins given a pid and a dictionary.
     *
     * @param dictionary The configuration dictionary to be modified.
     * @return The description of what the method returns.
     */
    public synchronized ConfigurationDictionary callPlugins(ServiceReference targetServiceReference, String servicePid,
            ConfigurationDictionary dictionary) {
        callPlugins(targetServiceReference, servicePid, dictionary, preModificationPlugins, false);

        dictionary = callPlugins(targetServiceReference, servicePid, dictionary, modifyingPlugins, true);

        callPlugins(targetServiceReference, servicePid, dictionary, postModificationPlugins, false);

        return dictionary;
    }

    public synchronized void setBundleLocation(String servicePid, String factoryPid, String location,
            ConfigurationDictionary dictionary) {
        String currentLocation = dictionary.getLocation();
        boolean currentDynamic = dictionary.isDynamic();
        String newLocation = location;
        boolean newDynamic = (location != null);

        if (location == null) {
            dictionary.setLocation(null);
            ServiceReference[] serviceReferences = null;
            if (factoryPid == null) {
                serviceReferences = getTargetServiceReferences(ManagedService.class, servicePid);
            } else {
                serviceReferences = getTargetServiceReferences(ManagedServiceFactory.class, factoryPid);
            }
            if (serviceReferences != null && serviceReferences.length > 0) {
                newLocation = serviceReferences[0].getBundle().getLocation();
                dictionary.setLocation(newLocation);
            }
            dictionary.setDynamic(true);
        } else {
            dictionary.setLocation(location);
            dictionary.setDynamic(false);
        }
        if (EqualsUtils.isNotEqual(currentLocation, newLocation) || currentDynamic == newDynamic) {
            try {
                update(factoryPid, servicePid, dictionary);
            } catch (IOException e) {
                LOG.error("Cannot update configuration");
                dictionary.setLocation(currentLocation);
                dictionary.setDynamic(currentDynamic);
            }
        }
    }

    public synchronized void delete(String servicePid) {
        configurationDictionaryManager.delete(servicePid);
    }

    public synchronized void stop() {
        asyncDispatcher.stop();
    }

    private String[] getServicePids(ServiceReference serviceReference) {
        Object prop = serviceReference.getProperty(Constants.SERVICE_PID);
        if (prop == null) {
            return null;
        } else if (prop instanceof String) {
            return new String[] { (String) prop };
        } else if (prop instanceof String[]) {
            return (String[]) prop;
        } else if (prop instanceof Collection) {
            return (String[]) ((Collection) prop).toArray(new String[((Collection) prop).size()]);
        } else {
            return new String[0];
        }
    }

    private void sendConfigurationEvent(ConfigurationEvent event) {
        ServiceReference[] serviceReferences = null;

        try {
            serviceReferences = context.getServiceReferences(ConfigurationListener.class.getName(), null);
        } catch (InvalidSyntaxException ignored) {
        }

        if (serviceReferences != null) {
            // we have listeners
            asyncDispatcher.send(new ListenerEvent(context, serviceReferences, event));
        }
    }

    private void sendUpdateEvent(ServiceReference serviceReference, String servicePid, String factoryPid,
            ConfigurationDictionary dictionary) {
        asyncDispatcher.send(new UpdateEvent(this, context, serviceReference, servicePid, factoryPid, dictionary));
    }

    private boolean isUnrealBundleLocation(String bundleLocation) {
        return bundleLocation != null && !bundleLocations.contains(bundleLocation);
    }

    private ConfigurationDictionary bindLocationIfNecessary(ServiceReference[] servicePids, ConfigurationDictionary dictionary)
            throws IOException {
        if (servicePids == null || servicePids.length == 0) {
            return dictionary;
        }
        String configLocation = dictionary.getLocation();
        LOG.info(ArrayUtils.toString(servicePids, "[", "]", ", "));
        if (configLocation != null) {
            LOG.info(configLocation + " " + configLocation.getClass());
        }
        Enumeration e = dictionary.keys();
        while (e.hasMoreElements()) {
            String key = (String) e.nextElement();
            LOG.info(key + " -> " + dictionary.get(key));
        }

        if (isUnrealBundleLocation(configLocation)) {
            if (dictionary.isDynamic()) {
                configLocation = null;
                dictionary.setLocation(null);
            }
        }

        if (configLocation == null) {
            String pid = (String) dictionary.get(Constants.SERVICE_PID);
            String serviceLocation = servicePids[0].getBundle().getLocation();

            dictionary.setLocation(serviceLocation);
            configurationDictionaryManager.save(pid, dictionary);
        }
        return dictionary;
    }

    private ServiceReference[] filterOnMatchingLocations(ServiceReference[] serviceReferences, String configLocation) {
        if (serviceReferences.length == 1) {
            String serviceLocation = serviceReferences[0].getBundle().getLocation();
            if (locationsMatch(serviceLocation, configLocation)) {
                return serviceReferences;
            }
            LOG.error("The bundle " + serviceLocation + " has registered a ManagedService(Factory) for a pid bound to "
                    + configLocation);
            return new ServiceReference[0];
        }
        List list = new ArrayList();
        for (int i = 0; i < serviceReferences.length; ++i) {
            String serviceLocation = serviceReferences[i].getBundle().getLocation();
            if (locationsMatch(serviceLocation, configLocation)) {
                list.add(serviceReferences[i]);
            } else {
                LOG.error("The bundle " + serviceLocation + " has registered a ManagedService(Factory) for a pid bound to "
                        + configLocation);
            }
        }
        ServiceReference[] matching = new ServiceReference[list.size()];
        matching = (ServiceReference[]) list.toArray(matching);
        return matching;
    }

    private boolean locationsMatch(String serviceLocation, String configLocation) {
        if (configLocation == null) {
            return false;
        } else if (configLocation.equals(serviceLocation)) {
            return true;
        } else {
            return false;
        }
    }

    private void registerPidsAndCheck(ServiceReference serviceReference, String[] servicePids) {
        for (int i = 0; i < servicePids.length; ++i) {
            String servicePid = servicePids[i];
            List/* <ServiceReference> */serviceReferences = (List) pidToServiceReferences.get(servicePid);
            if (serviceReferences != null) {
                LOG.error("Multiple ManagedServices registered with same pid (" + servicePid + "): "
                        + ((ServiceReference) serviceReferences.get(0)).getBundle().getLocation() + " and "
                        + serviceReference.getBundle().getLocation());
            } else {
                serviceReferences = new ArrayList/* <ServiceReference> */();
                pidToServiceReferences.put(servicePid, serviceReferences);
            }
            serviceReferences.add(serviceReference);
        }
    }

    private void unregisterPids(ServiceReference serviceReference) {
        Iterator/* <Entry<String, List<ServiceReference>>> */it = pidToServiceReferences.entrySet().iterator();
        Entry/* <String, List<ServiceReference>> */entry = null;
        List/* <ServiceReference> */serviceReferences = null;
        List/* <String> */toRemove = new ArrayList();
        while (it.hasNext()) {
            entry = (Entry) it.next();
            serviceReferences = (List) entry.getValue();
            if (serviceReferences.remove(serviceReference)) {
                if (serviceReferences.size() == 0) {
                    toRemove.add(entry.getKey());
                }
            }
        }
        Iterator/* <String> */toRemoveIt = toRemove.iterator();
        while (toRemoveIt.hasNext()) {
            pidToServiceReferences.remove(toRemoveIt.next());
        }
    }

    private void updateTargetServicesMatching(String servicePid, String factoryPid, String location,
            ConfigurationDictionary dictionary) throws IOException {
        if (factoryPid == null) {
            updateManagedServicesMatching(servicePid, location, dictionary);
        } else {
            updateManagedServiceFactoriesMatching(servicePid, factoryPid, location, dictionary);
        }
    }

    private void updateManagedServiceFactoriesMatching(String servicePid, String factoryPid, String bundleLocation,
            ConfigurationDictionary dictionary) throws IOException {
        ServiceReference[] srs = getTargetServiceReferences(ManagedServiceFactory.class, factoryPid);
        if (dictionary == null) {
            updateManagedServiceFactories(srs, servicePid, factoryPid, bundleLocation);
        } else {
            updateManagedServiceFactories(srs, servicePid, factoryPid, dictionary);
        }
    }

    private void updateManagedServiceFactories(ServiceReference[] srs, String servicePid, String factoryPid,
            ConfigurationDictionary cd) throws IOException {
        ConfigurationDictionary bound = bindLocationIfNecessary(srs, cd);
        String boundLocation = bound.getLocation();
        ServiceReference[] filtered = filterOnMatchingLocations(srs, boundLocation);
        for (int i = 0; i < filtered.length; ++i) {
            sendUpdateEvent(filtered[i], servicePid, factoryPid, bound);
        }
    }

    private void updateManagedServiceFactories(ServiceReference[] serviceReferences, String servicePid, String factoryPid,
            String boundLocation) {
        ServiceReference[] filtered = filterOnMatchingLocations(serviceReferences, boundLocation);
        for (int i = 0; i < filtered.length; ++i) {
            sendUpdateEvent(filtered[i], servicePid, factoryPid, null);
        }
    }

    private void updateManagedServiceFactory(ServiceReference serviceReference, String[] factoryPids) throws IOException {
        for (int j = 0; j < factoryPids.length; ++j) {
            String factoryPid = factoryPids[j];
            List/* <ConfigurationDictionary> */cds = configurationDictionaryManager.loadAll(factoryPid);
            if (cds != null && cds.size() > 0) {
                ServiceReference[] srs = new ServiceReference[] { serviceReference };
                Iterator/* <ConfigurationDictionary> */it = cds.iterator();
                ConfigurationDictionary dictionary = null;
                while (it.hasNext()) {
                    dictionary = (ConfigurationDictionary) it.next();
                    String servicePid = (String) dictionary.get(Constants.SERVICE_PID);
                    updateManagedServiceFactories(srs, servicePid, factoryPid, dictionary);
                }
            }
        }
    }

    private void updateManagedServicesMatching(String servicePid, String bundleLocation, ConfigurationDictionary dictionary)
            throws IOException {
        ServiceReference[] srs = getTargetServiceReferences(ManagedService.class, servicePid);
        if (dictionary == null) {
            updateManagedServices(srs, servicePid, bundleLocation);
        } else {
            updateManagedServices(srs, servicePid, dictionary);
        }
    }

    private void updateManagedServices(ServiceReference[] serviceReferences, String servicePid, String boundLocation) {
        ServiceReference[] filtered = filterOnMatchingLocations(serviceReferences, boundLocation);
        for (int i = 0; i < filtered.length; ++i) {
            sendUpdateEvent(filtered[i], servicePid, null, null);
        }
    }

    private void updateManagedServices(ServiceReference[] serviceReferences, String servicePid,
            ConfigurationDictionary dictionary) throws IOException {
        ConfigurationDictionary bound = bindLocationIfNecessary(serviceReferences, dictionary);
        String boundLocation = bound.getLocation();
        ServiceReference[] filtered = filterOnMatchingLocations(serviceReferences, boundLocation);
        for (int i = 0; i < filtered.length; ++i) {
            sendUpdateEvent(filtered[i], servicePid, null, bound);
        }
    }

    private void updateManagedService(ServiceReference serviceReference, String[] servicePids) throws IOException {
        for (int j = 0; j < servicePids.length; ++j) {
            String servicePid = servicePids[j];
            ServiceReference[] srs = new ServiceReference[] { serviceReference };
            ConfigurationDictionary cd = configurationDictionaryManager.load(servicePid);
            if (cd == null) {
                for (int i = 0; i < srs.length; ++i) {
                    sendUpdateEvent(srs[i], servicePid, null, null);
                }
            } else {
                cd = bindLocationIfNecessary(srs, cd);
                String boundLocation = cd.getLocation();
                srs = filterOnMatchingLocations(srs, boundLocation);
                for (int i = 0; i < srs.length; ++i) {
                    sendUpdateEvent(srs[i], servicePid, null, cd);
                }
            }
        }
    }

    private ServiceReference[] getTargetServiceReferences(Class serviceClass, String servicePid) {
        String filter = "(" + Constants.SERVICE_PID + "=" + servicePid + ")";
        try {
            ServiceReference[] srs = context.getServiceReferences(serviceClass.getName(), filter);
            return srs == null ? new ServiceReference[0] : srs;
        } catch (InvalidSyntaxException e) {
            LOG.error("Faulty ldap filter " + filter, e);
            return new ServiceReference[0];
        }
    }

    /**
     * * Insert a ServiceReference to a ConfigurationPlugin in the correct * list based on its ranking. * *
     *
     * @param serviceReference The ServiceReference. *
     * @param ranking The ranking the ServiceReference.
     */
    private void insertPluginReference(ServiceReference serviceReference, int ranking) {
        if (ranking < 0) {
            insertPluginReference(serviceReference, ranking, preModificationPlugins);
        } else if (0 <= ranking && ranking <= 1000) {
            insertPluginReference(serviceReference, ranking, modifyingPlugins);
        } else {
            insertPluginReference(serviceReference, ranking, postModificationPlugins);
        }
    }

    /**
     * * Insert a ServiceReference in a list sorted on cm.ranking property. * *
     *
     * @param serviceReference The ServiceReference. *
     * @param pluginsList The list.
     */
    private void insertPluginReference(ServiceReference serviceReference, int ranking, List pluginsList) {
        int i;
        for (i = 0; i < pluginsList.size(); ++i) {
            ServiceReference nextReference = (ServiceReference) pluginsList.get(i);
            Long serviceId = (Long) nextReference.getProperty(Constants.SERVICE_ID);
            Integer rankingOfNextReference = (Integer) pluginRankings.get(serviceId);
            if (ranking < rankingOfNextReference.intValue()) {
                break;
            }
        }
        pluginsList.set(i, serviceReference);
    }

    /**
     * * Remove a ServiceReference to a ConfigurationPlugin given * a service.id and a ranking. * *
     *
     * @param serviceId The service.id of the ConfigurationPlugin. *
     * @param ranking The ranking of the ConfigurationPlugin.
     */
    private void removePluginReference(Object serviceId, int ranking) {
        if (ranking < 0) {
            removePluginReference(serviceId, preModificationPlugins);
        } else if (0 <= ranking && ranking <= 1000) {
            removePluginReference(serviceId, modifyingPlugins);
        } else {
            removePluginReference(serviceId, postModificationPlugins);
        }
    }

    private void removePluginReference(Object serviceId, List pluginsList) {
        for (int i = 0; i < pluginsList.size(); ++i) {
            ServiceReference serviceReference = (ServiceReference) pluginsList.get(i);
            Long currentId = (Long) serviceReference.getProperty(Constants.SERVICE_ID);
            if (currentId.equals(serviceId)) {
                pluginsList.remove(i);
                return;
            }
        }
    }

    /**
     * * Call all plugins contained in a list and * optionally allow modifications. * *
     *
     * @param targetServiceReference Reference to the target ManagedService(Factory). *
     * @param servicePid The service PID corresponding to the current configuration
     * @param dictionary The configuration dictionary to process. *
     * @param plugins list of references to ConfigurationPlugins. *
     * @param allowModification Should modifications to the configuration dictionary be allowed. * *
     * @return The modified configuration dictionary.
     */
    private ConfigurationDictionary callPlugins(ServiceReference targetServiceReference, String servicePid,
            ConfigurationDictionary dictionary, List plugins, boolean allowModification) {
        ConfigurationDictionary currentDictionary = dictionary;
        Iterator it = plugins.iterator();
        while (it.hasNext()) {
            ServiceReference pluginReference = (ServiceReference) it.next();

            // Only call the plugin if no cm.target is specified or if it
            // matches the pid of the target service
            String cmTarget = (String) pluginReference.getProperty(ConfigurationPlugin.CM_TARGET);
            if (cmTarget == null || cmTarget.equals(servicePid)) {
                ConfigurationPlugin plugin = (ConfigurationPlugin) context.getService(pluginReference);
                if (plugin == null) {
                    continue;
                }
                ConfigurationDictionary dictionaryCopy = new ConfigurationDictionary(dictionary);
                try {
                    plugin.modifyConfiguration(targetServiceReference, dictionaryCopy);
                    if (allowModification) {
                        currentDictionary = dictionaryCopy;
                    }
                } catch (Exception exception) {
                    LOG.error("[CM] Exception thrown by plugin: " + exception.getMessage());
                }
            }
        }
        return currentDictionary;
    }

    private List/* <Configuration> */getConfigurations(String filterString, String callingBundleLocation, boolean checkLocation)
            throws IOException, InvalidSyntaxException {
        Filter filter = (filterString != null) ? context.createFilter(filterString) : null;
        Iterator/* <String> */pids = configurationDictionaryManager.listPids();
        List/* <Configuration> */matchingConfigurations = new ArrayList();
        while (pids.hasNext()) {

            String pid = (String) pids.next();
            ConfigurationDictionary config = configurationDictionaryManager.load(pid);
            if (filter == null) {
                matchingConfigurations.add(new ConfigurationImpl(this, config));
            } else {
                if (filter.matchCase(config.toSearchableProperties())) {
                    if (!checkLocation) {
                        matchingConfigurations.add(new ConfigurationImpl(this, config));
                    } else {
                        if (callingBundleLocation.equals(config.getLocation())) {
                            matchingConfigurations.add(new ConfigurationImpl(this, config));
                        }
                    }
                }
            }
        }
        return matchingConfigurations;
    }
}
