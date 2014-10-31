package com.actility.m2m.servlet.song;

public interface ChannelClientListener {
    /**
     * Channel is in error
     */
    void channelError();

    /**
     * Channel is back to normal state
     */
    void channelOk();
}
