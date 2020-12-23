package org.musicpd;
import org.musicpd.IMainCallback;

interface IMain
{
    void start();
    void stop();
    void setWakelockEnabled(boolean enabled);
    boolean isRunning();
    void registerCallback(IMainCallback cb);
    void unregisterCallback(IMainCallback cb);
}
