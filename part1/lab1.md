# Lab 1 (Demo Only)
1. Ping Both VMs from host
`ping 192.168.0.192`
`ping 198.168.0.193`

2. Ping VM1 from VM2
`ping 198.168.0.193`

3. Start web server and ping from host
`python3 -m http.server 8000`
`curl 192.168.0.192:8000`

