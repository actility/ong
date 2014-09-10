package com.actility.m2m.song.binding.http.jni.command;

import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import org.apache.felix.shell.Command;

import com.actility.m2m.song.binding.http.jni.impl.JHttpClients;

public class HttpClientDumpTransactions implements Command {

    private JHttpClients clients;

    public HttpClientDumpTransactions(JHttpClients clients) {
        this.clients = clients;
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        Date now = Calendar.getInstance().getTime();
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss");
        out.print("At ");
        out.print(sdf.format(now));
        out.println(":");
        out.print(clients.dumpPendingTransactions());
        out.println();
    }

    public String getName() {
        return "http-pending-transactions";
    }

    public String getShortDescription() {
        return "Get information on HTTP client pending transactions";
    }

    public String getUsage() {
        return "http-pending-transactions\n";
    }
}
