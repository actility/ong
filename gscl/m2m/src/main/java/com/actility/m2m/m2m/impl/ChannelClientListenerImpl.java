package com.actility.m2m.m2m.impl;

import com.actility.m2m.servlet.song.ChannelClientListener;

public class ChannelClientListenerImpl implements ChannelClientListener {

    private final com.actility.m2m.m2m.ChannelClientListener channelListener;

    public ChannelClientListenerImpl(com.actility.m2m.m2m.ChannelClientListener channelListener) {
        this.channelListener = channelListener;
    }

    public void channelError() {
        channelListener.channelError();
    }

    public void channelOk() {
        channelListener.channelOk();
    }

}
