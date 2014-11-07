package com.actility.m2m.m2m.impl;

import junit.framework.TestCase;

import org.easymock.EasyMock;

import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.xo.XoService;

public class M2MUtilsImplTest extends TestCase {

    private Object[] mocks;
    private XoService xoService;
    private M2MUtilsImpl m2mUtils;

    @Override
    public void setUp() {
        xoService = EasyMock.createMock(XoService.class);

        mocks = new Object[] { xoService };

        EasyMock.expect(xoService.isXmlSupported()).andReturn(true);
        EasyMock.expect(xoService.isExiSupported()).andReturn(true);

        replay();

        try {
            m2mUtils = new M2MUtilsImpl(xoService);
        } finally {
            verify();
            reset();
        }
    }

    private void replay() {
        EasyMock.replay(mocks);
    }

    private void verify() {
        EasyMock.verify(mocks);
    }

    private void reset() {
        EasyMock.reset(mocks);
    }

    public void testGetAcceptedXoMediaType1() throws M2MException {
        assertEquals("application/xml", m2mUtils.getAcceptedXoMediaType("text/html, */*"));
    }

    public void testGetAcceptedXoMediaType2() throws M2MException {
        try {
            m2mUtils.getAcceptedXoMediaType("");
            fail();
        } catch (M2MException e) {

        }
    }

    public void testGetAcceptedXoMediaType3() throws M2MException {
        try {
            m2mUtils.getAcceptedXoMediaType("text/plain; q=0.5, text/html, text/x-dvi; q=0.8, text/x-c");
            fail();
        } catch (M2MException e) {

        }
    }

    public void testGetAcceptedXoMediaType4() throws M2MException {
        assertEquals("application/xml",
                m2mUtils.getAcceptedXoMediaType("text/plain; q=0.5, text/html, text/x-dvi; q=0.8, text/x-c, */*;q=0.1"));
    }

    public void testGetAcceptedXoMediaType5() throws M2MException {
        assertEquals(
                "application/xml",
                m2mUtils.getAcceptedXoMediaType("text/plain; q=1, text/html, application/xml; q=0.3, application/exi;q=0.2, */*;q=0.1"));
    }

    public void testGetAcceptedXoMediaType6() throws M2MException {
        assertEquals(
                "application/exi",
                m2mUtils.getAcceptedXoMediaType("text/plain; q=1, text/html, application/xml; q=0.2, application/exi;q=0.3, */*;q=0.1"));
    }

    public void testGetAcceptedXoMediaType7() throws M2MException {
        assertEquals("application/exi",
                m2mUtils.getAcceptedXoMediaType("application/xml; q=0.2, application/exi;q=0.3, */*;q=1"));
    }
}
