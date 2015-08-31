ALSA音频工具amixer,aplay,arecord

 aplay -Dhw:0,0 -r44100 -f S16_LE -c 2 -t raw 20150831_173341.raw






1.compile

sw@sw-ThinkPad-Edge:~/.ua101$ gcc *.c -lasound
test.c:2:30: fatal error: alsa/asoundlib.h: 没有那个文件或目录
 #include <alsa/asoundlib.h>  
                              ^
compilation terminated.


sudo apt-get install libasound2-dev






"aplay -l" to get a list of the devices on your system. 
The hw:X,Y comes from this mapping of your hardware -- in this case, 
X is the card number, 
while Y is the device number. 

$ aplay -l   # note lower-case "L" option
**** List of PLAYBACK Hardware Devices ****
card 0: T71Space [Terratec Aureon 7.1-Space], device 0: ICE1724 [ICE1724]
  Subdevices: 1/1
  Subdevice #0: subdevice #0
card 0: T71Space [Terratec Aureon 7.1-Space], device 1: IEC1724 IEC958 [IEC1724 IEC958]
  Subdevices: 0/1
  Subdevice #0: subdevice #0
card 0: T71Space [Terratec Aureon 7.1-Space], device 2: ICE1724 Surrounds [ICE1724 Surround PCM]
  Subdevices: 3/3
  Subdevice #0: subdevice #0
  Subdevice #1: subdevice #1
  Subdevice #2: subdevice #2










aplay -l
cat /proc/asound/card










sw@sw-ThinkPad-Edge:~/.ua101$ aplay -L
default
    Playback/recording through the PulseAudio sound server
null
    Discard all samples (playback) or generate zero samples (capture)
pulse
    PulseAudio Sound Server
sysdefault:CARD=MID
    HDA Intel MID, ALC269 Analog
    Default Audio Device
front:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    Front speakers
surround40:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    4.0 Surround output to Front and Rear speakers
surround41:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    4.1 Surround output to Front, Rear and Subwoofer speakers
surround50:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    5.0 Surround output to Front, Center and Rear speakers
surround51:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    5.1 Surround output to Front, Center, Rear and Subwoofer speakers
surround71:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    7.1 Surround output to Front, Center, Side, Rear and Woofer speakers
dmix:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    Direct sample mixing device
dsnoop:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    Direct sample snooping device
hw:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    Direct hardware device without any conversions
plughw:CARD=MID,DEV=0
    HDA Intel MID, ALC269 Analog
    Hardware device with all software conversions
hdmi:CARD=HDMI,DEV=0
    HDA ATI HDMI, HDMI 0
    HDMI Audio Output
dmix:CARD=HDMI,DEV=3
    HDA ATI HDMI, HDMI 0
    Direct sample mixing device
dsnoop:CARD=HDMI,DEV=3
    HDA ATI HDMI, HDMI 0
    Direct sample snooping device
hw:CARD=HDMI,DEV=3
    HDA ATI HDMI, HDMI 0
    Direct hardware device without any conversions
plughw:CARD=HDMI,DEV=3
    HDA ATI HDMI, HDMI 0
    Hardware device with all software conversions


#So your aplay command would become "aplay -D front:CARD=CK804,DEV=0 somefile.wav", and you can use the same devicename in your .asoundrc
