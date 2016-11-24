package com.actility.m2m.m2m.impl;

import com.actility.m2m.servlet.song.ChannelServerListener;

public class ChannelServerListenerImpl implements ChannelServerListener {

    private final com.actility.m2m.m2m.ChannelServerListener channelListener;

    public ChannelServerListenerImpl(com.actility.m2m.m2m.ChannelServerListener channelListener) {
        this.channelListener = channelListener;
    }

    public void channelInactive() {
        channelListener.channelInactive();
    }

    public void channelExpired() {
        channelListener.channelExpired();
    }

}
