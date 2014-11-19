/**
 *
 */
package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Date;
import java.util.HashMap;
import java.util.List;

import com.actility.m2m.storage.DeletionHandler;
import com.actility.m2m.storage.Document;

/**
 * @author mlouiset
 *
 */
public class DeletionHandlerImpl implements DeletionHandler {

    HashMap/*<String, Document>*/ deletedDocs = new HashMap/*<String, Document>*/();
    public DeletionHandlerImpl(){
//        System.out.println("==================New Handler==================");
    }
    public void assertNbDeleted(int i) {
        junit.framework.Assert.assertEquals(i, deletedDocs.size());
    }

    public void assertDeleted(String path, String content, Date date, Integer cTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT) {

        Document doc = (Document) deletedDocs.get(path);
        junit.framework.Assert.assertNotNull(doc);

        junit.framework.Assert.assertEquals(path, doc.getPath());
        junit.framework.Assert.assertEquals(content, new String(doc.getContent()));
        junit.framework.Assert.assertEquals(new Integer(content.length()), doc.getIntegerAttribute("contentSize"));
        junit.framework.Assert.assertEquals(date, doc.getDateAttribute("creationTime"));
        junit.framework.Assert.assertEquals("http://invalid.invalid" + path, doc.getStringAttribute("link"));
        List attributes = doc.getListAttribute("contentTypes");
        junit.framework.Assert.assertNotNull(attributes);
        junit.framework.Assert.assertTrue(attributes.size() == 3);
        junit.framework.Assert.assertTrue(attributes.contains(date));
        junit.framework.Assert.assertTrue(attributes.contains("content type 1"));
        junit.framework.Assert.assertTrue(attributes.contains(cTYPES_INT_M2M_APPLICATIONS_ACCESSRIGHT));
    }

    public void deleted(Document document) {
//        System.out.println("Deleted path:"+document.getPath());
        if (deletedDocs.containsKey(document.getPath())) {
            junit.framework.Assert.fail("Duplicate notification for document deletion (path:" + document.getPath() + ")");
        }
        deletedDocs.put(document.getPath(), document);
    }

}
