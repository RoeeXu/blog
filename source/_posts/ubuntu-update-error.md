---
title: ubuntu update error
date: 2017-08-16 15:56:00
tags: cnn
---

在用apt-get安装软件时出现类似如下错误：

	E: Sub-process /usr/bin/dpkg returned an error code (1)

解决办法如下：
	
<!--more-->
	sudo mv /var/lib/dpkg/info /var/lib/dpkg/info.bak
	sudo mkdir /var/lib/dpkg/info
	sudo apt-get update
	sudo apt-get -f install
	sudo mv /var/lib/dpkg/info/* /var/lib/dpkg/info.bak
	sudo rm -rf /var/lib/dpkg/info
	sudo mv /var/lib/dpkg/info.bak /var/lib/dpkg/info