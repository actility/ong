package com.actility.m2m.scl.mem;

import java.text.ParseException;

import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.xo.XoException;

public class ExpirationTimerDeleteOp implements TransientOp {

    private final SclManager manager;
    private final String path;

    public ExpirationTimerDeleteOp(SclManager manager, String path) {
        this.manager = manager;
        this.path = path;
    }

    public void prepare() throws ParseException, StorageException, XoException, M2MException {
        // Nothing to do
    }

    public void rollback() {
        // Nothing to do
    }

    public void postCommit() {
        String previousExpirationTimeId = (String) manager.getM2MContext().getAttribute(path);
        if (previousExpirationTimeId != null) {
            manager.getM2MContext().cancelTimer(previousExpirationTimeId);
        }
        manager.getM2MContext().removeAttribute(path);
    }
}
