#!/usr/bin/python
# Johan Dahlin 2010

import os
import sys

import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import glib
import gtk
import wnck

SPOTIFY_TITLE_SEPARATOR = ' \xe2\x80\x93 '


class SongChecker(object):
    def __init__(self):
        self.artist = None
        self.title = None
        self.status = 'stopped'
        self.service = None

    def _setStatus(self, status):
        if self.status == status:
            return
        self.status = status
        if self.service:
            self.service.StatusChange(self.service.GetStatus())

    def _setSong(self, artist, title):
        if (artist == self.artist and
            title == self.title):
            return
        self.artist = artist
        self.title = title
        if self.service:
            self.service.TrackChange(self.service.GetMetadata())

        print self.artist, self.title

    def check(self):
        screen = wnck.screen_get_default()
        while gtk.events_pending():
            gtk.main_iteration()

        artist = None
        title = None
        for window in screen.get_windows():
            name = window.get_name()
            if not name.startswith('Spotify'):
                continue
            if name[7:10] == ' - ':
                data = name[10:]
                if SPOTIFY_TITLE_SEPARATOR in data:
                    artist, title = data.split(SPOTIFY_TITLE_SEPARATOR, 1)
                    self._setSong(artist, title)
                    self._setStatus('playing')
            else:
                self._setStatus('plaused')
            break
        else:
            self._setStatus('stopped')


class MPRISService(dbus.service.Object):
    def __init__(self, object_path, checker):
        bus_name = dbus.service.BusName('org.mpris.spotify', bus=dbus.SessionBus())
        dbus.service.Object.__init__(self, bus_name, object_path)
        self.checker = checker

    @dbus.service.method(dbus_interface='org.freedesktop.MediaPlayer', out_signature='s')
    def GetIdentity(self):
        return 'Spotify 0.3'

    @dbus.service.method(dbus_interface='org.freedesktop.MediaPlayer', out_signature='(iiii)')
    def GetStatus(self):
        if self.checker.status == 'playing':
            status = 0
        elif self.checker.status == 'paused':
            status = 1
        else:
            status = 2
        return status, 0, 0, 0

    @dbus.service.method(dbus_interface='org.freedesktop.MediaPlayer', out_signature='a{sv}')
    def GetMetadata(self):
        return { 'artist': self.checker.artist,
                 'title': self.checker.title,
                 'album': '',
                 'time': 60 }

    @dbus.service.method(dbus_interface='org.freedesktop.MediaPlayer', out_signature='i')
    def PositionGet(self):
        return 30

    @dbus.service.signal(dbus_interface='org.freedesktop.MediaPlayer', signature='(iiii)')
    def StatusChange(self, status):
        pass

    @dbus.service.signal(dbus_interface='org.freedesktop.MediaPlayer', signature='a{sv}')
    def TrackChange(self, metadata):
        pass


def checker(sc):
    sc.check()
    return True


def main():
    DBusGMainLoop(set_as_default=True)

    def on_MediaPlayerKeyPressed(*keys):
        for key in keys:
            if key == 'Play':
                os.system('./spotify_cmd.exe playpause')
            else:
                print key
    bus = dbus.SessionBus()
    sd = bus.get_object('org.gnome.SettingsDaemon',
        '/org/gnome/SettingsDaemon/MediaKeys')
    sd.connect_to_signal("MediaPlayerKeyPressed", on_MediaPlayerKeyPressed)

    sc = SongChecker()
    service = MPRISService('/Player', sc)
    sc.service = service
    mainloop = glib.MainLoop()
    glib.timeout_add(1000, checker, sc)
    mainloop.run()
    return 0

if __name__ == "__main__":
    sys.exit(main())
