/**
 *
 */
package com.actility.m2m.storage.driver.sqlite.impl;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.NoSuchElementException;

import junit.framework.TestCase;

import org.osgi.framework.BundleContext;

import com.actility.m2m.storage.AttributeOperation;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;

/**
 * @author mlouiset
 */
public class StorageRequestDriverExecutorImplTest extends TestCase {

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

    // this is not a test
    public static void controlDoc(Document doc, String path, String content, Date date, Integer contentTypeInt) {
        assertEquals(path, doc.getPath());
        assertEquals(content, new String(doc.getContent()));
        assertEquals(new Integer(content.length()), doc.getIntegerAttribute("contentSize"));
        assertEquals(date, doc.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + path, doc.getStringAttribute("link"));
        List attributes = doc.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(date));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(contentTypeInt));
    }

    private void controlDocIsM2mApplications(Document doc) {
        controlDoc(doc, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);
    }

    /**
     * connect to the database and reset it.
     */
    private void initConnection() {
        context = new BundleContextImpl();
        File sqliteDb = new File("target/sqlite.db");
        if (sqliteDb.exists()) {
            sqliteDb.delete();
        }
        Dictionary/* <String, Object> */config = new Hashtable/* <String, Object> */();
        config.put("fileName", sqliteDb.getPath());
        try {
            storage = new StorageRequestDriverExecutorImpl(config);
        } catch (StorageException e) {
            fail(e.getMessage());
        }
    }

    private void createDocuments() {
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

        createDocuments();
    }

    public void tearDown() {

    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // CREATE
    public void testCreateDocumentNoDocument() {
        try {
            storage.create(null, null);
            fail("create operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testCreateDocumentNoPath() {
        Document doc = new Document() {

            public Object getId() {
                return null;
            }

            public String getPath() {
                return null;
            }

            public byte[] getContent() {
                return null;
            }

            public void setContent(byte[] content) {

            }

            public Object getAttribute(String name) {
                return null;
            }

            public Integer getIntegerAttribute(String name) {
                return null;
            }

            public Date getDateAttribute(String name) {
                return null;
            }

            public String getStringAttribute(String name) {
                return null;
            }

            public List getListAttribute(String name) {
                return null;
            }

            public void setAttribute(String name, Object value) {

            }

            public Object removeAttribute(String name) {
                return null;
            }

            public Iterator getAttributes() {
                return null;
            }

        };
        try {
            storage.create(null, doc);
            fail("create operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testCreateEmptyPath() {

        String docName = "";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        // the document is a special one, this should succeed
        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation on " + docName + " failed");
        }
    }

    public void testCreateWrongPath1() {

        String docName = "pathWithoutSlash";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        try {
            storage.create(null, doc);
            fail("create operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testCreateWrongPath2() {

        String docName = "/path/With/Slash/At/End/";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        try {
            storage.create(null, doc);
            fail("create operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testCreateLongName() throws SQLException {

        String docName = "/a/path/aNameThatIsTooLongToBeInsertedIntoTheDatabaseAndShouldThenRaiseAnException"
                + "ANameThatIsTooLongToBeInsertedIntoTheDatabaseAndShouldThenRaiseAnException"
                + "ANameThatIsTooLongToBeInsertedIntoTheDatabaseAndShouldThenRaiseAnException";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        try {
            assertTrue(storage.create(null, doc));
            Document temp = storage.retrieve(null, docName, null);
            assertNotNull(temp);
            // fail("create operation succeed, but should not");
        } catch (StorageException e) {
            // assertNotNull(e);
            fail(e.getMessage());
        }
    }

    public void testCreateLongPath() throws SQLException {

        String docName = "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "/a/path/that/is/too/long/to/be/inserted/into/the/database/and/should/then/raise/an/exception/"
                + "documentName";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        try {
            assertTrue(storage.create(null, doc));
            Document temp = storage.retrieve(null, docName, null);
            assertNotNull(temp);
            // fail("create operation succeed, but should not");
        } catch (StorageException e) {
            // assertNotNull(e);
            fail(e.getMessage());
        }
    }

    public void testCreateUnsupportedAttrType() {

        String docName = "/m2m/otherResource";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("invalidAttr", new Float(1.2));
        doc.setContent("a content".getBytes());
        // the document is a special one, this should succeed
        try {
            storage.create(null, doc);
            fail("create operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testCreateUnsupportedAttrTypeInList() {

        String docName = "/m2m/otherResource";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("invalidAttr", new Integer(1));
        doc.setAttribute("invalidAttr", new Float(1.2));
        doc.setContent("a content".getBytes());
        // the document is a special one, this should succeed
        try {
            storage.create(null, doc);
            fail("create operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testCreateDocument1() {

        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        // the document already exist, this should fail
        try {
            storage.create(null, doc);
        } catch (StorageException e) {
            fail("create operation on " + docName + " failed: " + e);
            // assertTrue(e.getMessage().contains("Duplicate"));
        }
    }

    public void testCreateDocument2() {

        String docName = "/m2m/applications/APP_00000001";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());

        // the document only needs a path and an attribute to be created, the content may be null
        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation on " + docName + " failed: " + e);
        }
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // DELETE

    public void testDeleteExactNoDocument() {
        try {
            storage.delete(null, null, null);
            fail("delete (no scope) operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testDeleteDocumentNoPath() {
        Document doc = new Document() {

            public Object getId() {
                return null;
            }

            public String getPath() {
                return null;
            }

            public byte[] getContent() {
                return null;
            }

            public void setContent(byte[] content) {

            }

            public Object getAttribute(String name) {
                return null;
            }

            public Integer getIntegerAttribute(String name) {
                return null;
            }

            public Date getDateAttribute(String name) {
                return null;
            }

            public String getStringAttribute(String name) {
                return null;
            }

            public List getListAttribute(String name) {
                return null;
            }

            public void setAttribute(String name, Object value) {

            }

            public Object removeAttribute(String name) {
                return null;
            }

            public Iterator getAttributes() {
                return null;
            }

        };
        try {
            storage.delete(null, doc, null);
            fail("delete operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testDeleteWrongPath1() {

        String docName = "pathWithoutSlash";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        try {
            storage.delete(null, doc, null);
            fail("delete operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testDeleteWrongPath2() {

        String docName = "/path/With/Slash/At/End/";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent(CONTENT_M2M_APPLICATIONS.getBytes());
        try {
            storage.delete(null, doc, null);
            fail("delete operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testDeleteExactDocuments2() {

        // delete "/m2m/resourceDoesNotExist" should fail
        String docName = "/m2m/resourceDoesNotExist";
        Document doc = new DocumentImpl(null, docName);
        try {
            assertFalse(storage.delete(null, doc, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }
    }

    public void testDeleteOneLevelNoDocuments() {
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        try {
            assertTrue(storage.delete(null, null, StorageRequestExecutor.SCOPE_ONE_LEVEL, null));
            fail("delete (scope OneLevel) operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
        deletionHandler.assertNbDeleted(0);
    }

    public void testDeleteOneLevelDocuments1() {
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);

        // delete "/m2m/applications" using scope oneLevel (i.e. delete /m2m/applications/obix and /m2m/applications/accessRight
        // as well as the records in ATTRIBUTE table that comes with these two documents)
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }

        deletionHandler.assertNbDeleted(2);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX,
                CREATION_DATE_M2M_APPLICATIONS_OBIX, CTYPES_INT_M2M_APPLICATIONS_OBIX);

        deletionHandler = new DeletionHandlerImpl();
        // if one try to delete the same doc, it should fail
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, null));
        } catch (StorageException e) {
            fail("exception while trying to delete " + docName + " a second time: " + e);
        }
        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteOneLevelAndDeletionHandler() {

        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        // delete "/m2m/applications/accessRight" using scope oneLevel (i.e. delete /m2m/applications/accessRight/AR_1 and
        // /m2m/applications/accessRight/AR_3 as well as the records in ATTRIBUTE table that comes with these two documents)
        String docName = "/m2m/applications/accessRight";
        Document doc = new DocumentImpl(null, docName);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }

        deletionHandler.assertNbDeleted(3);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        // if one try to delete the same doc, it should fail
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, null));
        } catch (StorageException e) {
            fail("exception while trying to delete " + docName + " a second time: " + e);
        }
        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteSubTreeNoDocuments() {
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        try {
            assertTrue(storage.delete(null, null, StorageRequestExecutor.SCOPE_SUB_TREE, null));
            fail("delete (scope SubTree) operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteSubTreeDocuments1() {
        // System.out.println("BAAAAAAAAAAAAAAAAAAAAAAAAAAAANNNNNNNNNNNNNNSSSSSSSAAAAAAAAAAAAAIIIIIIIIIIIIIIIIIIIIIII");
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        // delete "/m2m/applications" using scope subtree (i.e. delete /m2m/applications*
        // as well as the records in ATTRIBUTE table that comes with these documents)
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }

        deletionHandler.assertNbDeleted(6);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX,
                CREATION_DATE_M2M_APPLICATIONS_OBIX, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        deletionHandler = new DeletionHandlerImpl();
        // if one try to delete the same doc, it should fail
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null));
        } catch (StorageException e) {
            fail("exception while trying to delete " + docName + " a second time: " + e);
        }
        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteAll1() {
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        // delete "/m2m/applications" using scope subtree (i.e. delete /m2m/applications*
        // as well as the records in ATTRIBUTE table that comes with these documents)
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }

        deletionHandler.assertNbDeleted(6);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX,
                CREATION_DATE_M2M_APPLICATIONS_OBIX, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        // delete "/m2m" using scope subtree (i.e. delete /m2m and /m2m/*
        // as well as the records in ATTRIBUTE table that comes with these documents)
        // should also succeed with hole in tree (i.e. /m2m/applications already removed)
        docName = "/m2m";
        doc = new DocumentImpl(null, docName);
        deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }
        deletionHandler.assertNbDeleted(3);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHT, CONTENT_M2M_ACCESSRIGHT, CREATION_DATE_M2M_ACCESSRIGHT,
                CTYPES_INT_M2M_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHTS, CONTENT_M2M_ACCESSRIGHTS, CREATION_DATE_M2M_ACCESSRIGHTS,
                CTYPES_INT_M2M_ACCESSRIGHTS);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHTS_LOCADMINAR, CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR, CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteAll2() {

        // delete "/m2m/applications" using scope subtree (i.e. delete /m2m/*
        // as well as the records in ATTRIBUTE table that comes with these documents)
        String docName = "/m2m";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }
        deletionHandler.assertNbDeleted(10);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHT, CONTENT_M2M_ACCESSRIGHT, CREATION_DATE_M2M_ACCESSRIGHT,
                CTYPES_INT_M2M_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHTS, CONTENT_M2M_ACCESSRIGHTS, CREATION_DATE_M2M_ACCESSRIGHTS,
                CTYPES_INT_M2M_ACCESSRIGHTS);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHTS_LOCADMINAR, CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR, CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX,
                CREATION_DATE_M2M_APPLICATIONS_OBIX, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteAllAndDeletionHandler() {

        // delete "/m2m" using scope subtree (i.e. delete /m2m and /m2m/*
        // as well as the records in ATTRIBUTE table that comes with these documents)
        String docName = "/m2m";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null));
        } catch (StorageException e) {
            fail("delete operation on " + docName + " failed: " + e);
        }
        deletionHandler.assertNbDeleted(10);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHT, CONTENT_M2M_ACCESSRIGHT, CREATION_DATE_M2M_ACCESSRIGHT,
                CTYPES_INT_M2M_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_ACCESSRIGHTS_LOCADMINAR, CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR, CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX,
                CREATION_DATE_M2M_APPLICATIONS_OBIX, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteExactDocumentsWithConditions1() {

        // delete "/m2m/accessRights" (no scope, i.e. only /m2m/accessRights and attributes)
        String docName = "/m2m/accessRights";
        Document doc = new DocumentImpl(null, docName);

        // String parameter "ss" does not exist, the delete should fail
        Condition condition1 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");
        try {
            assertFalse(storage.delete(null, doc, condition1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
    }

    public void testDeleteExactDocumentsWithConditions2() {

        // delete "/m2m/accessRights" (no scope, i.e. only /m2m/accessRights and attributes)
        String docName = "/m2m/accessRights";
        Document doc = new DocumentImpl(null, docName);

        // String parameter "link" exists but does not equal "http://link.invalid", the delete should fail
        Condition condition1 = new ConditionImpl("link", Condition.ATTR_OP_EQUAL, "http://link.invalid");
        try {
            assertFalse(storage.delete(null, doc, condition1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
    }

    public void testDeleteExactDocumentsWithConditions3() {

        // delete "/m2m/accessRights" (no scope, i.e. only /m2m/accessRights and attributes)
        String docName = "/m2m/accessRights";
        Document doc = new DocumentImpl(null, docName);

        // condition2 is met, but not condition1; should fail
        Condition condition1 = new ConditionImpl("link", Condition.ATTR_OP_EQUAL, "http://link.invalid");
        Condition condition2 = new ConditionImpl("contentSize", Condition.ATTR_OP_LOWER_OR_EQUAL, new Integer(100));
        Condition superCond1 = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            assertFalse(storage.delete(null, doc, superCond1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
    }

    public void testDeleteExactDocumentsWithConditions4() {

        // delete "/m2m/accessRights" (no scope, i.e. only /m2m/accessRights and attributes)
        String docName = "/m2m/accessRights";
        Document doc = new DocumentImpl(null, docName);

        // condition2 is met, but not condition1; should fail
        Condition condition1 = new ConditionImpl("link", Condition.ATTR_OP_EQUAL, "http://link.invalid");
        Condition condition2 = new ConditionImpl("contentSize", Condition.ATTR_OP_LOWER_OR_EQUAL, new Integer(100));
        // same conditions but with a OR operator, should succeed
        Condition superCond1 = new ConditionImpl(Condition.COND_OP_OR,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            assertTrue(storage.delete(null, doc, superCond1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }

        // if one try to delete the same doc, it should fail
        try {
            assertFalse(storage.delete(null, doc, superCond1));
        } catch (StorageException e) {
            fail("exception while trying to delete " + docName + " a second time: " + e);
        }
    }


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

    public void testDeleteOneLevelDocumentsWithConditions2() {
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
        // (condition1 AND condition2) -> false (condition2 is not met)
        // condition3 -> true
        // ((condition1 AND condition2) OR condition3) -> true
        Condition superCond1 = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        Condition superCond2 = new ConditionImpl(Condition.COND_OP_OR,
                Arrays.asList(new Condition[] { superCond1, condition3 }));
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, superCond2));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }

        deletionHandler.assertNbDeleted(2);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX,
                CREATION_DATE_M2M_APPLICATIONS_OBIX, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        deletionHandler = new DeletionHandlerImpl();
        // if one try to delete the same doc, it should fail
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, null));
        } catch (StorageException e) {
            fail("exception while trying to delete " + docName + " a second time: " + e);
        }
        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteSubLevelDocumentsWithConditions1() {

        String docName = "/m2m/applications/obix";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        // first delete "/m2m/applications/obix"
        try {
            assertTrue(storage.delete(null, doc, null));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
        // watch for absence of deleted document
        // second, the real test: try to delete "/m2m/applications/obix" using scope subtree
        // this should fail because no entry in DOCUMENT table match "/m2m/applications/" as PATH and "obix" as NAME,
        // so the condition cannot be met.
        Condition condition1 = new ConditionImpl("link", Condition.ATTR_OP_EQUAL, "http://invalid.invalid"
                + PATH_M2M_APPLICATIONS_OBIX);
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, condition1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteSubLevelDocumentsWithConditions2() {
        // try to delete "/m2m/applications" using scope subtree (i.e. delete /m2m/applications*
        // as well as the records in ATTRIBUTE table that comes with these documents)
        // this should fail because no entry in DOCUMENT table match "/m2m/" as PATH and "applications" as NAME,
        // so the condition cannot be met.
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        Condition condition1 = new ConditionImpl("link", Condition.ATTR_OP_EQUAL, "http://invalid.invalid"
                + PATH_M2M_APPLICATIONS);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, condition1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }

        deletionHandler.assertNbDeleted(6);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX,
                CREATION_DATE_M2M_APPLICATIONS_OBIX, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        deletionHandler.assertDeleted(PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        // watch for absence of deleted document
        deletionHandler = new DeletionHandlerImpl();
        // if one try to delete the same doc, it should fail
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, condition1));
        } catch (StorageException e) {
            fail("exception while trying to delete " + docName + " a second time: " + e);
        }
        deletionHandler.assertNbDeleted(0);
        storage.unbindDeletionHandler(deletionHandler);
    }

    public void testDeleteOneLevelNoDocumentWithConditions() {
        String docName = "/m2m";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        Condition condition1 = new ConditionImpl("link", Condition.ATTR_OP_EQUAL, "http://invalid.invalid"
                + PATH_M2M_APPLICATIONS);
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, condition1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
    }

    public void testDeleteOneLevelNoDocumentWithoutConditions() {
        String docName = "/m2m";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_ONE_LEVEL, null));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
        storage.unbindDeletionHandler(deletionHandler);

    }

    public void testDeleteSubLevelNoDocumentWithConditions() {
        String docName = "/m2m";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        Condition condition1 = new ConditionImpl("link", Condition.ATTR_OP_EQUAL, "http://invalid.invalid"
                + PATH_M2M_APPLICATIONS);
        try {
            assertFalse(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, condition1));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
    }

    public void testDeleteSubLevelNoDocumentWithoutConditions() {
        String docName = "/m2m";
        Document doc = new DocumentImpl(null, docName);
        DeletionHandlerImpl deletionHandler = new DeletionHandlerImpl();
        storage.bindDeletionHandler(deletionHandler);
        try {
            assertTrue(storage.delete(null, doc, StorageRequestExecutor.SCOPE_SUB_TREE, null));
        } catch (StorageException e) {
            fail("delete operation failed on " + docName + ": " + e);
        }
        storage.unbindDeletionHandler(deletionHandler);

    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // RETRIEVE

    public void testRetrieveNoPath() {

        try {
            storage.retrieve(null, null, null);
            fail("retrieve operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testRetrieveWrongPath1() {

        String docName = "pathWithoutSlash";

        try {
            storage.retrieve(null, docName, null);
            fail("retrieve operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testRetrieveWrongPath2() {

        String docName = "/path/With/Slash/At/End/";

        try {
            storage.retrieve(null, docName, null);
            fail("retrieve operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testRetrieve1() {

        Document doc = null;
        String docName = "/m2m";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNull(doc);
    }

    public void testRetrieve2() {

        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieve3() {
        String docName = "/m2m/applications/APP_00000001";
        Document doc = new DocumentImpl(null, docName);
        Date now = new Date();
        doc.setAttribute("creationTime", now);

        // create a document with no content and no attributes
        try {
            assertTrue(storage.create(null, doc));
        } catch (StorageException e) {
            fail("create operation on " + docName + " failed: " + e);
        }

        // and now try to retrieve it
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        assertEquals(docName, doc.getPath());
        assertEquals(null, doc.getContent());
        assertNull(doc.getAttribute("doesNotExist"));
        assertNull(doc.getDateAttribute("doesNotExist"));
        assertNull(doc.getStringAttribute("doesNotExist"));
        assertNull(doc.getIntegerAttribute("doesNotExist"));
        assertNotNull(doc.getId());
        assertEquals(now, doc.getDateAttribute("creationTime"));
    }

    public void testRetrieveFromId() {

        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        assertTrue(((Long) doc.getId()).intValue() > 0);
        try {
            doc = storage.retrieve(null, doc.getId(), null);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + doc.getId() + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    // example of conditions
    // Condition condition1 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");
    // Condition condition2 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");
    // Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1,
    // condition2 }));
    public void testRetrieveWithCondition1() {

        Document doc = null;
        // /////////////////////// test on /m2m/applications /////////////////////////////
        String docName = "/m2m/applications";
        Condition condition1 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");
        try {
            doc = storage.retrieve(null, docName, condition1);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNull(doc);
    }

    public void testRetrieveWithCondition2() {
        Document doc = null;
        String docName = "/m2m/applications";

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        try {
            doc = storage.retrieve(null, docName, condition1);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveWithCondition3() {

        Document doc = null;
        String docName = "/m2m/applications";

        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        try {
            doc = storage.retrieve(null, docName, condition2);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveWithCondition4() {

        Document doc = null;
        String docName = "/m2m/applications";
        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            doc = storage.retrieve(null, docName, condition);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveWithCondition5() {

        Document doc = null;
        String docName = "/m2m/applications";
        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        // no contentTypes match "content type 2"
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 2");
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            doc = storage.retrieve(null, docName, condition);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNull(doc);
    }

    public void testRetrieveWithCondition6() {

        Document doc = null;
        String docName = "/m2m/applications";
        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        // conditions are all met (condition1 on contentSize, condition2 on contentTypes, condition3 and condition4 on
        // creationTime
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_GREATER_OR_EQUAL, new Date(
                CREATION_DATE_M2M_APPLICATIONS.getTime() - 10));
        Condition condition4 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL, new Date(
                CREATION_DATE_M2M_APPLICATIONS.getTime() + 10));
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3, condition4 }));
        try {
            doc = storage.retrieve(null, docName, condition);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveWithCondition7() {

        Document doc = null;
        String docName = "/m2m/applications";
        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        // condition on creationTime is not met
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_GREATER_OR_EQUAL, new Date(
                CREATION_DATE_M2M_APPLICATIONS.getTime() + 10));
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));
        try {
            doc = storage.retrieve(null, docName, condition);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNull(doc);
    }

    public void testRetrieveWithCondition8() {

        Document doc = null;
        String docName = "/m2m/applications";
        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_GREATER_OR_EQUAL, new Integer(1));
        // condition2 is not met
        Condition condition2 = new ConditionImpl("creationTime", Condition.ATTR_OP_NOT_EQUAL, CREATION_DATE_M2M_APPLICATIONS);
        Condition condition3 = new ConditionImpl("contentTypes", Condition.ATTR_OP_STARTS_WITH, "content");
        // (condition1 AND condition2) -> false (condition2 is not met)
        // condition3 -> true
        // ((condition1 AND condition2) OR condition3) -> true
        Condition superCond1 = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        Condition superCond2 = new ConditionImpl(Condition.COND_OP_OR,
                Arrays.asList(new Condition[] { superCond1, condition3 }));
        try {
            doc = storage.retrieve(null, docName, superCond2);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    // example of conditions
    // Condition condition1 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");
    // Condition condition2 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");
    // Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1,
    // condition2 }));
    public void testRetrieveFromIdWithCondition1() {

        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        assertTrue(((Long) doc.getId()).intValue() > 0);

        Condition condition1 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");
        try {
            doc = storage.retrieve(null, doc.getId(), condition1);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + doc.getId() + " failed: " + e);
        }
        assertNull(doc);
    }

    public void testRetrieveFromIdWithCondition2() {
        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        assertTrue(((Long) doc.getId()).intValue() > 0);

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        try {
            doc = storage.retrieve(null, doc.getId(), condition1);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + doc.getId() + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveFromIdWithCondition3() {
        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        int m2mApplicationsId = ((Long) doc.getId()).intValue();
        assertTrue(m2mApplicationsId > 0);

        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        try {
            doc = storage.retrieve(null, doc.getId(), condition2);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + m2mApplicationsId + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveFromIdWithCondition4() {
        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        int m2mApplicationsId = ((Long) doc.getId()).intValue();
        assertTrue(m2mApplicationsId > 0);

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            doc = storage.retrieve(null, doc.getId(), condition);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + m2mApplicationsId + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveFromIdWithCondition5() {
        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        int m2mApplicationsId = ((Long) doc.getId()).intValue();
        assertTrue(m2mApplicationsId > 0);

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        // no contentTypes match "content type 2"
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 2");
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            doc = storage.retrieve(null, doc.getId(), condition);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + m2mApplicationsId + " failed: " + e);
        }
        assertNull(doc);
    }

    public void testRetrieveFromIdWithCondition6() {
        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        int m2mApplicationsId = ((Long) doc.getId()).intValue();
        assertTrue(m2mApplicationsId > 0);

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        // conditions are all met (condition1 on contentSize, condition2 on contentTypes, condition3 and condition4 on
        // creationTime
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_GREATER_OR_EQUAL, new Date(
                CREATION_DATE_M2M_APPLICATIONS.getTime() - 10));
        Condition condition4 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL, new Date(
                CREATION_DATE_M2M_APPLICATIONS.getTime() + 10));
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3, condition4 }));
        try {
            doc = storage.retrieve(null, doc.getId(), condition);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + m2mApplicationsId + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    public void testRetrieveFromIdWithCondition7() {
        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        int m2mApplicationsId = ((Long) doc.getId()).intValue();
        assertTrue(m2mApplicationsId > 0);

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_EQUAL, new Integer(
                CONTENT_M2M_APPLICATIONS.length()));
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        // condition on creationTime is not met
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_GREATER_OR_EQUAL, new Date(
                CREATION_DATE_M2M_APPLICATIONS.getTime() + 10));
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));
        try {
            doc = storage.retrieve(null, doc.getId(), condition);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + m2mApplicationsId + " failed: " + e);
        }
        assertNull(doc);
    }

    public void testRetrieveFromIdWithCondition8() {
        Document doc = null;
        String docName = "/m2m/applications";
        try {
            doc = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation on " + docName + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);

        int m2mApplicationsId = ((Long) doc.getId()).intValue();
        assertTrue(m2mApplicationsId > 0);

        Condition condition1 = new ConditionImpl("contentSize", Condition.ATTR_OP_GREATER_OR_EQUAL, new Integer(1));
        // condition2 is not met
        Condition condition2 = new ConditionImpl("creationTime", Condition.ATTR_OP_NOT_EQUAL, CREATION_DATE_M2M_APPLICATIONS);
        Condition condition3 = new ConditionImpl("contentTypes", Condition.ATTR_OP_STARTS_WITH, "content");
        // (condition1 AND condition2) -> false (condition2 is not met)
        // condition3 -> true
        // ((condition1 AND condition2) OR condition3) -> true
        Condition superCond1 = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        Condition superCond2 = new ConditionImpl(Condition.COND_OP_OR,
                Arrays.asList(new Condition[] { superCond1, condition3 }));
        try {
            doc = storage.retrieve(null, doc.getId(), superCond2);
        } catch (StorageException e) {
            fail("retrieve operation on ID " + m2mApplicationsId + " failed: " + e);
        }
        assertNotNull(doc);
        controlDocIsM2mApplications(doc);
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // UPDATE
    public void testUpdateNoDocument() {
        try {
            storage.update(null, null, null);
            fail("update operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testUpdateDocumentNoPath() {
        Document doc = new Document() {

            public Object getId() {
                return null;
            }

            public String getPath() {
                return null;
            }

            public byte[] getContent() {
                return null;
            }

            public void setContent(byte[] content) {

            }

            public Object getAttribute(String name) {
                return null;
            }

            public Integer getIntegerAttribute(String name) {
                return null;
            }

            public Date getDateAttribute(String name) {
                return null;
            }

            public String getStringAttribute(String name) {
                return null;
            }

            public List getListAttribute(String name) {
                return null;
            }

            public void setAttribute(String name, Object value) {

            }

            public Object removeAttribute(String name) {
                return null;
            }

            public Iterator getAttributes() {
                return null;
            }

        };
        try {
            storage.update(null, doc, null);
            fail("update operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testUpdateNoAttribute() {
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);

        try {
            storage.update(null, doc, null);
            fail("update operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testUpdateWrongPath1() {

        String docName = "pathWithoutSlash";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("contentSize", new Integer(2));
        try {
            storage.update(null, doc, null);
            fail("update operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testUpdateWrongPath2() {

        String docName = "/path/With/Slash/At/End/";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("contentSize", new Integer(2));
        try {
            storage.update(null, doc, null);
            fail("update operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testUpdateDocumentThatDoesNotExist() {
        String docName = "/m2m/doesNotExists";
        Document doc = new DocumentImpl(null, docName);
        Date now = new Date();
        doc.setAttribute("creationTime", now);
        doc.setContent("content".getBytes());
        try {
            assertFalse(storage.update(null, doc, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }
    }

    static void assertByteArrayEquals(byte[] tab1, byte[] tab2) {
        assertEquals(true, Arrays.equals(tab1, tab2));
    }

    static void assertDocumentEqualsExceptId(Document expected, Document actual) {
        assertEquals(expected.getPath(), actual.getPath());
        assertByteArrayEquals(expected.getContent(), actual.getContent());
        Iterator expectedIt = expected.getAttributes();
        while (expectedIt.hasNext()) {
            Entry/* <String, Object> */entry = (Entry/* <String, Object> */) expectedIt.next();
            String attrName = (String) entry.getKey();
            Object attrValue = entry.getValue();
            System.out.println(attrName + " : " + attrValue + "\tType:"
                    + (attrValue != null ? attrValue.getClass().toString() : "null"));
            Object actualValue = actual.getAttribute(attrName);
            System.out.println(" ->" + actualValue);
            assertNotNull(actualValue);
            if (attrValue instanceof List) {
                assertTrue(actualValue instanceof List);
                List attrListValue = (List) attrValue;
                List actualListValue = (List) actualValue;
                System.out.println("Old:" + attrValue.toString() + " - Actual:" + actualListValue.toString());
                assertEquals(attrListValue.size(), actualListValue.size());
                for (int i = 0; i < actualListValue.size(); ++i) {
                    assertEquals(attrListValue.get(i), actualListValue.get(i));
                }
            } else {
                assertEquals(attrValue, actualValue);
            }
        }
    }

    public void testUpdate1() {
        String docName = "/m2m/applications";
        Document docRes = null;
        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("first retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        Long docId = (Long) docRes.getId();

        Document doc = new DocumentImpl(null, docName);
        Date now = new Date();
        doc.setAttribute("creationTime", now);
        try {
            assertTrue(storage.update(null, doc, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertEquals(docId, docRes.getId());
        assertNull(docRes.getContent());
        assertNull(docRes.getAttribute("contentTypes"));
        assertNull(docRes.getAttribute("link"));
        assertNull(docRes.getAttribute("contentSize"));
        assertEquals(now, docRes.getDateAttribute("creationTime"));
    }

    public void testUpdate2() {
        String docName = "/m2m/applications";
        Document docRes = null;
        Document doc = new DocumentImpl(null, docName);
        Date now = new Date();
        doc.setAttribute("creationTime", now);
        doc.setAttribute("newAttribute", "newValue");
        try {
            assertTrue(storage.update(null, doc, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertNull(docRes.getContent());
        assertNull(docRes.getAttribute("contentTypes"));
        assertNull(docRes.getAttribute("link"));
        assertNull(docRes.getAttribute("contentSize"));
        assertEquals(now, docRes.getDateAttribute("creationTime"));
        assertEquals("newValue", docRes.getStringAttribute("newAttribute"));
    }

    public void testUpdateNullAttribute() {
        String docName = "/m2m/applications";
        Document docRes = null;
        Document doc = new DocumentImpl(null, docName);
        Date now = new Date();
        doc.setAttribute("creationTime", now);
        doc.setAttribute("newAttribute", null);
        try {
            assertTrue(storage.update(null, doc, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertNull(docRes.getContent());
        assertNull(docRes.getAttribute("contentTypes"));
        assertNull(docRes.getAttribute("link"));
        assertNull(docRes.getAttribute("contentSize"));
        assertEquals(now, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("newAttribute"));
    }

    public void testUpdateWithCondition1() {
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        Date now = new Date();
        doc.setAttribute("creationTime", now);
        doc.setAttribute("newAttribute", "newValue");

        Condition condition1 = new ConditionImpl("ss", Condition.ATTR_OP_EQUAL, "1");

        try {
            assertFalse(storage.update(null, doc, condition1));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }
    }

    public void testUpdateWithCondition2() {
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        Date now = new Date();
        doc.setAttribute("creationTime", now);
        doc.setAttribute("newAttribute", "newValue");

        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_STARTS_WITH, "content");

        try {
            assertTrue(storage.update(null, doc, condition1));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        Document docRes = null;
        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertNull(docRes.getContent());
        assertNull(docRes.getAttribute("contentTypes"));
        assertNull(docRes.getAttribute("link"));
        assertNull(docRes.getAttribute("contentSize"));
        assertEquals(now, docRes.getDateAttribute("creationTime"));
        assertEquals("newValue", docRes.getStringAttribute("newAttribute"));
    }

    /**/
    // /////////////////////////////////////////////////////////////////////////////////////////////
    // PARTIAL UPDATE
    public void testPartialUpdateNoDocument() {
        try {
            storage.partialUpdate(null, null, null, null, null);
            fail("partialUpdate operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testPartialUpdateDocumentNoPath() {
        Document doc = new Document() {

            public Object getId() {
                return null;
            }

            public String getPath() {
                return null;
            }

            public byte[] getContent() {
                return null;
            }

            public void setContent(byte[] content) {

            }

            public Object getAttribute(String name) {
                return null;
            }

            public Integer getIntegerAttribute(String name) {
                return null;
            }

            public Date getDateAttribute(String name) {
                return null;
            }

            public String getStringAttribute(String name) {
                return null;
            }

            public List getListAttribute(String name) {
                return null;
            }

            public void setAttribute(String name, Object value) {

            }

            public Object removeAttribute(String name) {
                return null;
            }

            public Iterator getAttributes() {
                return null;
            }

        };
        try {
            storage.partialUpdate(null, doc, null, null, null);
            fail("partialUpdate operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testPartialUpdateWrongPath1() {

        String docName = "pathWithoutSlash";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("contentSize", new Integer(2));
        doc.setContent("content".getBytes());
        try {
            storage.partialUpdate(null, doc, null, null, null);
            fail("partialUpdate operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testPartialUpdateWrongPath2() {

        String docName = "/path/With/Slash/At/End/";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("contentSize", new Integer(2));
        doc.setContent("content".getBytes());
        try {
            storage.partialUpdate(null, doc, null, null, null);
            fail("partialUpdate operation on " + docName + " succeed, but should not");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testPartialUpdateDocumentThatDoesNotExist() {
        String docName = "/m2m/doesNotExists";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", new Date());
        doc.setContent("content".getBytes());

        try {
            assertFalse(storage.partialUpdate(null, doc, null, null, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }
    }

    public void testPartialUpdateDocumentNoContentChanges() {
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS);

        try {
            assertFalse(storage.partialUpdate(null, doc, null, null, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }
    }

    public void testPartialUpdateDocumentNoAttributeChanges() {
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);
        doc.setAttribute("creationTime", CREATION_DATE_M2M_APPLICATIONS);
        List attrOps = new ArrayList();
        attrOps.add(new AttributeOperationImpl("creationTime", AttributeOperation.TYPE_SET, CREATION_DATE_M2M_APPLICATIONS));

        try {
            assertTrue(storage.partialUpdate(null, doc, null, attrOps, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }
    }

    public void testPartialUpdate1() {
        String docName = "/m2m/applications";
        Document doc = new DocumentImpl(null, docName);

        // nothing to update in this document, should fail
        try {
            assertFalse(storage.partialUpdate(null, doc, null, null, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }
    }

    public void testPartialUpdate2() {
        String docName = "/m2m/applications";

        // retrieve the DOCUMENT.ID
        Document docRes = null;
        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        Long docId = (Long) docRes.getId();

        Document doc = new DocumentImpl(null, docName);
        doc.setContent("doc content".getBytes());

        // only update the content
        try {
            assertTrue(storage.partialUpdate(null, doc, "update content".getBytes(), null, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertEquals(docId, docRes.getId());
        assertEquals(PATH_M2M_APPLICATIONS, docRes.getPath());
        assertEquals("update content", new String(docRes.getContent()));
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS));
    }

    public void testPartialUpdate3() {
        String docName = "/m2m/applications";

        // retrieve the DOCUMENT.ID
        Document docRes = null;
        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        Long docId = (Long) docRes.getId();

        Date now = new Date();
        Document doc = new DocumentImpl(null, docName);
        List attrOps = new ArrayList();
        attrOps.add(new AttributeOperationImpl("contentSize", AttributeOperation.TYPE_SET, new Integer("content".length())));
        attrOps.add(new AttributeOperationImpl("creationTime", AttributeOperation.TYPE_SET, now));
        attrOps.add(new AttributeOperationImpl("link", AttributeOperation.TYPE_UNSET, null));

        // only update the content
        try {
            assertTrue(storage.partialUpdate(null, doc, null, attrOps, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertEquals(docId, docRes.getId());
        assertEquals(PATH_M2M_APPLICATIONS, docRes.getPath());
        assertEquals(CONTENT_M2M_APPLICATIONS, new String(docRes.getContent()));
        assertEquals(new Integer("content".length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(now, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS));
    }

    public void testPartialUpdateUnsetUnknownAttribute() {
        String docName = "/m2m/applications";

        // retrieve the DOCUMENT.ID
        Document docRes = null;
        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation failed: " + e);
        }

        Document doc = new DocumentImpl(null, docName);
        List attrOps = new ArrayList();
        attrOps.add(new AttributeOperationImpl("unknownAttribute", AttributeOperation.TYPE_UNSET, null));

        // only update the content
        try {
            assertTrue(storage.partialUpdate(null, doc, null, attrOps, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        controlDocIsM2mApplications(docRes);
    }

    public void testPartialUpdateOnList1() {
        String docName = "/m2m/applications";

        // retrieve the DOCUMENT.ID
        Document docRes = null;
        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        Long docId = (Long) docRes.getId();

        Document doc = new DocumentImpl(null, docName);
        List attrOps = new ArrayList();
        attrOps.add(new AttributeOperationImpl("contentTypes", AttributeOperation.TYPE_UNSET, null));

        // only update the content
        try {
            assertTrue(storage.partialUpdate(null, doc, null, attrOps, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertEquals(docId, docRes.getId());
        assertEquals(PATH_M2M_APPLICATIONS, docRes.getPath());
        assertEquals(CONTENT_M2M_APPLICATIONS, new String(docRes.getContent()));
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNull(attributes);
    }

    public void testPartialUpdateOnList2() {
        String docName = "/m2m/applications";

        // retrieve the DOCUMENT.ID
        Document docRes = null;
        String listElt = "new list elt";
        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed! " + e);
        }
        Long docId = (Long) docRes.getId();

        Document doc = new DocumentImpl(null, docName);
        List attrOps = new ArrayList();
        List contentTypes = new ArrayList();
        contentTypes.add(listElt);
        attrOps.add(new AttributeOperationImpl("contentTypes", AttributeOperation.TYPE_SET, contentTypes));

        // only update the content
        try {
            assertTrue(storage.partialUpdate(null, doc, null, attrOps, null));
        } catch (StorageException e) {
            fail("update operation failed: " + e);
        }

        try {
            docRes = storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("second retrieve operation failed: " + e);
        }
        assertNotNull(docRes);
        assertEquals(docId, docRes.getId());
        assertEquals(PATH_M2M_APPLICATIONS, docRes.getPath());
        assertEquals(CONTENT_M2M_APPLICATIONS, new String(docRes.getContent()));
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 1);
        assertTrue(attributes.contains(listElt));

    }

    public void testInsertAttributeFailure() throws SecurityException, NoSuchMethodException, IllegalArgumentException,
            IllegalAccessException, InvocationTargetException, SQLException {
        String docName = "/m2m/applications";
        try {
            storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation failed: " + e);
        }

    }

    public void testInsertAttributeFailure1() throws SecurityException, NoSuchMethodException, IllegalArgumentException,
            IllegalAccessException, InvocationTargetException, SQLException {
        String docName = "/m2m/applications";
        try {
            storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation failed: " + e);
        }

    }

    public void testInsertAttributeFailure2() throws SecurityException, NoSuchMethodException, IllegalArgumentException,
            IllegalAccessException, InvocationTargetException, SQLException {
        String docName = "/m2m/applications";
        try {
            storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation failed: " + e);
        }

    }

    public void testInsertAttributesFailure() throws SecurityException, NoSuchMethodException, IllegalArgumentException,
            IllegalAccessException, InvocationTargetException, SQLException {
        String docName = "/m2m/applications";
        try {
            storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation failed: " + e);
        }
    }

    public void testInsertAttributesListFailure() throws SecurityException, NoSuchMethodException, IllegalArgumentException,
            IllegalAccessException, InvocationTargetException, SQLException {
        String docName = "/m2m/applications";
        try {
            storage.retrieve(null, docName, null);
        } catch (StorageException e) {
            fail("retrieve operation failed: " + e);
        }
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // SEARCH ONE_LEVEL WITHOUT CONDITION

    public void testSearchOneLevelNoPath() {
        try {
            storage.search(null, null, StorageRequestExecutor.SCOPE_ONE_LEVEL, null, StorageRequestExecutor.ORDER_UNKNOWN, 1,
                    true, null);
            fail("Search operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testSearchOneLevelEmptyPathNoResults() {
        SearchResult res = null;
        try {
            res = storage.search(null, "", StorageRequestExecutor.SCOPE_ONE_LEVEL, null, StorageRequestExecutor.ORDER_UNKNOWN,
                    1, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();
        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }
    }

    public void testSearchOneLevelWrongPath1() {

        String docName = "pathWithoutSlash";
        try {
            storage.search(null, docName, StorageRequestExecutor.SCOPE_ONE_LEVEL, null, StorageRequestExecutor.ORDER_UNKNOWN,
                    1, true, null);
            fail("Search operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testSearchOneLevelWrongPath2() {

        String docName = "/path/With/Slash/At/End/";
        try {
            storage.search(null, docName, StorageRequestExecutor.SCOPE_ONE_LEVEL, null, StorageRequestExecutor.ORDER_UNKNOWN,
                    1, true, null);
            fail("Search operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testSearchSubTreeNoPath() {
        try {
            storage.search(null, null, StorageRequestExecutor.SCOPE_SUB_TREE, null, StorageRequestExecutor.ORDER_UNKNOWN, 1,
                    true, null);
            fail("Search operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testSearchSubTreeEmptyPathResults() {
        SearchResult res = null;
        try {
            res = storage.search(null, "", StorageRequestExecutor.SCOPE_SUB_TREE, null, StorageRequestExecutor.ORDER_UNKNOWN,
                    StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHT, CONTENT_M2M_ACCESSRIGHT, CREATION_DATE_M2M_ACCESSRIGHT,
                CTYPES_INT_M2M_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHTS, CONTENT_M2M_ACCESSRIGHTS, CREATION_DATE_M2M_ACCESSRIGHTS,
                CTYPES_INT_M2M_ACCESSRIGHTS);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHTS_LOCADMINAR, CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR, CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchSubTreeWrongPath1() {

        String docName = "pathWithoutSlash";
        try {
            storage.search(null, docName, StorageRequestExecutor.SCOPE_SUB_TREE, null, StorageRequestExecutor.ORDER_UNKNOWN, 1,
                    true, null);
            fail("Search operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testSearchSubTreeWrongPath2() {

        String docName = "/path/With/Slash/At/End/";
        try {
            storage.search(null, docName, StorageRequestExecutor.SCOPE_SUB_TREE, null, StorageRequestExecutor.ORDER_UNKNOWN, 1,
                    true, null);
            fail("Search operation succeeded! but should not...");
        } catch (StorageException e) {
            assertNotNull(e);
        }
    }

    public void testSearchOneLevelNoResult() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/DoesNotExist", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_UNKNOWN, 1, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();
        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelNoOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelNoOrderLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_UNKNOWN, 1, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelAscOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelDescOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_DESC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelAscOrderNoLimitWithoutContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_OBIX, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_OBIX.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_OBIX, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_OBIX, docRes.getStringAttribute("link"));
        attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_OBIX));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_OBIX));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelAscOrderNoLimitWithoutContentWithoutAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, new ArrayList());
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_OBIX, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelAscOrderNoLimitWithoutContentWithOneAttribute() {
        SearchResult res = null;
        List attrs = new ArrayList();
        attrs.add("link");

        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, attrs);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_OBIX, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_OBIX, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelAscOrderLimitWithContentWithOneAttributeList() {
        SearchResult res = null;
        List attrs = new ArrayList();
        attrs.add("contentTypes");

        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, null,
                    StorageRequestExecutor.ORDER_ASC, 1, true, attrs);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT, new String(docRes.getContent()));
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // SEARCH ONE_LEVEL WITH CONDITION

    public void testSearchOneLevelConditionNoOrderNoLimitWithContentWithAllAttributes1() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_LOWER, new Date());
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition1,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionNoOrderNoLimitWithContentWithAllAttributes2() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_GREATER,
                CREATION_DATE_M2M_APPLICATIONS_OBIX);
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition1,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionNoOrderNoLimitWithContentWithAllAttributes3() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_GREATER,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR);
        Condition condition2 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_LOWER,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionNoOrderLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_GREATER,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR);
        Condition condition2 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_LOWER,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_UNKNOWN, 1, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionAscOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));
        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }
    }

    public void testSearchOneLevelConditionDescOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_DESC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionAscOrderNoLimitWithoutContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, null);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getStringAttribute("link"));
        attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionAscOrderNoLimitWithoutContentWithoutAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));
        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, new ArrayList());
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionAscOrderNoLimitWithoutContentWithOneAttribute() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));
        List attrs = new ArrayList();
        attrs.add("link");

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, attrs);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchOneLevelConditionAscOrderLimitWithContentWithOneAttributeList() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition3 = new ConditionImpl("creationTime", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND, Arrays.asList(new Condition[] { condition1, condition2,
                condition3 }));
        List attrs = new ArrayList();
        attrs.add("contentTypes");

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_ONE_LEVEL, condition,
                    StorageRequestExecutor.ORDER_ASC, 1, true, attrs);
        } catch (StorageException e) {
            fail("Search operation failed: " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2, new String(docRes.getContent()));
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // SEARCH SUB_TREE WITHOUT CONDITION

    public void testSearchSubTreeNoResult() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/DoesNotExist", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_UNKNOWN, 1, true, null);
        } catch (StorageException e) {
            fail("Search operation failed");
        }

        assertNotNull(res);

        Iterator iter = res.getResults();
        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results: " + e);
        }

    }

    public void testSearchSubTreeAllEntriesNoOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHT, CONTENT_M2M_ACCESSRIGHT, CREATION_DATE_M2M_ACCESSRIGHT,
                CTYPES_INT_M2M_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHTS, CONTENT_M2M_ACCESSRIGHTS, CREATION_DATE_M2M_ACCESSRIGHTS,
                CTYPES_INT_M2M_ACCESSRIGHTS);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHTS_LOCADMINAR, CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR, CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeNoOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);
        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeNoOrderLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_UNKNOWN, 1, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeAscOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }
    }

    public void testSearchSubTreeDescOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_DESC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }
    }

    public void testSearchSubTreeAscOrderNoLimitWithoutContentWithAllAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getStringAttribute("link"));
        attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getStringAttribute("link"));
        attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getStringAttribute("link"));
        attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeAscOrderNoLimitWithoutContentWithoutAttributes() {
        SearchResult res = null;
        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, new ArrayList());
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeAscOrderNoLimitWithoutContentWithOneAttribute() {
        SearchResult res = null;
        List attrs = new ArrayList();
        attrs.add("link");

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, attrs);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeAscOrderLimitWithContentWithOneAttributeList() {
        SearchResult res = null;
        List attrs = new ArrayList();
        attrs.add("contentTypes");

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, null,
                    StorageRequestExecutor.ORDER_ASC, 1, true, attrs);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT, new String(docRes.getContent()));
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }
    }

    // /////////////////////////////////////////////////////////////////////////////////////////////
    // SEARCH SUB_TREE WITH CONDITION
    public void testSearchSubTreeConditionAllEntriesNoOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_GREATER,
                CREATION_DATE_M2M_APPLICATIONS);
        Condition condition2 = new ConditionImpl("creationTime", Condition.ATTR_OP_STRICTLY_LOWER,
                CREATION_DATE_M2M_APPLICATIONS_OBIX);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m", StorageRequestExecutor.SCOPE_SUB_TREE, condition,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHT, CONTENT_M2M_ACCESSRIGHT, CREATION_DATE_M2M_ACCESSRIGHT,
                CTYPES_INT_M2M_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHTS, CONTENT_M2M_ACCESSRIGHTS, CREATION_DATE_M2M_ACCESSRIGHTS,
                CTYPES_INT_M2M_ACCESSRIGHTS);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_ACCESSRIGHTS_LOCADMINAR, CONTENT_M2M_ACCESSRIGHTS_LOCADMINAR,
                CREATION_DATE_M2M_ACCESSRIGHTS_LOCADMINAR, CTYPES_INT_M2M_ACCESSRIGHTS_LOCADMINAR);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeConditionNoOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, condition,
                    StorageRequestExecutor.ORDER_UNKNOWN, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeConditionNoOrderLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, condition2,
                    StorageRequestExecutor.ORDER_UNKNOWN, 1, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS,
                CREATION_DATE_M2M_APPLICATIONS, CTYPES_INT_M2M_APPLICATIONS);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeConditionAscOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        Condition condition = new ConditionImpl(Condition.COND_OP_OR, Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }
    }

    public void testSearchSubTreeConditionDescOrderNoLimitWithContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, "content type 1");
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_EQUAL, CTYPES_INT_M2M_APPLICATIONS_OBIX);
        Condition condition = new ConditionImpl(Condition.COND_OP_OR, Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m/applications", StorageRequestExecutor.SCOPE_SUB_TREE, condition,
                    StorageRequestExecutor.ORDER_DESC, StorageRequestExecutor.NO_LIMIT, true, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CONTENT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CREATION_DATE_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d,
                CTYPES_INT_M2M_APPLICATIONS_SUBSCRIPTIONS_23b312a4435bfe2d);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_OBIX, CONTENT_M2M_APPLICATIONS_OBIX, CREATION_DATE_M2M_APPLICATIONS_OBIX,
                CTYPES_INT_M2M_APPLICATIONS_OBIX);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS_ACCESSRIGHT, CONTENT_M2M_APPLICATIONS_ACCESSRIGHT,
                CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT, CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT);

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        controlDoc(docRes, PATH_M2M_APPLICATIONS, CONTENT_M2M_APPLICATIONS, CREATION_DATE_M2M_APPLICATIONS,
                CTYPES_INT_M2M_APPLICATIONS);

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }
    }

    public void testSearchSubTreeConditionAscOrderNoLimitWithoutContentWithAllAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, null);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR1.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR1));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR2.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getStringAttribute("link"));
        attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR2));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR2));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(new Integer(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT_AR3.length()), docRes.getIntegerAttribute("contentSize"));
        assertEquals(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getStringAttribute("link"));
        attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT_AR3));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeConditionAscOrderNoLimitWithoutContentWithoutAttributes() {
        SearchResult res = null;
        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));
        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, new ArrayList());
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);

        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeConditionAscOrderNoLimitWithoutContentWithOneAttribute() {
        SearchResult res = null;
        List attrs = new ArrayList();
        attrs.add("link");

        Condition condition1 = new ConditionImpl("contentTypes", Condition.ATTR_OP_GREATER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR1);
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);
        Condition condition = new ConditionImpl(Condition.COND_OP_AND,
                Arrays.asList(new Condition[] { condition1, condition2 }));

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, condition,
                    StorageRequestExecutor.ORDER_ASC, StorageRequestExecutor.NO_LIMIT, false, attrs);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);
        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR1, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR2, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertTrue(iter.hasNext());
        docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getPath());
        assertEquals(null, docRes.getContent());
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals("http://invalid.invalid" + PATH_M2M_APPLICATIONS_ACCESSRIGHT_AR3, docRes.getStringAttribute("link"));
        assertNull(docRes.getListAttribute("contentTypes"));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }

    }

    public void testSearchSubTreeConditionAscOrderLimitWithContentWithOneAttributeList() {
        SearchResult res = null;
        List attrs = new ArrayList();
        attrs.add("contentTypes");
        Condition condition2 = new ConditionImpl("contentTypes", Condition.ATTR_OP_LOWER_OR_EQUAL,
                CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT_AR3);

        try {
            res = storage.search(null, "/m2m/applications/accessRight", StorageRequestExecutor.SCOPE_SUB_TREE, condition2,
                    StorageRequestExecutor.ORDER_ASC, 1, true, attrs);
        } catch (StorageException e) {
            fail("Search operation failed " + e);
        }

        assertNotNull(res);
        Iterator iter = res.getResults();

        assertTrue(iter.hasNext());
        Document docRes = (Document) iter.next();
        assertNotNull(docRes);
        assertEquals(PATH_M2M_APPLICATIONS_ACCESSRIGHT, docRes.getPath());
        assertEquals(CONTENT_M2M_APPLICATIONS_ACCESSRIGHT, new String(docRes.getContent()));
        assertEquals(null, docRes.getIntegerAttribute("contentSize"));
        assertEquals(null, docRes.getDateAttribute("creationTime"));
        assertEquals(null, docRes.getStringAttribute("link"));
        List attributes = docRes.getListAttribute("contentTypes");
        assertNotNull(attributes);
        assertTrue(attributes.size() == 3);
        assertTrue(attributes.contains(CREATION_DATE_M2M_APPLICATIONS_ACCESSRIGHT));
        assertTrue(attributes.contains("content type 1"));
        assertTrue(attributes.contains(CTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT));

        assertFalse(iter.hasNext());
        try {
            iter.next();
            fail("succeeded to get next element, but should not");
        } catch (NoSuchElementException e) {
            assertNotNull(e);
        }

        try {
            res.close();
        } catch (StorageException e) {
            fail("Fails to close Search results " + e);
        }
    }
}
