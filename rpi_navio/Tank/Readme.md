sudo apt-get install uv4l uv4l-raspicam uv4l-raspicam-extras  uv4l-server uv4l-uvc uv4l-xscreen uv4l-mjpegstream uv4l-dummy uv4l-raspidisp uv4l-webrtc

dmesg | grep cam (get idVendor:idProduct)

//test
uv4l --enable-server --driver uvc --device-id 1b71:0056

sudo nano /etc/uv4l/uv4l-raspicam.conf
Add: server-option = --device-id 1b71:0056

sudo nano /etc/systemd/system/uv4l_raspicam.service
Replace:
ExecStart=/usr/bin/uv4l -f -k --sched-fifo --mem-lock --enable-server --driver uvc --device-id 1b71:0056
sudo systemctl enable uv4l_raspicam.service
sudo systemctl start uv4l_raspicam.service
