# NXDavSync: Sync local folders with WebDAV

Nintendo Switch port of [`3DavSync`](https://github.com/szclsya/3DavSync).

`NXDavSync` allows you to sync folders on the NSW SD card to a remote WebDAV server.

## Build

```
docker pull devkitpro/devkita64:latest
docker run --rm -v "$PWD":/app -w /app devkitpro/devkita64:latest make
```

## Configuration
NXDavSync accepts an ini-formatted config file at `/switch/NXDavSync.ini`. This file should look like this:

```ini
[General]
# List webdav configs that will be synced
Enabled=saves roms

# Example: Sync Checkpoint save folder with Nextcloud/ownCloud
[saves]
Url=https://example.org/remote.php/dav/files/username/saves
LocalPath=/switch/Checkpoint/saves
# Specify credential here
Username=REDACTED
Password=REDACTED

# Example: Sync roms
[roms]
Url=https://example.org/whatever/webdav
LocalPath=/roms
Username=REDACTED
Password=REDACTED
```
