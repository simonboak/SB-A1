# SB-A1: My Apple 1 clone with printer and SD Card storage

TL;DR: This project adds an SD storage interface and thermal printer to Ruud van Falier's excellent [Apple 1 Mini project](https://dutchmaker.com), who gave permission to share my modifications here.

There is no difference to the Apple 1 side of the circuit: all of my modifications are solely to the Arduino which was used for serial IO. To get around the low pin count of the Arduino the main change is on the keyboard data to feed this to the PIA from a shift register.

![SBA1](https://unimplementedtrap.com/media/sb-a1/sb-a1-800.jpg)

[Read all about this on my blog post](https://unimplementedtrap.com/apple-1-with-printer-sd-storage)
