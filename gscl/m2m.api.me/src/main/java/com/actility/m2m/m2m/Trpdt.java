package com.actility.m2m.m2m;

import java.util.Date;

public final class Trpdt {
    private long tolerableDelay;
    private Date tolerableTime;

    public long getTolerableDelay() {
        return tolerableDelay;
    }

    public void setTolerableDelay(long tolerableDelay) {
        this.tolerableDelay = tolerableDelay;
    }

    public Date getTolerableTime() {
        return tolerableTime;
    }

    public void setTolerableTime(Date tolerableTime) {
        this.tolerableTime = tolerableTime;
    }

}
