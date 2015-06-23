package com.actility.m2m.m2m;

/**
 * Listener to receive events from &lt;notificationChannel&gt; or &lt;communicationChannel&gt;
 */
public interface ChannelServerListener {
    /**
     * Channel is inactive
     */
    void channelInactive();

    /**
     * Channel is expired
     */
    void channelExpired();
}
