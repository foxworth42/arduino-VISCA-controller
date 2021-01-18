# Arduino VISCA Controller

This is a project for using an Arduino to send VISCA commands.  This is specifically tailored for interfacing with the
Cisco TelePresence Precision series of conference cameras (specifically testing with the TTC8-02 12x zoom w/ SDI output).

The TelePresence cameras have a slight quirk to controlling it over VISCA.  To get manual control over the pan/tilt/zoom
rates the camera needs to be sent the IR disable command, otherwise all movements behave with a speed ramp as if you 
were controlling it with an IR remote.

The Arduino Shield board present in the EAGLE directory is intended to make connecting to a TelePresence camera and
wiring I/O easier.  The shield has headers to allow for powering the Arduino off its internal voltage regulator or via
VIN (includes header points to make swapping easy).  There is also a header to provide 12v power over the RJ45
connection to remotely power the camera.  The shield incudes an RS232 transciever so it is ready to plug into the
camera's control port with a standard ethernet cable. 
