---
title: svn服务器创建和使用
date: 2017-09-23 23:30:37
tags: svn
---

## 简介 ##
svn表示SubVersion。Subversion 是一款开放源代码的版本控制系统。使用 Subversion，您可以重新加载源代码和文档的历史版本。Subversion 管理了源代码在各个时期的版本。一个文件树被集中放置在文件仓库中。这个文件仓库很像是一个传统的文件服务器，只不过它能够记住文件和目录的每一次变化。

本文介绍怎么在ubuntu环境下搭建svn服务器，并在windows和ubuntu客户端使用svn服务器存储信息。

<!--more-->

## 安装 Subversion ##

	sudo apt-get install subversion
	sudo apt-get install libapache2-svn
	
## 创建 SVN 仓库 ##

增加组，并且把用户加到组里

	sudo addgroup subversion
	sudo usermod -G subversion -a www-data
	
您需要注销然后再登录以便您能够成为 subversion 组的一员，然后就可以执行签入文件（Check in，也称提交文件）的操作了。

现在执行下面的命令
	
	sudo mkdir /home/svn
	cd /home/svn
	sudo mkdir myproject
	sudo chown -R www-data:subversion myproject

下面的命令用于创建 SVN 文件仓库 `myproject`：

	sudo svnadmin create /home/svn/myproject

赋予组成员对所有新加入文件仓库的文件拥有相应的权限：

	sudo chmod -R g+rws myproject
	
## 添加用户 ##

在`myproject`下打开文件夹`conf`
编辑`svnserve.conf`
将

	...
	# anon-access = read
	...
	# auth-access = write
	...
	# password-db = passwd
	...
	# authz-db = authz
	...
	
去掉`#`注释，并且修改`read`为`none`

	...
	anon-access = none
	...
	auth-access = write
	...
	password-db = passwd
	...
	authz-db = authz
	...
	
编辑`passwd`
最后加上

	username = password
	
其中`username`代表用户名称，`password`代表密码，例如

	roee = 123456
	
表示用户名为`roee`，密码为`123456`
编辑`authz`，底部加入代码

	[myproject:/]
	roee = rw
	
表示对于`myproject`这个仓库，`roee`拥有读和写的操作

	* = rw
	
表示任何用户都有读写权限

## 通过自带协议访问 ##

### ubuntu ###

输入

	svn co svn://"ip地址"/"仓库名" "文件夹名称"
	
例如

	svn co svn://222.222.222.222/worklag mywork
	
即可拷贝文件下来

输入

	svn update
	
更新仓库到最新

输入

	svn add * --force
	
添加修改仓库

输入

	svn delete sth
	
修改仓库删除sth

输入

	svn commit -m ""
	
提交修改信息

### windows ###

下载svn相关客户端
输入仓库地址

	svn://222.222.222.222/worklag
	
即可下载