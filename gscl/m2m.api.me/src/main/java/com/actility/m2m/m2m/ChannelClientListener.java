package com.actility.m2m.m2m;

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
