package com.actility.m2m.scl.mem;

import java.text.ParseException;

import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.xo.XoException;

public class ExpirationTimerUpdateOp implements TransientOp {

    private final SclManager manager;
    private final String path;
    private final int ctrlId;
    private final long duration;

    public ExpirationTimerUpdateOp(SclManager manager, String path, int ctrlId, long duration) {
        this.manager = manager;
        this.path = path;
        this.ctrlId = ctrlId;
        this.duration = duration;
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
        String expirationTimeId = manager.startResourceTimer(duration, path, ctrlId, null);
        manager.getM2MContext().setAttribute(path, expirationTimeId);
    }
}
