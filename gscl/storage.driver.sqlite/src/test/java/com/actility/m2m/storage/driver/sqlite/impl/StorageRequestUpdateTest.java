package com.actility.m2m.storage.driver.sqlite.impl;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.List;

import junit.framework.TestCase;

import org.osgi.framework.BundleContext;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;

public class StorageRequestUpdateTest extends TestCase {

    private static String PATH_M2M_APPLICATIONS = "/m2m/applications";
    private static String PATH_M2M_ACCESSRIGHT = "/m2m/accessRight";
    private static String PATH_M2M_ACCESSRIGHTS = "/m2m/accessRights";
    private static String PATH_M2M_ACCESSRIGHTS_LOCADMINAR = PATH_M2M_ACCESSRIGHTS + "/Locadmin_AR";
    private static String PATH_M2M_APPLICATIONS_OBIX = PATH_M2M_APPLICATIONS + "/obix";
    private static String PATH_M2M_APPLICATIONS_ACCESSRIGHT = PATH_M2M_APPLICATIONS + "/accessRight";
    private static String PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1 = PATH_M2M_APPLICATIONS_ACCESSRIGHT + "/AR_1";
    private static String PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2 = PATH_M2M_APPLICATIONS_ACCESSRIGHT + "/AR_2";
    private static String PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3 = PATH_M2M_APPLICATIONS_ACCESSRIGHT + "/AR_3";
    private static String PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d = PATH_M2M_APPLICATIONS
            + "/subscriptions/23b312a4435bfe2d";

    private static String CONTENT_M2M_APPLICATIONS = "<obj>a collection of applications</obj>";
    private static String CONTENT_M2M_ACCESSRIGHT = "<obj>a collection of access rights</obj>";
    private static String CONTENT_M2M_ACCESSRIGHTS = "<obj>another collection of access rights</obj>";
    private static String CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR = "<obj>an access right</obj>";
    private static String CONTENT_M2M_APPLICATIONS_OBIX = "<xml>0:obix</xml>";
    private static String CONTENT_M2M_APPLICATIONS_ACCESSRIGHT = "<obj>a collection of access rights at a weird place</obj>";
    private static String CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1 = "<xml>1:blah blah blah</xml>";
    private static String CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2 = "<xml>2:bluh blah blah</xml>";
    private static String CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3 = "<xml>3:blih blah blah</xml>";
    private static String CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d = "<obj>a subscription</obj>";

    private static Date CREATION_DATE_M2M_APPLICATIONS = null;
    private static Date CREATION_DATE_M2M_ACCESSRIGHT = null;
    private static Date CREATION_DATE_M2M_ACCESSRIGHTS = null;
    private static Date CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR = null;
    private static Date CREATION_DATE_M2M_APPLICATIONS_OBIX = null;
    private static Date CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT = null;
    private static Date CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1 = null;
    private static Date CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2 = null;
    private static Date CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3 = null;
    private static Date CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d = null;

    private static Integer CTYPES_INT_M2M_APPLICATIONS = new Integer(1);
    private static Integer CTYPES_INT_M2M_ACCESSRIGHT = new Integer(2);
    private static Integer CTYPES_INT_M2M_ACCESSRIGHTS = new Integer(3);
    private static Integer CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR = new Integer(4);
    private static Integer CTYPES_INT_M2M_APPLICATIONS_OBIX = new Integer(5);
    private static Integer CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT = new Integer(6);
    private static Integer CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1 = new Integer(7);
    private static Integer CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2 = new Integer(8);
    private static Integer CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3 = new Integer(9);
    private static Integer CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d = new Integer(10);

    private BundleContext context;
    private StorageRequestDriverExecutorImpl storage;

    private void initConnection() {
        context = new BundleContextImpl();
        File sqliteDb = new File("src/main/target/resources/lib/centos6-x86_64/sqlite.db");
        if (sqliteDb.exists()) {
            System.out.println("Database exists");
            sqliteDb.delete();
        }
        Dictionary/* <String, Object> */config = new Hashtable/* <String, Object> */();
        config.put("fileName", sqliteDb.getPath());
        try {
            storage = new StorageRequestDriverExecutorImpl(config);
        } catch (StorageException e) {
            // TODO Auto-generated catch block
            fail(e.getMessage());
        }
    }

    /*
     * private void resetTables() { }
     */

    private void createDocuments() {

        // Connection conn = null;
        //
        // try {
        // conn = JdbcConnectionManager.getInstance().getConnection();
        // } catch (SQLException e) {
        // fail("Unable to get an active connection " + e);
        // }
        //
        // conn.setAutoCommit(true);

        // mysql> select * from DOCUMENT order by DOCUMENT.PATH;
        // +----+---------------------------------+-----------------+----------------------------------------------------------+
        // |.ID.|.PATH............................|.NAME............|.CONTENT..................................................|
        // +----+---------------------------------+-----------------+----------------------------------------------------------+
        // |.1..|./m2m/...........................|.applications....|.<obj>a collection of applications</obj>..................|
        // |.2..|./m2m/...........................|.accessRight.....|.<obj>a collection of access rights</obj>.................|
        // |.3..|./m2m/...........................|.accessRights....|.<obj>another collection of access rights</obj>...........|
        // |.4..|./m2m/accessRights/..............|.Locadmin_AR.....|.<obj>an access right</obj>...............................|
        // |.5..|./m2m/applications/..............|.obix............|.<xml>0:obix</xml>........................................|
        // |.6..|./m2m/applications/..............|.accessRight.....|.<obj>a collection of access rights at a weird place</obj>|
        // |.7..|./m2m/applications/accessRight/..|.AR_1............|.<xml>1:blah blah blah</xml>..............................|
        // |.8..|./m2m/applications/accessRight/..|.AR_2............|.<xml>2:bluh blah blah</xml>..............................|
        // |.9..|./m2m/applications/accessRight/..|.AR_3............|.<xml>3:bluh blah blah</xml>..............................|
        // |.10.|./m2m/applications/subscriptions/|.23b312a4435bfe2d|.<obj>a subscription</obj>................................|
        // +----+---------------------------------+-----------------+----------------------------------------------------------+

        // create "/m2m/applications"
        Document doc = new DocumentImpl(null, PATH_M2M_APPLICATIONS);
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        CREATION_DATE_M2M_APPLICATIONS = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_APPLICATIONS.length()));
        List/**/contentTypes = new ArrayList/**/();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_APPLICATIONS);
        contentTypes.add(CREATION_DATE_M2M_APPLICATIONS);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_APPLICATIONS);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_APPLICATIONS + ": " + e);
        }

        // create "/m2m/accessRight"
        doc = new DocumentImpl(null, PATH_M2M_ACCESSRIGHT);
        doc.setContent(CONTENT_M2M_ACCESSRIGHT.getBytes());
        CREATION_DATE_M2M_ACCESSRIGHT = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_ACCESSRIGHT);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_ACCESSRIGHT.length()));
        contentTypes = new ArrayList/**/();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_ACCESSRIGHT);
        contentTypes.add(CREATION_DATE_M2M_ACCESSRIGHT);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_ACCESSRIGHT);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_ACCESSRIGHT + ": " + e);
        }

        // create "/m2m/accessRights"
        doc = new DocumentImpl(null, PATH_M2M_ACCESSRIGHTS);
        doc.setContent(CONTENT_M2M_ACCESSRIGHTS.getBytes());
        CREATION_DATE_M2M_ACCESSRIGHTS = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_ACCESSRIGHTS);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_ACCESSRIGHTS.length()));
        contentTypes = new ArrayList/**/();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_ACCESSRIGHTS);
        contentTypes.add(CREATION_DATE_M2M_ACCESSRIGHTS);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_ACCESSRIGHTS);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_ACCESSRIGHTS + ": " + e);
        }

        // /m2m/accessRights/Locadmin_AR
        doc = new DocumentImpl(null, PATH_M2M_ACCESSRIGHTS_LOCADMINAR);
        doc.setContent(CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR.getBytes());
        CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR.length()));
        contentTypes = new ArrayList();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR);
        contentTypes.add(CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_ACCESSRIGHTS_LOCADMINAR);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_ACCESSRIGHTS_LOCADMINAR + ": " + e);
        }

        // /m2m/applications/obix
        doc = new DocumentImpl(null, PATH_M2M_APPLICATIONS_OBIX);
        doc.setContent(CONTENT_M2M_APPLICATIONS_OBIX.getBytes());
        CREATION_DATE_M2M_APPLICATIONS_OBIX = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS_OBIX);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_APPLICATIONS_OBIX.length()));
        contentTypes = new ArrayList();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_APPLICATIONS_OBIX);
        contentTypes.add(CREATION_DATE_M2M_APPLICATIONS_OBIX);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_APPLICATIONS_OBIX);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_APPLICATIONS_OBIX + ": " + e);
        }

        // /m2m/applications/accessRight
        doc = new DocumentImpl(null, PATH_M2M_APPLICATIONS_ACCESSRIGHT);
        doc.setContent(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT.getBytes());
        CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT.length()));
        contentTypes = new ArrayList();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        contentTypes.add(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_APPLICATIONS_ACCESSRIGHT + ": " + e);
        }

        // /m2m/applications/accessRight/AR_1
        doc = new DocumentImpl(null, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        doc.setContent(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1.getBytes());
        CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1 = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1.length()));
        contentTypes = new ArrayList();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        contentTypes.add(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1 + ": " + e);
        }

        // /m2m/applications/accessRight/AR_2
        doc = new DocumentImpl(null, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        doc.setContent(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2.getBytes());
        CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2 = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2.length()));
        contentTypes = new ArrayList();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        contentTypes.add(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2 + ": " + e);
        }

        // /m2m/applications/accessRight/AR_3
        doc = new DocumentImpl(null, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        doc.setContent(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3.getBytes());
        CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3 = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3.length()));
        contentTypes = new ArrayList();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        contentTypes.add(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3 + ": " + e);
        }

        // /m2m/applications/subscrptions/23b312a4435bfe2d
        doc = new DocumentImpl(null, PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        doc.setContent(CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d.getBytes());
        CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d = new Date();
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        doc.setAttribute("contentSize", new Integer(CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d.length()));
        contentTypes = new ArrayList();
        contentTypes.add("content type 1");
        contentTypes.add(CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        contentTypes.add(CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        doc.setAttribute("contentTypes", contentTypes);
        doc.setAttribute("link", "http://invalid.invalid" + PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        doc.setAttribute(Document.ATTR_NOTIFY_WHEN_DELETED, new Integer(1));

        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation failed on " + PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d + ": " + e);
        }

    }

    public void setUp() {

        initConnection();

        // resetTables();
        createDocuments();
    }

    public void tearDown() {

    }


//     public void testMultiThreadCreate() {
//         System.out.println("======================== testMultiThreadCreate ========================");
//         LinkedList threads = new LinkedList();
//         for (int i = 0; i < 10; i++) {
//             RunnableMain run = new RunnableMain(storage.sqlite);
//             Thread thread = new Thread(run);
//             thread.setName("TestThread:" + i);
//             thread.start();
//             threads.add(thread);
//         }
//         for (int i = 0; i < threads.size(); i++) {
//             try {
//                 ((Thread) threads.get(i)).join();
//             } catch (InterruptedException e) {
//                 fail(e.getMessage()); } // run.run(); }
//         }
//     }

//     public void testMultiThreadRetrieve() {
//         System.out.println("======================== testMultiThreadCreate ========================");
//         LinkedList threads = new LinkedList();
//         for (int i = 0; i < 50; i++) {
//             Runnable run = new Runnable() {
//                 public void run() {
//                     try { SearchResult result = storage.search(null, "", StorageRequestExecutor.SCOPE_SUB_TREE, null, StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
//                     } catch (StorageException e) {
//                         fail(e.getMessage());
//                     }
//                 }
//             };
//             Thread thread = new Thread(run);
//             thread.setName("TestThread:" + i);
//             thread.start();
//             threads.add(thread);
//         }
//         for (int i = 0; i < threads.size(); i++) {
//             try {
//                 ((Thread) threads.get(i)).join();
//             } catch (InterruptedException e) {
//                 fail(e.getMessage());
//             }
//         }
//     }

    public void testDeleteOneLevelDocumentsWithConditions1() {
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        // delete "/m2m/applications" using scope oneLevel (i.e. delete /m2m/applications/obix and /m2m/applications/accessRight
        // as well as the records in ATTRIBUTE table that comes with these two documents)
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_GREATER_OR_EQUAL, new Integer(1));
        // condition2 is not met
        Condition condition2 = new ConditionImpl("creationTime", Condition.ATTR_OP_NOT_EQUAL, CREATION_DATE_M2M_APPLICATIONS);
        Condition condition3 = new ConditionImpl("contentTypes", Condition.ATTR_OP_STARTS_WITH, "content");
        Condition superCond1 = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, superCond1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }

        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

//    public void testMultiThreadAll() {
//        System.out.println("======================== testMultiThreadCreate ========================");
//        LinkedList threads = new LinkedList();
//        for (int i = 0; i < 50; i++) {
//            Runnable run = new Runnable() {
//                public void run() {
//                    String docName = "/m2m/applications/APP_0000000" + Thread.currentThread().getName();
//                    Document doc = new DocumentImpl(null, docName);
//                    doc.setAttribute("creationTime", new Date());
//                    switch ((int)(Math.random() * (5))) {
//                    case 0:
//                        // CREATE
//                        try {
//                            storage.create(null, doc);
//                        } catch (StorageException e) {
//                            fail(e.getMessage());
//                        }
//                        break;
//                    case 1:
//                        // UPDATE
//                        try {
//                            storage.update(null, doc, null);
//                        } catch (StorageException e) {
//                            fail(e.getMessage());
//                        }
//                        break;
//                    case 2:
//                        // DELETE
//                        try {
//                            storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null);
//                        } catch (StorageException e) {
//                            fail(e.getMessage());
//                        }
//                        break;
//                    case 3:
//                        // SEARCH
//                        System.out.println("#################Search################"+Thread.currentThread().getName());
//                        try {
//                            storage.search(null, "", StorageRequestExecutor.SCOPE_SUB_TREE, null, StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, false, null);
//                        } catch (StorageException e) {
////                            fail(e.getMessage());
//                        }
//                        break;
//                    case 4:
//                        // RETRIEVE
//                        System.out.println("#################Retrieve################+"+Thread.currentThread().getName());
//                        try {
//                            storage.retrieve(null, docName, null);
//                        } catch (StorageException e) {
//                            fail(e.getMessage());
//                        }
//                        break;
//                    default:
//                        break;
//                    }
//                }
//            };
//            Thread thread = new Thread(run);
//            thread.setName("TestThread:" + i);
//            thread.start();
//            threads.add(thread);
//        }
//        for (int i = 0; i < threads.size(); i++) {
//            try {
//                ((Thread) threads.get(i)).join();
//            } catch (InterruptedException e) {
//                // TODO Auto-generated catch block
//                fail(e.getMessage());
//            }
//            // run.run();
//        }
//    }

}
