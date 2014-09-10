package com.actility.m2m.song.binding.http.jni.impl;

import java.util.Map.Entry;

public class HeaderEntry implements Entry {

    private String name;
    private String value;

    public HeaderEntry(String name, String value) {
        this.name = new String(name);
        this.value = new String(value);
    }

    public Object getKey() {
        return name;
    }

    public Object getValue() {
        return value;
    }

    public Object setValue(Object value) {
        return null;
    }
}
