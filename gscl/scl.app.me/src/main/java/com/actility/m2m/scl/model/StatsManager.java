package com.actility.m2m.scl.model;

public final class StatsManager {
    private int nbOfSubscriptions;

    public int getNbOfSubscriptions() {
        return nbOfSubscriptions;
    }

    public void incNbOfSubscriptions() {
        ++nbOfSubscriptions;
    }

    public void decNbOfSubscriptions() {
        --nbOfSubscriptions;
    }
}
