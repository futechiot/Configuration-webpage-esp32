# Configuration-webpage-esp32

A web page hosted on wifi accesspoint of 32 bit micro-controller to provide each configuration details to micro-controller required for data sending to server, details includes connection of Router(dhcp and static modes) To Server(HTTP, TCP, MQTT). Give details through web page and connect your esp32 board to your wifi connection and with your server to send data.

![login_page_2_50](https://user-images.githubusercontent.com/38309034/69948008-d2e56680-1514-11ea-9f9c-45ff2e7cb815.jpeg)
![main_page_status_50](https://user-images.githubusercontent.com/38309034/69948226-3ff8fc00-1515-11ea-9384-f77cf304bf8e.jpeg)
![wifi_settings_50](https://user-images.githubusercontent.com/38309034/69948228-412a2900-1515-11ea-9005-a9f3e0c9566d.jpeg)
![service_50](https://user-images.githubusercontent.com/38309034/69948239-44251980-1515-11ea-947c-306a484070c7.jpeg)
![rebootreset_52](https://user-images.githubusercontent.com/38309034/69948514-cca3ba00-1515-11ea-85bb-06951b0ef737.jpeg)


### Configuration Process
1. Upload .ino code to your ESP32.
1. Uplad Data folder using Esp32 sketch data upload.
2. Connect mobile with hotspot : ESP32_PAGE_2.
3. Go to web browser and hit 192.168.4.1.
4. Scan for your wifi network, write password hit apply.
5. Press Reboot. 
6. esp32 is now connected with your wifi network.
7. Go to service section give necessary server credential for MQTT or HTTP or TCP to connect. 
8. press reboot.
9. esp32 will connect with your server and send your data to your server. 

##### Library Used 

- ESp32_arduino :-    https://github.com/espressif/arduino-esp32
- ESPAsyncWebServer:- https://github.com/me-no-dev/ESPAsyncWebServer
- AsyncTCP:- https://github.com/me-no-dev/AsyncTCP
- esp32 SPIFF File system :- https://github.com/me-no-dev/arduino-esp32fs-plugin
