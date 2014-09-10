package com.actility.m2m.song.binding.http.jni.command;

import java.io.PrintStream;
import java.util.Iterator;
import java.util.Map.Entry;

import org.apache.felix.shell.Command;

import com.actility.m2m.song.binding.http.jni.impl.HttpClientStatsImpl;

public class HttpClientStatsCommand implements Command {

    private HttpClientStatsImpl statsModule;

    public HttpClientStatsCommand(HttpClientStatsImpl statsModule) {
        this.statsModule = statsModule;
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        out.print("Nb requests sent: ");
        out.print(statsModule.getRequestSent());
        out.println();

        out.print("Nb responses received: ");
        out.print(statsModule.getResponseCompleted());
        out.println();

        out.println("Status codes:");
        Iterator iter = statsModule.getStatusCodeEntries().iterator();
        while (iter.hasNext()) {
            Entry entry = (Entry) iter.next();
            out.print(entry.getKey());
            out.print(": ");
            out.print(entry.getValue());
            out.println();
        }
        out.println();
    }

    public String getName() {
        return "http-stats";
    }

    public String getShortDescription() {
        return "Get HTTP client statistics";
    }

    public String getUsage() {
        return "http-stats\n";
    }
}
