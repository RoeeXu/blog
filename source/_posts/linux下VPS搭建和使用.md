---
title: linux下VPS搭建和使用
date: 2017-10-22 23:30:37
tags: vps
---

## INTRODUCTION ##
How to surf the internet without blocking by the GREAT FIREWALL?

I wanna to tell you an effective way.

<!--more-->

## BUY VPS ##
[https://bwh1.net/](https://bwh1.net/)
I prefer to use paypal

## SETTING SS ##
log in your VPS, download and run:
[shadowsocks.sh](/code/shadowsocks.sh)
set your own config
until the code complete setting
write down the infomation of shaowsocks

## SHADOWSOCKS ##
write you config doc
	
	{
		"server":"0.0.0.0", # your ip address
		"server_port":XXXX, # your passwd
		"local_address":"127.0.0.1",
		"local_port":1080,
		"password":"XX", # your passwd
		"timeout":300,
		"method":"XX-XX", # your crypt method
		"fast_open":false
	}

save as shadowsocks.json
run:

	sslocal -c shadowsocks.json

to connect to your VPS

## CHROME ##
download the latest chrome
download [SwichyOmega](/code/SwitchyOmega.crx)
apply the setting [OmegaOptions](/code/OmegaOptions.bak)

Now you can use your own VPS to surf the internet freedomly.