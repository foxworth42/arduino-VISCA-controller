# Arduino VISCA Controller

## The project is a work-in-progress

This is a project for using an Arduino to send VISCA commands.  This is tailored for interfacing with the
Cisco TelePresence PrecisionHD series of conference cameras (specifically testing with the TTC8-02 12x zoom w/ SDI output).

The TelePresence cameras have a slight quirk to controlling it over VISCA.  To get manual control over the pan/tilt/zoom
rates the camera needs to be sent the IR disable command, otherwise all movements behave with a speed ramp as if you 
were controlling it with an IR remote.

The Arduino Shield board present in the EAGLE directory is intended to make connecting to a TelePresence camera and
wiring I/O easier.  The shield has headers to allow for powering the Arduino off its internal voltage regulator or via
VIN (includes header points to make swapping easy).  There is also a header to provide 12v power over the RJ45
connection to remotely power the camera.  The shield incudes an RS232 transciever so it is ready to plug into the
camera's control port with a standard ethernet cable.

[Cisco TelePresence PrecisionHD User Guide](https://www.cisco.com/c/dam/en/us/td/docs/telepresence/endpoint/camera/precisionhd/user_guide/precisionhd_1080p-720p_camera_user_guide.pdf) (includes supported VISCA command details, cable wiring, etc.)

# Arduino Shield components

|Component|Quantity|Notes|Link|
|---|---|---|---|
|Arduino Shield|1|See EAGLE directory|[Link](https://oshpark.com/shared_projects/oP5dpPBc)|
|RS232 Transciever|1| |[Link](https://www.mouser.com/ProductDetail/968-HIN202IBNZ-T/)|
|0.1uf SMD Capacitor|5| |[Link](https://www.mouser.com/ProductDetail/80-C0805C104J5R/)|
|RJ45 Jack|1| |[Link](https://www.mouser.com/ProductDetail/523-RJHSE-5381/)|
|2pin Molex Header|2|Or whatever header you prefer|[Link](https://www.mouser.com/ProductDetail/538-22-23-2021/)|
|2pin Molex Housing|2| |[Link](https://www.mouser.com/ProductDetail/538-22-01-3027/)|
|3pin Molex Header|4| |[Link](https://www.mouser.com/ProductDetail/538-22-23-2031/)|
|3pin Molex Housing|4| |[Link](https://www.mouser.com/ProductDetail/538-22-01-3037/)|
|4pin Molex Header|2| |[Link](https://www.mouser.com/ProductDetail/538-22-23-2041/)|
|4pin Molex Housing|2| |[Link](https://www.mouser.com/ProductDetail/538-22-01-3047/)|
|8pin Molex Header|2| |[Link](https://www.mouser.com/ProductDetail/538-22-23-2081/)|
|8pin Molex Housing|2| |[Link](https://www.mouser.com/ProductDetail/538-22-01-3087/)|
|Crimp contacts for Molex housings|Lots| |[Link](https://www.mouser.com/ProductDetail/538-08-51-0108-LP/)|
|Crimp tool for Molex contacts|1| |[Link](https://www.amazon.com/gp/product/B078WPT5M1/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1)|
|Bunch of single row 2.54 pitch pin headers|Bunch| | |
|10k Ohm Resistors|10| |
