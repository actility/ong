package com.actility.m2m.servlet.song;

/**
 * Listener to receive events from <notificationChannel> or <communicationChannel>
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